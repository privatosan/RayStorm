Register: register.o interface.o
	mcppc register.o interface.o -o Register -l amiga

register.o: interface.h register.cpp
	mcppc -c register.cpp

interface.o: interface.c interface.h
	mcppc -c interface.c

