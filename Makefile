all: client server

.PHONY: client server clean

client:
	cd client && make

server:
	cd server && make

clean:
	cd client && make clean
	cd server && make clean
