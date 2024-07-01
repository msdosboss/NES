all: display.c fileio.c 6502.c
	gcc -o main fileio.c 6502.c display.c `sdl2-config --cflags --libs`

clean:
	rm main
