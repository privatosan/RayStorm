png-handler: png.o pngerror.o pngio.o pngmem.o pngrcb.o pngread.o pngrtran.o pngrutil.o pngtrans.o pngwrite.o\
 pngwtran.o pngwutil.o adler32.o compress.o crc32.o deflate.o gzio.o infblock.o infcodes.o inffast.o inflate.o\
 inftrees.o infutil.o trees.o uncompr.o zutil.o
	mcppc png.o pngerror.o pngio.o pngmem.o pngrcb.o pngread.o pngrtran.o pngrutil.o pngtrans.o pngwrite.o\
 pngwtran.o pngwutil.o png-handler.o adler32.o compress.o crc32.o deflate.o gzio.o infblock.o infcodes.o\
 inffast.o inflate.o inftrees.o infutil.o trees.o uncompr.o zutil.o -o png-handler

png.o: PNG.C png.h pngconf.h
	mcppc -c PNG.C

pngerror.o: png.h pngconf.h PNGERROR.C
	mcppc -c PNGERROR.C

pngio.o: png.h pngconf.h PNGIO.C
	mcppc -c PNGIO.C

pngmem.o: png.h pngconf.h PNGMEM.C
	mcppc -c PNGMEM.C

pngrcb.o: png.h pngconf.h PNGRCB.C
	mcppc -c PNGRCB.C

pngread.o: png.h pngconf.h PNGREAD.C
	mcppc -c PNGREAD.C

pngrtran.o: png.h pngconf.h PNGRTRAN.C
	mcppc -c PNGRTRAN.C

pngrutil.o: png.h pngconf.h PNGRUTIL.C
	mcppc -c PNGRUTIL.C

pngtrans.o: png.h pngconf.h PNGTRANS.C
	mcppc -c PNGTRANS.C

pngwrite.o: png.h pngconf.h PNGWRITE.C
	mcppc -c PNGWRITE.C

pngwtran.o: png.h pngconf.h PNGWTRAN.C
	mcppc -c PNGWTRAN.C

pngwutil.o: png.h pngconf.h PNGWUTIL.C
	mcppc -c PNGWUTIL.C

png-handler.o: png.h pngconf.h
	

adler32.o: zlib/adler32.c zlib/zconf.h zlib/zlib.h
	mcppc -c zlib/adler32.c

compress.o: zlib/compress.c zlib/zconf.h zlib/zlib.h
	mcppc -c zlib/compress.c

crc32.o: zlib/crc32.c zlib/zconf.h zlib/zlib.h
	mcppc -c zlib/crc32.c

deflate.o: zlib/deflate.c zlib/deflate.h zlib/zconf.h zlib/zlib.h zlib/zutil.h
	mcppc -c zlib/deflate.c

gzio.o: zlib/gzio.c zlib/zconf.h zlib/zlib.h zlib/zutil.h
	mcppc -c zlib/gzio.c

infblock.o: zlib/infblock.c zlib/infblock.h zlib/infcodes.h zlib/inftrees.h zlib/infutil.h zlib/zconf.h\
 zlib/zlib.h zlib/zutil.h
	mcppc -c zlib/infblock.c

infcodes.o: zlib/infblock.h zlib/infcodes.c zlib/infcodes.h zlib/inffast.h zlib/inftrees.h zlib/infutil.h\
 zlib/zconf.h zlib/zlib.h zlib/zutil.h
	mcppc -c zlib/infcodes.c

inffast.o: zlib/infblock.h zlib/infcodes.h zlib/inffast.c zlib/inffast.h zlib/inftrees.h zlib/infutil.h\
 zlib/zconf.h zlib/zlib.h zlib/zutil.h
	mcppc -c zlib/inffast.c

inflate.o: zlib/infblock.h zlib/inflate.c zlib/zconf.h zlib/zlib.h zlib/zutil.h
	mcppc -c zlib/inflate.c

inftrees.o: zlib/inftrees.c zlib/inftrees.h zlib/zconf.h zlib/zlib.h zlib/zutil.h
	mcppc -c zlib/inftrees.c

infutil.o: zlib/infblock.h zlib/infcodes.h zlib/inftrees.h zlib/infutil.c zlib/infutil.h zlib/zconf.h\
 zlib/zlib.h zlib/zutil.h
	mcppc -c zlib/infutil.c

trees.o: zlib/deflate.h zlib/trees.c zlib/zconf.h zlib/zlib.h zlib/zutil.h
	mcppc -c zlib/trees.c

uncompr.o: zlib/uncompr.c zlib/zconf.h zlib/zlib.h
	mcppc -c zlib/uncompr.c

zutil.o: zlib/zconf.h zlib/zlib.h zlib/zutil.c zlib/zutil.h
	mcppc -c zlib/zutil.c

