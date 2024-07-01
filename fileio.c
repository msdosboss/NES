#include "fileio.h"

#define MAXLINELENGTH 1024

char *loadRom(char *fileName){
	FILE *file = fopen(fileName, "r");
	if(file == NULL){
		printf("%s is not a file\n", fileName);
	}

	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *rom = malloc(sizeof(char) * size);
	char temp[1024];

	for(int i = 0; fgets(&rom[i], size, file) != NULL; i = strlen(rom)){
		continue;
	}
	
	return rom;
}

/*int main(){
	loadRom("snake.rom");
}*/
