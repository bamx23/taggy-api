FROM phusion/baseimage:0.9.17
MAINTAINER Nikolay Volosatov <bamx23@gmail.com>

RUN apt-get update && \
    apt-get install -y git nginx-light automake libtool doxygen build-essential libboost-dev libboost-date-time-dev libboost-system-dev libboost-thread-dev libboost-iostreams-dev libfcgi-dev spawn-fcgi && \
    git clone https://github.com/BamX/fastcgipp.git && \
    cd fastcgipp && \
    ./bootstrap && \
    ./configure && \
    make && \
    make install && \
    cd .. && \
    apt-get -y purge git && \
    apt-get -y autoremove && \
    rm -rf fastcgipp /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN ln -s /usr/local/lib/libfastcgipp.so.2.0.0 /usr/lib/libfastcgipp.so.2

COPY nginx.conf /etc/nginx/nginx.conf.pre

COPY start.sh /
RUN chmod +x start.sh

ENV FCGI_LIBS="-lfastcgipp -lboost_thread -lboost_system -lboost_date_time -pthread" \
    FCGI_SRC="main.cpp" \
    FCGI_BUILD_LOG="/var/log/fcgi/build.log" \
    FCGI_DEBUG_LOG="/var/log/fcgi/debug.log" \
    NGINX_ACCESS_LOG="/var/log/fcgi/access.log" \
    NGINX_ERROR_LOG="/var/log/fcgi/error.log"

CMD ["/sbin/my_init", "--", "/start.sh"]

VOLUME ["src", "data", "/var/log/fcgi"]
EXPOSE 80
