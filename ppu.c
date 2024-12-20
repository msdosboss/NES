#include "ppu.h"

/*
struct PPU{
	unsigned char *chrRom;
	unsigned char paletteTable[32];
	unsigned char vram[2048];
	unsigned char oamAddr;
	unsigned char oamData[256];
	int mirrorMode;

	struct AddrRegister addr;
	struct ScrollRegister scroll;
	unsigned char status;
	unsigned char controller;
	unsigned char mask;
	unsigned char dataBuffer;

	int scanLines;
	int cycles;

	int nmiInt;
};
*/

void initPPU(struct PPU *ppu, struct Rom *rom){
	ppu->chrRom = rom->chrRom;
	memset(ppu->paletteTable, 0, sizeof(ppu->paletteTable));
	memset(ppu->vram, 0, sizeof(ppu->vram));
	memset(ppu->oamData, 0, sizeof(ppu->oamData));
	ppu->mirrorMode = rom->mirrorMode;
	initAddrRegister(&(ppu->addr));
	initScrollRegister(&(ppu->scroll));
	ppu->cycles = 0;
	ppu->scanLines = 0;
	ppu->nmiInt = 0;
}

int isSprite0Hit(struct PPU *ppu, int cycles){
	int y = ppu->oamData[0];
	int x = ppu->oamData[3];

	return ((y == ppu->scanLines) && (x <= cycles) && (ppu->mask & 0b00010000));
}

int ppuTick(struct PPU *ppu, int cycles){
	ppu->cycles += cycles;
	if(ppu->cycles >= 341){
		if(isSprite0Hit(ppu, ppu->cycles)){
			ppu->status = (ppu->status | 0b01000000);	//turn on zero hit
		}
		ppu->cycles -= 341;
		ppu->scanLines += 1;

		if(ppu->scanLines == 241){
			statusVblankOn(&(ppu->status));
			ppu->status = (ppu->status & 0b10111111);	//turn off zero hit
			spriteZeroHitOff(&(ppu->status));
			if(isNMIIntOn(ppu->controller)){
				statusVblankOn(&(ppu->status));
				ppu->nmiInt = 1;
			}
		}
		if(ppu->scanLines >= 262){
			ppu->scanLines = 0;
			ppu->nmiInt = 0;
			statusVblankOff(&(ppu->status));
			return 1;
		}
	}
	return 0;
	
}

unsigned char ppuRead(struct PPU *ppu){
	unsigned short addr = getAddrRegister(&(ppu->addr));
	for(int i = 0; i < vramAddrIncAmount(ppu->controller); i++){
		incrementAddrRegister(&(ppu->addr));
	}
	if(addr >= 0x0 && addr <= 0x1fff){
		unsigned char data = ppu->dataBuffer;
		ppu->dataBuffer = ppu->chrRom[addr];
		return data;
	}

	else if(addr >= 0x2000 && addr <= 0x2fff){
		unsigned char data = ppu->dataBuffer;
		ppu->dataBuffer = ppu->vram[mirroredVramAddr(ppu, addr)];	//NEED TO IMPLEMENT MIRRORING!!!!
		return data;
	}

	else if(addr >= 0x3000 && addr <= 0x3eff){
		printf("addr space %x is not suppose to be used\n", addr);
		return -1;
	}

	else if(addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c){	//addresses 0x3f10, 0x3f14, 0x3f18, 0x3f1c mirror down to 0x3f00, 0x3f04, 0x3f08, 0x3f0c
		addr -= 0x10;
		return ppu->paletteTable[addr - 0x3f00];
	}
	else if(addr >= 0x3f00 && addr <= 0x3fff){
		return ppu->paletteTable[addr - 0x3f00];
	}
}

unsigned char ppuBusRead(struct PPU *ppu){
	unsigned short addr = getAddrRegister(&(ppu->addr));
	if(addr >= 0x0 && addr <= 0x1fff){
		unsigned char data = ppu->dataBuffer;
		return data;
	}

	else if(addr >= 0x2000 && addr <= 0x2fff){
		unsigned char data = ppu->dataBuffer;
		return data;
	}

	else if(addr >= 0x3000 && addr <= 0x3eff){
		printf("addr space %x is not suppose to be used\n", addr);
		return -1;
	}

	else if(addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c){	//addresses 0x3f10, 0x3f14, 0x3f18, 0x3f1c mirror down to 0x3f00, 0x3f04, 0x3f08, 0x3f0c
		addr -= 0x10;
		return ppu->paletteTable[addr - 0x3f00];
	}
	else if(addr >= 0x3f00 && addr <= 0x3fff){
		return ppu->paletteTable[addr - 0x3f00];
	}
}

void ppuWrite(struct PPU *ppu, unsigned char data){
	unsigned short address = getAddrRegister(&(ppu->addr));
	for(int i = 0; i < vramAddrIncAmount(ppu->controller); i++){
		incrementAddrRegister(&(ppu->addr));
	}

	if(address >= 0x0 && address < 0x2000){
		printf("Trying to write to ROM at address %x\n", address);
	}

	else if(address >= 0x2000 && address < 0x2fff){
		ppu->vram[mirroredVramAddr(ppu, address)] = data;
	}

	else if(address >= 0x3000 && address < 0x3f00){
		printf("addr space %x is not suppose to be used!\n", address);
	}
	
	else if(address == 0x3f10 || address == 0x3f14 || address == 0x3f18 || address == 0x3f1c){	//addresses 0x3f10, 0x3f14, 0x3f18, 0x3f1c mirror down to 0x3f00, 0x3f04, 0x3f08, 0x3f0c
		address -= 0x10;
		//printf("Writing %x to paletteTable[%x]\n", data, address - 0x3f00);
		ppu->paletteTable[address - 0x3f00] = data;
	}

	else if(address >= 0x3f00 && address < 0x4000){
		//printf("Writing %x to paletteTable[%x]\n", data, address - 0x3f00);
		ppu->paletteTable[address - 0x3f00] = data;
	}
}

unsigned short mirroredVramAddr(struct PPU *ppu, unsigned short address){
	unsigned short mirroredVram = address & 0b10111111111111;	//mirror down 0x3000-0x3eff to 0x2000-0x20eff
	mirroredVram -= 0x2000;
	unsigned short nameTable = mirroredVram / 0x400;	//Name table index 

	if(ppu->mirrorMode == HORIZONTAL){
		if(nameTable == 1 || nameTable == 2){
			mirroredVram -= 0x400;
		}
		if(nameTable == 3){
			mirroredVram -= 0x800;
		}
	}
	else if(ppu->mirrorMode == VERTICAL){
		if(nameTable == 2 || nameTable == 3){
			mirroredVram -= 0x800;
		}
	}

	return mirroredVram;
}

unsigned char statusRead(struct PPU *ppu){
	unsigned char oldStatus = ppu->status;
	addrLatch(&(ppu->addr));
	scrollLatch(&(ppu->scroll));
	ppu->status &= 0b01111111;
	return oldStatus;
}


void writeToCtrl(struct PPU *ppu, unsigned char data){
	int beforeNmiStatus = ppu->controller & 0b10000000;
	ppu->controller = data;

	if(!beforeNmiStatus && (ppu->controller & 0b10000000) && (ppu->status & 0b10000000)){	//checking if the controller register has the vblank flag on and checking if the status register has generate NMI flag on
		ppu->nmiInt = 1;
	}
}

void writeToOamAddr(struct PPU *ppu, unsigned char data){
	ppu->oamAddr = data;
}

void writeToOamData(struct PPU *ppu, unsigned char data){
	ppu->oamData[ppu->oamAddr] = data;
	ppu->oamAddr++;
}

void writeToScroll(struct PPU *ppu, unsigned char data){
	scrollWrite(&(ppu->scroll), data);
}

/*void setPixel(struct Pixel *pixel, struct PaletteEntry paletteEntry){
	pixel->red = paletteEntry.red;
	pixel->blue = paletteEntry.blue;
	pixel->green = paletteEntry.green;
}*/

void parseChrRom(struct PPU *ppu, struct PixelFrame *pixelFrame, int bank){
	bank *= 0x1000;
	int hor = 0;
	int ver = 0;
	for(int i = 0; i <= 0x1000; i += 0x10){
		for(int j = 0; j < 0x8; j++){
			unsigned char first = ppu->chrRom[i + j + bank];
			unsigned char second = ppu->chrRom[i + j + bank + 8];
			for(int k = 7; k >= 0; k--){
				if(((first & 0b00000001) | (second & 0b00000001)) == 0){
					//frame->tiles[ver][hor].pixels[j][k] = 0;
					pixelFrame->pixels[ver * 8 + j][hor * 8 + k] = 0;
				}
				else if((((first & 0b00000001) == 1) && ((second & 0b00000001) == 0))){
					//frame->tiles[ver][hor].pixels[j][k] = 1;
					pixelFrame->pixels[ver * 8 + j][hor * 8 + k] = 1;
				}
				else if(((first & 0b00000001) == 0) && ((second & 0b00000001) == 1)){
					//frame->tiles[ver][hor].pixels[j][k] = 2;
					pixelFrame->pixels[ver * 8 + j][hor * 8 + k] = 2;
				}
				else if(((first & 0b00000001) == 1) && ((second & 0b00000001) == 1)){
					//frame->tiles[ver][hor].pixels[j][k] = 3;
					pixelFrame->pixels[ver * 8 + j][hor * 8 + k] = 3;
				}
				first >>= 1;
				second >>= 1;
			}
		}
		if(hor == 32){
			hor = 0;
			ver++;
		}
		else{
			hor++;
		}
	}
}

int bgPalette(struct PPU *ppu, unsigned char *nameTable, int hor, int ver){
	int attribIndex = 0x3c0 + (ver / 4 * 8 + hor / 4);
	int attribByte = nameTable[attribIndex];	//this is still hardcoding name table
	if(hor % 4 / 2 == 0 && ver % 4 / 2 == 0){
		attribByte &= 0b11;
	}

	else if(hor % 4 / 2 == 1 && ver % 4 / 2 == 0){
		attribByte >>= 2;
		attribByte &= 0b11;
	}

	else if(hor % 4 / 2 == 0 && ver % 4 / 2 == 1){
		attribByte >>= 4;
		attribByte &= 0b11;
	}

	else if(hor % 4 / 2 == 1 && ver % 4 / 2 == 1){
		attribByte >>= 6;
		attribByte &= 0b11;
	}

	return 1 + attribByte * 4;
}

int spritePalette(unsigned char paletteIndex){
	return 0x11 + (paletteIndex * 4);
}

void parseNametable(struct PPU *ppu, struct PixelFrame *pixelFrame, unsigned char *nameTable, struct ViewableRect rect, int shiftX, int shiftY){

	int bank = 0x1000 * ((ppu->controller & 0b00010000) >> 4);	//checking if bit is on in the controller register

	for(int i = 0; i < 0x3c0; i++){	//background parsing
		int hor = i % 32;
		int ver = i / 32;
		unsigned char *tile = &ppu->chrRom[16 * nameTable[i] + bank];
		int bgPaletteOffset = bgPalette(ppu, nameTable, hor, ver);
		for(int j = 0; j < 0x8; j++){
			unsigned char first = tile[j];
			unsigned char second = tile[j + 8];
			for(int k = 7; k >= 0; k--){
				int colorIndex = ((second << 1) | (first & 0b1)) & 0b11;

				first >>= 1;
				second >>= 1;
				
				int pixelX = hor * 8 + k;
				int pixelY = ver * 8 + j;

				if(pixelX < rect.x1 || pixelX >= rect.x2 || pixelY < rect.y1 || pixelY >= rect.y2){
					continue;
				}

				int xNegativeOffset = pixelX % 8 + k;

				switch(colorIndex){
					case 0:
						pixelFrame->pixels[pixelY + shiftY][pixelX + shiftX] = ppu->paletteTable[0];
						//frame->tiles[ver + shiftY / 8][hor + xNegativeOffset / 8 + shiftX / 8].pixels[j + (shiftY % 8)][k + (shiftX % 8)] = ppu->paletteTable[0];
						break;
					case 1:
						pixelFrame->pixels[pixelY + shiftY][pixelX + shiftX] = ppu->paletteTable[bgPaletteOffset];
						//frame->tiles[ver + shiftY / 8][hor + xNegativeOffset / 8 + shiftX / 8].pixels[j + (shiftY % 8)][k + (shiftX % 8)] = ppu->paletteTable[bgPaletteOffset];
						break;
					case 2:
						pixelFrame->pixels[pixelY + shiftY][pixelX + shiftX] = ppu->paletteTable[bgPaletteOffset + 1];
						//frame->tiles[ver + shiftY / 8][hor + xNegativeOffset / 8 + shiftX / 8].pixels[j + (shiftY % 8)][k + (shiftX % 8)] = ppu->paletteTable[bgPaletteOffset + 1];
						break;
					case 3:
						pixelFrame->pixels[pixelY + shiftY][pixelX + shiftX] = ppu->paletteTable[bgPaletteOffset + 2];
						//frame->tiles[ver + shiftY / 8][hor + xNegativeOffset / 8 + shiftX / 8].pixels[j + (shiftY % 8)][k + (shiftX % 8)] = ppu->paletteTable[bgPaletteOffset + 2];
						break;
				}
			}
		}
	}

}

void parseVram(struct PPU *ppu, struct PixelFrame *pixelFrame){

	int scrollX = ppu->scroll.scrollX;
	int scrollY = ppu->scroll.scrollY;

	unsigned char *mainNametable;
	unsigned char *secondNametable;

	if((ppu->mirrorMode == VERTICAL && nametableAddr(ppu->controller) == 0x2000) || (ppu->mirrorMode == VERTICAL && nametableAddr(ppu->controller) == 0x2800) || (ppu->mirrorMode == HORIZONTAL && nametableAddr(ppu->controller) == 0x2000) || (ppu->mirrorMode == HORIZONTAL && nametableAddr(ppu->controller) == 0x2400)){
		mainNametable = ppu->vram;
		secondNametable = &ppu->vram[0x400];
	}
	else{
		mainNametable = &ppu->vram[0x400];
		secondNametable = ppu->vram;
	}

	struct ViewableRect mainRect = {scrollX, scrollY, 256, 240};
	
	parseNametable(ppu, pixelFrame, mainNametable, mainRect, -scrollX, -scrollY);

	if(scrollX > 0){
		struct ViewableRect secondRect = {0, 0, scrollX, 240};
		parseNametable(ppu, pixelFrame, secondNametable, secondRect, 256 - scrollX, 0);
	}

	if(scrollY > 0){
		struct ViewableRect secondRect = {0, 0, 256, scrollY};
		parseNametable(ppu, pixelFrame, secondNametable, secondRect, 0, 240 - scrollY);
	}

	//unsigned char backgroundBuffer[240][256];

	/*int bank = 0x1000 * ((ppu->controller & 0b00010000) >> 4);	//checking if bit is on in the controller register
	
	for(int i = 0; i < 0x3c0; i++){	//background parsing
		int hor = i % 32;
		int ver = i / 32;
		unsigned char *tile = &ppu->chrRom[16 * ppu->vram[i] + bank];
		int bgPaletteOffset = bgPalette(ppu, ppu->vram, hor, ver);
		for(int j = 0; j < 0x8; j++){
			unsigned char first = tile[j];
			unsigned char second = tile[j + 8];
			for(int k = 7; k >= 0; k--){
				int colorIndex = ((second << 1) | (first & 0b1)) & 0b11;
				switch(colorIndex){
					case 0:
						frame->tiles[ver][hor].pixels[j][k] = ppu->paletteTable[0];
						break;
					case 1:
						frame->tiles[ver][hor].pixels[j][k] = ppu->paletteTable[bgPaletteOffset];
						break;
					case 2:
						frame->tiles[ver][hor].pixels[j][k] = ppu->paletteTable[bgPaletteOffset + 1];
						break;
					case 3:
						frame->tiles[ver][hor].pixels[j][k] = ppu->paletteTable[bgPaletteOffset + 2];
						break;
				}
				first >>= 1;
				second >>= 1;
			}
		}
	}*/

	//int spriteZeroHit = 0;

	for(int i = 0; i < 256; i += 4){	//sprite parsing
		int yPos = ppu->oamData[i];
		int tileIndex = ppu->oamData[i + 1];
		unsigned char attribs = ppu->oamData[i + 2];
		int xPos = ppu->oamData[i + 3];

		int flipVert = ((attribs >> 7) & 0b1);
		int flipHor = ((attribs >> 6) & 0b1);

		unsigned char paletteIndex = (attribs & 0b11);
		int spritePaletteOffset = spritePalette(paletteIndex);

		int bank = 0x1000 * ((ppu->controller & 0b00001000) >> 3);

		unsigned char *tile = &ppu->chrRom[bank + tileIndex * 16];

		if(!(attribs & 0b00100000) && (ppu->mask & 0b00010000)){	//bit 5 of attribs determines if it is render in front of or behind the background bit 4 of mask determines if sprites are rendered
			for(int j = 0; j < 0x8; j++){
				unsigned char first = tile[j];
				unsigned char second = tile[j + 8];
				for(int k = 7; k >= 0; k--){
					int pixelPalleteIndex;
					int colorIndex = ((second << 1) | (first & 0b1)) & 0b11;
					switch(colorIndex){
						case 0:
							pixelPalleteIndex = 255;
							break;
						case 1:
							pixelPalleteIndex = ppu->paletteTable[spritePaletteOffset];
							break;
						case 2:
							pixelPalleteIndex = ppu->paletteTable[spritePaletteOffset + 1];
							break;
						case 3:
							pixelPalleteIndex = ppu->paletteTable[spritePaletteOffset + 2];
							break;
					}

					first >>= 1;
					second >>= 1;
					
					int xFlip = !flipHor ? k : 7 - k;
					int yFlip = !flipVert ? j : 7 - j;

					if(pixelPalleteIndex != 255){
						//frame->tiles[(yPos / 8) + yPosOverflow / 8][(xPos / 8) + xPosOverflow / 8].pixels[yPosOverflow % 8][xPosOverflow % 8] = pixelPalleteIndex;
						pixelFrame->pixels[yPos + yFlip][xPos + xFlip] = pixelPalleteIndex;
						/*if(i == 0 && !spriteZeroHit && backgroundBuffer[yPos + yPosOverflow][xPos + xPosOverflow] != ppu->paletteTable[0]){
							spriteZeroHit = 1;
							ppu->status |= 0b01000000;	//turn on sprite 0 hit flag in status reg
						}*/
					}		
				}
			}
		}
	}	
}

