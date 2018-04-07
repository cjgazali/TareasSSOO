#include <stdio.h>

int main(int argc, char const *argv[])
{
	char* n;
	char* null;
	if (n) {
		printf("not\n");
	}
	n = null;
	if (n) {
		printf("not\n");
	}
	n = "hola";
	if (n) {
		printf("%s\n", n);
	}
	return 0;
}