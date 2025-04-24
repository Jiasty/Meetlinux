.PHONY: all
all: tcpserver tcpclient

tcpserver:tcpServer.cc
	g++ -o $@ $^ -std=c++14 -pthread
tcpclient:tcpClient.cc
	g++ -o $@ $^ -std=c++14 -pthread
.PHONY:clean
clean:
	rm -rf tcpserver tcpclient