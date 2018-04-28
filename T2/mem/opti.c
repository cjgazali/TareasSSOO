#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>

int i, j, k, m, n, o;


void update(int l, int *global_min, int *l_bits, int *l_bits_return) {
	int local_min = 0;
	for (o = 0; o < l - 1; o++) {
		local_min += pow(2, l_bits[o]) * l_bits[o + 1];
	}
	local_min += pow(2, l_bits[l - 1]) * 11;
	if (local_min < *global_min) {
		printf("update!\n");
		*global_min = local_min;
		for (o = 0; o < l; o++) {
			l_bits_return[o] = l_bits[o];
			printf("%d ", l_bits[o]);
		}
		printf("\n");
	}
}

int new_efficient_dir(int l, int* l_bits_return) {

	int global_min = 2147483647;
	int l_bits[l];

	if (l == 1) {
		l_bits_return[0] = 20;
		global_min = pow(2, 20) * 11;
		return global_min;
	}

	for (i = 1; i < 20; i++) {
		l_bits[0] = i;
		for (j = 1; j < 20; j++) {
			l_bits[1] = j;
			if (l > 2) {
				for (k = 1; k < 20; k++) {
					l_bits[2] = k;
					if (l > 3) {
						for (m = 1; m < 20; m++) {
							l_bits[3] = m;
							if (l > 4) {
								for (n = 1; n < 20; n++) {
									l_bits[4] = n;
									if (i + j + k + m + n == 20) {
										update(l, &global_min, l_bits, l_bits_return);
									}
								}
							}
							else {
								if (i + j + k + m == 20) {
									update(l, &global_min, l_bits, l_bits_return);
								}
							}
						}
					}
					else {
						if (i + j + k == 20) {
							update(l, &global_min, l_bits, l_bits_return);
						}
					}
				}
			}
			else {
				if (i + j == 20) {
					update(l, &global_min, l_bits, l_bits_return);
				}
			}
		}
	}
	return global_min;
}

int main(int argc, char const *argv[])
{
	if(argc < 2)
    {
      printf("Mal uso de parámetros.\n");
      exit(2);
    }

    int levels = atoi(argv[1]);
    int levels_bits[levels];

    int mem_use = new_efficient_dir(levels, levels_bits);

    for (i = 0; i < levels; i++) {
    	printf("%d\n", levels_bits[i]);
    }
    printf("mem use %d\n", mem_use);
}