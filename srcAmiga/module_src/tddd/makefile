//modules/object/tddd: tddd.o vecmath.o lowlevel.o
	mcppc tddd.o vecmath.o lowlevel.o -o //modules/object/tddd -gs -l amiga

tddd.o: tddd.cpp tddd.h
	mcppc -c tddd.cpp

vecmath.o: rsi:stats.h rsi:typedefs.h rsi:vecmath.cpp rsi:vecmath.h
	mcppc -c rsi:vecmath.cpp

lowlevel.o: rsi:lowlevel.asm
	stormc:PhxAss/bin/phxass rsi:lowlevel.asm to lowlevel.o noexe

