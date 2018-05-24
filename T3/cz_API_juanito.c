#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int i;
int z;

int n_bytes;

//int pos_ultima_llamada_read = 11; // para que parta de 12, saltando metadata
int indice_bloque_actual = 12;
int byte_actual_bloque = 0;

typedef struct Block {
	unsigned char kilobyte[1024];
} block;

typedef struct dirBlockEntry {
	unsigned char valid;
	char name[11];
	unsigned int index_block;
} direntry;

typedef struct czFILEstruct {
	char name[11];
	char mode;
	unsigned int index_block;
	int size;
	time_t timestamp_create;
	time_t timestamp_update;
	//block** content_blocks;  // 508
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
		n_bytes += fread(buffer,sizeof(buffer),1,filedesc) * 16;
		dir_block[i] = calloc(1, sizeof(direntry));
		dir_block[i] -> valid = buffer[0];
		// printf("Validez: %u \n", dir_block[i] -> valid);
		guardar_caracteres(dir_block[i] -> name, buffer, 1, 11);
		// printf("Nombre: %s \n", dir_block[i] -> name);
		dir_block[i] -> index_block = (buffer[12] << 24) | (buffer[13] << 16) | (buffer[14] << 8) | buffer[15];
		// printf("Índice de bloque: %i \n", dir_block[i] -> index_block);
	}
	// printf("read %d blocks\n", n_bytes / 1024);
}

void build_bit_map(FILE* filedesc) {
	n_bytes += fread(bitmap, 1, 1024 * 8, filedesc);
	// printf("read %d blocks\n", n_bytes / 1024);
}

char get_bitmap_bit(unsigned int iblock) {
	int byte = iblock / 8;
	char bit = iblock % 8;
	char pass = 7 - bit;  // cuál bit en byte, de izquierda a derecha
	unsigned char mask = 1 << pass;  // máscara AND

	// filtra bit y coloca a la derecha; retorna 1 o 0
	return (bitmap[byte] & mask) >> pass;
}

void up_bitmap_bit(unsigned int iblock) {
	int byte = iblock / 8;
	char bit = iblock % 8;
	char up = 7 - bit;  // cuál bit en byte, de izquierda a derecha
	unsigned char mask = 1 << up;  // máscara OR

	// guarda OR con mask que deja 1 en iblock que era 0
	bitmap[byte] = bitmap[byte] | mask;
}

void down_bitmap_bit(unsigned int iblock) {
	int byte = iblock / 8;
	char bit = iblock % 8;
	char down = 7 - bit;  // cuál bit en byte, de izquierda a derecha
	unsigned char mask = 1 << down;  // máscara XOR

	// guarda XOR con mask que deja 0 en iblock que era 1
	bitmap[byte] = bitmap[byte] ^ mask;
}

int get_first_available() {
	int ibit;
	int has_space = 0;
	for (ibit = 0; ibit < 65536; ibit++) {
		if (!get_bitmap_bit(ibit)) {
			has_space = 1;
			return ibit;
		}
	}
	return -1;
}

void build_blocks(FILE* filedesc) {
	int n_blocks = 65536 - 9;
	blocks = calloc(n_blocks, sizeof(block*));
	for (i = 0; i < n_blocks; i++) {
		blocks[i] = calloc(1, sizeof(block));
		n_bytes += fread(blocks[i] -> kilobyte, 1, 1024, filedesc);
	}
	// printf("read %d blocks\n", n_bytes / 1024);
}

block* get_block(int b) {
	return blocks[b - 9];
}

int indice_dir_nombre (char* filename){
	direntry* entry;
	for (i = 0; i < 64; i++) {
		entry = dir_block[i];
		if (entry -> valid && !strcmp(filename, entry -> name)) {
			return i;
		}
	}
	return -1;
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

czFILE* cz_open(char* filename, char mode) {

	int exists = cz_exists(filename);
	direntry* entry;
	if (mode == 'r') {
		if (!exists) {
			return NULL;
		} else {
			// build czFILE
			int indice_bloque_leido = indice_dir_nombre(filename);
			entry = dir_block[indice_bloque_leido];
			czFILE* czfd = calloc(1, sizeof(czFILE));
			save_name(czfd -> name, filename);
			czfd -> mode = 'r'; // cambiado
			czfd -> index_block = entry -> index_block; // cambiado, voy a la entrada y pido index_block
			block* ptr_aux = get_block(czfd -> index_block); // para que no salga tan largo
			// czfd -> size = (get_block[czfd -> index_block][0] << 24) |
			// (get_block[czfd -> index_block][1] << 16) | 
			// (get_block[czfd -> index_block][2] << 8) | 
			// (get_block[czfd -> index_block][3]);
			czfd -> size = (unsigned int)((ptr_aux->kilobyte[0]<<24) | (ptr_aux->kilobyte[1]<<16)| (ptr_aux->kilobyte[2]<<8) | (ptr_aux->kilobyte[3])); // cambiado
			printf("%u %u %u %u \n", ptr_aux->kilobyte[0], ptr_aux->kilobyte[1], ptr_aux->kilobyte[2], ptr_aux->kilobyte[3]);
			time_t now = time(NULL);
			czfd -> timestamp_create = now;
			czfd -> timestamp_update = now;
			// Falta el atributo block** content_blocks...
			return czfd;
		}
	} else if (mode == 'w') {
		if (exists) {
			return NULL;
		} else {
			int has_space = 0;
			for (i = 0; i < 64; i++) {
				entry = dir_block[i];
				if (!(entry -> valid)) {
					has_space = 1;
					break;
				}
			}
			if (!has_space) {
				return NULL;
			}

			// busca primer bloque disponible para bloque índice
			int ibit = get_first_available();
			if (ibit == -1) {
				return NULL;
			}
			up_bitmap_bit(ibit);
			// has_space = 0;
			// for (ibit = 0; ibit < 65536; ibit++) {
			// 	if (!get_bitmap_bit(ibit)) {
			// 		has_space = 1;
			// 		break;
			// 	}
			// }
			// if (!has_space) {
			// 	return NULL;
			// }
			// printf("%d\n", ibit);  // si está vacío debería ser 9

			// actualiza valores de entrada de bloque de directorio
			entry -> valid = 1;
			save_name(entry -> name, filename);
			entry -> index_block = ibit;

			// inicializa estructura czFILE
			czFILE* czfd = calloc(1, sizeof(czFILE));
			save_name(czfd -> name, filename);
			czfd -> mode = 'w';
			czfd -> index_block = ibit;
			czfd -> size = 0;
			time_t now = time(NULL);
			czfd -> timestamp_create = now;
			czfd -> timestamp_update = now;
			// czfd -> content_blocks = calloc(508, sizeof(block*));

			return czfd;
		}
	}
	return NULL;
}

int cz_read(czFILE* file_desc, void* buffer, int nbytes){
 	// marcar pos_ultima_llamada_read
 	int num_bytes_leidos = 0;
 	// Primer error, pueden haber más...
 	if (file_desc -> mode != 'w'){
 		return -1;
 	}
 	unsigned char* cbuffer = (unsigned char*) buffer;
 	block* bloque_indice = get_block(file_desc -> index_block);
 	int byte_actual_buffer = 0; 
 	// block* bloque_actual = blocks[bloque_indice->kilobyte[indice_bloque_actual]]; //get block, y usar 4
 	int entero_para_get_block = (bloque_indice->kilobyte[4*indice_bloque_actual]<<24) | (bloque_indice->kilobyte[4*indice_bloque_actual+1]<<16) | (bloque_indice->kilobyte[4*indice_bloque_actual+2]<<8) | (bloque_indice->kilobyte[4*indice_bloque_actual+3]);
 	block* bloque_actual = get_block(entero_para_get_block);
 	while (nbytes > 0){ // revisar condiciones... // && pos_ultima_llamada_read < file_desc -> size 
 		while (nbytes > 0 && byte_actual_bloque < 1024){
 			cbuffer[byte_actual_buffer] = bloque_actual->kilobyte[byte_actual_bloque];
 			byte_actual_buffer++;
 			byte_actual_bloque++;
 			nbytes--;
 			num_bytes_leidos++;
 		}
 		if (indice_bloque_actual == 253){ // uso la indirección, equivale a cambiar el bloque índice
 			bloque_indice = get_block((bloque_indice->kilobyte[1020]<<24) | (bloque_indice->kilobyte[1021]<<16) | (bloque_indice->kilobyte[1022]<<8) | (bloque_indice->kilobyte[1023]));
 			byte_actual_bloque = 0; // reinicio el bloque a leer...
 			indice_bloque_actual = 0; // y parto del 0, no del 12
 		}
 		if (byte_actual_bloque == 1024){
 			indice_bloque_actual++; // a menos que me pase...
 			entero_para_get_block = (bloque_indice->kilobyte[4*indice_bloque_actual]<<24) | (bloque_indice->kilobyte[4*indice_bloque_actual+1]<<16) | (bloque_indice->kilobyte[4*indice_bloque_actual+2]<<8) | (bloque_indice->kilobyte[4*indice_bloque_actual+3]);
 			block* bloque_actual = get_block(entero_para_get_block);
 			byte_actual_bloque = 0;
 		}
 	}
 	buffer = (void*) cbuffer; // devolver al buffer real
 	return num_bytes_leidos;
 }


int cz_write(czFILE* file_desc, void* buffer, int nbytes) {
	if (file_desc -> mode != 'w') {
		return -1;
	}

	// get index block kilobyte
	char* index_KB = blocks[file_desc -> index_block] -> kilobyte;
	//// set index block metadata
	
	// set creation timestamp bytes
	index_KB[4] = file_desc -> timestamp_create >> 24;
	index_KB[5] = file_desc -> timestamp_create >> 16;
	index_KB[6] = file_desc -> timestamp_create >> 8;
	index_KB[7] = file_desc -> timestamp_create;


	//// variables útiles:
	int effective_bytes = 0;  // tamaño guardado de archivo en B, valor de retorno
	int i_to_fill;  // índice de bloque disponible encontrado
	// int current_buffer_byte = 0;  // escritura total
	char* current_kilobyte;  // puntero a KB de bloque disponible
	int current_kilobyte_byte;  // índice de byte escribiendo en bloque
	int n_first_level_block = 0;  // # bloques de primer nivel llenados
	int base;

	//// escritura:  UN NIVEL DE MOMENTO
	while (effective_bytes < nbytes) {

		i_to_fill = get_first_available();
		if (i_to_fill == -1) {  // no se puede seguir escribiendo porque no quedan bloques
			return effective_bytes;
		}
		up_bitmap_bit(i_to_fill);

		// file_desc -> content_blocks[0] = blocks[i_to_fill];

		base = 12 + (n_first_level_block * 4);
		index_KB[base] = i_to_fill >> 24;
		index_KB[base + 1] = i_to_fill >> 16;
		index_KB[base + 2] = i_to_fill >> 8;
		index_KB[base + 3] = i_to_fill;

		current_kilobyte = blocks[i_to_fill] -> kilobyte;
		current_kilobyte_byte = 0;

		while (current_kilobyte_byte < 1024 && effective_bytes < nbytes) {
			// current_kilobyte[current_kilobyte_byte] = buffer[effective_bytes];
			current_kilobyte_byte++;
			effective_bytes++;
		}
	}


	////
	
	// usar dos niveles si es necesario

	////
	

	// set size and size metadata bytes
	file_desc -> size = effective_bytes;
	index_KB[0] = effective_bytes >> 24;
	index_KB[1] = effective_bytes >> 16;
	index_KB[2] = effective_bytes >> 8;
	index_KB[3] = effective_bytes;
	// set update timestamp bytes
	file_desc -> timestamp_update = time(NULL);  // ahora se escribe
	index_KB[8] = file_desc -> timestamp_update >> 24;
	index_KB[9] = file_desc -> timestamp_update >> 16;
	index_KB[10] = file_desc -> timestamp_update >> 8;
	index_KB[11] = file_desc -> timestamp_update;

	return effective_bytes;
}


// int cz_close(czFILE* file_desc)


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


int cz_cp(char* orig, char* dest){
	if (!cz_exists(orig)){
		return 1; // no existe el archivo de origen
	}
	// reviso que no exista dest
	if (!cz_exists(dest)){
		return 2; // ya existe dest
	}
	// guardo el índice de orig en indice_orig
	int indice_orig = indice_dir_nombre(orig);
	// copiar de verdad el archivo de índice indice_orig... RELLENAR
	return 0;
}


int cz_rm (char* filename){
	if (!cz_exists(filename)){
		return 1; // no existe, no se puede borrar
	}
	int indice_file = indice_dir_nombre(filename);
	// dejamos el valid bit en 0
	dir_block[indice_file] -> valid = 0;
	// no debiera ser necesario cambiar el resto de las cosas...
	// llenar de 0 los content_blocks... RELLENAR
	return 0;
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


int main(int argc, char const *argv[])
{

	FILE * ptr = fopen("simdiskfilled.bin","rb");
	//FILE * ptr = fopen("db_test.bin","rb");  
	// r for read, b for binary

	build_dir_block(ptr);
	build_bit_map(ptr);
	build_blocks(ptr);


	czFILE* fd = cz_open("texto.txt", 'r');
	printf("Name: %s \n", fd -> name);
	printf("Mode: %c \n", fd -> mode);
	printf("Size: %u \n", fd -> size);
	printf("Index Block: %i \n", fd -> index_block);

	fd = cz_open("chest.wav", 'r');
	printf("Name: %s \n", fd -> name);
	printf("Mode: %c \n", fd -> mode);
	printf("Size: %u \n", fd -> size);
	printf("Index Block: %i \n", fd -> index_block);

	fd = cz_open("smile.png", 'r');
	printf("Name: %s \n", fd -> name);
	printf("Mode: %c \n", fd -> mode);
	printf("Size: %u \n", fd -> size);
	printf("Index Block: %i \n", fd -> index_block);

	fd = cz_open("aaaaa.gif", 'r');
	printf("Name: %s \n", fd -> name);
	printf("Mode: %c \n", fd -> mode);
	printf("Size: %u \n", fd -> size);
	printf("Index Block: %i \n", fd -> index_block);

	cz_ls();

	// printf("%d\n", n_bytes / 1024);

	// printf("%i \n", cz_rm("no existe")); // 1, no existe el archivo
	// printf("%i \n", cz_cp("no existe", "archivo")); // 1, no existe orig
	// printf("%i \n", cz_cp("arch", "archivo")); // 2, ya existe dest
	
	fclose(ptr);

	return 0;
}