
all: passwd
	gcc crash.c -o ./bin/crash
	gcc crasher.c -o ./bin/crasher
	gcc exploit.c -o ./bin/exploit
	gcc -D_FILE_OFFSET_BITS=64 -o ./bin/fuse_op fuse_op.c -Wall -std=gnu99 `pkg-config fuse --cflags --libs`

clean:
	fusermount -u fuse_mount
	rm -rf fuse_mount

passwd: scripts
	gcc filegen.c -o ./bin/filegen
	./bin/filegen

scripts:
	rm -rf bin
	mkdir bin
	./init-files.sh
	