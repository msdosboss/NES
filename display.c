//Compile this on linux
//gcc -o display display.c `sdl2-config --cflags --libs`
//Compile this on windows: 
//		gcc display.c -I"SDL2\include\SDL2" -L"SDL2\lib" -Wall -lmingw32 -lSDL2main -lSDL2 -o display.exe
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <unistd.h>
#include <threads.h>
#include "6502.h"
#include "fileio.h"
#include "log.h"
/*
#define WIDTH 1280
#define HEIGHT 1200
*/
#define WIDTH 1024
#define HEIGHT 960
#define COLLUMNS 256
#define ROWS 240
#define SIZE 200
#define FPS 60
#define SQUARESIZE WIDTH/COLLUMNS

void graggleIntro(SDL_Window *wind, SDL_Renderer *rend){
	SDL_Texture *graggle = IMG_LoadTexture(rend, "img/graggleNes.png");
	SDL_RenderCopy(rend, graggle, NULL, NULL);
	SDL_RenderPresent(rend);
	SDL_Delay(5000);
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

int displayLoop(SDL_Window *wind, SDL_Renderer *rend, struct CPU *cpu, struct PixelFrame *pixelFrame, struct PaletteEntry *palette){
	/* Main loop */
	graggleIntro(wind, rend);
	bool running = true;
	SDL_Texture *texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, COLLUMNS, ROWS);

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
							cpu->bus.joypad->buttonStatus |= LEFTDPAD;
							break;
						case SDL_SCANCODE_UP:
						case SDL_SCANCODE_W:
							cpu->bus.joypad->buttonStatus |= UPDPAD;
							break;
	    					case SDL_SCANCODE_D:
						case SDL_SCANCODE_RIGHT:
							cpu->bus.joypad->buttonStatus |= RIGHTDPAD;
							break;
						case SDL_SCANCODE_S:
						case SDL_SCANCODE_DOWN:
							cpu->bus.joypad->buttonStatus |= DOWNDPAD;
							break;
						case SDL_SCANCODE_M:
							cpu->bus.joypad->buttonStatus |= STARTBUTTON;
							break;
						case SDL_SCANCODE_N:
							cpu->bus.joypad->buttonStatus |= SELECTBUTTON;
							break;
						case SDL_SCANCODE_Z:
							cpu->bus.joypad->buttonStatus |= ABUTTON;
							break;
						case SDL_SCANCODE_X:
							cpu->bus.joypad->buttonStatus |= BBUTTON;
							break;
						case SDL_SCANCODE_SPACE:
							break;
					}
					break;
				case SDL_KEYUP:
	  				switch (event.key.keysym.scancode){
						case SDL_SCANCODE_A:
						case SDL_SCANCODE_LEFT:
							cpu->bus.joypad->buttonStatus &= ~LEFTDPAD;
							break;
						case SDL_SCANCODE_UP:
						case SDL_SCANCODE_W:
							cpu->bus.joypad->buttonStatus &= ~UPDPAD;
							break;
	    					case SDL_SCANCODE_D:
						case SDL_SCANCODE_RIGHT:
							cpu->bus.joypad->buttonStatus &= ~RIGHTDPAD;
							break;
						case SDL_SCANCODE_S:
						case SDL_SCANCODE_DOWN:
							cpu->bus.joypad->buttonStatus &= ~DOWNDPAD;
							break;
						case SDL_SCANCODE_M:
							cpu->bus.joypad->buttonStatus &= ~STARTBUTTON;
							break;
						case SDL_SCANCODE_N:
							cpu->bus.joypad->buttonStatus &= ~SELECTBUTTON;
							break;
						case SDL_SCANCODE_Z:
							cpu->bus.joypad->buttonStatus &= ~ABUTTON;
							break;
						case SDL_SCANCODE_X:
							cpu->bus.joypad->buttonStatus &= ~BBUTTON;
							break;						
					}
	  				break;
				default:
	  				break;
			}
		}

	int bufferFlag;
	if((cpu->processorStatus & 0b00010000) == 0){
		//char *str = malloc(sizeof(char) * 97);
		//struct Opcode opcodes[0x100];
		//createOpArray(opcodes);
		//cycleLog(cpu, cpu->opCodes[busRead(&(cpu->bus), cpu->PC)], str);	running cycle log will cause it to break currently due to double ppu read
		//printf("%s\n", str);
		int beforeNMI = cpu->bus.ppu->nmiInt;
		cpuLoop(cpu);
		int afterNMI = cpu->bus.ppu->nmiInt;
		if(beforeNMI != afterNMI){
			parseVram(cpu->bus.ppu, pixelFrame);
			bufferFlag = 1;
		}
		else{
			bufferFlag = 0;
		}
		//struct timespec req = {0, 50000L};
		//thrd_sleep(&req, NULL);
	}
	else{	
		//printf("CPU done\n");
	}

	if(bufferFlag){	//check if buff changed
		Uint32 *pixels = malloc(sizeof(Uint32) * ROWS * COLLUMNS);
		for(int i = 0; i < ROWS; i++){
			for(int j = 0; j < COLLUMNS; j++){
				//unsigned char val = frame->tiles[i / 8][j / 8].pixels[i % 8][j % 8];
				unsigned char val = pixelFrame->pixels[i][j];
				if(val != 255){
					pixels[j + i * COLLUMNS] = (palette[val].red << 24) | (palette[val].green << 16) | (palette[val].blue << 8) | 255;
				}
				else{
					pixels[j + i * COLLUMNS] = 0;
				}
			}
		}
		SDL_UpdateTexture(texture, NULL, pixels, sizeof(Uint32) * COLLUMNS);
		/* Draw to window and loop */
		//SDL_RenderClear(rend);
		SDL_RenderCopy(rend, texture, NULL, NULL);
		SDL_RenderPresent(rend);
		free(pixels);
		//SDL_Delay(1000/FPS);
		}
	}
	/* Release resources */
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(wind);
	freeCPU(cpu);
	free(palette);
	SDL_Quit();
	return 0;

}

int main(int argc, char* argv[]){

	if(argc < 2){
		printf("Please enter a rom to run");
		return 1;
	}

	struct CPU cpu = {0};

	struct PixelFrame pixelFrame;

	cpu.bus.rom = nesCartRead(argv[1]);

	initCPU(&cpu);

	struct PaletteEntry *palette = createPalette("palette.pal", 0);

	//parseChrRom(cpu.bus.ppu, &frame, 1);

	SDL_Window *wind = initDisplay();

	SDL_Renderer *rend = initRender(wind);

	displayLoop(wind, rend, &cpu, &pixelFrame, palette);
}
