.PHONY:all
all:mykill testsig

mykill:mykill.cc
	g++ -o $@ $^ -std=c++11
testsig:testsig.cc
	g++ -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm -f mykill testsig