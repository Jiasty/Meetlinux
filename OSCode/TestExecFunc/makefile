.PHONY:all
all:testExec myproc

testExec:testExec.c
	gcc -o $@ $^ -std=c99
myproc:myproc.cc
	g++ -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm -f testExec