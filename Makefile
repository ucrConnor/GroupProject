
all:
	gcc crash.c -o crash
	gcc crasher.c -o crasher
	gcc exploit.c -o exploit

clean:
	rm  crash
	rm  crasher
	rm exploit