.PHONY: all
all: server

server: Server.cc
	g++ -o $@ $^ -std=c++14

.PHONY:clean
clean:
	rm -rf server;