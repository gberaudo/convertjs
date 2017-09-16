build:
	docker build -t convertjs:latest .

run: build
	docker run --rm -ti --name convertjs convertjs:latest

attach:
	docker exec -ti --name convertjs bash
