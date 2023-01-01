CC = gcc           # define gcc as compiler
CFLAGS = -g -Wall  # define compiler flags, -g is debug, -Wall turns on all warngings

default: shell      # name of our program

# shell is dependent on my_shell.o
# if my_shell.o has a date newer than shell, invoke rule
shell: my_shell.c
	$(CC) $(CFLAGS) my_shell.c -o shell
  
# enter $ make clean to delete part and all .o files
clean:
	rm -f shell *.o
