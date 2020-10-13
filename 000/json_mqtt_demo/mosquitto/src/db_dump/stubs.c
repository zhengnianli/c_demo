#include <stdlib.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto_internal.h"

struct mosquitto *context__init(struct mosquitto_db *db, mosq_sock_t sock)
{
	return NULL;
}

int db__message_store(struct mosquitto_db *db, const struct mosquitto *source, uint16_t source_mid, char *topic, int qos, uint32_t payloadlen, mosquitto__payload_uhpa *payload, int retain, struct mosquitto_msg_store **stored, uint32_t message_expiry_interval, mosquitto_property *properties, dbid_t store_id, enum mosquitto_msg_origin origin)
{
    return 0;
}

void db__msg_store_ref_inc(struct mosquitto_msg_store *store)
{
}

int handle__packet(struct mosquitto_db *db, struct mosquitto *context)
{
	return 0;
}

int log__printf(struct mosquitto *mosq, int level, const char *fmt, ...)
{
	return 0;
}


void *mosquitto__calloc(size_t nmemb, size_t len)
{
	return calloc(nmemb, len);
}

void mosquitto__free(void *p)
{
	free(p);
}

FILE *mosquitto__fopen(const char *path, const char *mode, bool restrict_read)
{
	return NULL;
}

enum mosquitto_client_state mosquitto__get_state(struct mosquitto *mosq)
{
	return mosq_cs_new;
}

void *mosquitto__malloc(size_t len)
{
	return malloc(len);
}

char *mosquitto__strdup(const char *s)
{
	return strdup(s);
}

ssize_t net__read(struct mosquitto *mosq, void *buf, size_t count)
{
	return 0;
}

ssize_t net__write(struct mosquitto *mosq, void *buf, size_t count)
{
	return 0;
}

int retain__store(struct mosquitto_db *db, const char *topic, struct mosquitto_msg_store *stored, char **split_topics)
{
	return 0;
}

int sub__add(struct mosquitto_db *db, struct mosquitto *context, const char *sub, int qos, uint32_t identifier, int options, struct mosquitto__subhier **root)
{
	return 0;
}

int sub__messages_queue(struct mosquitto_db *db, const char *source_id, const char *topic, int qos, int retain, struct mosquitto_msg_store **stored)
{
	return 0;
}
