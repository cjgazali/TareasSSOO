#include <stdio.h>
#include <string.h>


//void save_name(char* old, char *dest) {
//	for (z = 0; z < strlen(dest); z++) {
//		old[z] = dest[z];
//		if (z == 9) {  // recorta si se excede
//			break;
//		}
//	}
//	old[z + 1] = '\0';
//}

int main(int argc, char const *argv[])
{
	char c[11] = "hola";
	int i;
	for (i = 0; i < strlen(c); i++) {
		printf("i %d\n", i);
	}
	printf("%s\n", c);
	printf("%d\n", strcmp(c, "hola"));
	printf("%d\n", strcmp(c, "hola\0"));
	printf("%d\n", strcmp(c, "holas"));
	printf("%d\n", strcmp(c, "Hholass"));

	printf("\n");

	unsigned int n = 30;
	printf("%d\n", n / 8);
	printf("%d\n", n % 8);

	printf("\n");

	char byte = 5;
	char pass = 2;
	char mask = 1 << pass;
	printf("%d\n", byte & mask);
	printf("%d\n", (byte & mask) >> pass);

	return 0;
}