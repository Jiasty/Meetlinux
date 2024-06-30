bin=myshell
src=myshell.c

$(bin):$(src)
		@gcc -o $@ $^ -g
		@echo "complier $(src) to $(bin)..."
.PHONY:clean
clean:
		@rm -f $(bin)
		@echo "clean..."

