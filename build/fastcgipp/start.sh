#!/bin/bash
cd data
make
spawn-fcgi -n -p 8023 main.fcgi
