FROM phusion/baseimage:0.9.17
MAINTAINER Nikolay Volosatov <bamx23@gmail.com>

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
    apt-get -y install python-software-properties software-properties-common && \
    add-apt-repository ppa:yandex-load/main && \
    apt-get update && \
    apt-get -y install yandex-load-tank-base && \
    apt-get -y remove python-software-properties software-properties-common && \
    apt-get -y autoremove && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

COPY start.sh /
RUN chmod +x start.sh

COPY key.prv /root/.ssh/id_rsa

VOLUME ["/data", "/config"]

ENV TANK_LOAD="load.ini"

CMD ["/sbin/my_init", "--", "/start.sh"]
