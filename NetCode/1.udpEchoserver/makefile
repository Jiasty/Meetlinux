.PHONY:all
all:udpserver udpclient

udpserver: UdpServer.cc
	g++ -o $@ $^ -std=c++14
udpclient: UdpClient.cc
	g++ -o $@ $^ -std=c++14

.PHONY:clean
clean:
	rm -rf udpserver udpclient