#!/bin/bash -e
/usr/bin/clang -std=c17 -Wall -Wextra -pedantic -Werror -D_GNU_SOURCE test_client.c -o test_client
