#!/bin/bash

echo "Build" $(date) > "$FCGI_BUILD_LOG"

cd src
g++ -c -Wall -O2 --std=c++11 "$FCGI_SRC" -o /tmp/main.o >> "$FCGI_BUILD_LOG" 2>&1 && \
g++ /tmp/main.o $FCGI_LIBS -o /var/main.fcgi >> "$FCGI_BUILD_LOG" 2>&1 || \
( sleep 10 && exit 1 )

rm -rf /tmp/*
cd /

cp -f /etc/nginx/nginx.conf.pre /etc/nginx/nginx.conf
sed -i -e "s|NGINX_ERROR_LOG|${NGINX_ERROR_LOG}|g" /etc/nginx/nginx.conf
sed -i -e "s|NGINX_ACCESS_LOG|${NGINX_ACCESS_LOG}|g" /etc/nginx/nginx.conf

service nginx start
spawn-fcgi -n -p 8080 /var/main.fcgi
