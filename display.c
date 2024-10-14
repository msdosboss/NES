//Compile this on linux
//gcc -o display display.c `sdl2-config --cflags --libs`
//Compile this on windows: 
//		gcc display.c -I"SDL2\include\SDL2" -L"SDL2\lib" -Wall -lmingw32 -lSDL2main -lSDL2 -o display.exe
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
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

void rendColor(SDL_Renderer *rend, unsigned char val, struct PaletteEntry *palette){
	
	if(val != 255){
		SDL_SetRenderDrawColor(rend, palette[val].red, palette[val].green, palette[val].blue, 127);
	}
	else{	
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	}
	/*switch(val){
		case 0x00:
			SDL_SetRenderDrawColor(rend, palette[0x01].red, palette[0x01].blue, palette[0x01].green, 127);
			break;

		case 0x01:
			SDL_SetRenderDrawColor(rend, palette[0x23].red, palette[0x23].blue, palette[0x23].green, 127);
			break;

		case 0x02:
			SDL_SetRenderDrawColor(rend, palette[0x27].red, palette[0x27].blue, palette[0x27].green, 127);
			break;

		case 0x03:
			SDL_SetRenderDrawColor(rend, palette[0x30].red, palette[0x30].blue, palette[0x30].green, 127);
			break;
		
		default:
			SDL_SetRenderDrawColor(rend, 0, 0, 0, 127);
			break;
	}*/
}

void loadBuffer(struct Frame *frame, unsigned char **buffer){
	for(int i = 0; i < COLLUMNS; i++){
		for(int j = 0; j < ROWS; j++){
			buffer[j][i] = frame->tiles[j / 8][i / 8].pixels[j % 8][i % 8];
		}
	}
	/*for(int i = 0; i < ROWS * COLLUMNS; i++){
		buffer[i] = busRead(&(cpu->bus), 0x200 + i);	//This cant work with a 256x240 display becuase 256 * 240 = 61440. The bus does not have that much memory
	}*/
}

int checkBuffer(struct Frame *frame, unsigned char **buffer){
	for(int i = 0; i < COLLUMNS; i++){
		for(int j = 0; j < ROWS; j++){
			if(buffer[j][i] != frame->tiles[j / 8][i / 8].pixels[j % 8][i % 8]){
				return 1;
			}
		}
	}
	/*for(int i = 0; i < ROWS * COLLUMNS; i++){
		if(buffer[i] != busRead(&(cpu->bus), 0x200 + i)){
			return 1;
		}
	}*/
	return 0;
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

int displayLoop(SDL_Window *wind, SDL_Renderer *rend, struct CPU *cpu, struct Frame *frame, struct PaletteEntry *palette, unsigned char **buffer){
	/* Main loop */
	bool running = true, left_pressed = false, right_pressed = false, space_pressed = false;
	float x_pos = (WIDTH-SQUARESIZE)/2, y_pos = (HEIGHT-SQUARESIZE)/2, x_change = 0, y_change = 0;
	SDL_Rect **rects;
	rects = malloc(sizeof(SDL_Rect *) * COLLUMNS);
	for(int i = 0; i < COLLUMNS; i++){
		rects[i] = malloc(sizeof(SDL_Rect) * ROWS);
		for(int j = 0; j < ROWS; j++){
			rects[i][j] = (SDL_Rect){i * SQUARESIZE, j * SQUARESIZE, SQUARESIZE, SQUARESIZE};
		}
	}
	SDL_Rect rect = {(int) x_pos, (int) y_pos, SQUARESIZE, SQUARESIZE};
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
							busWrite(&(cpu->bus), 0xff, 0x61);
							printf("button pressed\n");
							break;
						case SDL_SCANCODE_UP:
						case SDL_SCANCODE_W:
							busWrite(&(cpu->bus), 0xff, 0x77);
							left_pressed = true;
							printf("button pressed\n");
							break;
	    					case SDL_SCANCODE_D:
						case SDL_SCANCODE_RIGHT:
							busWrite(&(cpu->bus), 0xff, 0x64);
							right_pressed = true;
							printf("button pressed\n");
							break;
						case SDL_SCANCODE_S:
						case SDL_SCANCODE_DOWN:
							busWrite(&(cpu->bus), 0xff, 0x73);
							printf("button pressed\n");
						case SDL_SCANCODE_SPACE:
							space_pressed = true;
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

	//unsigned char buffer[ROWS][COLLUMNS];
	int bufferFlag;
	if((cpu->processorStatus & 0b00010000) == 0){
		//bufferFlag = checkBuffer(frame, buffer);
		busWrite(&(cpu->bus), 0xfe, rand() % 500);	//random number genorator for fe
		char *str = malloc(sizeof(char) * 93);
		struct Opcode opcodes[0x100];
		createOpArray(opcodes);
		cycleLog(cpu, opcodes[busRead(&(cpu->bus), cpu->PC)], str);
		printf("%s\n", str);
		unsigned char opCode = busRead(&(cpu->bus), cpu->PC);
		int beforeNMI = cpu->bus.ppu->nmiInt;
		cpuLoop(cpu);
		int afterNMI = cpu->bus.ppu->nmiInt;
		if(beforeNMI != afterNMI){
			parseVram(cpu->bus.ppu, frame);
			bufferFlag = 1;
			//cpu->bus.ppu->nmiInt = 0;
		}
		else{
			bufferFlag = 0;
		}
		//loadBuffer(frame, buffer);
		//struct timespec req = {0, 50000L};
		//thrd_sleep(&req, NULL);
	}
	else{	
		//printf("CPU done\n");
	}

	if(bufferFlag){	//check if buff changed
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
		for(int i = 0; i < ROWS; i++){
			for(int j = 0; j < COLLUMNS; j++){
				
				/*if(((j + 1) % 2 != 0 && (i + 1) % 2 != 0) || ((j + 1) % 2 == 0 && (i + 1) % 2 == 0)){
					SDL_SetRenderDrawColor(rend, 255, 0, 255, 127);
				}
				else{
					SDL_SetRenderDrawColor(rend, 255, 0, 0, 127);
				}*/
				//rendColor(rend, busRead(&(cpu->bus), 0x200 + (i * ROWS + j)));
				rendColor(rend, frame->tiles[i / 8][j / 8].pixels[i % 8][j % 8], palette);
				SDL_RenderFillRect(rend, &rects[j][i]);
				//SDL_RenderPresent(rend);
			}
		}
		/* Draw to window and loop */
		SDL_RenderPresent(rend);
		SDL_Delay(1000/FPS);
		}
	}
	/* Release resources */
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(wind);
	SDL_Quit();
	return 0;

}

unsigned char **initBuffer(){
	unsigned char **buffer;
	buffer = malloc(sizeof(unsigned char *) * ROWS);
	for(int i = 0; i < ROWS; i++){
		buffer[i] = malloc(sizeof(unsigned char) * COLLUMNS);
		memset(buffer[i], 0, sizeof(unsigned char) * COLLUMNS);
	}
	return buffer;
}

int main(int argc, char* argv[]){

	if(argc < 2){
		printf("Please enter a rom to run");
		return 1;
	}

	struct CPU cpu = {0};

	cpu.bus.rom = nesCartRead(argv[1]);

	initCPU(&cpu, cpu.bus.rom.prgRom, cpu.bus.rom.prgRomLen);

	unsigned char **buffer = initBuffer();

	struct Frame frame = createFrame();

	struct PaletteEntry *palette = createPalette("palette.pal", 0);

	parseChrRom(cpu.bus.ppu, &frame, 0);

	SDL_Window *wind = initDisplay();

	SDL_Renderer *rend = initRender(wind);

	displayLoop(wind, rend, &cpu, &frame, palette, buffer);
}
