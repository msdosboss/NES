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
	rom.hexVals = malloc(sizeof(unsigned char) * rom.len);
	while(str[i] != '\0'){
		while(str[i] != '\n' && str[i] != '\0'){
			while(str[i] == ' '){
				i++;
			}
			if(str[i] == ','){
				rom.hexVals[j++] = hexToChar((&(str[i])) - sizeof(char) * 2);	//starts the string after 0x
			}
			i++;
		}
		i++;
	}
	rom.hexVals[j] = hexToChar((&(str[i - 1])) - sizeof(char) * 2);

	return rom;
	
	
}

struct Rom loadRom(char *fileName){
	FILE *file = fopen(fileName, "r");
	if(file == NULL){
		printf("%s is not a file\n", fileName);
	}

	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *str = malloc(sizeof(char) * size);
	char temp[1024];

	for(int i = 0; fgets(&str[i], size, file) != NULL; i = strlen(str)){
		continue;
	}

	struct Rom rom = parse(str);

	return rom;
}

/*int main(){
	loadRom("snake.rom");
}*/
