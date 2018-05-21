#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void guardar_caracteres(char* old, unsigned char* dest, int inicio, int final) {
	int z;
	int aux;
	for (z = 0; z < final - 1; z++) {
		aux = dest[inicio + z];
		if (aux == 0){
			break;
		}
		old[z] = (char) aux;
		// printf("Vamos en el char %i, de valor %c y número %i \n", z, old[z], aux);
	}
	old[z + 1] = '\0';
}

typedef struct dirBlockEntry {
	unsigned char valid;
	char name[11];
	int index_block;
} direntry;

void mostrar_letras(char* array){
	for (int i; i < strlen(array); i++){
		printf("%c", array[i]);
	}
	printf("\n");
}

int main(int argc, char const *argv[])
{

	int c1 = 16;

	unsigned char buffer[c1];
	FILE * ptr = fopen("db_test.bin","rb");
	// r for read, b for binary

	//fread(buffer,sizeof(buffer),1,ptr); 
	// read 16 bytes to our buffer

	//printf("Bytes: ");
	//for(int i = 0; i<c1; i++)
	//    printf("%u ", buffer[i]); // prints a series of bytes
	//printf("\n");

	direntry** bloque1 = calloc(64, sizeof(direntry*));
	for (int i=0;i<64;i++){
		fread(buffer,sizeof(buffer),1,ptr);
		bloque1[i] = calloc(1, sizeof(direntry));
		bloque1[i] -> valid = buffer[0];
		printf("Validez: %u \n", bloque1[i] -> valid);
		guardar_caracteres(bloque1[i] -> name, buffer, 1, 11);
		printf("Nombre: %s \n", bloque1[i] -> name);
		bloque1[i] -> index_block = (buffer[12] << 24) | (buffer[13] << 16) | (buffer[14] << 8) | buffer[15];
		printf("Índice de bloque: %i \n", bloque1[i] -> index_block);
	}
	
	fclose(ptr);

	//char c[11] = "hola\0";
	//printf("%s\n", c);
	//printf("%d\n", strcmp(c, "hola"));
	//printf("%d\n", strcmp(c, "hola\0"));
	//printf("%d\n", strcmp(c, "holas"));
	//printf("%d\n", strcmp(c, "Hholass"));
	return 0;
}