all:
	docker-compose build
	docker-compose up -d --force-recreate

restart:
	docker-compose restart
