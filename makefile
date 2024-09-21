all: display.c fileio.c 6502.c bus.c
	gcc -o main fileio.c 6502.c bus.c display.c opcode.c log.c `sdl2-config --cflags --libs`



clean:
	rm main
