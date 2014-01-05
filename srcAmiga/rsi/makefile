raystorm.library: raystorm.library.o actor.o box.o camera.o cone.o csg.o cylinder.o brush.o fileutil.o flare.o\
 htexture.o light.o list.o object.o objhand.o octree.o param.o plane.o polysolv.o rsi.o sor.o sphere.o\
 star.o surface.o texture.o triangle.o voxel.o world.o vecmath.o noise.o
	mcppc raystorm.library.o actor.o box.o camera.o cone.o csg.o cylinder.o brush.o fileutil.o flare.o\
 htexture.o light.o list.o object.o objhand.o octree.o param.o plane.o polysolv.o rsi.o sor.o sphere.o\
 star.o surface.o texture.o triangle.o voxel.o world.o vecmath.o noise.o -o raystorm.library -gs -l amiga

raystorm.library.o: raystorm.library.asm
	acom:oma raystorm.library.asm -m -o raystorm.library.o

actor.o: actor.cpp actor.h color.h intersec.h list.h stats.h typedefs.h voxel.h world.h
	mcppc -c actor.cpp

box.o: actor.h box.cpp box.h brush.h camera.h color.h intersec.h list.h object.h octree.h stats.h\
 surface.h typedefs.h voxel.h world.h
	mcppc -c box.cpp

camera.o: actor.h camera.cpp camera.h color.h intersec.h list.h stats.h typedefs.h voxel.h world.h
	mcppc -c -pp -go camera.cpp

cone.o: actor.h brush.h camera.h color.h cone.cpp cone.h intersec.h list.h object.h octree.h stats.h\
 surface.h typedefs.h voxel.h world.h
	mcppc -c cone.cpp

csg.o: actor.h brush.h camera.h color.h csg.cpp csg.h intersec.h list.h object.h octree.h stats.h\
 surface.h typedefs.h voxel.h world.h
	mcppc -c csg.cpp

cylinder.o: actor.h brush.h camera.h color.h cylinder.cpp cylinder.h intersec.h list.h object.h octree.h\
 stats.h surface.h typedefs.h voxel.h world.h
	mcppc -c cylinder.cpp

brush.o: actor.h brush.cpp brush.h color.h error.h fileutil.h intersec.h list.h ray:keyfile/keyfile_lib.h\
 rmod:pichand_lib.h stats.h surface.h typedefs.h voxel.h world.h
	mcppc -c -pp brush.cpp

fileutil.o: fileutil.c fileutil.h stats.h typedefs.h
	mcppc -c fileutil.c

flare.o: color.h flares.cpp flares.h list.h stats.h typedefs.h
	mcppc -c flares.cpp

htexture.o: actor.h brush.h camera.h color.h htexture.cpp htexture.h intersec.h list.h object.h octree.h\
 ray:lib_noise/noise.h sphere.h stats.h surface.h typedefs.h voxel.h world.h
	mcppc -c htexture.cpp

light.o: actor.h brush.h camera.h color.h flares.h intersec.h light.cpp light.h list.h object.h octree.h\
 star.h stats.h surface.h texture.h typedefs.h voxel.h world.h
	mcppc -c -pp light.cpp

list.o: list.cpp list.h stats.h typedefs.h
	mcppc -c -pp list.cpp

object.o: actor.h brush.h camera.h color.h flares.h htexture.h intersec.h light.h list.h logo.h object.cpp\
 object.h octree.h ray:keyfile/keyfile_lib.h ray:lib_noise/noise.h star.h stats.h surface.h texture.h\
 triangle.h typedefs.h voxel.h world.h
	mcppc -c -pp object.cpp

objhand.o: actor.h box.h brush.h camera.h color.h cone.h csg.h cylinder.h error.h fileutil.h flares.h\
 intersec.h light.h list.h object.h objhand.cpp objhand.h objlink.h octree.h plane.h rmod:objhand_lib.h\
 sor.h sphere.h star.h stats.h surface.h texture.h triangle.h typedefs.h voxel.h world.h
	mcppc -c objhand.cpp

octree.o: actor.h brush.h camera.h color.h error.h intersec.h list.h object.h octree.cpp octree.h stats.h\
 surface.h typedefs.h voxel.h world.h
	mcppc -c -pp octree.cpp

param.o: param.cpp param.h stats.h typedefs.h
	mcppc -c param.cpp

plane.o: actor.h brush.h camera.h color.h intersec.h list.h object.h octree.h plane.cpp plane.h stats.h\
 surface.h typedefs.h voxel.h world.h
	mcppc -c -pp plane.cpp

polysolv.o: list.h polysolv.cpp stats.h typedefs.h
	mcppc -c polysolv.cpp

rsi.o: actor.h box.h brush.h camera.h color.h cone.h csg.h cylinder.h error.h fileutil.h flares.h\
 htexture.h intersec.h light.h list.h object.h objhand.h octree.h param.h plane.h ray:keyfile/keyfile_lib.h\
 ray:lib_noise/noise.h rsi.cpp rsi.h sor.h sphere.h star.h stats.h surface.h texture.h triangle.h typedefs.h\
 voxel.h world.h
	mcppc -c -go rsi.cpp

sor.o: actor.h brush.h camera.h color.h intersec.h list.h object.h octree.h polysolv.h sor.cpp sor.h\
 stats.h surface.h typedefs.h voxel.h world.h
	mcppc -c sor.cpp

sphere.o: actor.h brush.h camera.h color.h intersec.h list.h object.h octree.h sphere.cpp sphere.h stats.h\
 surface.h typedefs.h voxel.h world.h
	mcppc -c -pp sphere.cpp

star.o: color.h list.h star.cpp star.h stats.h typedefs.h
	mcppc -c star.cpp

surface.o: actor.h brush.h camera.h color.h htexture.h intersec.h list.h object.h octree.h ray:lib_noise/noise.h\
 stats.h surface.cpp surface.h texture.h typedefs.h voxel.h world.h
	mcppc -c -pp surface.cpp

texture.o: actor.h brush.h camera.h color.h error.h fileutil.h intersec.h list.h object.h octree.h stats.h\
 surface.h texture.cpp texture.h typedefs.h voxel.h world.h
	mcppc -c -pp texture.cpp

triangle.o: actor.h brush.h camera.h color.h error.h intersec.h list.h object.h octree.h stats.h surface.h\
 texture.h triangle.cpp triangle.h typedefs.h voxel.h world.h
	mcppc -c -pp triangle.cpp

voxel.o: list.h stats.h typedefs.h voxel.cpp voxel.h
	mcppc -c -pp voxel.cpp

world.o: actor.h brush.h color.h intersec.h list.h stats.h typedefs.h voxel.h world.cpp world.h
	mcppc -c world.cpp

vecmath.o: ray:lib_vecmath/vecmath.cpp ray:lib_vecmath/vecmath.h
	mcppc -c ray:lib_vecmath/vecmath.cpp

noise.o: ray:lib_noise/noise.c
	mcppc -c ray:lib_noise/noise.c

