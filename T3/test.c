#include <stdio.h>
#include <string.h>


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
	return 0;
}