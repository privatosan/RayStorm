//modules/object/raw: raw.o lowlevel.o vecmath.o
	mcppc raw.o lowlevel.o vecmath.o -o //modules/object/raw -l amiga

raw.o: RAW.CPP raw.h
	mcppc -c RAW.CPP

lowlevel.o:
	acom:oma rsi:lowlevel.asm -m -o lowlevel.o

vecmath.o: rsi:typedefs.h rsi:vecmath.cpp rsi:vecmath.h
	mcppc -c rsi:vecmath.cpp

