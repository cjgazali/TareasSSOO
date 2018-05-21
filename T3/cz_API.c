#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int i;
int z;

typedef struct dirBlockEntry {
	unsigned char valid;
	char name[11];
	int index_block;
} direntry;

// int dir_block_length 64;

// typedef struct dirBlock {
// 	dire** entries[64];
// } dirblock;

// dirblock* create_dir_block() {
// 	db = calloc(1, sizeof(dirblock));
// 	db -> length = 64;
// }

direntry** dir_block;  // = calloc(64, sizeof(direntry*));

int cz_exists(char* filename) {
	for (i = 0; i < 64; i++) {
		if (!strcmp(filename, dir_block[i] -> name)) {
			return 1;
		}
	}
	return 0;
}

void save_name(char* old, char *dest) {
	for (z = 0; z < strlen(dest); z++) {
		old[z] = dest[z];
		if (z == 9) {  // recorta si se excede
			break;
		}
	}
	old[z + 1] = '\0';
}

int cz_mv(char* orig, char *dest) {
	for (i = 0; i < 64; i++) {
		if (!strcmp(dest, dir_block[i] -> name)) {
			return 1;
		}
	}
	for (i = 0; i < 64; i++) {
		if (!strcmp(orig, dir_block[i] -> name)) {
			save_name(dir_block[i] -> name, dest);
			break;
		}
	}
	return 0;
}


void cs_ls() {
	for (i = 0; i < 64; i++) {
		direntry* entry = dir_block[i];
		if (entry -> valid) {
			printf("%s\n", entry -> name);
		}
	}
}
