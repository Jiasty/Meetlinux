bin=time
src=perpetual.c

$(bin):$(src)
		@gcc -o $@ $^ -g
		@echo "complier $(src) to $(bin)..."
.PHONY:clean
clean:
		@rm -f $(bin)
		@echo "clean..."



#mytest.exe:test.o
#	gcc test.o -o mytest.exe
#test.o:test.s
#	gcc -c test.s -o test.o
#test.s:test.i
#	gcc -S test.i -o test.s
#test.i:test.c
#	gcc -E test.c -o test.i
##完整的过程
#
#.PHONY:clean
#clean:
#	rm -f test.i test.s test.o mytest.exe
