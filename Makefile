C = gcc
#Compiler flags
CFLAGS = -g
#When given no options, create an executable called ass2
all: oss user
oss: oss.c
	gcc  -g -o  oss oss.c
user: user.c
	gcc  -g -o  user user.c
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
.c.o:
	$(CC) $(CFLAGS) -c $<

#Remove all generated object files
.PHONY: clean
clean:
	rm -rf *o oss user x.log child.log

