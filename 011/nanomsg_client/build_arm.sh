#!/bin/bash
gcc nanomsg_client.c -o nanomsg_client -I /home/book/git_clone/nanomsg/build/nanomsg_x86_lib/include -L /home/book/git_clone/nanomsg/build/nanomsg_x86_lib/lib -lnanomsg