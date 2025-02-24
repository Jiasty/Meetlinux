.PHONY:all
all:mykill testsig testCore

mykill:mykill.cc
	g++ -o $@ $^ -std=c++11
testsig:testsig.cc
	g++ -o $@ $^ -std=c++11
testCore:testCore.cc
	g++ -o $@ $^ -std=c++11 -g #-g使其可以调试
.PHONY:clean
clean:
	rm -f mykill testsig testCore