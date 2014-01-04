/***************
 * PROGRAM:       Modeler
 * NAME:          object.h
 * DESCRIPTION:   object baseclass definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    14.10.96 ah    cleanup of class structure (friend-functions)
 *    15.12.96 ah    added tracking to all objects
 *    07.01.97 ah    added DeleteObject()
 *    11.01.97 ah    added OBJECT_BOX
 *    30.01.97 ah    added DeChainObject()
 *    06.02.97 ah    added TestSelectedBox()
 *    22.02.97 ah    added TestSelectedPoints()
 *    27.02.97 ah    added SelectObject() and DeslectObject()
 *    28.02.97 ah    added UpdateTracking()
 *    25.03.97 ah    added OBJECT_CYLINDER
 *    31.03.97 ah    added OBJECT_NODEOPEN
 *    01.04.97 ah    added OBJECT_CONE
 *    04.04.97 ah    added OBJECT_CSG
 *    09.04.97 ah    added OBJECT_SOR
 *    03.06.97 ah    added AddBrush(), AddTexture()
 *    19.06.97 ah    added GetName(), SetName(), WriteSurface()
 *    25.06.97 ah    added AddPoint()
 *    15.07.97 ah    added OBJECT_OPEN_SETTINGS
 ***************/

#ifndef OBJECT_H
#define OBJECT_H

#ifdef __AMIGA__
	#include <libraries/gadtools.h>
	#ifndef RSI_LIB_H
	#include "rsi_lib.h"
	#endif
//   #if defined(__MIXEDBINARY__) && !defined(__PPC__)
	#ifdef __MIXEDBINARY__
		#include "ppc_rsi_stubs.h"
	#endif
#endif

#define CTXT rc

//#if defined(__MIXEDBINARY__) && !defined(__PPC__)
#ifdef __MIXEDBINARY__
#define PPC_STUB(name) stub_ ## name
#else
#define PPC_STUB(name) name
#endif // __MIXEDBINARY__ && !__PPC__

#ifndef IFF_UTIL_H
#include "iff_util.h"
#endif

class CAMERA;
class MESH;
class BRUSH_OBJECT;
class TEXTURE_OBJECT;

#ifndef LIST_H
#include "list.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifdef __AMIGA__
#ifndef POPUP_MENU_H
#include "popup_menu.h"
#endif
#endif

#ifndef UNDO_H
#include "undo.h"
#endif

// flags
#define OBJECT_DIRTY                0x000001     // object has been selected and is now unselected and reverse
#define OBJECT_NODEOPEN             0x000002     // browser node is open
#define OBJECT_OPENTOP              0x000004     // open top cap (cylinder and sor)
#define OBJECT_OPENBOTTOM           0x000008     // open bottom cap (cylinder, cone and sor)
#define OBJECT_INVERTED             0x000010     // object is inverted (needed for CSG)
#define OBJECT_NOPHONG              0x000020     // disable phong shading (mesh)
#define OBJECT_APPLYSURF            0x000040     // apply surface to child objects (only for external objects) (mesh)
#define OBJECT_HANDLEDIRTY          0x000080     // remove dirty handles from screen (mesh and SOR)
#define OBJECT_MESHEDITING          0x000100     // set while a mesh or points object is edited
#define OBJECT_SOR_ACCURATE         0x000200     // use accurate but slow algorithm for sor

#define OBJECT_LIGHT_SHADOW         0x000800     // casts shadows
#define OBJECT_LIGHT_FLARES         0x001000     // has flares
#define OBJECT_LIGHT_STAR           0x002000     // has star
#define OBJECT_LIGHT_SHADOWMAP      0x004000     // has shadowmap
#define OBJECT_LIGHT_TRACKFALLOFF   0x008000     // takes fall off form track distance

#define OBJECT_CAMERA_VFOV          0x010000     // take VFOV from resolution
#define OBJECT_CAMERA_FOCUSTRACK    0x020000     // set focal distance to track distance
#define OBJECT_CAMERA_FASTDOF       0x040000     // apply fast depth of field
#define OBJECT_CAMERA_HIDDENAREA    0x080000     //
#define OBJECT_CAMERA_ACCELERATE    0x100000     //

#define OBJECT_HANDLEDIRTY1         0x200000     // draw dirty handles screen (mesh and SOR)

// qualifier for GetObjects() and GetSelectedObjects()
#define GETOBJECT_BRUSHES           0x01        // get selected brushes too
#define GETOBJECT_TEXTURES          0x02        // get selected textures too

// states of external objects
enum
{
	EXTERNAL_NONE,       // no external object
	EXTERNAL_ROOT,       // root of an external object
	EXTERNAL_CHILD,      // child of an external object
	EXTERNAL_RSOB_ROOT,  // root of an external RSOB object (changeable)
	EXTERNAL_RSOB_CHILD  // child of an external RSOB object (changeable)
};

enum
{
	OBJECT_CAMERA,
	OBJECT_POINTLIGHT,
	OBJECT_SPOTLIGHT,
	OBJECT_DIRECTIONALLIGHT,
	OBJECT_MESH,
	OBJECT_PLANE,
	OBJECT_SPHERE,
	OBJECT_BRUSH,
	OBJECT_TEXTURE,
	OBJECT_BOX,
	OBJECT_CYLINDER,
	OBJECT_CONE,
	OBJECT_CSG,
	OBJECT_SOR
};

class OBJECT : public LIST    // object
{
	private:
		char *name;       // name
	public:
		SURFACE *surf;    // surface definition
		BRUSH_OBJECT *brush; // brush axis definition
		TEXTURE_OBJECT *texture; // texture axis definition
		VECTOR pos;       // position
		VECTOR orient_x;  // orientation
		VECTOR orient_y;
		VECTOR orient_z;
		VECTOR align;
		VECTOR size;      // axis size
		VECTOR bboxmax;
		VECTOR bboxmin;
		BOOL selected;    // object is selected
		BOOL istracked;   // object is tracked
		OBJECT *track;    // object to which this object tracks to
		ULONG flags;
		UWORD external;   // state of object (external or RSOB root or child)
		UWORD opencnt;    // amount of open settings requesters of secondary selected objects

#ifdef __AMIGA__
		Object *win;      // set if the settings requester for this object is open
#else
		CDialog *win;
#endif

#ifdef __AMIGA__
		virtual struct NewMenu *GetPopUpList();
#else
		virtual int GetResourceID();
#endif

		OBJECT();
		~OBJECT();
		OBJECT *Iterate(MATRIX_STACK*);
		OBJECT *Iterate();
		void Insert(OBJECT*,int);
		void Append();
		void Remove();
		void InsertToBuffer(OBJECT*,int);
		void FreeObjectsList();
		virtual void SetDrawColor(DISPLAY*);
		virtual void Draw(DISPLAY*,MATRIX_STACK*)=0;
		virtual void SetObject(DISPLAY*,VECTOR*,VECTOR*,VECTOR*,VECTOR*,VECTOR*)=0;
		virtual void CalcBBox() { };
		void SetTreeSizes(VECTOR*);
		BOOL WriteCommon(struct IFFHandle*);
		BOOL ReadAxis(struct IFFHandle*);
		BOOL WriteSurface(struct IFFHandle*);
		BOOL WriteFlags(struct IFFHandle*);
		BOOL ReadFlags(struct IFFHandle*);
		BOOL WriteTrack(struct IFFHandle*);
		BOOL ReadTrack(struct IFFHandle*);
		BOOL WriteName(struct IFFHandle*);
		BOOL ReadName(struct IFFHandle*);
		virtual BOOL SetName(char*);
		virtual char *GetName() { return name; }
		virtual int GetType()=0;
		virtual BOOL CSGAble() { return FALSE; }
		virtual int GetSize()=0;
		virtual BOOL IsVisible(DISPLAY*) { return TRUE; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**) { return rsiERR_NONE; };
		BOOL GetObjectMatrix(MATRIX *);
		OBJECT *Duplicate();
		virtual OBJECT *DupObj() = 0;
		void IsFirstSelected();
		friend void NoFirstSelected();
		virtual BOOL TestSelectedPoints(DISPLAY*,int,int,BOOL,MATRIX_STACK*) { return FALSE; };
		virtual BOOL TestSelectedEdges(DISPLAY*,int,int,BOOL,MATRIX_STACK*) { return FALSE; };
		virtual BOOL TestSelectedTrias(DISPLAY*,int,int,BOOL,MATRIX_STACK*) { return FALSE; };
		virtual void FreeSelectedPoints() { };
		virtual BOOL DeleteSelectedPoints(UNDO_DELETE*) { return FALSE; }
		virtual BOOL DeleteSelectedEdges(UNDO_DELETE*) { return FALSE; }
		virtual BOOL DeleteSelectedTrias(UNDO_DELETE*) { return FALSE; }
		virtual BOOL AddPoint(DISPLAY*, int, int) { return FALSE; }
		virtual BOOL AddEdge(DISPLAY*, int, int) { return FALSE; }
		virtual BOOL AddFace(DISPLAY*, int, int) { return FALSE; }
		void AddBrush(BRUSH_OBJECT*);
		void AddBrushTop(BRUSH_OBJECT*);
		void RemoveBrushes();
		void AddTexture(TEXTURE_OBJECT*);
		void AddTextureTop(TEXTURE_OBJECT*);
		void RemoveTextures();
		void SetActiveObject(DISPLAY*);
		void SetActiveObject(DISPLAY*, MATRIX_STACK*, BOOL);

		BOOL UpdateTracking(VECTOR*);

		void TranslateNamesList();

		void Select();
		void SelectList();

		void ExternToScene();

		int GetSizeOfTree();

		void DeleteObject();
		void DeChainObject();

		void SelectObject();
		void DeselectObject();

	private:
		friend void FreeObjects();

		friend void DrawObs(DISPLAY *, int);
		BOOL DrawObsList(DISPLAY*, int, MATRIX_STACK*, VECTOR*);
		void DrawDirty(DISPLAY *, MATRIX_STACK *, VECTOR *);
		void DrawSelected(DISPLAY *, MATRIX_STACK *, VECTOR *);

		friend void DrawObjects(DISPLAY *, BOOL);
		void DrawObj(DISPLAY*,MATRIX_STACK*,VECTOR*);
		BOOL DrawList(DISPLAY*,MATRIX_STACK*,VECTOR*,BOOL,BOOL);

		friend void RemoveSelected(DISPLAY *);

		friend OBJECT* TestSelected(DISPLAY*,WORD,WORD,BOOL);
		void Deselect(OBJECT *, BOOL);
		void DeselectList(OBJECT *, BOOL);
		OBJECT *TestSelected(DISPLAY *,WORD,WORD,BOOL,MATRIX_STACK*);
		OBJECT *TestSelectedList(DISPLAY *,WORD,WORD,BOOL,MATRIX_STACK*);

		friend BOOL TestSelectedPoints(DISPLAY*,WORD,WORD,BOOL);
		BOOL TestSelectedPointsList(DISPLAY *,WORD,WORD,BOOL,MATRIX_STACK*);

		friend OBJECT *TestSelectedBox(DISPLAY*, RECTANGLE2D*, BOOL);
		OBJECT *TestSelectedBox(DISPLAY*, RECTANGLE2D*, BOOL, MATRIX_STACK*);
		OBJECT *TestSelectedBoxList(DISPLAY*, RECTANGLE2D*, BOOL, MATRIX_STACK *);

		friend void SetActiveObjects(DISPLAY *);
		void SetActiveObjects(DISPLAY *disp,MATRIX_STACK*,OBJECT*);
		void SetActiveObjectsList(DISPLAY *,MATRIX_STACK*,BOOL,BOOL);

		void SetTreeSize(VECTOR*);
		void SetTreeSizesList(VECTOR*);

		friend BOOL GetActiveObjects(OBJECT**,BOOL withsurface=FALSE);
		BOOL GetActiveObjs(OBJECT**,BOOL);

		friend void DeleteSelected(OBJECT*, ULONG);
		void DeleteSelectedList(OBJECT*, ULONG, UNDO_DELETE*);

		friend OBJECT *GetObjectByName(char *);
		OBJECT *GetObjByName(char*);

		friend BOOL WriteObjects(struct IFFHandle *, BOOL);
		BOOL WriteObj(struct IFFHandle *,BOOL);
		virtual BOOL WriteObject(struct IFFHandle *) = 0;

		BOOL WriteSelectedSurfacesList(struct IFFHandle*);
		friend BOOL WriteSelectedSurfaces(struct IFFHandle*);

		friend void GetObjects(void (*func)(OBJECT*, void*), void*, ULONG);
		void GetObjectsList(void (*func)(OBJECT*, void*), void*, ULONG);

		friend void GetSelectedObjects(void (*func)(OBJECT*, void*), void*, ULONG);
		void GetSelectedObjectsList(void (*func)(OBJECT*, void*), void*, ULONG);

		friend void TranslateNames();

		friend rsiResult ToRSI(rsiCONTEXT *, OBJECT*);
		rsiResult ToRSIList(rsiCONTEXT*, MATRIX_STACK*, OBJECT*);

		friend BOOL GetBBox(VECTOR*,VECTOR*,BOOL);
		void GetBBoxList(VECTOR*,VECTOR*,BOOL,BOOL*,MATRIX_STACK*);

		friend void DeselectAll();

		friend BOOL Cut(OBJECT*);
		BOOL CutList(OBJECT *,OBJECT *,int,UNDO_DELETE*);

		friend BOOL Copy();
		BOOL CopyList(OBJECT *,int);

		friend BOOL Paste();
		BOOL PasteList(OBJECT *,int);

		BOOL GetObjectMatrixList(OBJECT *, MATRIX *, MATRIX_STACK *);
		void GetBBox(VECTOR *, VECTOR *, MATRIX_STACK *);

		friend void DeselectBrushes();
		void DeselectBrushesList();

		friend void DeselectTextures();
		void DeselectTexturesList();

		friend void SelectAll();
		friend void SelectPrev();
		friend void SelectNext();

		friend BOOL GroupObjects();
		void GroupObjectsList(OBJECT *, VECTOR *, MATRIX *);

		friend void UnGroupObjects();
		void UnGroupObjectsList();

		void ExternToSceneList();

		void UpdateTrackingList();
		friend void UpdateTracking();

		friend void Untrack(OBJECT*);

		friend BOOL DuplicateObjects(OBJECT**, OBJECT*);
		BOOL DuplicateObjectsList(OBJECT**, OBJECT*, int);

		friend OBJECT *IsSurfaceUsed(SURFACE*);
		friend BOOL UpdateSurfaceAssignment(SURFACE*);

		friend void RemoveDirtyFlag();
		void RemoveDirtyFlagList();

#ifdef __AMIGA__
		void IncOpenCounterList(Object *);
		friend void IncOpenCounter(Object *);
#endif
};

char *TestForSaveObject();
char *LoadObject(char*, MESH *root = NULL);
char *SaveObject(char*);
char *GetLoadError();
BOOL ParseExternal(struct IFFHandle *);
#ifdef __OPENGL__
void SetupOpenGLLights(DISPLAY*);
#endif // __OPENGL__
void CalcSceneBBox(DISPLAY*, VECTOR*, VECTOR*);

#ifdef __MIXEDBINARY__
#include "ppc_opengl.h"
#endif

#endif /* OBJECT_H */
