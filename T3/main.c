#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "cz_API.h"


int i;

int main(int argc, char const *argv[])
{

	if (argc < 2) {
		printf("Mal uso de parámetros.\n");
    	exit(2);
	}

	// inicializa datos a partir de archivo .bin fuente
	init((char*)argv[1]);

	// archivos que vienen
	printf("\nsource files:\n");
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
	cz_ls();

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

	// existe nuevo
	printf("\nExiste 'newfile': %d\n", cz_exists("newfile"));

	// cambia nombre
	printf("\nMOVE\n");
	cz_mv("newfile", "iFile");

	// muestra lista con iFile
	printf("'newfile' to 'iFile':\n");
	cz_ls();

	// copia iFile en myFile
	printf("\nCOPY\n");
	cz_cp("iFile", "myFile");

	// muestra lista con copia
	printf("'iFile' cp to 'myFile':\n");
	cz_ls();

	// borra archivos: cambios en validez y bitmap pero ruido queda
	printf("\nREMOVE\n");
	cz_rm("iFile");
	cz_rm("myFile");

	// muestra lista sin iFile
	printf("post remove:\n");
	cz_ls();

	printf("\n");

	liberar();

	return 0;
}