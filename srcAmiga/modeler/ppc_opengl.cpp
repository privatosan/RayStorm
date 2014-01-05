#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/Amigamesa.h>
#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>
#include <powerpc/powerpc.h>

#ifdef __PPC__
#define PPC_STUB(a) stub_ ## a
#else
#define PPC_STUB(a) a
#endif

void stub_glMaterialf( GLenum face, GLenum pname, GLfloat param )
{
}

void stub_glMaterialfv( GLenum face, GLenum pname, const GLfloat *params )
{
}

extern "ASM" void _exit()
{
	SPrintF ("Internal failure in MESA OpenGL!", NULL)
}

extern "ASM" void __saveds _PPCStub_stub_glGetFloatv__EPf(long *args)
{
	glGetFloatv(
		(GLenum) args[0],
		(GLfloat*) args[1]
	);
}

extern "ASM" void OpenGLDrawMesh__P08TRIANGLEP04EDGEP06VECTORP06VECTORUj(void *arg1,void *arg2,void *arg3,void *arg4,unsigned long arg5);
extern "ASM" void __saveds _PPCStub_OpenGLDrawMesh__P08TRIANGLEP04EDGEP06VECTORP06VECTORUj(long *args)
{
	OpenGLDrawMesh__P08TRIANGLEP04EDGEP06VECTORP06VECTORUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3],
		(unsigned long) args[4]
	);
}

extern "ASM" void OpenGLDrawPointLight__P06VECTOR(void *arg1);
extern "ASM" void __saveds _PPCStub_OpenGLDrawPointLight__P06VECTOR(long *args)
{
	OpenGLDrawPointLight__P06VECTOR(
		(void *) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glPopMatrix_(long *args)
{
	glPopMatrix();
}

extern "ASM" void __saveds _PPCStub_stub_AmigaMesaDestroyContext__P17amigamesa_context(long *args)
{
	AmigaMesaDestroyContext(
		(struct amigamesa_context *) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glVertex3fv__PCf(long *args)
{
	glVertex3fv(
		(const GLfloat *) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_AmigaMesaMakeCurrent__P17amigamesa_contextP16amigamesa_buffer(long *args)
{
	AmigaMesaMakeCurrent(
		(struct amigamesa_context *) args[0],
		(struct amigamesa_buffer *) args[1]
	);
}

extern "ASM" void OpenGLDrawCamera__P07DISPLAYffffffP06VECTOR(void *arg1,float arg2,float arg3,float arg4,float arg5,float arg6,float arg7,void *arg8);
extern "ASM" void __saveds _PPCStub_OpenGLDrawCamera__P07DISPLAYffffffP06VECTOR(long *args)
{
	OpenGLDrawCamera__P07DISPLAYffffffP06VECTOR(
		(void *) args[0],
		*((float *) &args[1]),
		*((float *) &args[2]),
		*((float *) &args[3]),
		*((float *) &args[4]),
		*((float *) &args[5]),
		*((float *) &args[6]),
		(void *) args[7]
	);
}

extern "ASM" void OpenGLDrawCone__P06VECTORUj(void *arg1,unsigned long arg2);
extern "ASM" void __saveds _PPCStub_OpenGLDrawCone__P06VECTORUj(long *args)
{
	OpenGLDrawCone__P06VECTORUj(
		(void *) args[0],
		(unsigned long) args[1]
	);
}

extern "ASM" void OpenGLDrawPlane__P06VECTOR(void *arg1);
extern "ASM" void __saveds _PPCStub_OpenGLDrawPlane__P06VECTOR(long *args)
{
	OpenGLDrawPlane__P06VECTOR(
		(void *) args[0]
	);
}

extern "ASM" void OpenGLDrawSpotLight__P06VECTORP06VECTORf(void *arg1,void *arg2,float arg3);
extern "ASM" void __saveds _PPCStub_OpenGLDrawSpotLight__P06VECTORP06VECTORf(long *args)
{
	OpenGLDrawSpotLight__P06VECTORP06VECTORf(
		(void *) args[0],
		(void *) args[1],
		*((float *) &args[2])
	);
}

extern "ASM" void __saveds _PPCStub_stub_glClear__Ui(long *args)
{
	glClear(
		(GLbitfield) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glLightfv__EEPCf(long *args)
{
	glLightfv(
		(GLenum) args[0],
		(GLenum) args[1],
		(const GLfloat *) args[2]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glViewport__iiii(long *args)
{
	glViewport(
		(GLint) args[0],
		(GLint) args[1],
		(GLsizei) args[2],
		(GLsizei) args[3]
	);
}

extern "ASM" void __saveds _PPCStub_stub_gluLookAt__ddddddddd(long *args)
{
	gluLookAt(
		*((GLdouble *) &args[0]),
		*((GLdouble *) &args[2]),
		*((GLdouble *) &args[4]),
		*((GLdouble *) &args[6]),
		*((GLdouble *) &args[8]),
		*((GLdouble *) &args[10]),
		*((GLdouble *) &args[12]),
		*((GLdouble *) &args[14]),
		*((GLdouble *) &args[16])
	);
}

extern "ASM" void __saveds _PPCStub_stub_glEnable__E(long *args)
{
	glEnable(
		(GLenum) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_AmigaMesaSwapBuffers__P17amigamesa_context(long *args)
{
	AmigaMesaSwapBuffers(
		(struct amigamesa_context *) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glLoadMatrixf__PCf(long *args)
{
	glLoadMatrixf(
		(const GLfloat *) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glGetIntegerv__EPi(long *args)
{
	glGetIntegerv(
		(GLenum) args[0],
		(GLint *) args[1]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glLightModeli__Ei(long *args)
{
	glLightModeli(
		(GLenum) args[0],
		(GLint) args[1]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glColor3fv__Pf(long *args)
{
	glColor3fv(
		(GLfloat *) args[0]
	);
}

extern "ASM" void OpenGLDrawCylinder__P06VECTORUj(void *arg1,unsigned long arg2);
extern "ASM" void __saveds _PPCStub_OpenGLDrawCylinder__P06VECTORUj(long *args)
{
	OpenGLDrawCylinder__P06VECTORUj(
		(void *) args[0],
		(unsigned long) args[1]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glPushMatrix_(long *args)
{
	glPushMatrix();
}

extern "ASM" void __saveds _PPCStub_stub_glLoadIdentity_(long *args)
{
	glLoadIdentity( );
}

extern "ASM" void __saveds _PPCStub_stub_glDisable__E(long *args)
{
	glDisable(
		(GLenum) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glMatrixMode__E(long *args)
{
	glMatrixMode(
		(GLenum) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glClearColor__ffff(long *args)
{
	glClearColor(
		*((GLclampf *) &args[0]),
		*((GLclampf *) &args[1]),
		*((GLclampf *) &args[2]),
		*((GLclampf *) &args[3])
	);
}

extern "ASM" void OpenGLDrawBox__P06VECTORP06VECTOR(void *arg1,void *arg2);
extern "ASM" void __saveds _PPCStub_OpenGLDrawBox__P06VECTORP06VECTOR(long *args)
{
	OpenGLDrawBox__P06VECTORP06VECTOR(
		(void *) args[0],
		(void *) args[1]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glBegin__E(long *args)
{
	glBegin(
		(GLenum) args[0]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glLineStipple__iUs(long *args)
{
	glLineStipple(
		(GLint) args[0],
		(GLushort) args[1]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glVertex3f__fff(long *args)
{
	glVertex3f(
		*((GLfloat *) &args[0]),
		*((GLfloat *) &args[1]),
		*((GLfloat *) &args[2])
	);
}

extern "ASM" void __saveds _PPCStub_stub_gluPerspective__dddd(long *args)
{
	gluPerspective(
		*((GLdouble *) &args[0]),
		*((GLdouble *) &args[2]),
		*((GLdouble *) &args[4]),
		*((GLdouble *) &args[6])
	);
}

extern "ASM" void OpenGLDrawSphere__f(float arg1);
extern "ASM" void __saveds _PPCStub_OpenGLDrawSphere__f(long *args)
{
	OpenGLDrawSphere__f(
		*((float *) &args[0])
	);
}

extern "ASM" void __saveds _PPCStub_stub_glOrtho__dddddd(long *args)
{
	glOrtho(
		*((GLdouble *) &args[0]),
		*((GLdouble *) &args[2]),
		*((GLdouble *) &args[4]),
		*((GLdouble *) &args[6]),
		*((GLdouble *) &args[8]),
		*((GLdouble *) &args[10])
	);
}

extern "ASM" void __saveds _PPCStub_stub_glEnd_(long *args)
{
	glEnd();
}

extern "ASM" void __saveds _PPCStub_stub_glColorMaterial__EE(long *args)
{
	glColorMaterial(
		(GLenum) args[0],
		(GLenum) args[1]
	);
}

extern "ASM" void __saveds _PPCStub_stub_glVertex2f__ff(long *args)
{
	glVertex2f(
		*((GLfloat *) &args[0]),
		*((GLfloat *) &args[1])
	);
}

extern "ASM" void __saveds _PPCStub_stub_glMaterialf__EEf(long *args)
{
	glMaterialf(
		(GLenum) args[0],
		(GLenum) args[1],
		*((GLfloat *) &args[2])
	);
}

extern "ASM" void __saveds _PPCStub_stub_glMaterialfv__EEPCf(long *args)
{
	glMaterialfv(
		(GLenum) args[0],
		(GLenum) args[1],
		(const GLfloat *) args[2]
	);
}

extern "ASM" void OpenGLDrawSelBox__P06VECTORP06VECTORP06VECTORP06VECTOR(void *arg1,void *arg2,void *arg3,void *arg4);
extern "ASM" void __saveds _PPCStub_OpenGLDrawSelBox__P06VECTORP06VECTORP06VECTORP06VECTOR(long *args)
{
	OpenGLDrawSelBox__P06VECTORP06VECTORP06VECTORP06VECTOR(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3]
	);
}

extern "ASM" void *__saveds _PPCStub_stub_AmigaMesaCreateContext__P07TagItem(long *args)
{
	return AmigaMesaCreateContext((struct TagItem*)args[0]);
}

extern "ASM" void OGLLoadMatrix__P06MATRIX(void *arg1);
extern "ASM" void __saveds _PPCStub_OGLLoadMatrix__P06MATRIX(long *args)
{
	OGLLoadMatrix__P06MATRIX(
		(void *) args[0]
	);
}

extern "ASM" void OpenGLDrawGrid__P07DISPLAY(void *arg1);
extern "ASM" void __saveds _PPCStub_OpenGLDrawGrid__P07DISPLAY(long *args)
{
	OpenGLDrawGrid__P07DISPLAY(
		(void *) args[0]
	);
}

extern "ASM" void SetGLColor__i(long arg1);
extern "ASM" void __saveds _PPCStub_SetGLColor__i(long *args)
{
	SetGLColor__i(
		(long) args[0]
	);
}

extern "ASM" void OpenGLDrawDirectionalLight__P06VECTOR(void *arg1);
extern "ASM" void __saveds _PPCStub_OpenGLDrawDirectionalLight__P06VECTOR(long *args)
{
	OpenGLDrawDirectionalLight__P06VECTOR(
		(void *) args[0]
	);
}

extern "ASM" void OpenGLDrawRenderLines__P07DISPLAYP09RECTANGLEs(void *arg1,void *arg2,short arg3);
extern "ASM" void __saveds _PPCStub_OpenGLDrawRenderLines__P07DISPLAYP09RECTANGLEs(long *args)
{
	OpenGLDrawRenderLines__P07DISPLAYP09RECTANGLEs(
		(void *) args[0],
		(void *) args[1],
		(short) args[2]
	);
}

extern "ASM" void OpenGLSetupFront__P07DISPLAYP05COLOR(void *arg1,void *arg2);
extern "ASM" void __saveds _PPCStub_OpenGLSetupFront__P07DISPLAYP05COLOR(long *args)
{
	OpenGLSetupFront__P07DISPLAYP05COLOR(
		(void *) args[0],
		(void *) args[1]
	);
}

extern "ASM" void OpenGLSetupTop__P07DISPLAYP05COLOR(void *arg1,void *arg2);
extern "ASM" void __saveds _PPCStub_OpenGLSetupTop__P07DISPLAYP05COLOR(long *args)
{
	OpenGLSetupTop__P07DISPLAYP05COLOR(
		(void *) args[0],
		(void *) args[1]
	);
}

extern "ASM" void OpenGLSetupPerspective__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5,float arg6,float arg7);
extern "ASM" void __saveds _PPCStub_OpenGLSetupPerspective__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(long *args)
{
	OpenGLSetupPerspective__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3],
		(void *) args[4],
		*((float *) &args[5]),
		*((float *) &args[6])
	);
}

extern "ASM" void OpenGLSetupRight__P07DISPLAYP05COLOR(void *arg1,void *arg2);
extern "ASM" void __saveds _PPCStub_OpenGLSetupRight__P07DISPLAYP05COLOR(long *args)
{
	OpenGLSetupRight__P07DISPLAYP05COLOR(
		(void *) args[0],
		(void *) args[1]
	);
}

extern "ASM" void OpenGLSetupCamera__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5,float arg6,float arg7);
extern "ASM" void __saveds _PPCStub_OpenGLSetupCamera__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(long *args)
{
	OpenGLSetupCamera__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3],
		(void *) args[4],
		*((float *) &args[5]),
		*((float *) &args[6])
	);
}

