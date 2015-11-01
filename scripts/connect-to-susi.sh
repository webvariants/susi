#!/bin/bash

ADDR=$1
PORT=$2

if [ x"$ADDR" = x"" ]; then
    ADDR="localhost"
fi

if [ x"$PORT" = x"" ]; then
    PORT="4000"
fi

if [ ! -f key.pem -o ! -f cert.pem ]; then
    openssl req -batch -nodes -x509 -newkey rsa:2048 -days 36500\
        -keyout key.pem \
        -out cert.pem 2>/dev/null
fi

ledit | ncat --ssl-key key.pem --ssl-cert cert.pem $ADDR $PORT

exit $?
