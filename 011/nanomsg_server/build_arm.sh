#!/bin/bash
gcc nanomsg_server.c -o nanomsg_server -I /home/book/git_clone/nanomsg/build/nanomsg_x86_lib/include -L /home/book/git_clone/nanomsg/build/nanomsg_x86_lib/lib -lnanomsg