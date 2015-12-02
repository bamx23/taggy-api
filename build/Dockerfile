FROM python:2.7

ENV FCGI_MAIN="app.py"

RUN apt-get update && \
    apt-get install -y spawn-fcgi nginx-full
    rm -rf /var/lib/apt/lists/*

RUN pip install flup web.py

COPY start.sh /
COPY nginx.conf /etc/nginx/nginx.conf

RUN chmod +x start.sh

VOLUME ["data", "/var/www/static", "/var/www/html", "/var/log/nginx"]

CMD ["bash", "start.sh"]
