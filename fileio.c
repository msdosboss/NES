#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINELENGTH 1024

char *loadRom(char *fileName){
	FILE *file = fopen(fileName, "r");
	if(file == NULL){
		printf("%s is not a file\n", fileName);
	}

	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	int lineCount = 0;

	char *rom = malloc(sizeof(char) * size);
	char temp[1024];

	while(fgets(rom, size, file) != NULL){
		lineCount++;
	}
	
	fseek(file, 0, SEEK_SET);

	for(int i = 0; fgets(&rom[i], size, file) != NULL; i = strlen(rom)){
		continue;
	}
	
	printf("%s\n", rom);
	return rom;
}

int main(){
	loadRom("snake.rom");
}
