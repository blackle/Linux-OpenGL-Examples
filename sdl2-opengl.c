#define GL_GLEXT_PROTOTYPES

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdint.h>

#include <SDL2/SDL.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "shader.h"

#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
// #define KEY_HANDLING
// #define DEBUG

static void on_render ()
{
	glRecti(-1,-1,1,1);
}

static void on_realize()
{
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
}

void _start() {
	asm volatile("sub $8, %rsp\n");

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_Window* mainwindow = SDL_CreateWindow(
		"",
		0,
		0,
		CANVAS_WIDTH,
		CANVAS_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN
	);

	SDL_GL_CreateContext(mainwindow);
	SDL_ShowCursor(false);
	on_realize();

	while (true) {
		SDL_Event Event;
		while (SDL_PollEvent(&Event)) {
			if (Event.type == SDL_QUIT
#if defined(KEY_HANDLING)
				|| (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE)
#endif
			) {
				asm volatile(".intel_syntax noprefix");
				asm volatile("push 231"); //exit_group
				asm volatile("pop rax");
				asm volatile("xor edi, edi");
				asm volatile("syscall");
				asm volatile(".att_syntax prefix");
				__builtin_unreachable();
			}
			if (Event.type == SDL_WINDOWEVENT) {
				switch(Event.window.event) {
					case SDL_WINDOWEVENT_EXPOSED:
						on_render();
						SDL_GL_SwapWindow(mainwindow);
					default:
						break;
				}
			}
		}
	}
	__builtin_unreachable();
}
