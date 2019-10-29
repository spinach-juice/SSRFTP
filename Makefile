all: client server

.PHONY: common client server clean

common:
	cd common && $(MAKE)

client: common
	cd client && $(MAKE)

server: common
	cd server && $(MAKE)

clean:
	cd client && $(MAKE) clean
	cd server && $(MAKE) clean
