OBJS = ppu.o PPURegisters/addr.o PPURegisters/controller.o PPURegisters/mask.o PPURegisters/status.o

#add -pg for performance logging
all: display.c fileio.c 6502.c bus.c $(OBJS) display.c opcode.c log.c
	gcc -pg -o main fileio.c 6502.c bus.c joypad.c $(OBJS) display.c opcode.c log.c `sdl2-config --cflags --libs` -lSDL2_image

ppu.o: ppu.c
	gcc -c ppu.c -o ppu.o

PPURegisters/addr.o: PPURegisters/addr.c
	gcc -c PPURegisters/addr.c -o PPURegisters/addr.o

PPURegisters/controller.o: PPURegisters/controller.c
	gcc -c PPURegisters/controller.c -o PPURegisters/controller.o

PPURegisters/mask.o: PPURegisters/mask.c
	gcc -c PPURegisters/mask.c -o PPURegisters/mask.o

PPURegisters/status.o: PPURegisters/status.c
	gcc -c PPURegisters/status.c -o PPURegisters/status.o

clean:
	rm -f main $(OBJS)
