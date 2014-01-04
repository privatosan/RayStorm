----------------------
 TODO for version 2.0
----------------------

Muss unbedingt gemacht werden:

- Probleme mit Renderwindow
- Farben alle Schwarz bei Projektladen


- Objekte verschwinden bei Mehrfachselekt
- Renderwindow verschwindet
- Attrtest  Fehler bei MatView-Klasse mit Anklicken (Attrtest.scn)
- TDDD-Fehler bei chess.iob

----------------------
 TODO for version 2.1
----------------------

 - installer sollte nicht einfach nur "file corrupted schreiben"
 - SHIFT-x, SHIFT-y, SHIFT-z selektieren einen Filter und
   deselektieren alle anderen. x,y,z (ohne shift) toggeln
   den Filter..
 - Jedes Objekt hat jetzt ein Element 'win' um die Dialoge parallel offen zu lassen.
 - Im Destruktor von OBJECT den Dialog schließen.
 - Jede Surface hat jetzt auch ein Element 'win', das gesetzt ist, wenn der Dialog
   offen ist. Du musst in surface.h auch so was rein machen, und im Destruktor
   den Dialog schließen, wenn er offen ist.
 - renderbox als thread
 - Materialien nach Namen sortieren
 - LWOB-Loader
 - OpenGL
   - Perspektiv-Modus
	- Quadview
	- Clipping-Problem
	- Farbiges Grid
 - Star und Flares abspeichern
 - RayStorm textures

 Wish list:

 - Text
 - utility.CheckToolbarItem(), utility.EnableToolbarItem() nötig ?
 - 256 Farben!
 - Depth of field
 - 3DS Oberflächennormalen
 - help buttons in dialogen
 - Preview im Lens Flare Requester
 - koord. Anzeige ist relativ
 - examples
 - 3DS: namen der objekte, hierarchie
 - Coordinates should be shown in the bottom status bar as the cursor moves in one of the viewing areas (F, R, T).
 - image viewer


 Änderungen durch Depth of Field

 - camera.h, camera.cpp
 - object.cpp