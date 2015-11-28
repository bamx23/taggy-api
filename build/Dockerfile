FROM ubuntu

ENV FCGI_LIBS="-lfastcgipp -lboost_thread -lboost_system -pthread -lboost_date_time" \
    FCGI_MAIN="main.fcgi"

RUN apt-get update && \
    apt-get install -y wget build-essential libboost-dev libboost-date-time-dev libboost-system-dev libboost-thread-dev libboost-iostreams-dev libfcgi-dev spawn-fcgi && \
    wget http://download.savannah.nongnu.org/releases/fastcgipp/fastcgi++-2.1.tar.bz2 && \
    tar -xvjf fastcgi++-2.1.tar.bz2 && \
    rm fastcgi++-2.1.tar.bz2 && \
    cd fastcgi++-2.1 && \
    ./configure && \
    make && \
    make install && \
    cd .. && \
    rm -rf fastcgi++-2.1 && \
    apt-get install -y nginx-full && \
    rm -rf /var/lib/apt/lists/*

COPY start.sh /
COPY nginx.conf /etc/nginx/nginx.conf

RUN ln -s /usr/local/lib/libfastcgipp.so.2.0.0 /usr/lib/libfastcgipp.so.2
RUN chmod +x start.sh

CMD ["bash", "start.sh"]

VOLUME ["data", "/var/www/static", "/var/www/html", "/var/log/fastcgipp", "/var/log/nginx"]
