#ifdef _WIN32
#define _UNICODE
#endif 
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shaders.h"

typedef struct _winConfig
{
	int screenWidth;
	int screenHeight;
	int newWidth;
	int newHeight;
	float aspectRatio;
	float aspect;
} twinConfig;
typedef struct _engineConfs
{
	SDL_Window *window;
	SDL_GLContext glContext;
	GLenum glewError;
	bool quit;
}tengineConfs;

typedef twinConfig *winConfig;
typedef tengineConfs *engineConfs;


/*functions*/
void prepareWin(winConfig windowConfs);
void fixAspectRatio(winConfig windowConfs);
int prepareEngine(engineConfs engineConfs, winConfig windowConfs);

#ifdef _WIN32
int wmain(int argc, char *argv[]) {
#endif 
#ifdef __linux__
int main(int argc, char *argv[]) {
#endif 
	/* Hacer declaraciones */
	winConfig windowConfs = malloc(sizeof(twinConfig));
	engineConfs engineConfs = malloc(sizeof(tengineConfs));
	GLuint vertShader;
	GLuint fragShader;
	GLuint shaderProg;
	GLuint VBO, VAO;
	float vertices[] =
	{
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};
	prepareWin(windowConfs);
	if (prepareEngine(engineConfs, windowConfs) == -1)
	{
		return 0;
	}
	/* prepare shaders */
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertShader);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragShader);
	/* link shaders */
	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertShader);
	glAttachShader(shaderProg, fragShader);
	glLinkProgram(shaderProg);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	/* define vbo and vao */
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	/* set up vbo and vao */
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/* unbind vao */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	/* Bucle principal*/
	while (!engineConfs->quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
				case SDL_QUIT:
					engineConfs->quit = true;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_q)
					{
						engineConfs->quit = true;
					}
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						windowConfs->newWidth = event.window.data1;
						windowConfs->newHeight = event.window.data2;
						fixAspectRatio(windowConfs);
					}
			}
		}
		
		/* Limpiar el búfer de color*/
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		/* draw */
		glUseProgram(shaderProg);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		/* Intercambiar los búferes*/
		SDL_GL_SwapWindow(engineConfs->window);
	}

	/* Liberar recursos*/
	SDL_GL_DeleteContext(engineConfs->glContext);
	SDL_DestroyWindow(engineConfs->window);
	SDL_Quit();
	free(windowConfs);
	free(engineConfs);

	return 0;
}

void prepareWin(winConfig windowConfs)
{
	windowConfs->aspectRatio = 1;
	windowConfs->screenWidth = 320;
	windowConfs->screenHeight = 180;
	windowConfs->newWidth = 0;
	windowConfs->newHeight = 0;
	windowConfs->aspect = 0;
}

void fixAspectRatio(winConfig windowConfs)
{
	windowConfs->screenWidth = windowConfs->newWidth;
	windowConfs->screenHeight = windowConfs->newHeight;

	glViewport(0, 0, windowConfs->screenWidth, windowConfs->screenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (windowConfs->screenWidth >= windowConfs->screenHeight * windowConfs->aspectRatio)
	{
		windowConfs->aspect = (float) windowConfs->screenWidth / (float) windowConfs->screenHeight;
		glOrtho(-windowConfs->aspect * windowConfs->aspectRatio, windowConfs->aspect * windowConfs->aspectRatio, -1.0, 1.0, -1.0, 1.0);
	}
	else
	{
		windowConfs->aspect = (float)windowConfs->screenHeight / (float)windowConfs->screenWidth;
		glOrtho(-1.0, 1.0, -windowConfs->aspect / windowConfs->aspectRatio, windowConfs->aspect / windowConfs->aspectRatio, -1.0, 1.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	printf("Pantalla redimensionada\n");
}

int prepareEngine(engineConfs engineConfs, winConfig windowConfs)
{
	engineConfs->quit = false;


	/* Inicializar SDL*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Error al inicializar SDL: %s\n", SDL_GetError());
		return -1;
	}

	/* Crear ventana*/
	engineConfs->window = SDL_CreateWindow
		(
			"CPONG",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			windowConfs->screenWidth, windowConfs->screenHeight,
			SDL_WINDOW_OPENGL |SDL_WINDOW_RESIZABLE
		);

	if (!engineConfs->window) {
		fprintf(stderr, "Error al crear la ventana: %s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	/* Crear contexto OpenGL*/
	engineConfs->glContext = SDL_GL_CreateContext(engineConfs->window);
	if (!engineConfs->glContext) {
		fprintf(stderr, "Error al crear el contexto OpenGL: %s\n", SDL_GetError());
		SDL_DestroyWindow(engineConfs->window);
		SDL_Quit();
		return -1;
	}

	/* Inicializar GLEW*/
	engineConfs->glewError = glewInit();
	if (engineConfs->glewError != GLEW_OK) {
		fprintf(stderr, "Error al inicializar GLEW: %s\n", glewGetErrorString(engineConfs->glewError));
		SDL_GL_DeleteContext(engineConfs->glContext);
		SDL_DestroyWindow(engineConfs->window);
		SDL_Quit();
		return -1;
	}

	/* Configurar el color de fondo*/
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return 1;
}
