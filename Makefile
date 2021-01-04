cc := gcc
exe := fs
obj := main.c FileNode.c FileNode.h

.PHONY: clean
build: $(obj)
	$(cc) -o FileNode.o -c FileNode.c
	$(cc) -o main.o -c main.c `pkg-config fuse3 --cflags --libs`
	$(cc) -Wall -D_GNU_SOURCE -o $(exe) main.o FileNode.o `pkg-config fuse3 --cflags --libs`
	echo 'To Mount: ./fs -f [mount point]'

clean:
	rm fs
