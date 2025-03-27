.PHONY: all
all: server client
server: Server.cc
	g++ -o $@ $^ -std=c++14 -ljsoncpp
client: Client.cc
	g++ -o $@ $^ -std=c++14 -ljsoncpp

.PHONY: clean
clean:
	rm -rf server client