#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#ifdef _WIN32
#define _UNICODE
#endif
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shaders.h"

typedef struct _paddle
{
	float x;
	float y;
	float width;
	float height;
	int score;
	int up;
	int down;
	SDL_Rect sprite;
	SDL_Texture *tex;
	int noted;
} tpaddle;

typedef struct _ball
{
	float x;
	float y;
	float spd_x;
	float spd_y;
	float r;
	SDL_Rect sprite;
	SDL_Texture *tex;
	int vert_dir;
	int hort_dir;
} tball;

typedef tpaddle *paddleObj;
typedef tball *ballObj;

SDL_Surface *surface = NULL;
SDL_Window *window = NULL;
SDL_Surface *image = NULL;

void handleKeyboardEvents(SDL_Event * event, tpaddle *paddle_p1, tpaddle *paddle_p2, int *value);
void menuKeyEvents(SDL_Event * event);
void movePaddle(tpaddle *paddle);
void reset_game(tpaddle *paddle_p1, tpaddle *paddle_p2, tball *ball);

#ifdef _WIN32
int wmain(int argc, char *argv[])
{
#endif
#ifdef __linux__

int main(int argc, char *argv[])
{
#endif
	paddleObj paddle_p1 = malloc(sizeof(tpaddle));
	paddleObj paddle_p2 = malloc(sizeof(tpaddle));
	ballObj ball = malloc(sizeof(tball));
	char *p1_text_score = malloc(sizeof(char)*32);
	char *p2_text_score = malloc(sizeof(char)*32);
	Mix_Chunk *bounce1;
	Mix_Chunk *russiaWins;
	Mix_Chunk *americaWins;
	Mix_MasterVolume(75);
	int game_start = 0;
	int scored = 0;
	float mult_vert = 2.5f;
	float mult_hor = 2.5f;
	paddle_p1->x = 25.0f;
	paddle_p1->y = 150.0f;
	paddle_p1->width = 20.0f;
	paddle_p1->height = 100.0f;
	paddle_p1->score = 0;
	paddle_p1->sprite.x = (int)paddle_p1->x;
	paddle_p1->sprite.y = (int)paddle_p1->y;
	paddle_p1->sprite.w = (int)paddle_p1->width;
	paddle_p1->sprite.h = (int)paddle_p1->height;
	paddle_p1->up = 0;
	paddle_p1->down = 0;
	paddle_p1->noted = 0;

	paddle_p2->x = 750.0f;
	paddle_p2->y = 150.0f;
	paddle_p2->width = 20.0f;
	paddle_p2->height = 100.0f;
	paddle_p2->score = 0;
	paddle_p2->sprite.x = paddle_p2->x;
	paddle_p2->sprite.y = paddle_p2->y;
	paddle_p2->sprite.w = paddle_p2->width;
	paddle_p2->sprite.h = paddle_p2->height;
	paddle_p2->up = 0;
	paddle_p2->down = 0;
	paddle_p2->noted = 0;
	
	ball->x = 390.0f;
	ball->y = 190.0f;
	ball->spd_x = 2.5f;
	ball->spd_y = 2.0f;
	ball->r = 20;
	ball->sprite.x = ball->x;
	ball->sprite.y = ball->y;
	ball->sprite.w = ball->r;
	ball->sprite.h = ball->r;
	ball->vert_dir = 1;
	ball->hort_dir = 1;


	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL no pudo inicializarse. Error: %s\n", SDL_GetError());
		return -1;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL no pudo inicializar el mixer %s\n", Mix_GetError);
		SDL_Quit();
		return -1;
	}
	bounce1 = Mix_LoadWAV("bounce.wav");
	americaWins = Mix_LoadWAV("americaWins.wav");
	russiaWins = Mix_LoadWAV("russiaWin.wav");
	if (!bounce1)
	{
		printf("No se pudo cargar el sonido %s\n", Mix_GetError);
		Mix_CloseAudio();
		SDL_Quit();
		return -1;
	}
	if (TTF_Init() == -1)
	{
		printf("Error al iniciar las fuentes %s\n", TTF_GetError);
		SDL_Quit();
		return -1;
	}

	window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 400, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf("No se pudo crear la ventana SDL. Error: %s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		printf("Error al crear el renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		IMG_Quit();
		SDL_Quit();
		return -1;
	}

	// Superficie para dibujar
	surface = SDL_GetWindowSurface(window);
	if (!surface)
	{
		printf("No se pudo obtener la superficie de la ventana. Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}
	SDL_Surface *img_surf = IMG_Load("background.png");
	if (!img_surf)
	{
		printf("Error al cargar la imagen %s\n", IMG_GetError);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		IMG_Quit();
		SDL_Quit();
		return -1;
	}
	SDL_Texture *img_tex = SDL_CreateTextureFromSurface(renderer, img_surf);
	img_surf = IMG_Load("menu_bkg.png");
	SDL_Texture *menu_bkg = SDL_CreateTextureFromSurface(renderer, img_surf);
	img_surf = IMG_Load("PaddleSovietico.png");
	paddle_p1->tex = SDL_CreateTextureFromSurface(renderer, img_surf);
	img_surf = IMG_Load("PaddleAmericano.png");
	paddle_p2->tex = SDL_CreateTextureFromSurface(renderer, img_surf);
	img_surf = IMG_Load("ball.png");
	ball->tex = SDL_CreateTextureFromSurface(renderer, img_surf);
	SDL_FreeSurface(img_surf);
	
	if (!img_tex)
	{
		printf("Error al crear la textura %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		IMG_Quit();
		SDL_Quit();
		return -1;
	}

	TTF_Font *font = TTF_OpenFont("Strengthen.ttf", 50);
	if (font == NULL)
	{
		printf("Error al cargar nuestra fuente %s\n", TTF_GetError);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		IMG_Quit();
		SDL_Quit();
		return -1;
	}
	SDL_Color color = {255, 255, 255, 255};
	img_surf = TTF_RenderText_Solid(font, "hello", color);
	SDL_Texture *score_tex = SDL_CreateTextureFromSurface(renderer, img_surf);

	SDL_UpdateWindowSurface(window);
	SDL_Event event;
	int menu = 1;

	while (menu)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				SDL_DestroyWindow(window);
				SDL_Quit();
				return 0;
			}
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					SDL_DestroyWindow(window);
					SDL_Quit();
					return 0;
				}
				if (event.key.keysym.sym == SDLK_RETURN)
				{
					menu = 0;
				}
			}
		}

		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, menu_bkg, NULL, NULL);
		
		SDL_RenderPresent(renderer);
	}

	while (true)
	{
		const int windowHeight = 400; // Altura de la ventana
		const int paddleHeight = 100; // Altura de la barra (paddle)
		
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				SDL_DestroyWindow(window);
				SDL_Quit();
				return 0;
			}
			handleKeyboardEvents(&event, paddle_p1, paddle_p2, &game_start); // Llamar a la función para manejar eventos del teclado
		}

		// Lógica del juego
		if (game_start)
		{
			ball->x += ball->spd_x*ball->hort_dir;
			ball->y += ball->spd_y*ball->vert_dir;
		}
		ball->sprite.x = ball->x;
		ball->sprite.y = ball->y;

		if (ball->y >= 380 || ball->y <= 0)
		{
			ball->vert_dir = -ball->vert_dir;
			Mix_PlayChannel(-1, bounce1, 0);
		}


		if (ball->x <= 10 && (ball->y <= paddle_p1->y || ball->y >= paddle_p1->y + paddle_p1->height))
		{
			reset_game(paddle_p1, paddle_p2, ball);
			paddle_p2->score++;
			paddle_p1->noted = 1;
			Mix_PlayChannel(-1, americaWins, 0);
			printf("¡Jugador 2 anotó un punto! Puntuación: Jugador 1: %d, Jugador 2: %d\n", paddle_p1->score, paddle_p2->score);

			// Incrementar la velocidad de la pelota después de anotar un punto
		 // ball->spd_y -= 0.2f; // Incremento en Y
			printf("Velocidad X: %f, Velocidad Y: %f\n", ball->spd_x, ball->spd_y);
			game_start = 0;
		}

		// Si la pelota cruza la línea 730 y no está cerca del paddle derecho, se reinicia
		if (ball->x >= 780 && (ball->y <= paddle_p2->y || ball->y >= paddle_p2->y + paddle_p2->height))
		{
			Mix_PlayChannel(-1, russiaWins, 0);
			reset_game(paddle_p1, paddle_p2, ball);
			game_start = 0;
			paddle_p1->score++;
			paddle_p2->noted = 1;
			printf("¡Jugador 1 anotó un punto! Puntuación: Jugador 1: %d, Jugador 2: %d\n", paddle_p1->score, paddle_p2->score);

			// Incrementar la velocidad de la pelota después de anotar un punto
			printf("Velocidad X: %f, Velocidad Y: %f\n", ball->spd_x, ball->spd_y);
		}

		if (ball->x >= paddle_p1->x && ball->x < paddle_p1->width+ball->r && ball->y + ball->r >= paddle_p1->y && ball->y <= paddle_p1->y + paddle_p1->height)
		{
			//ball->spd_x = -ball->spd_x;
			ball->hort_dir = -ball->hort_dir;
			Mix_PlayChannel(-1, bounce1, 0);
			if (!paddle_p1->noted && !paddle_p2->noted && scored)
			{
				ball->spd_x /= mult_hor;
				ball->spd_y /= mult_vert;
				scored = 0;
				ball->x += 8.0f;
			}
			if (paddle_p1->noted)
			{
				ball->spd_x *= mult_hor;
				ball->spd_y *= mult_vert;
				paddle_p1->noted = 0;
				scored = 1;
			}
		}
		if (ball->x >= paddle_p2->x - ball->r && ball->x <= paddle_p2->x && ball->y + ball->r >= paddle_p2->y && ball->y <= paddle_p2->y + paddle_p2->height)
		{
			ball->hort_dir = -ball->hort_dir;
			Mix_PlayChannel(-1, bounce1, 0);
			if (!paddle_p1->noted && !paddle_p2->noted && scored)
			{
				ball->spd_x /= mult_hor;
				ball->spd_y /= mult_vert;
				scored = 0;
				ball->x -= 8.0f;
			}
			if (paddle_p2->noted)
			{
				ball->spd_x *= mult_vert;
				ball->spd_y *= mult_hor;
				paddle_p2->noted = 0;
				scored = 1;
			}
		}


		if (paddle_p1->y > windowHeight)
		{
			paddle_p1->y = -paddleHeight; // Si la barra sale por abajo, aparece por arriba
		}
		else if (paddle_p1->y < -paddleHeight)
		{
			paddle_p1->y = windowHeight; // Si la barra sale por arriba, aparece por abajo
		}

		if (paddle_p2->y > windowHeight)
		{
			paddle_p2->y = -paddleHeight; // Lo mismo para la segunda barra
		}
		else if (paddle_p2->y < -paddleHeight)
		{
			paddle_p2->y = windowHeight; // Si la barra sale por arriba, aparece por abajo
		}
		// En el bucle principal:
		movePaddle(paddle_p1);
		movePaddle(paddle_p2);
		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, img_tex, NULL, NULL);
		
		SDL_RenderFillRect(renderer, &(paddle_p2->sprite));
		
		SDL_RenderCopy(renderer, ball->tex, NULL, &ball->sprite);
		SDL_RenderCopy(renderer, paddle_p1->tex, NULL, &paddle_p1->sprite);
		SDL_RenderCopy(renderer, paddle_p2->tex, NULL, &paddle_p2->sprite);
		
		SDL_Rect dstRect = {830, 50, 30, 50};
		sprintf(p1_text_score, "%d", paddle_p1->score);
		img_surf = TTF_RenderText_Solid(font, p1_text_score, color);
		score_tex = SDL_CreateTextureFromSurface(renderer, img_surf);
		SDL_RenderCopy(renderer, score_tex, NULL, &dstRect);
		dstRect.x = 940;
		sprintf(p2_text_score, "%d", paddle_p2->score);
		img_surf = TTF_RenderText_Solid(font, p2_text_score, color);
		score_tex = SDL_CreateTextureFromSurface(renderer, img_surf);
		SDL_RenderCopy(renderer, score_tex, NULL, &dstRect);
		
		
		SDL_RenderPresent(renderer);
		
		SDL_Delay(10);
	}
	SDL_FreeSurface(image);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void handleKeyboardEvents(SDL_Event * event, tpaddle *paddle_p1, tpaddle *paddle_p2, int *game_start)
{
	if (event->type == SDL_KEYDOWN)
	{
		*game_start = 1;
		if (event->key.keysym.sym == SDLK_w)
		{
			paddle_p1->up = 1;
		}
		if (event->key.keysym.sym == SDLK_s)
		{
			paddle_p1->down = 1;
		}
		if (event->key.keysym.sym == SDLK_UP)
		{
			paddle_p2->up = 1;
		}
		if (event->key.keysym.sym == SDLK_DOWN)
		{
			paddle_p2->down = 1;
		}

		if (event->key.keysym.sym == SDLK_ESCAPE)
		{
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
	}

	if (event->type == SDL_KEYUP)
	{
		if (event->key.keysym.sym == SDLK_w)
		{
			paddle_p1->up = 0;
		}
		if (event->key.keysym.sym == SDLK_s)
		{
			paddle_p1->down = 0;
		}
		if (event->key.keysym.sym == SDLK_UP)
		{
			paddle_p2->up = 0;
		}
		if (event->key.keysym.sym == SDLK_DOWN)
		{
			paddle_p2->down = 0;
		}
	}

}

void movePaddle(tpaddle *paddle)
{

	if (paddle->up)
	{
		// Mover la raqueta izquierda hacia arriba
		paddle->y -= 4.0f;
		paddle->sprite.y = paddle->y;
	}
	if (paddle->down)
	{
		// Mover la raqueta izquierda hacia abajo
		paddle->y += 4.0f;
		paddle->sprite.y = paddle->y;
	}
}

void reset_game(tpaddle *paddle_p1, tpaddle *paddle_p2, tball *ball)
{
	ball->x = 390;
	ball->y = 190;
	paddle_p1->y = 150.0f;
	paddle_p2->y = 150.0f;
	paddle_p1->sprite.y = paddle_p1->y;
	paddle_p2->sprite.y = paddle_p2->y;
	ball->spd_x = 2.5f;
	ball->spd_y = 2.0f;
}
