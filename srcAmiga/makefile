raystorm: itemlist.o raystorm.o csgstack.o
	mcppc itemlist.o raystorm.o csgstack.o -o raystorm -l amiga

itemlist.o: itemlist.cpp itemlist.h
	mcppc -c itemlist.cpp

raystorm.o: csgstack.h itemlist.h ray:keyfile/keyfile_lib.h raystorm.cpp
	mcppc -c raystorm.cpp

csgstack.o: csgstack.cpp csgstack.h
	mcppc -c csgstack.cpp

