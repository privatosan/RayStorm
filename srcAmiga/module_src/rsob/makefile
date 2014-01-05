ray:modules/object/rsob: rsob.o iff_util.o list.o obj_lib.o
	mcppc rsob.o iff_util.o list.o obj_lib.o -o ray:modules/object/rsob -gs -l amiga

rsob.o: rsob.cpp
	mcppc -c rsob.cpp

iff_util.o: mod:iff_util.cpp mod:iff_util.h
	mcppc -c mod:iff_util.cpp

list.o: mod:list.cpp mod:list.h mod:typedefs.h
	mcppc -c mod:list.cpp

obj_lib.o: /objlib.cpp rmod:objlib.h
	mcppc -c /objlib.cpp

