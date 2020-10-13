/*
Copyright (c) 2012-2020 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License v1.0
and Eclipse Distribution License v1.0 which accompany this distribution.
 
The Eclipse Public License is available at
   http://www.eclipse.org/legal/epl-v10.html
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.
 
Contributors:
   Roger Light - initial implementation and documentation.
*/

#include "config.h"

#include <errno.h>
#include <openssl/opensslv.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef WIN32
#  include <windows.h>
#  include <process.h>
#	ifndef __cplusplus
#		if defined(_MSC_VER) && _MSC_VER < 1900
#			define bool char
#			define true 1
#			define false 0
#		else
#			include <stdbool.h>
#		endif
#	endif
#   define snprintf sprintf_s
#	include <io.h>
#	include <windows.h>
#else
#  include <stdbool.h>
#  include <unistd.h>
#  include <termios.h>
#  include <sys/stat.h>
#endif

#define MAX_BUFFER_LEN 65536
#define SALT_LEN 12

#include "misc_mosq.h"

struct cb_helper {
	const char *line;
	const char *username;
	const char *password;
	bool found;
};

#ifdef WIN32
static FILE *mpw_tmpfile(void)
{
	return tmpfile();
}
#else

static char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

static unsigned char tmpfile_path[36];
static FILE *mpw_tmpfile(void)
{
	int fd;
	size_t i;

	if(RAND_bytes(tmpfile_path, sizeof(tmpfile_path)) != 1){
		return NULL;
	}

	strcpy((char *)tmpfile_path, "/tmp/");

	for(i=strlen((char *)tmpfile_path); i<sizeof(tmpfile_path)-8; i++){
		tmpfile_path[i] = alphanum[tmpfile_path[i]%(sizeof(alphanum)-1)];
	}
	tmpfile_path[sizeof(tmpfile_path)-8] = '-';
	for(i=sizeof(tmpfile_path)-7; i<sizeof(tmpfile_path)-1; i++){
		tmpfile_path[i] = 'X';
	}
	tmpfile_path[sizeof(tmpfile_path)-1] = '\0';

	umask(077);
	fd = mkstemp((char *)tmpfile_path);
	if(fd < 0) return NULL;
	unlink((char *)tmpfile_path);

	return fdopen(fd, "w+");
}
#endif


int base64_encode(unsigned char *in, unsigned int in_len, char **encoded)
{
	BIO *bmem, *b64;
	BUF_MEM *bptr;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, in, in_len);
	if(BIO_flush(b64) != 1){
		BIO_free_all(b64);
		return 1;
	}
	BIO_get_mem_ptr(b64, &bptr);
	*encoded = malloc(bptr->length+1);
	if(!(*encoded)){
		BIO_free_all(b64);
		return 1;
	}
	memcpy(*encoded, bptr->data, bptr->length);
	(*encoded)[bptr->length] = '\0';
	BIO_free_all(b64);

	return 0;
}


void print_usage(void)
{
	printf("mosquitto_passwd is a tool for managing password files for mosquitto.\n\n");
	printf("Usage: mosquitto_passwd [-c | -D] passwordfile username\n");
	printf("       mosquitto_passwd [-c] -b passwordfile username password\n");
	printf("       mosquitto_passwd -U passwordfile\n");
	printf(" -b : run in batch mode to allow passing passwords on the command line.\n");
	printf(" -c : create a new password file. This will overwrite existing files.\n");
	printf(" -D : delete the username rather than adding/updating its password.\n");
	printf(" -U : update a plain text password file to use hashed passwords.\n");
	printf("\nSee https://mosquitto.org/ for more information.\n\n");
}

int output_new_password(FILE *fptr, const char *username, const char *password)
{
	int rc;
	unsigned char salt[SALT_LEN];
	char *salt64 = NULL, *hash64 = NULL;
	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int hash_len;
	const EVP_MD *digest;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	EVP_MD_CTX context;
#else
	EVP_MD_CTX *context;
#endif

	rc = RAND_bytes(salt, SALT_LEN);
	if(!rc){
		fprintf(stderr, "Error: Insufficient entropy available to perform password generation.\n");
		return 1;
	}

	rc = base64_encode(salt, SALT_LEN, &salt64);
	if(rc){
		free(salt64);
		fprintf(stderr, "Error: Unable to encode salt.\n");
		return 1;
	}


	digest = EVP_get_digestbyname("sha512");
	if(!digest){
		free(salt64);
		fprintf(stderr, "Error: Unable to create openssl digest.\n");
		return 1;
	}

#if OPENSSL_VERSION_NUMBER < 0x10100000L
	EVP_MD_CTX_init(&context);
	EVP_DigestInit_ex(&context, digest, NULL);
	EVP_DigestUpdate(&context, password, strlen(password));
	EVP_DigestUpdate(&context, salt, SALT_LEN);
	EVP_DigestFinal_ex(&context, hash, &hash_len);
	EVP_MD_CTX_cleanup(&context);
#else
	context = EVP_MD_CTX_new();
	EVP_DigestInit_ex(context, digest, NULL);
	EVP_DigestUpdate(context, password, strlen(password));
	EVP_DigestUpdate(context, salt, SALT_LEN);
	EVP_DigestFinal_ex(context, hash, &hash_len);
	EVP_MD_CTX_free(context);
#endif

	rc = base64_encode(hash, hash_len, &hash64);
	if(rc){
		free(salt64);
		free(hash64);
		fprintf(stderr, "Error: Unable to encode hash.\n");
		return 1;
	}

	fprintf(fptr, "%s:$6$%s$%s\n", username, salt64, hash64);
	free(salt64);
	free(hash64);

	return 0;
}


static int pwfile_iterate(FILE *fptr, FILE *ftmp,
		int (*cb)(FILE *, FILE *, const char *, const char *, const char *, struct cb_helper *),
		struct cb_helper *helper)
{
	char *buf;
	int buflen = 1024;
	char *lbuf;
	int lbuflen;
	int rc = 1;
	int line = 0;
	char *username, *password;

	buf = malloc(buflen);
	if(buf == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}
	lbuflen = buflen;
	lbuf = malloc(lbuflen);
	if(lbuf == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		free(buf);
		return 1;
	}

	while(!feof(fptr) && fgets_extending(&buf, &buflen, fptr)){
		if(lbuflen != buflen){
			free(lbuf);
			lbuflen = buflen;
			lbuf = malloc(lbuflen);
			if(lbuf == NULL){
				fprintf(stderr, "Error: Out of memory.\n");
				free(buf);
				return 1;
			}
		}
		memcpy(lbuf, buf, buflen);
		line++;
		username = strtok(buf, ":");
		password = strtok(NULL, ":");
		if(username == NULL || password == NULL){
			fprintf(stderr, "Error: Corrupt password file at line %d.\n", line);
			free(lbuf);
			free(buf);
			return 1;
		}
		username = misc__trimblanks(username);
		password = misc__trimblanks(password);

		if(strlen(username) == 0 || strlen(password) == 0){
			fprintf(stderr, "Error: Corrupt password file at line %d.\n", line);
			free(lbuf);
			free(buf);
			return 1;
		}

		rc = cb(fptr, ftmp, username, password, lbuf, helper);
		if(rc){
			break;
		}
	}
	free(lbuf);
	free(buf);

	return rc;
}


/* ======================================================================
 * Delete a user from the password file
 * ====================================================================== */
static int delete_pwuser_cb(FILE *fptr, FILE *ftmp, const char *username, const char *password, const char *line, struct cb_helper *helper)
{
	if(strcmp(username, helper->username)){
		/* If this isn't the username to delete, write it to the new file */
		fprintf(ftmp, "%s", line);
	}else{
		/* Don't write the matching username to the file. */
		helper->found = true;
	}
	return 0;
}

int delete_pwuser(FILE *fptr, FILE *ftmp, const char *username)
{
	struct cb_helper helper;
	int rc;

	memset(&helper, 0, sizeof(helper));
	helper.username = username;
	rc = pwfile_iterate(fptr, ftmp, delete_pwuser_cb, &helper);

	if(helper.found == false){
		fprintf(stderr, "Warning: User %s not found in password file.\n", username);
		return 1;
	}
	return rc;
}



/* ======================================================================
 * Update a plain text password file to use hashes
 * ====================================================================== */
static int update_file_cb(FILE *fptr, FILE *ftmp, const char *username, const char *password, const char *line, struct cb_helper *helper)
{
	return output_new_password(ftmp, username, password);
}

int update_file(FILE *fptr, FILE *ftmp)
{
	return pwfile_iterate(fptr, ftmp, update_file_cb, NULL);
}


/* ======================================================================
 * Update an existing user password / create a new password
 * ====================================================================== */
static int update_pwuser_cb(FILE *fptr, FILE *ftmp, const char *username, const char *password, const char *line, struct cb_helper *helper)
{
	int rc = 0;

	if(strcmp(username, helper->username)){
		/* If this isn't the matching user, then writing out the exiting line */
		fprintf(ftmp, "%s", line);
	}else{
		/* Write out a new line for our matching username */
		helper->found = true;
		rc = output_new_password(ftmp, username, helper->password);
	}
	return rc;
}

int update_pwuser(FILE *fptr, FILE *ftmp, const char *username, const char *password)
{
	struct cb_helper helper;
	int rc;

	memset(&helper, 0, sizeof(helper));
	helper.username = username;
	helper.password = password;
	rc = pwfile_iterate(fptr, ftmp, update_pwuser_cb, &helper);

	if(helper.found){
		return rc;
	}else{
		return output_new_password(ftmp, username, password);
	}
}


int gets_quiet(char *s, int len)
{
#ifdef WIN32
	HANDLE h;
	DWORD con_orig, con_quiet = 0;
	DWORD read_len = 0;

	memset(s, 0, len);
	h  = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(h, &con_orig);
	con_quiet = con_orig;
	con_quiet &= ~ENABLE_ECHO_INPUT;
	con_quiet |= ENABLE_LINE_INPUT;
	SetConsoleMode(h, con_quiet);
	if(!ReadConsole(h, s, len, &read_len, NULL)){
		SetConsoleMode(h, con_orig);
		return 1;
	}
	while(s[strlen(s)-1] == 10 || s[strlen(s)-1] == 13){
		s[strlen(s)-1] = 0;
	}
	if(strlen(s) == 0){
		return 1;
	}
	SetConsoleMode(h, con_orig);

	return 0;
#else
	struct termios ts_quiet, ts_orig;
	char *rs;

	memset(s, 0, len);
	tcgetattr(0, &ts_orig);
	ts_quiet = ts_orig;
	ts_quiet.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(0, TCSANOW, &ts_quiet);

	rs = fgets(s, len, stdin);
	tcsetattr(0, TCSANOW, &ts_orig);

	if(!rs){
		return 1;
	}else{
		while(s[strlen(s)-1] == 10 || s[strlen(s)-1] == 13){
			s[strlen(s)-1] = 0;
		}
		if(strlen(s) == 0){
			return 1;
		}
	}
	return 0;
#endif
}

int get_password(char *password, size_t len)
{
	char pw1[MAX_BUFFER_LEN], pw2[MAX_BUFFER_LEN];
	size_t minLen;
	minLen = len < MAX_BUFFER_LEN ? len : MAX_BUFFER_LEN;

	printf("Password: ");
	fflush(stdout);
	if(gets_quiet(pw1, minLen)){
		fprintf(stderr, "Error: Empty password.\n");
		return 1;
	}
	printf("\n");

	printf("Reenter password: ");
	fflush(stdout);
	if(gets_quiet(pw2, minLen)){
		fprintf(stderr, "Error: Empty password.\n");
		return 1;
	}
	printf("\n");

	if(strcmp(pw1, pw2)){
		fprintf(stderr, "Error: Passwords do not match.\n");
		return 1;
	}

	strncpy(password, pw1, minLen);
	return 0;
}

int copy_contents(FILE *src, FILE *dest)
{
	char buf[MAX_BUFFER_LEN];
	size_t len;

	rewind(src);
	rewind(dest);
	
#ifdef WIN32
	_chsize(fileno(dest), 0);
#else
	if(ftruncate(fileno(dest), 0)) return 1;
#endif

	while(!feof(src)){
		len = fread(buf, 1, MAX_BUFFER_LEN, src);
		if(len > 0){
			if(fwrite(buf, 1, len, dest) != len){
				return 1;
			}
		}else{
			return !feof(src);
		}
	}
	return 0;
}

int create_backup(const char *backup_file, FILE *fptr)
{
	FILE *fbackup;

	fbackup = fopen(backup_file, "wt");
	if(!fbackup){
		fprintf(stderr, "Error creating backup password file \"%s\", not continuing.\n", backup_file);
		return 1;
	}
	if(copy_contents(fptr, fbackup)){
		fprintf(stderr, "Error copying data to backup password file \"%s\", not continuing.\n", backup_file);
		fclose(fbackup);
		return 1;
	}
	fclose(fbackup);
	rewind(fptr);
	return 0;
}
void handle_sigint(int signal)
{
#ifndef WIN32
	struct termios ts;

	tcgetattr(0, &ts);
	ts.c_lflag |= ECHO | ICANON;
	tcsetattr(0, TCSANOW, &ts);
#endif

	UNUSED(signal);

	exit(0);
}

int main(int argc, char *argv[])
{
	char *password_file_tmp = NULL;
	char *password_file = NULL;
	char *username = NULL;
	char *password_cmd = NULL;
	bool batch_mode = false;
	bool create_new = false;
	bool delete_user = false;
	FILE *fptr, *ftmp;
	char password[MAX_BUFFER_LEN];
	int rc;
	bool do_update_file = false;
	char *backup_file;

	signal(SIGINT, handle_sigint);
	signal(SIGTERM, handle_sigint);

#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_API_COMPAT < 0x10100000L
	OpenSSL_add_all_digests();
#else
	OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS \
			| OPENSSL_INIT_ADD_ALL_DIGESTS \
			| OPENSSL_INIT_LOAD_CONFIG, NULL);
#endif

	if(argc == 1){
		print_usage();
		return 1;
	}

	if(!strcmp(argv[1], "-c")){
		create_new = true;
		if(argc == 4){
			password_file_tmp = argv[2];
			username = argv[3];
		}else if(argc == 6){
			if(!strcmp(argv[2], "-b")){
				batch_mode = true;
				password_file_tmp = argv[3];
				username = argv[4];
				password_cmd = argv[5];
			}else{
				fprintf(stderr, "Error: Incorrect number of arguments.\n");
				return 1;
			}
		}else{
			fprintf(stderr, "Error: -c argument given but password file or username missing.\n");
			return 1;
		}
	}else if(!strcmp(argv[1], "-D")){
		delete_user = true;
		if(argc != 4){
			fprintf(stderr, "Error: -D argument given but password file or username missing.\n");
			return 1;
		}else{
			password_file_tmp = argv[2];
			username = argv[3];
		}
	}else if(!strcmp(argv[1], "-b")){
		batch_mode = true;
		if(argc != 5){
			fprintf(stderr, "Error: -b argument given but password file, username or password missing.\n");
			return 1;
		}else{
			password_file_tmp = argv[2];
			username = argv[3];
			password_cmd = argv[4];
		}
	}else if(!strcmp(argv[1], "-U")){
		if(argc != 3){
			fprintf(stderr, "Error: -U argument given but password file missing.\n");
			return 1;
		}else{
			do_update_file = true;
			password_file_tmp = argv[2];
		}
	}else if(argc == 3){
		password_file_tmp = argv[1];
		username = argv[2];
	}else{
		print_usage();
		return 1;
	}
	if(username && strlen(username) > 65535){
		fprintf(stderr, "Error: Username must be less than 65536 characters long.\n");
		return 1;
	}
	if(password_cmd && strlen(password_cmd) > 65535){
		fprintf(stderr, "Error: Password must be less than 65536 characters long.\n");
		return 1;
	}

#ifdef WIN32
	password_file = _fullpath(NULL, password_file_tmp, 0);
	if(!password_file){
		fprintf(stderr, "Error getting full path for password file.\n");
		return 1;
	}
#else
	password_file = realpath(password_file_tmp, NULL);
	if(!password_file){
		if(errno == ENOENT){
			password_file = strdup(password_file_tmp);
			if(!password_file){
				fprintf(stderr, "Error: Out of memory.\n");
				return 1;
			}
		}else{
			fprintf(stderr, "Error reading password file: %s\n", strerror(errno));
			return 1;
		}
	}
#endif

	if(create_new){
		if(batch_mode == false){
			rc = get_password(password, MAX_BUFFER_LEN);
			if(rc){
				free(password_file);
				return rc;
			}
			password_cmd = password;
		}
		fptr = fopen(password_file, "wt");
		if(!fptr){
			fprintf(stderr, "Error: Unable to open file %s for writing. %s.\n", password_file, strerror(errno));
			free(password_file);
			return 1;
		}
		free(password_file);
		rc = output_new_password(fptr, username, password_cmd);
		fclose(fptr);
		return rc;
	}else{
		fptr = fopen(password_file, "r+t");
		if(!fptr){
			fprintf(stderr, "Error: Unable to open password file %s. %s.\n", password_file, strerror(errno));
			free(password_file);
			return 1;
		}

		backup_file = malloc(strlen(password_file)+5);
		if(!backup_file){
			fprintf(stderr, "Error: Out of memory.\n");
			free(password_file);
			return 1;
		}
		snprintf(backup_file, strlen(password_file)+5, "%s.tmp", password_file);
		free(password_file);
		password_file = NULL;

		if(create_backup(backup_file, fptr)){
			fclose(fptr);
			free(backup_file);
			return 1;
		}

		ftmp = mpw_tmpfile();
		if(!ftmp){
			fprintf(stderr, "Error: Unable to open temporary file. %s.\n", strerror(errno));
			fclose(fptr);
			free(backup_file);
			return 1;
		}
		if(delete_user){
			rc = delete_pwuser(fptr, ftmp, username);
		}else if(do_update_file){
			rc = update_file(fptr, ftmp);
		}else{
			if(batch_mode){
				/* Update password for individual user */
				rc = update_pwuser(fptr, ftmp, username, password_cmd);
			}else{
				rc = get_password(password, MAX_BUFFER_LEN);
				if(rc){
					fclose(fptr);
					fclose(ftmp);
					unlink(backup_file);
					free(backup_file);
					return rc;
				}
				/* Update password for individual user */
				rc = update_pwuser(fptr, ftmp, username, password);
			}
		}
		if(rc){
			fclose(fptr);
			fclose(ftmp);
			unlink(backup_file);
			free(backup_file);
			return rc;
		}

		if(copy_contents(ftmp, fptr)){
			fclose(fptr);
			fclose(ftmp);
			fprintf(stderr, "Error occurred updating password file.\n");
			fprintf(stderr, "Password file may be corrupt, check the backup file: %s.\n", backup_file);
			free(backup_file);
			return 1;
		}
		fclose(fptr);
		fclose(ftmp);

		/* Everything was ok so backup no longer needed. May contain old
		 * passwords so shouldn't be kept around. */
		unlink(backup_file);
		free(backup_file);
	}

	return 0;
}
