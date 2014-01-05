test1: main.o
	mcppc main.o -o test1 -l amiga -l ray:lib_vecmath/maxon881/vecmath.lib

main.o: main.cpp ray:keyfile/codekey.h ray:keyfile/keyfile_lib.h
	mcppc -c main.cpp

