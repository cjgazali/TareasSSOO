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

	printf("\n");

	char ch = 'w';
	if (ch == 'w') {
		printf("okay\n");
	}

	printf("\n");

	int num = 65534;
	unsigned char c1 = num >> 8;
	printf("%d\n", c1);
	unsigned char c2 = num;
	printf("%d\n", c2);
	unsigned int recover = c1 << 8 | c2;
	printf("recovered %d\n", recover);

	unsigned int numl = 4294967294;
	unsigned char ca = numl >> 24;
	unsigned char cb = numl >> 16;
	unsigned char cc = numl >> 8;
	unsigned char cd = numl;
	printf("%d\n", ca);
	printf("%d\n", cb);
	printf("%d\n", cc);
	printf("%d\n", cd);
	unsigned int recover1 = ca << 24 | cb << 16 | cc << 8 | cd;
	printf("recovered %u\n", recover1);

	return 0;
}