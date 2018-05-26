#ifndef CZ_API_H
#define CZ_API_H

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
} czFILE;

void save_name(char* old, char *dest);
void save_full_name(char* old, char *dest);
void guardar_caracteres(char* old, unsigned char* dest, int inicio, int final);
void mostrar_letras(char* array);
void save_int(unsigned char* buffer, int index, int integer);
int load_int(unsigned char* buffer, int index);
void build_dir_block(FILE* filedesc);
void build_bit_map(FILE* filedesc);
char get_bitmap_bit(unsigned int iblock);
void up_bitmap_bit(unsigned int iblock);
void down_bitmap_bit(unsigned int iblock);
int get_first_available();
void build_blocks(FILE* filedesc);
block* get_block(int b);
int indice_dir_nombre (char* filename);
void save_changes(char* bin_file);
void init(char* source_bin);
int cz_exists(char* filename);
czFILE* cz_open(char* filename, char mode);
int cz_read(czFILE* file_desc, void* buffer, int nbytes);
int cz_write(czFILE* file_desc, void* buffer, int nbytes);
int cz_close(czFILE* file_desc);
int cz_mv(char* orig, char *dest);
int cz_cp(char* orig, char* dest);
int cz_rm (char* filename);
void cz_ls();


#endif