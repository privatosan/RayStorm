#ifndef PPC_OPENGL_H
#define PPC_OPENGL_H

#include <gl/gl.h>

void PPC_STUB(glClearColor)( GLclampf red,
	GLclampf green,
	GLclampf blue,
	GLclampf alpha );
void PPC_STUB(glClear)( GLbitfield mask );
void PPC_STUB(glMatrixMode)( GLenum mode );
void PPC_STUB(glLoadIdentity)( void );
void PPC_STUB(glMultMatrixf)( const GLfloat *m );
void PPC_STUB(glOrtho)( GLdouble left, GLdouble right,
	GLdouble bottom, GLdouble top,
	GLdouble near_val, GLdouble far_val );
void PPC_STUB(gluPerspective)( GLdouble fovy, GLdouble aspect,
	GLdouble zNear, GLdouble zFar );
void PPC_STUB(gluLookAt)( GLdouble eyex, GLdouble eyey, GLdouble eyez,
	GLdouble centerx, GLdouble centery, GLdouble centerz,
	GLdouble upx, GLdouble upy, GLdouble upz );
void PPC_STUB(glLoadMatrixf)( const GLfloat *m );
void PPC_STUB(glEnable)( GLenum cap );
void PPC_STUB(glDisable)( GLenum cap );
void PPC_STUB(glColorMaterial)( GLenum face, GLenum mode );
void PPC_STUB(glLightModeli)( GLenum pname, GLint param );
void PPC_STUB(glColor3f)( GLfloat red, GLfloat green, GLfloat blue );
void PPC_STUB(glColor3fv)( GLfloat *color );
void PPC_STUB(glPushMatrix)( void );
void PPC_STUB(glPopMatrix)( void );
void PPC_STUB(glGetIntegerv)( GLenum pname, GLint *params );
void PPC_STUB(glLightfv)( GLenum light, GLenum pname, const GLfloat *params );
struct amigamesa_context *PPC_STUB(AmigaMesaCreateContext)(struct TagItem *taglist);
void PPC_STUB(AmigaMesaSwapBuffers)(struct amigamesa_context *amesa);
void PPC_STUB(AmigaMesaDestroyContext)(struct amigamesa_context *c );
void PPC_STUB(AmigaMesaMakeCurrent)(struct amigamesa_context *c ,struct amigamesa_buffer *b);
void PPC_STUB(AmigaMesaSetDefs)(struct amigamesa_context *c ,struct TagItem *taglist);
void PPC_STUB(glViewport)( GLint x, GLint y, GLsizei width, GLsizei height );
void PPC_STUB(glLineStipple)( GLint factor, GLushort pattern );
void PPC_STUB(glBegin)( GLenum mode );
void PPC_STUB(glEnd)( void );
void PPC_STUB(glVertex3fv)( const GLfloat *v );
void PPC_STUB(glVertex3f)( GLfloat x, GLfloat y, GLfloat z );
void PPC_STUB(glColor3f)( GLfloat r, GLfloat g, GLfloat b );
void PPC_STUB(glVertex2f)( GLfloat x, GLfloat y );
void PPC_STUB(glGetFloatv)( GLenum pname, GLfloat *params );
void PPC_STUB(glMaterialf)( GLenum face, GLenum pname, GLfloat param );
void PPC_STUB(glMaterialfv)( GLenum face, GLenum pname, const GLfloat *params );

#endif // PPC_OPENGL_H
