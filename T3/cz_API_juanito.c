#include <stdio.h>
#include <string.h>
#include <stdlib>

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

direntry** dir_block = calloc(64, sizeof(direntry*))

int cz_exists(char* filename) {
	for (i = 0; i < 64; i++) {
		entry_name = dir_block[i] -> name;
		if (!strcmp(filename, entry_name)) {
			return 1;
		}
	}
	return 0;
}