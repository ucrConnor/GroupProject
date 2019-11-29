
all: scripts
	gcc crash.c -o ./bin/crash
	gcc crasher.c -o ./bin/crasher
	gcc exploit.c -o ./bin/exploit

clean:
	rm  crash
	rm  crasher
	rm exploit

scripts:
	rm -rf bin
	mkdir bin
	./init-files.sh