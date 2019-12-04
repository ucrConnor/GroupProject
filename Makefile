
all: scripts passwd
	gcc crash.c -o ./bin/crash
	gcc crasher.c -o ./bin/crasher
	gcc exploit.c -o ./bin/exploit

clean:
	rm  crash
	rm  crasher
	rm exploit

passwd:
	gcc filegen.c -o ./bin/filegen
	./bin/filegen

scripts:
	rm -rf bin
	mkdir bin
	./init-files.sh