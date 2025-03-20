.PHONY: all
all: tcpserver tcpclient

tcpserver:tcpServer.cc
	g++ -o $@ $^ -std=c++14
tcpclient:tcpClient.cc
	g++ -o $@ $^ -std=c++14
.PHONY:clean
clean:
	rm -rf tcpserver tcpclient