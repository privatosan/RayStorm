/***************
 * PROGRAM:       Modeler
 * NAME:          mesh.h
 * DESCRIPTION:   This file includes the mesh class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    14.03.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 *    22.02.97 ah    added TestSelectedPoints()
 ***************/

#ifndef MESH_H
#define MESH_H

//#define NORMALS

#ifndef VECMATH_H
#include "vecmath.h"
#endif

class MESH;

/*class MESHSURF : public SLIST
{
	public:
		SURFACE *surf;
};*/

typedef struct
{
	UWORD p[2];    // point numbers
	UWORD flags;
} EDGE;

#define EDGE_SHARP      1
#define EDGE_SELECTED   2
#define EDGE_DIRTY      4

/*************
 * FUNCTION:      SetEdge
 * DESCRIPTION:   Sets the points of an edge
 * INPUT:         edge     edge
 *                p1,p2    points
 * OUTPUT:        -
 *************/
inline void SetEdge(EDGE *edge, UWORD p1, UWORD p2)
{
	edge->p[0] = p1;
	edge->p[1] = p2;
}

typedef struct
{
// SURFACE *surf;
	UWORD e[3];    // edge numbers
#ifdef NORMAL
	UWORD n[3];    // normal numbers
#endif // NORMAL
	UWORD flags;
} TRIANGLE;

#define TRIA_SELECTED   1
#define TRIA_DIRTY      2

#ifndef OBJECT_H
#include "object.h"
#endif

/*************
 * FUNCTION:      SetTriangleEdges
 * DESCRIPTION:   Sets the edges of a triangle
 * INPUT:         tria     triangle
 *                e1,e2,e3 edges
 * OUTPUT:        -
 *************/
inline void SetTriangleEdges(TRIANGLE *tria, UWORD e1, UWORD e2, UWORD e3)
{
	tria->e[0] = e1;
	tria->e[1] = e2;
	tria->e[2] = e3;
}

/*************
 * FUNCTION:      SetTriangleNormals
 * DESCRIPTION:   Sets the normals of a triangle
 * INPUT:         tria     triangle
 *                n1,n2,n3 normals
 * OUTPUT:        -
 *************/
inline void SetTriangleNormals(TRIANGLE *tria, UWORD n1, UWORD n2, UWORD n3)
{
#ifdef NORMAL
	tria->n[0] = n1;
	tria->n[1] = n2;
	tria->n[2] = n3;
#endif // NORMAL
}

#define SELFLAG_NONE    0
#define SELFLAG_FIRST   1
#define SELFLAG_SECOND  2

class MESH : public OBJECT
{
	public:
		char *file;          // filename for external objects

		int maxpoints;       // max amount of point space allocated
		int actpoints;       // actual fill level of point array
		VECTOR *points;      // pointer to memory block
		UBYTE *selpoints;    // selection state of points

		int maxedges;
		int actedges;
		EDGE *edges;

		int maxtrias;
		int acttrias;
		TRIANGLE *trias;

#ifdef NORMAL
		int maxnormals;
		int curnormals;
		VECTOR *normals;
#endif // NORMAL

		UWORD selpoint[2];   // indexes of selected points in triangle and edge select mode
		UWORD selflag;

//    MESHSURF *msurfs;

		MESH();
		~MESH();
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void CalcBBox();
		virtual void SetObject(DISPLAY*,VECTOR*,VECTOR*,VECTOR*,VECTOR*,VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_MESH; }
		virtual int GetSize();
		virtual BOOL TestSelectedPoints(DISPLAY*,int,int,BOOL,MATRIX_STACK*);
		virtual BOOL TestSelectedEdges(DISPLAY*,int,int,BOOL,MATRIX_STACK*);
		virtual BOOL TestSelectedTrias(DISPLAY*,int,int,BOOL,MATRIX_STACK*);
		virtual void FreeSelectedPoints();
		virtual BOOL AddPoint(DISPLAY*, int, int);
		virtual BOOL AddEdge(DISPLAY*, int, int);
		virtual BOOL AddFace(DISPLAY*, int, int);
		virtual BOOL IsVisible(DISPLAY*);
		BOOL Insert(SURFACE *);
		BOOL SetFileName(char *);
		int AddPoint(int);
		void DeletePoint(UWORD);
		int AddEdge(int);
		void DeleteEdge(UWORD);
		int AddTria(int);
		void DeleteTria(UWORD);
#ifdef NORMAL
		int AddNormal(int);
		void DeleteNormal(UWORD);
#endif // NORMAL

		// mesh modify functions
		BOOL Join(MESH*);

		// functions to create primitive types
		BOOL CreateCube(VECTOR *);
		BOOL CreateSphere(float, int, int);
		BOOL CreatePlane(VECTOR *, int, int);
		BOOL CreateTube(float, float, int, int, BOOL, BOOL);
		BOOL CreateCone(float, float, int, int, BOOL);
		BOOL CreateTorus(float, float, int, int);
		void ToScene();
#ifdef __AMIGA__
		virtual struct NewMenu *GetPopUpList();
#endif
	private:
		void DrawTrackPoints(DISPLAY*, MATRIX_STACK*);
		void DrawEdited(DISPLAY*, MATRIX_STACK*);
		BOOL TestSelectedPoint(DISPLAY*,int,int,BOOL,MATRIX_STACK*,int*);
		BOOL DeleteSelectedPoints(UNDO_DELETE*);
		BOOL DeleteSelectedEdges(UNDO_DELETE*);
		BOOL DeleteSelectedTrias(UNDO_DELETE*);
		void DeleteEdges(UWORD);
		void DeleteFaces(UWORD);
};

OBJECT *ParseExternal(struct IFFHandle *, OBJECT *, int);
OBJECT *ParseMesh(struct IFFHandle *, OBJECT *, int);

#endif /* MESH_H */
