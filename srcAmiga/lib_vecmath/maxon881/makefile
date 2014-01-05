a.out: vecmath.o asm_vecmath.o
	mcppc vecmath.o asm_vecmath.o -o a.out -gs -l amiga

vecmath.o: /vecmath.cpp /vecmath.h
	mcppc -c /vecmath.cpp

asm_vecmath.o:
	stormc:phxass/bin/phxass set "__MAXON__=1" /asm_vecmath.asm to asm_vecmath.o

