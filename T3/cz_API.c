#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int i;
int z;

int n_bytes;

int n_blocks = 65536 - 9;

char bin_file_name[256];

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
	// block** content_blocks;  // 508
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

void save_full_name(char* old, char *dest) {
	for (z = 0; z < strlen(dest); z++) {
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


void save_int(unsigned char* buffer, int index, int integer) {
	buffer[index] = integer >> 24;
	buffer[index + 1] = integer >> 16;
	buffer[index + 2] = integer >> 8;
	buffer[index + 3] = integer;
}


int load_int(unsigned char* buffer, int index) {
	return buffer[index] << 24
		| buffer[index + 1] << 16
		| buffer[index + 2] << 8
		| buffer[index + 3];
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
		dir_block[i] -> index_block = load_int(buffer, 12);  // (buffer[12] << 24) | (buffer[13] << 16) | (buffer[14] << 8) | buffer[15];
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


void save_changes(char* bin_file) {
	unsigned char wbuffer_entry[16];
	direntry* entry;

	FILE* testfp = fopen(bin_file, "w");  // bin_file_name

	// cambios bloque directorio
	for (i = 0; i < 64; i++) {

		entry = dir_block[i];

		wbuffer_entry[0] = entry -> valid;

		char* name = entry -> name;
		for (z = 0; z < 11; z++) {
			wbuffer_entry[z + 1] = name[z];
		}

		save_int(wbuffer_entry, 12, entry -> index_block);

		fwrite(wbuffer_entry, 1, 16, testfp);

	}

	// cambios bitmap
	fwrite(bitmap, 1, 1024 * 8, testfp);

	// cambios otros bloques
	for (i = 0; i < n_blocks; i++) {
		fwrite(blocks[i] -> kilobyte, 1, 1024, testfp);
	}

	fclose(testfp);
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
	if (mode == 'r') {
		if (!exists) {
			return NULL;
		} else {
			// build czFILE
			int indice_bloque_leido = indice_dir_nombre(filename);
			direntry* entry = dir_block[indice_bloque_leido];
			czFILE* czfd = calloc(1, sizeof(czFILE));
			save_name(czfd -> name, filename);
			czfd -> mode = 'r'; // cambiado
			czfd -> index_block = entry -> index_block; // cambiado, voy a la entrada y pido index_block
			block* ptr_aux = get_block(czfd -> index_block); // para que no salga tan largo
			// czfd -> size = (get_block[czfd -> index_block][0] << 24) |
			// (get_block[czfd -> index_block][1] << 16) | 
			// (get_block[czfd -> index_block][2] << 8) | 
			// (get_block[czfd -> index_block][3]);
			czfd -> size = load_int(ptr_aux->kilobyte, 0);  // (unsigned int)((ptr_aux->kilobyte[0]<<24) | (ptr_aux->kilobyte[1]<<16)| (ptr_aux->kilobyte[2]<<8) | (ptr_aux->kilobyte[3])); // cambiado
			// printf("%u %u %u %u \n", ptr_aux->kilobyte[0], ptr_aux->kilobyte[1], ptr_aux->kilobyte[2], ptr_aux->kilobyte[3]);
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
			// ve si cabe en bloque directorio
			int has_space = 0;
			direntry* entry;
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

			// get index block kilobyte
			unsigned char* index_KB = get_block(czfd -> index_block) -> kilobyte;

			//// set index block metadata
			// set size and size metadata bytes
			save_int(index_KB, 0, 0);
			// set creation timestamp bytes
			save_int(index_KB, 4, czfd -> timestamp_create);
			// set update timestamp bytes
			save_int(index_KB, 8, czfd -> timestamp_update);

			return czfd;
		}
	}
	return NULL;
}


int n_blocks_read = 0;  // cantidad total de bloques leídos
int second_level_read = 0;  // 1 si ya estoy en direccionamiento indirecto
int n_level_blocks_read = 0;  // bloques leídos en nivel
int current_block_byte = 0;  // bytes leídos en bloque


int cz_read(czFILE* file_desc, void* buffer, int nbytes){

 	if (file_desc -> mode != 'r'){
 		return -1;
 	}

 	unsigned char* cbuffer = (unsigned char*)buffer;

 	// get index block kilobyte
	unsigned char* index_KB = get_block(file_desc -> index_block) -> kilobyte;
	int i_indirect_block = load_int(index_KB, 1020);
	unsigned char* indirect_index_KB = get_block(i_indirect_block) -> kilobyte;

	//// variables útiles:
	int effective_bytes = 0;  // tamaño leído de archivo a, valor de retorno
	int current_buffer_byte = 0;  // byte de buffer que se está escribiendo
	// int i_to_read;  // índice de bloque disponible encontrado
	unsigned char* current_kilobyte;  // puntero a KB de bloque leído
	// int current_kilobyte_byte = current_byte;  // índice de byte leído en bloque
	int iblock;
	// int restart_current_block_byte;
	// if (current_block_byte == 1024) {
	// 	restart_current_block_byte = 1;
	// } else {
	// 	restart_current_block_byte = 0;
	// }
	

	// if (second_level_read) {
	// 	int i_indirect_block = load_int(index_KB, 1020);
	// 	unsigned char* indirect_index_KB = get_block(i_indirect_block);
	// }

	while (effective_bytes < nbytes) {

		if (n_blocks_read == 252) {  // se acabó primer nivel
			second_level_read = 1;
			// int i_indirect_block = load_int(index_KB, 1020);
			// unsigned char* indirect_index_KB = get_block(i_indirect_block);
			n_level_blocks_read = 0;
		}

		if (second_level_read) {
			iblock = load_int(indirect_index_KB, 4 * n_level_blocks_read);
			current_kilobyte = get_block(iblock) -> kilobyte;
		} else {
			iblock = load_int(index_KB, 12 + 4 * n_level_blocks_read);
			current_kilobyte = get_block(iblock) -> kilobyte;
		}

		// if (restart_current_block_byte) {
		// 	current_block_byte = 0;
		// } else {
		// 	restart_current_block_byte = 1;
		// }

		while (current_block_byte < 1024 && effective_bytes < nbytes) {
			cbuffer[current_buffer_byte] = current_kilobyte[current_block_byte];
			current_buffer_byte++;
			current_block_byte++;
			// n_bytes_read++;
			effective_bytes++;
		}

		if (current_block_byte < 1024) {
			break;
		}
		n_level_blocks_read++;
		n_blocks_read++;
		current_block_byte = 0;
	}

	return effective_bytes;

 }


int cz_write(czFILE* file_desc, void* buffer, int nbytes) {

	if (file_desc -> mode != 'w') {
		return -1;
	}

	unsigned char* cbuffer = (unsigned char*)buffer;

	// get index block kilobyte
	unsigned char* index_KB = get_block(file_desc -> index_block) -> kilobyte;

	//// variables útiles:
	int effective_bytes = 0;  // tamaño guardado de archivo en B, valor de retorno
	int i_to_fill;  // índice de bloque disponible encontrado
	unsigned char* current_kilobyte;  // puntero a KB de bloque disponible
	int current_kilobyte_byte;  // índice de byte escribiendo en bloque
	int n_first_level_block = 0;  // # bloques de primer nivel llenados
	int base;
	int second_level = 0;

	//// escritura:  UN NIVEL DE MOMENTO
	while (effective_bytes < nbytes) {

		i_to_fill = get_first_available();
		if (i_to_fill == -1) {  // no se puede seguir escribiendo porque no quedan bloques
			return effective_bytes;
		}
		up_bitmap_bit(i_to_fill);

		// file_desc -> content_blocks[0] = get_block(i_to_fill);

		base = 12 + (n_first_level_block * 4);
		save_int(index_KB, base, i_to_fill);
		// index_KB[base] = i_to_fill >> 24;
		// index_KB[base + 1] = i_to_fill >> 16;
		// index_KB[base + 2] = i_to_fill >> 8;
		// index_KB[base + 3] = i_to_fill;
		if (n_first_level_block == 252) {  // se acabó primer nivel
			second_level = 1;
			break;
		}
		n_first_level_block++;

		current_kilobyte = get_block(i_to_fill) -> kilobyte;
		current_kilobyte_byte = 0;

		while (current_kilobyte_byte < 1024 && effective_bytes < nbytes) {
			current_kilobyte[current_kilobyte_byte] = cbuffer[effective_bytes];
			current_kilobyte_byte++;
			effective_bytes++;
		}
	}


	if (second_level) {

		unsigned char* indirect_index_KB = get_block(i_to_fill) -> kilobyte;

		int n_second_level_block = 0;

		while (effective_bytes < nbytes) {
			// misma idea que el de primer nivel, otras variables y límite

			i_to_fill = get_first_available();
			if (i_to_fill == -1) {  // no se puede seguir escribiendo porque no quedan bloques
				return effective_bytes;
			}
			up_bitmap_bit(i_to_fill);

			// file_desc -> content_blocks[0] = get_block(i_to_fill);

			base = n_second_level_block * 4;
			save_int(indirect_index_KB, base, i_to_fill);
			n_second_level_block++;

			current_kilobyte = get_block(i_to_fill) -> kilobyte;
			current_kilobyte_byte = 0;

			while (current_kilobyte_byte < 1024 && effective_bytes < nbytes) {
				current_kilobyte[current_kilobyte_byte] = cbuffer[effective_bytes];
				current_kilobyte_byte++;
				effective_bytes++;
			}

			if (n_second_level_block == 256 && effective_bytes < nbytes) {
				// se acabó segundo nivel
				// solo entra si NO terminó de escribir nbytes además
				break;
			}
		}
	}
	

	//// set index block metadata
	// set size and size metadata bytes
	file_desc -> size = effective_bytes;
	save_int(index_KB, 0, effective_bytes);
	// set creation timestamp bytes
	save_int(index_KB, 4, file_desc -> timestamp_create);
	// set update timestamp bytes
	file_desc -> timestamp_update = time(NULL);  // ahora se escribe
	save_int(index_KB, 8, file_desc -> timestamp_update);


	return effective_bytes;
}


int cz_close(czFILE* file_desc) {
	if (file_desc -> mode == 'r') {
		n_blocks_read = 0;
		second_level_read = 0;
		n_level_blocks_read = 0;
		current_block_byte = 0;

		free(file_desc);
		return 0;
	}

	save_changes("my.bin");

	free(file_desc);
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
		return 1;  // no existe, no se puede borrar
	}
	int idir = indice_dir_nombre(filename);
	// dejamos el valid bit en 0
	direntry* entry = dir_block[idir];
	entry -> valid = 0;

	down_bitmap_bit(entry -> index_block);
	unsigned char* index_KB = get_block(entry -> index_block) -> kilobyte;
	
	int size = load_int(index_KB, 0);
	int KBs = size / 1024;
	if (size % 1024) {KBs++;}

	unsigned int block_to_down;
	int n_blocks = 0;
	int second_level = 0;

	while (n_blocks < KBs) {
		block_to_down = load_int(index_KB, 12 + (4 * n_blocks));
		down_bitmap_bit(block_to_down);
		if (n_blocks == 252) {
			second_level = 1;
			break;
		}
		n_blocks++;
	}

	if (second_level) {
		unsigned char* indirect_index_KB = get_block(block_to_down) -> kilobyte;
		int indirect_n_blocks = 0;
		while (n_blocks < KBs) {
			block_to_down = load_int(indirect_index_KB, 4 * indirect_n_blocks);
			down_bitmap_bit(block_to_down);
			indirect_n_blocks++;
			n_blocks++;
		}
	}

	save_changes("myrm.bin");

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


void init(char* source_bin) {
	save_full_name(bin_file_name, source_bin);
	printf("source: %s\n", bin_file_name);

	FILE* bfd = fopen(bin_file_name, "r");

	build_dir_block(bfd);
	build_bit_map(bfd);
	build_blocks(bfd);

	fclose(bfd);
}


int main(int argc, char const *argv[])
{

	// inicializa datos a partir de archivo .bin fuente
	init("simdiskfilled.bin");

	// archivos que vienen
	printf("source files:\n");
	cz_ls();

	// lee y hace dump de archivo que viene
	// ver nombres en terminal si es necesario y uncomment para generar archivo
	// cz_read_dump("texto.txt")

	// abre archivo 'newfile' en modo escritura
	czFILE* fd = cz_open("newfile", 'w');

	// crear buffer para escritura
	int wbuffer_size = 300;  // ajustar a gusto
	char c[wbuffer_size * 1024];
	
	for (i = 0; i < wbuffer_size * 1024; i++) {
		c[i] = 'a';  // 0x61
	}
	// para primer read:
	c[0] = 'a';
	c[1] = 'b';
	c[2] = 'c';
	c[3] = '1';
	c[4] = '9';
	c[5] = '7';
	c[6] = '3';
	// para segundo read
	c[15] = '8';
	// para read con direccionamiento indirecto
	c[295 * 1024] = 'x';

	// escribe buffer
	printf("\nWRITE\n");
	int bytes_writen = cz_write(fd, c, wbuffer_size * 1024);

	printf("KB written: %d\n", bytes_writen / 1024);

	// cierra archivio y garantiza cambios en .bin
	cz_close(fd);

	// muestra lista con newfile
	printf("added 'newfile':\n");
	//cz_ls();

	printf("\nREAD\n");
	fd = cz_open("newfile", 'r');

	char buf[11];
	printf("bytes leídos: %d\n", cz_read(fd, buf, 10));
	buf[10] = '\0';
	printf("primer read: %s\n", buf);
	
	printf("bytes leídos: %d\n", cz_read(fd, buf, 10));
	buf[10] = '\0';
	printf("segundo read: %s\n", buf);

	char buff[wbuffer_size * 1024];
	printf("bytes leídos: %d\n", cz_read(fd, buff, wbuffer_size * 1024 - 20));
	printf("read de 'x' en direccionamiento indirecto: %c\n", buff[295 * 1024 - 20]);

	cz_close(fd);

	// cambia nombre
	printf("\nMOVE\n");
	cz_mv("newfile", "iFile");

	// muestra lista con iFile
	printf("'newfile' to 'iFile':\n");
	//cz_ls();

	// copia iFile en myFile
	printf("\nCOPY\n");
	// cp

	// muestra lista con copia
	printf("'iFile' to 'myFile':\n");
	//cz_ls();

	// borra archivos: cambios en validez y bitmap pero ruido queda
	printf("\nREMOVE\n");
	cz_rm("iFile");
	// cz_rm("myFile");

	// muestra lista original
	printf("source files:\n");
	cz_ls();

	// printf("%i \n", cz_cp("no existe", "archivo")); // 1, no existe orig
	// printf("%i \n", cz_cp("arch", "archivo")); // 2, ya existe dest
	

	return 0;
}