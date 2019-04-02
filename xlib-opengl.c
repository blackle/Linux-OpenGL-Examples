#define GL_GLEXT_PROTOTYPES why
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdint.h>

#include<X11/X.h>
#include<X11/Xlib.h>
#include<X11/extensions/Xrandr.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include "shader.h"

#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
// #define DEBUG true

static unsigned char fbdata[4 * CANVAS_HEIGHT * CANVAS_WIDTH];

void _start() {
	asm volatile("sub $8, %rsp\n");

	Display* dpy = XOpenDisplay(NULL);
	Window root = DefaultRootWindow(dpy);

	static GLint att[] = { GLX_RGBA, None };
	XVisualInfo* vi = glXChooseVisual(dpy, 0, att);

	//I really hate this and I wish this call was unneeded. it feels useless
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

	//hide cursor
	XColor xcolor;
	Pixmap csr= XCreatePixmap(dpy,root,1,1,1);
	Cursor cursor= XCreatePixmapCursor(dpy,csr,csr,&xcolor,&xcolor,1,1); 

	//this enables things like events, fullscreen, and sets the invisible cursor
	static XSetWindowAttributes swa = { .override_redirect = 1, .event_mask = ExposureMask | KeyPressMask };
	swa.colormap = cmap;
	swa.cursor = cursor;
	Window win = XCreateWindow(dpy, root, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask | CWOverrideRedirect | CWCursor, &swa);

	//this actually opens the window
	XMapWindow(dpy, win);

	//now we can do opengl calls!!!!
	GLXContext glc = glXCreateContext(dpy, vi, NULL, 1);

	#ifdef DEBUG
		if (glc == NULL) {
			return;
		}
	#endif

	glXMakeCurrent(dpy, win, glc);

	glClear(GL_COLOR_BUFFER_BIT);

	//oh yeah grab the keyboard
	XGrabKeyboard(dpy, win, true, GrabModeAsync, GrabModeAsync, CurrentTime);

	//create a floating point backing texture for a framebuffer
	GLuint textureA;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureA);
	glBindTexture(GL_TEXTURE_2D, textureA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, CANVAS_WIDTH, CANVAS_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, fbdata);

	//create a framebuffer we can render everything to
	GLuint fboA;
	glGenFramebuffers(1, &fboA);
	glBindFramebuffer(GL_FRAMEBUFFER, fboA);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, textureA, 0);

	// compile shader
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f, 1, &shader_frag, NULL);
	glCompileShader(f);

	#ifdef DEBUG
		GLint isCompiled = 0;
		glGetShaderiv(f, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(f, GL_INFO_LOG_LENGTH, &maxLength);

			char* error = malloc(maxLength);
			glGetShaderInfoLog(f, maxLength, &maxLength, error);
			printf("%s\n", error);

			exit(-10);
		}
	#endif

	// link shader
	GLuint p = glCreateProgram();
	glAttachShader(p,f);
	glLinkProgram(p);

	#ifdef DEBUG
		GLint isLinked = 0;
		glGetProgramiv(p, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(p, GL_INFO_LOG_LENGTH, &maxLength);

			char* error = malloc(maxLength);
			glGetProgramInfoLog(p, maxLength, &maxLength,error);
			printf("%s\n", error);

			exit(-10);
		}
	#endif

	glUseProgram(p);

	//switch to using our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fboA);

	// glFinish();
	glRecti(-1,-1,1,1);

	//blit our framebuffer to the screen
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboA);

	glBlitFramebuffer(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	static XEvent xev;
	while(1) {
		XNextEvent(dpy, &xev);

		//wait for escape key, then exit without glib :3
		if(xev.type == KeyPress && xev.xkey.keycode == 0x09) {
			//blackle mori no likey AT&T
			asm volatile(".intel_syntax noprefix");
			asm volatile("push 60");
			asm volatile("pop rax");
			asm volatile("xor edi, edi");
			asm volatile("syscall");
			asm volatile(".att_syntax prefix");
			__builtin_unreachable();
		}
	}
}
