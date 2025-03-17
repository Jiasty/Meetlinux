.PHONY:all
all:udpserver udpclient

udpserver: UdpServer.cc
	g++ -o $@ $^ -std=c++14 -pthread
udpclient: UdpClient.cc
	g++ -o $@ $^ -std=c++14 -pthread

.PHONY:clean
clean:
	rm -rf udpserver udpclient