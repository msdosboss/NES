#include "fileio.h"

#define MAXLINELENGTH 1024

int instructionCount(char *str){
	int len = 0;
	for(int i = 0; str[i] != '\0'; i++){
		if(str[i] == ','){
			len++;
		}
	}
	return len + 1;
}

unsigned char hexToChar(char *hexVal){
	unsigned char val = 0;
	for(int i = 0; i < 2; i++){
		if(hexVal[i] >= '0' && hexVal[i] <= '9'){
			val = val * 16 + hexVal[i] - '0';
		}
		else{
			val = val * 16 + ((hexVal[i] - 'a') + 10);
		}
	}
	return val;
}


struct Rom parse(char *str){	//This func assumes the input is in this format (0x66, 0x22, ...)
	int i = 0;
	int j = 0;
	struct Rom rom;
	rom.len = instructionCount(str);
	rom.prgRom = malloc(sizeof(unsigned char) * rom.len);
	while(str[i] != '\0'){
		while(str[i] != '\n' && str[i] != '\0'){
			while(str[i] == ' '){
				i++;
			}
			if(str[i] == ','){
				rom.prgRom[j++] = hexToChar((&(str[i])) - sizeof(char) * 2);	//starts the string after 0x
			}
			i++;
		}
		i++;
	}
	rom.prgRom[j] = hexToChar((&(str[i - 1])) - sizeof(char) * 2);

	return rom;
	
	
}

struct Rom loadRom(char *fileName){
	FILE *file = fopen(fileName, "r");
	if(file == NULL){
		printf("%s is not a file\n", fileName);
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *str = malloc(sizeof(char) * size);
	char temp[1024];

	for(int i = 0; fgets(&str[i], size, file) != NULL; i = strlen(str)){
		continue;
	}

	struct Rom rom = parse(str);

	return rom;
}

int verifyFormat(unsigned char *raw){
	unsigned char formatIndicator[] = {0x4e, 0x45, 0x53, 0x1a};
	for(int i = 0; i < 4; i++){
		if(raw[i] != formatIndicator[i]){
			return 0;
		}
	}
	return 1;	
}

void mirrorMode(struct Rom *rom, unsigned char *raw){
	if((raw[6] & 0b1000) != 0){
		rom->mirrorMode = FOURSCREEN;
	}
	else if((raw[6] & 0b1) != 0){
		rom->mirrorMode = VERTICAL;
	}
	else{
		rom->mirrorMode = HORIZONTAL;
	}

}

int trainerFlag(unsigned char *raw){
	if((raw[6] & 0b100) != 0){
		return 512;
	}
	return 0;
}

struct Rom nesCartRead(char *fileName){
	FILE *file = fopen(fileName, "r");

	if(file == NULL){
		printf("%s in not a file\n", fileName);
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char *raw = malloc(sizeof(unsigned char) * size); 
	int i = 0;
	int ch;


	while((ch = fgetc(file)) != EOF){
		if(i < size){
			raw[i++] = (unsigned char)ch;
		}
	}

	struct Rom rom;

	if(!verifyFormat(raw)){
		printf("%s is not a valid Ines file\n", fileName);
	}

	rom.mapper = (raw[7] & 0b11110000) | (raw[6] >> 4);

	if(((raw[7] >> 2) & 0b11) != 0){
		printf("Ines 2.0 is not supported.");
	}

	mirrorMode(&rom, raw);

	long prgRomSize = raw[4] * PRGROMPAGESIZE;
	long chrRomSize = raw[5] * CHRROMPAGESIZE;

	int trainerOffset = trainerFlag(raw); 

	long prgRomStart = 16 + trainerOffset;
	long chrRomStart = prgRomStart + prgRomSize;

	rom.prgRom = malloc(sizeof(unsigned char) * prgRomSize);
	rom.chrRom = malloc(sizeof(unsigned char) * chrRomSize);


	int j = 0;
	for(int i = prgRomStart; i < prgRomStart + prgRomSize; i++){
		rom.prgRom[j++] = raw[i];
	}

	j = 0;
	for(int i = chrRomStart; i < chrRomStart + chrRomSize; i++){
		rom.chrRom[j++] = raw[i];
	}

	for(int i = 0; i < prgRomSize; i++){
		printf("rom.prgRom[%d] = %x\n", i, rom.prgRom[i]);
	}
		
}

int main(){
	//loadRom("snake.rom");
	nesCartRead("nesTest.nes");
}
