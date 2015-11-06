all: up-containers
	
build-images:
	docker-compose build

up-containers: build-images
	docker-compose up -d --force-recreate

restart:
	docker-compose restart

debug:
	docker run --name fastcgipp-debug -i -t -v $(shell pwd)/src:/data $(shell docker images | grep fastcgipp | awk '{print $$1}') /bin/bash && docker rm fastcgipp-debug
