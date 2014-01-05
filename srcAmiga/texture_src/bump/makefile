mcpp:work/Octree/textures/bump.itx: bump.o noise.o
	mcppc bump.o noise.o -o mcpp:work/Octree/textures/bump.itx -l amiga

bump.o: bump.c
	mcppc -c bump.c

noise.o: /noise.c
	mcppc -c /noise.c

