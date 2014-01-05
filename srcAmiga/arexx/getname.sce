/*
 * Selects the camera and prints the name, position, alingment and size
 */

signal on error

options results

address SCENARIO.1

'SELECTBYNAME Camera'

GETSELOBJNAME
name=result
say "Name : " name

GETOBJECTPOS name
say "Pos  :" result

GETOBJECTALIGN name
say "Align:" result

GETOBJECTSIZE name
say "Size :" result

exit 0
