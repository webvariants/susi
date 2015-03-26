#!/bin/bash

echo '{"type":"publish","data":{"topic":"chat-channel","payload":"debug text"}}' | nc localhost 4000

exit 0;
