/*
 * Simple Scenario ARexx script which creates a fractal object
 * made of spheres.
 * Change maxiterations to alter the iteration depth
 */

signal on error

options results

address SCENARIO.1

maxiterations = 3
iter = 0

CALL create 0,0,0,1,0,iter

EXIT 0

/*--------------*/
/* recursive function which creates the spheres */
/*--------------*/

create: PROCEDURE EXPOSE maxiterations
PARSE ARG x,y,z,size,from,iter

iter=iter+1
if iter > maxiterations then
	RETURN

'CREATEOBJECT SPHERE'
'SETOBJECTPOS <' || x || ',' || y || ',' || z || '>'
'SETOBJECTSIZE <'size','size','size'>'

if from ~= 2 then
	CALL create x+size*1.5,y,z,size*0.5,1,iter
if from ~= 1 then
	CALL create x-size*1.5,y,z,size*0.5,2,iter
if from ~= 4 then
	CALL create x,y+size*1.5,z,size*0.5,3,iter
if from ~= 3 then
	CALL create x,y-size*1.5,z,size*0.5,4,iter
if from ~= 6 then
	CALL create x,y,z+size*1.5,size*0.5,5,iter
if from ~= 5 then
	CALL create x,y,z-size*1.5,size*0.5,6,iter

RETURN
