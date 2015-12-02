FROM ubuntu

ENV FCGI_LIBS="-lfastcgipp -lboost_thread -lboost_system -lboost_date_time -pthread" \
    FCGI_MAIN="main.fcgi"

RUN apt-get update && \
    apt-get install -y git nginx-full automake libtool doxygen build-essential libboost-dev libboost-date-time-dev libboost-system-dev libboost-thread-dev libboost-iostreams-dev libfcgi-dev spawn-fcgi && \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/Wassasin/fastcgipp.git && \
    cd fastcgipp && \
    ./bootstrap && \
    ./configure && \
    make && \
    make install && \
    cd .. && \
    rm -rf fastcgipp

COPY start.sh /
COPY nginx.conf /etc/nginx/nginx.conf

RUN ln -s /usr/local/lib/libfastcgipp.so.2.0.0 /usr/lib/libfastcgipp.so.2
RUN chmod +x start.sh

CMD ["bash", "start.sh"]

VOLUME ["data", "/var/www/static", "/var/www/html", "/var/log/fastcgipp", "/var/log/nginx"]
