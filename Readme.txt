                                                              January 4th 2014

                                RayStorm

                                   by
                             Andreas Heumann
                                  and
                               Mike Hesser

INTRODUCTION

This is the RayStorm package. It includes a fast and easy to use modeler, a
powerful scriptlanguage and a fast raytracing kernel.

REQUIREMENTS AMIGA

(1) You will need at least Kickstart 2.0.

(2) PPC-version: PowerPC processor with 68040 (with internal FPU)

(3) 060-version: 68060 processor (with internal FPU)

(4) 040-version: 68040 processor (with internal FPU)

(5) 881-version: 68020 processor or better and a mathematical coprocessor)

(6) 4MB RAM minimum

(7) RayStorm was written using MUI. Therefore you need muimaster.library
    V3.3 or better to run RayStorm.

recommended: 68060, 32MB RAM, 1GB Harddisk, GFX-Board

FEATURES

 Scenario:
  - Create spheres, planes, boxes, cylinders, cones, lightsources, cameras and
    CSG objects
  - OpenGL support (StormMesa)
  - Create mesh objects: cubes, spheres, planes, tubes and cones
  - Edit and modify mesh objects
  - Create and maintain CSG structures
  - Each object can track each other
  - Unlimited amount of cameras to view scene from different positions
  - Define surfaces for all objects, including brushes and textures
  - Load external objects (RayStorm object files, Imagine TDDD files,
    AutoDesk 3DStudio files, Lightwave files, NFF files and RAW files)
  - Save and load scenes
  - Save and load objects
  - Group and ungroup objects
  - Preview of surfaces in material requester
  - Eender scenes with RayStorm Raytracer
  - Preview of renderd scenes
  - Quadview
  - Multilevel undo (only memory limited) and redo
  - Status bar
  - Object browser with drag & drop
  - material manager with fast preview
  - Plugins
  - ARexx port
  - OpenGL support
  - CyberGfx support

 RayStorm Script:

  - ARexx port with 47 commands.

 RayStorm Raytracer

  - Octree algorithm used for rendering.
  - Motion blur for realistic simulations of moving objects.
  - Color, reflectivity, filter, altitude and specular mapping.
  - Flat, cylinder and sphere mapping.
  - Soft brush mapping.
  - Mathematical textures: wood, marble, bumps, checker, linear, radial, stars
    and fire.
  - Hyper textures: explode.
  - Tranparency and physically correct refractions.
  - 8 levels of antialiasing (adaptive supersampling).
  - Rendering box.
  - Seven builtin object types: sphere, plane, box, cylinder, cone, surface of
    revolution and triangle.
  - CSG (Constructive Solid Geometry) with the operators: union, intersection,
    difference and invert.
  - Four light types: ambient light, point light, directional light and spot light.
  - Depth of field with adjustable focal distance and aperture.
  - Soft shadows.
  - Backdrop picture.
  - Global fog and foggy objects.
  - Material attributes for realistic objects: ambient color, diffuse color,
    specular color, specular reflection exponent, diffuse transmission color,
    specular transmission color, specular transmission exponent, specular
    transmittance, transparent color, reflective color, index of refraction,
    foglength.
  - Bright objects.
  - Quick rendering.
  - Global reflection map.
  - Image formates: IFF-ILBM, PNG, TGA, JPEG and Datatypes.
  - Object format: RayStorm object file, Imagine-TDDD, AutoDesk 3DStudio,
    Lightwave and RAW
  - New image- and object-formats can be easily included because of the
    modular concept.
  - Shadowcache for faster shadow calculation.
  - Shadowmap with definable size for each lightsource for smooth and fast
    shadows.
  - Definable limited amount of objects for avoiding the creation of octree.

