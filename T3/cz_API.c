#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int i;
int z;

typedef struct Block {
	char kilobyte[1024];
} block;

typedef struct dirBlockEntry {
	unsigned char valid;
	char name[11];
	unsigned int index_block;
} direntry;

// typedef struct BitMapByte {
// 	unsigned char bit0 : 1;
// 	unsigned char bit1 : 1;
// 	unsigned char bit2 : 1;
// 	unsigned char bit3 : 1;
// 	unsigned char bit4 : 1;
// 	unsigned char bit5 : 1;
// 	unsigned char bit6 : 1;
// 	unsigned char bit7 : 1;
// } bitmapbyte;
// 
// typedef struct BitMap {
// 	bitmapbyte** bytes;  // 1024 * 8
// }

typedef struct czFILEstruct {
	char name[11];
	unsigned int index_block;
	int size;
	int tmstmp_create;
	int tmstmp_mod;
	block** content_blocks;  // 508
} czFILE;


direntry** dir_block;  // = calloc(64, sizeof(direntry*));
unsigned char bitmap[1024 * 8];
block** blocks;  // = calloc(65536 - 9, sizeof(block*));


void save_name(char* old, char *dest) {
	for (z = 0; z < strlen(dest); z++) {
		if (z == 10) {  // recorta si se excede
			break;
		}
		old[z] = dest[z];
	}
	old[z] = '\0';
}

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

void mostrar_letras(char* array){
	for (int i; i < strlen(array); i++){
		printf("%c", array[i]);
	}
	printf("\n");
}


void build_dir_block(FILE* filedesc) {
	unsigned char buffer[16];
	dir_block = calloc(64, sizeof(direntry*));
	for (i = 0; i < 64; i++) {
		fread(buffer,sizeof(buffer),1,filedesc);
		dir_block[i] = calloc(1, sizeof(direntry));
		dir_block[i] -> valid = buffer[0];
		// printf("Validez: %u \n", dir_block[i] -> valid);
		guardar_caracteres(dir_block[i] -> name, buffer, 1, 11);
		// printf("Nombre: %s \n", dir_block[i] -> name);
		dir_block[i] -> index_block = (buffer[12] << 24) | (buffer[13] << 16) | (buffer[14] << 8) | buffer[15];
		// printf("Índice de bloque: %i \n", dir_block[i] -> index_block);
	}
}


void build_bit_map(FILE* filedesc) {
	int n_bytes = fread(bitmap, 1, 1024 * 8, filedesc);
	// printf("read %d\n", n_bytes);
}


char get_bitmap_bit(unsigned int iblock) {
	int byte = iblock / 8;
	char bit = iblock % 8;
	char pass = 7 - bit;  // cuál bit en byte, de izquierda a derecha
	unsigned char mask = 1 << pass;  // máscara AND

	// filtra bit y coloca a la derecha; retorna 1 o 0
	return (bitmap[byte] & mask) >> pass;
}


void build_blocks(FILE* filedesc) {
	int n_blocks = 65536 - 9;
	blocks = calloc(n_blocks, sizeof(block*));

}



// EMPIEZAN LAS CZ_
int cz_exists(char* filename) {
	direntry* entry;
	for (i = 0; i < 64; i++) {
		entry = dir_block[i];
		if (entry -> valid && !strcmp(filename, entry -> name)) {
			return 1;
		}
	}
	return 0;
}


int cz_mv(char* orig, char *dest) {
	direntry* entry;
	for (i = 0; i < 64; i++) {
		entry = dir_block[i];
		if (entry -> valid && !strcmp(dest, entry -> name)) {
			return 1;
		}
	}
	for (i = 0; i < 64; i++) {
		entry = dir_block[i];
		if (entry -> valid && !strcmp(orig, entry -> name)) {
			save_name(entry -> name, dest);
			return 0;
		}
	}
	return 1;
}


void cz_ls() {
	direntry* entry;
	for (i = 0; i < 64; i++) {
		entry = dir_block[i];
		if (entry -> valid) {
			printf("%s\n", entry -> name);
		}
	}
}


czFILE* cz_open(char* filename, char mode) {

	int exists = 0;
	for (i = 0; i < 64; i++) {
		direntry* entry = dir_block[i];
		if (entry -> valid && !strcmp(entry -> name, filename)) {
			exists = 1;
			break;
		}
	}
	if (!strcmp(&mode, "r")) {
		if (!exists) {
			return NULL;
		} else {
			// build czFILE
		}
	} else if (!strcmp(&mode, "w")) {
		if (exists) {
			return NULL;
		} else {
			// build czFILE
		}
	}
	return NULL;
}


int cz_cp(char* orig, char* dest){
	// guardo el índice de orig en indice_orig
	int indice_orig = 100;
	for (i = 0; i < 64; i++) {
		direntry* entry = dir_block[i];
		if (entry -> valid && !strcmp(entry -> name, orig)) {
			indice_orig = i;
			break;
		}
	}
	if (indice_orig == 100){
		return 1; // no existe el archivo de origen
	}
	// reviso que no exista dest
	if (!cz_exists(dest)){
		return 2; // ya existe dest
	}
	// copiar de verdad el archivo de índice indice_orig... RELLENAR
	return 0;
}


int cz_rm (char* filename){
	if (!cz_exists(filename)){
		return 1; // no existe, no se puede borrar
	}
	int indice_file; // índice del archivo a borrar
	for (i = 0; i < 64; i++) {
		direntry* entry = dir_block[i];
		if (entry -> valid && !strcmp(entry -> name, filename)) {
			indice_file = i;
			break;
		}
	}
	// dejamos el valid bit en 0
	dir_block[indice_file] -> valid = 0;
	// no debiera ser necesario cambiar el resto de las cosas...
	// llenar de 0 los content_blocks... RELLENAR
	return 0;
}


int main(int argc, char const *argv[])
{

	FILE * ptr = fopen("db_test.bin","rb");  // r for read, b for binary

	build_dir_block(ptr);
	build_bit_map(ptr);
	build_blocks(ptr);

	cz_ls();
	printf("%d \n", cz_exists("archivo"));
	cz_mv("archivo", "hi");
	cz_ls();
	printf("%d \n", cz_exists("archivo"));
	printf("%i \n", cz_rm("no existe"));
	printf("%i \n", cz_cp("no existe", "archivo")); // no existe orig
	printf("%i \n", cz_cp("arch", "archivo")); // ya existe dest
	
	fclose(ptr);

	return 0;
}