FROM ubuntu

RUN apt-get update && \
    apt-get install -y build-essential git debhelper autoconf-archive automake1.9 pkg-config autotools-dev libboost-dev libboost-thread-dev libfcgi-dev libxml2-dev libboost-regex-dev libtool libssl-dev && \
    git clone https://github.com/dieash/Fastcgi-Daemon.git && \
    cd Fastcgi-Daemon && \
    dpkg-buildpackage -rfakeroot && \
    cd .. && \
    sudo dpkg -i ./libfastcgi-daemon2-dev_2.10-26_amd64.deb \
        ./libfastcgi-daemon2_2.10-26_amd64.deb \
        ./fastcgi-daemon2-init_2.10-26_amd64.deb \
        ./fastcgi-daemon2_2.10-26_amd64.deb \
        ./libfastcgi2-syslog_2.10-26_amd64.deb && \
    rm -rf Fastcgi-Daemon *.deb fastcgi-daemon2* && \
    apt-get autoremove --purge -y debhelper automake1.9 pkg-config autotools-dev autoconf-archive && \
    rm -rf /var/lib/apt/lists/*

VOLUME ["data", "/tmp/fastcgi"]

ENV CONF_FILE=fastcgi.conf
ADD start.sh /

RUN chmod +x start.sh

CMD ["bash", "start.sh"]
