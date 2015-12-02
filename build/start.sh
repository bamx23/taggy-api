#!/bin/bash
cd data
service nginx start
chmod +x ${FCGI_MAIN}
spawn-fcgi -n -p 8080 ${FCGI_MAIN}
