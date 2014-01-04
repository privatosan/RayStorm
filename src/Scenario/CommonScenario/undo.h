/***************
 * PROGRAM:       Modeler
 * NAME:          undo.h
 * DESCRIPTION:   undo class definition
 * AUTHORS:       Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    16.12.96 ah    initial release
 *    07.01.97 ah    added UNDO_CREATE
 *    30.01.97 ah    added Redo()
 *    29.07.97 ah    changed the class UNDO_TRANSFORM: now it handles
 *       a list of objects when a couple of objects are selected
 ***************/

#ifndef UNDO_H
#define UNDO_H

class UNDO_DELETE;

#ifndef OBJECT_H
#include "object.h"
#endif

class UNDO : public SLIST
{
	public:
		void Add();
		virtual int Undo() = 0;
		virtual int Redo() = 0;
		virtual int GetSize() = 0;
};

class UNDO_TRANSFORM_OBJECT : public SLIST
{
	public:
		OBJECT *object;
		VECTOR pos;
		VECTOR orient_x;
		VECTOR orient_y;
		VECTOR orient_z;
		VECTOR align;
		VECTOR size;

	public:
		void Undo_Redo();
};

class UNDO_TRANSFORM : public UNDO
{
	private:
		UNDO_TRANSFORM_OBJECT *objects;

		void Undo_Redo();

	public:
		UNDO_TRANSFORM();
		~UNDO_TRANSFORM();
		virtual int Undo();
		virtual int Redo();
		virtual int GetSize();
		BOOL AddTransformed(OBJECT*);
		void AddSelectedObjects();
		void SetObjects(DISPLAY *);
		void UpdateObjects();
		void ResetOpenCount();
};

class OBJECTLIST : public SLIST
{
	public:
		OBJECT *object;
};

class SOR;
class SOR_NODE;

class UNDO_SORNODE : public SLIST
{
	public:
		SOR *sor;
		SOR_NODE *node;

		UNDO_SORNODE();
		~UNDO_SORNODE();
};

class REDO_SORNODE : public SLIST
{
	public:
		SOR *sor;
		VECT2D p;
};

class UNDO_DELETE : public UNDO
{
	private:
		OBJECT *objects;
		OBJECTLIST *redoobjects;
		UNDO_SORNODE *sornodes;
		REDO_SORNODE *redosornodes;

	public:
		UNDO_DELETE();
		~UNDO_DELETE();
		virtual int Undo();
		virtual int Redo();
		virtual int GetSize();
		void AddDeleted(OBJECT*);
		BOOL AddCutted(OBJECT*);
		BOOL AddSORNode(SOR*, SOR_NODE*);
};

class UNDO_CREATE : public UNDO
{
	private:
		OBJECTLIST *objects;
		OBJECT *redoobjects;

	public:
		UNDO_CREATE();
		~UNDO_CREATE();
		virtual int Undo();
		virtual int Redo();
		virtual int GetSize();
		BOOL AddCreated(OBJECT*);
};

enum
{
	UNDO_OK_NOREDRAW,    // all ok, don't redraw display after undo/redo
	UNDO_OK_REDRAW,      // all ok, redraw display after undo/redo
	UNDO_NOUNDO,         // there is nothing to undo
	UNDO_NOREDO,         // there is nothing to redo
	UNDO_FAILED          // undo/redo operation failed
};

int Undo();
int Redo();

void FreeUndo();

#endif /* UNDO_H */
