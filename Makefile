build:
	docker build -t convertjs:latest .

genereate: build
	docker cp app built

run: build
	docker run --rm -ti --name convertjs convertjs:latest

attach:
	docker exec -ti --name convertjs bash
