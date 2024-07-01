//Compile this on linux
//gcc -o display display.c `sdl2-config --cflags --libs`
//Compile this on windows: 
//		gcc display.c -I"SDL2\include\SDL2" -L"SDL2\lib" -Wall -lmingw32 -lSDL2main -lSDL2 -o display.exe
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <unistd.h>
#include <time.h>
#include "6502.h"
#include "fileio.h"
#define COLLUMNS 32
#define ROWS 32
#define WIDTH 640
#define HEIGHT 640
#define SIZE 200
#define FPS 60
#define SQUARESIZE 20

void rendColor(SDL_Renderer *rend, unsigned char val){
	switch(val){
		case 0x00:
			SDL_SetRenderDrawColor(rend, 0, 0, 0, 127);
			break;

		case 0x01:
			SDL_SetRenderDrawColor(rend, 255, 255, 255, 127);
			break;
		
		default:
			SDL_SetRenderDrawColor(rend, 255, 0, 0, 127);
			break;
	}
}

SDL_Window *initDisplay(){
	/* Initializes the timer, audio, video, joystick,
	haptic, gamecontroller and events subsystems */
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("Error initializing SDL: %s\n", SDL_GetError());
		return 0;
	}
	/* Create a window */
	SDL_Window* wind = SDL_CreateWindow("Hello Platformer!",
				      SDL_WINDOWPOS_CENTERED,
				      SDL_WINDOWPOS_CENTERED,
				      WIDTH, HEIGHT, 0);
	if(!wind){
		printf("Error creating window: %s\n", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	return wind;
}

SDL_Renderer *initRender(SDL_Window *wind){
	/* Create a renderer */
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer* rend = SDL_CreateRenderer(wind, -1, render_flags);
	if (!rend){
		printf("Error creating renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(wind);
		SDL_Quit();
		return 0;
	}

	return rend;
}

int displayLoop(SDL_Window *wind, SDL_Renderer *rend, struct CPU *cpu){
	/* Main loop */
	bool running = true, left_pressed = false, right_pressed = false;
	float x_pos = (WIDTH-SIZE)/2, y_pos = (HEIGHT-SIZE)/2, x_change = 0, y_change = 0;
	SDL_Rect **rects;
	rects = malloc(sizeof(SDL_Rect *) * COLLUMNS);
	for(int i = 0; i < COLLUMNS; i++){
		rects[i] = malloc(sizeof(SDL_Rect) * ROWS);
		for(int j = 0; j < ROWS; j++){
			rects[i][j] = (SDL_Rect){i * SQUARESIZE, j * SQUARESIZE, SQUARESIZE, SQUARESIZE};
		}
	}
	SDL_Rect rect = {(int) x_pos, (int) y_pos, SIZE, SIZE};
	SDL_Event event;
	while (running){
	/* Process events */
		while (SDL_PollEvent(&event)){
			switch (event.type){
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
	  				switch (event.key.keysym.scancode){
						case SDL_SCANCODE_A:
						case SDL_SCANCODE_LEFT:
							left_pressed = true;
							break;
	    					case SDL_SCANCODE_D:
						case SDL_SCANCODE_RIGHT:
							right_pressed = true;
							break;
						default:
							break;
					}
					break;
				case SDL_KEYUP:
	  				switch (event.key.keysym.scancode){
						case SDL_SCANCODE_A:
						case SDL_SCANCODE_LEFT:
							left_pressed = false;
							break;
						case SDL_SCANCODE_D:
						case SDL_SCANCODE_RIGHT:
							right_pressed = false;
							break;
						default:
							break;
					}
	  				break;
				default:
	  				break;
			}
	}
	/* Clear screen */
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	SDL_RenderClear(rend);
	/* Move the rectangle */
	x_change = right_pressed - left_pressed;
	//y_change = up_pressed - down_pressed;
	x_pos += x_change;
	y_pos += y_change;
	if (x_pos <= 0)
		x_pos = 0;
	if (x_pos >= WIDTH - rect.w)
		x_pos = WIDTH - rect.w;
	if (y_pos <= 0)
		y_pos = 0;
		rect.x = (int) x_pos;
		rect.y = (int) y_pos;
	/* Draw the rectangle */
	//SDL_SetRenderDrawColor(rend, 255, 0, 255, 127);
	//SDL_RenderFillRect(rend, &rect);
	for(int i = 0; i < COLLUMNS; i++){
		fflush(stdout);
		for(int j = 0; j < ROWS; j++){
			
			/*if(((j + 1) % 2 != 0 && (i + 1) % 2 != 0) || ((j + 1) % 2 == 0 && (i + 1) % 2 == 0)){
				SDL_SetRenderDrawColor(rend, 255, 0, 255, 127);
			}
			else{
				SDL_SetRenderDrawColor(rend, 255, 0, 0, 127);
			}*/
			rendColor(rend, cpu->memMap[0x200 + (i * ROWS + j)]);
			SDL_RenderFillRect(rend, &rects[i][j]);
			//SDL_RenderPresent(rend);
		}
	}
	if((cpu->processorStatus & 0b00010000) == 0){
		time_t t;
		srand((unsigned) time(&t));
		cpu->memMap[0xfe] = rand() % 500;	//random number genorator for fe
		cpuLoop(cpu);
		sleep(1);
	}
	else{
		printf("CPU done\n");
	}

	/* Draw to window and loop */
	SDL_RenderPresent(rend);
	SDL_Delay(1000/FPS);
	}
	/* Release resources */
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(wind);
	SDL_Quit();
	return 0;

}

int main(int argc, char* argv[]){

		//char* rom = loadRom("snake.rom");	//later I will load the rom from file but the loadRom function doesnt have it in the right form facator

	unsigned char instructions[] = {0x20, 0x06, 0x06, 0x20, 0x38, 0x06, 0x20, 0x0d, 0x06, 0x20, 0x2a, 0x06, 0x60, 0xa9, 0x02, 0x85,
    0x02, 0xa9, 0x04, 0x85, 0x03, 0xa9, 0x11, 0x85, 0x10, 0xa9, 0x10, 0x85, 0x12, 0xa9, 0x0f, 0x85,
    0x14, 0xa9, 0x04, 0x85, 0x11, 0x85, 0x13, 0x85, 0x15, 0x60, 0xa5, 0xfe, 0x85, 0x00, 0xa5, 0xfe,
    0x29, 0x03, 0x18, 0x69, 0x02, 0x85, 0x01, 0x60, 0x20, 0x4d, 0x06, 0x20, 0x8d, 0x06, 0x20, 0xc3,
    0x06, 0x20, 0x19, 0x07, 0x20, 0x20, 0x07, 0x20, 0x2d, 0x07, 0x4c, 0x38, 0x06, 0xa5, 0xff, 0xc9,
    0x77, 0xf0, 0x0d, 0xc9, 0x64, 0xf0, 0x14, 0xc9, 0x73, 0xf0, 0x1b, 0xc9, 0x61, 0xf0, 0x22, 0x60,
    0xa9, 0x04, 0x24, 0x02, 0xd0, 0x26, 0xa9, 0x01, 0x85, 0x02, 0x60, 0xa9, 0x08, 0x24, 0x02, 0xd0,
    0x1b, 0xa9, 0x02, 0x85, 0x02, 0x60, 0xa9, 0x01, 0x24, 0x02, 0xd0, 0x10, 0xa9, 0x04, 0x85, 0x02,
    0x60, 0xa9, 0x02, 0x24, 0x02, 0xd0, 0x05, 0xa9, 0x08, 0x85, 0x02, 0x60, 0x60, 0x20, 0x94, 0x06,
    0x20, 0xa8, 0x06, 0x60, 0xa5, 0x00, 0xc5, 0x10, 0xd0, 0x0d, 0xa5, 0x01, 0xc5, 0x11, 0xd0, 0x07,
    0xe6, 0x03, 0xe6, 0x03, 0x20, 0x2a, 0x06, 0x60, 0xa2, 0x02, 0xb5, 0x10, 0xc5, 0x10, 0xd0, 0x06,
    0xb5, 0x11, 0xc5, 0x11, 0xf0, 0x09, 0xe8, 0xe8, 0xe4, 0x03, 0xf0, 0x06, 0x4c, 0xaa, 0x06, 0x4c,
    0x35, 0x07, 0x60, 0xa6, 0x03, 0xca, 0x8a, 0xb5, 0x10, 0x95, 0x12, 0xca, 0x10, 0xf9, 0xa5, 0x02,
    0x4a, 0xb0, 0x09, 0x4a, 0xb0, 0x19, 0x4a, 0xb0, 0x1f, 0x4a, 0xb0, 0x2f, 0xa5, 0x10, 0x38, 0xe9,
    0x20, 0x85, 0x10, 0x90, 0x01, 0x60, 0xc6, 0x11, 0xa9, 0x01, 0xc5, 0x11, 0xf0, 0x28, 0x60, 0xe6,
    0x10, 0xa9, 0x1f, 0x24, 0x10, 0xf0, 0x1f, 0x60, 0xa5, 0x10, 0x18, 0x69, 0x20, 0x85, 0x10, 0xb0,
    0x01, 0x60, 0xe6, 0x11, 0xa9, 0x06, 0xc5, 0x11, 0xf0, 0x0c, 0x60, 0xc6, 0x10, 0xa5, 0x10, 0x29,
    0x1f, 0xc9, 0x1f, 0xf0, 0x01, 0x60, 0x4c, 0x35, 0x07, 0xa0, 0x00, 0xa5, 0xfe, 0x91, 0x00, 0x60,
    0xa6, 0x03, 0xa9, 0x00, 0x81, 0x10, 0xa2, 0x00, 0xa9, 0x01, 0x81, 0x10, 0x60, 0xa2, 0x00, 0xea,
    0xea, 0xca, 0xd0, 0xfb, 0x60};

	const int instructionsLen = 309;	//later I am going to automatically find the len of instuctions but for now it is manual

	struct CPU cpu = {0};

	initCPU(&cpu, instructions, instructionsLen);

	printf("PC = %x\n", *(cpu.programCounter));

	SDL_Window *wind = initDisplay();

	SDL_Renderer *rend = initRender(wind);

	displayLoop(wind, rend, &cpu);
}
