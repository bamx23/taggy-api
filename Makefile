HOST=$(shell docker-machine ip default 2>/dev/null || echo localhost)
URL=http://$(HOST):8068/api/v1/currency/

all: up-containers
	
build-images:
	docker-compose build

up-containers: build-images
	docker-compose up -d --force-recreate

restart:
	docker-compose restart

debug:
	docker run --name fastcgipp-debug -i -t -v $(shell pwd)/src:/src $(shell docker images | grep taggyapi_latest-rates | awk '{print $$1}') /bin/bash && docker rm fastcgipp-debug

test-curl:
	curl $(URL) 2>/dev/null | grep BYR -A 1
	curl -H 'Content-Type: application/json' -X POST -d '{"currency":[{"name":"BYR", "value":3200}]}' $(URL)
	curl $(URL) 2>/dev/null | grep BYR -A 1
	curl -H 'Content-Type: application/json' -X POST -d '{"currency":[{"name":"BYR", "value":18200}]}' $(URL)
	curl $(URL) 2>/dev/null | grep BYR -A 1
