all: key.prv build run

key.prv:
	cp ../keys/shared.key key.prv

build:
	docker build -t bamx23/taggy-tank .

run:
	docker run --rm -it --link taggy-balancer:balancer --link taggy-latest-rates:latest --link taggy-history-rates:history -v $(shell pwd)/data:/data -v $(shell pwd)/config:/config bamx23/taggy-tank
