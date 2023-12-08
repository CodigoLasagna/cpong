#ifdef _WIN32
#define _UNICODE
#endif 
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <stdlib.h> /* Se añadió la inclusión para malloc y free*/
#include <stdbool.h>

typedef struct _winConfig
{
	int screenWidth;
	int screenHeight;
	int newWidth;
	int newHeight;
	float aspectRatio;
	float aspect;
} twinConfig;

typedef twinConfig *winConfig;


/*functions*/
void prepareWin(winConfig windowConfs);
void fixAspectRatio(winConfig windowConfs);

#ifdef _WIN32
int wmain(int argc, char *argv[]) {
#endif 
#ifdef __linux__
int main(int argc, char *argv[]) {
#endif 
	/* Hacer declaraciones */
	SDL_Window *window;
	SDL_GLContext glContext;
	winConfig mainWinConfig = malloc(sizeof(twinConfig));
	GLenum glewError;
	bool quit = false;
	prepareWin(mainWinConfig);


	/* Inicializar SDL*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Error al inicializar SDL: %s\n", SDL_GetError());
		return -1;
	}

	/* Configurar OpenGL*/
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	/* Crear ventana*/
	window = SDL_CreateWindow
		(
			"CPONG",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			mainWinConfig->screenWidth, mainWinConfig->screenHeight,
			SDL_WINDOW_OPENGL |SDL_WINDOW_RESIZABLE
		);

	if (!window) {
		fprintf(stderr, "Error al crear la ventana: %s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	/* Crear contexto OpenGL*/
	glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		fprintf(stderr, "Error al crear el contexto OpenGL: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	/* Inicializar GLEW*/
	glewError = glewInit();
	if (glewError != GLEW_OK) {
		fprintf(stderr, "Error al inicializar GLEW: %s\n", glewGetErrorString(glewError));
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	/* Configurar el color de fondo*/
	glClearColor(0.0f, 0.5f, 0.2f, 1.0f);

	/* Bucle principal*/
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_q)
					{
						quit = true;
					}
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						fixAspectRatio(mainWinConfig);
					}
			}
		}

		/* Limpiar el búfer de color*/
		glClear(GL_COLOR_BUFFER_BIT);

		/* Aquí puedes agregar el código para renderizar tu contenido*/

		/* Intercambiar los búferes*/
		SDL_GL_SwapWindow(window);
	}

	/* Liberar recursos*/
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void prepareWin(winConfig windowConfs)
{
	windowConfs->aspectRatio = 20;
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
