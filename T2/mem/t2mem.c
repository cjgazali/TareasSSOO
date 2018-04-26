#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>

int i, j, k, m, n, o;


void update(int l, int *global_min, int *l_bits, int *l_bits_return) {
	int local_min = 0;
	for (n = 0; n < l - 1; n++) {
		local_min += pow(2, l_bits[j]);
	}
	local_min += pow(2, l_bits[l - 1]) * 1.375;
	if (local_min < *global_min) {
		printf("update!\n");
		*global_min = local_min;
		for (j = 0; j < l; j++) {
			l_bits_return[j] = l_bits[j];
		}
	}
}

void efficient_dir(int l, int *l_bits_return) {
	int l_bits[l];
	if (l == 1) {
		l_bits_return[0] = 20;
		return;
	}
	int global_min = 2147483647;
	
	for (i = 1; i < 20; i++) {
		l_bits[0] = i;
		if (l == 2) {
			l_bits[1] = 20 - i;
			update(l, &global_min, l_bits, l_bits_return);
		}
		else {
			for (j = 1; j < 20 - i + 1; j++) {
				l_bits[1] = j;
				// printf("%d\n", j);
				if (l == 3 && i + j < 20) {
					l_bits[2] = 20 - i - j;
					// printf("%d %d %d\n", l_bits[0], l_bits[1], l_bits[2]);
					update(l, &global_min, l_bits, l_bits_return);
					// printf("%d\n", j);
				}
				else {
					// printf("raro\n");
					for (k = 1; k < 20 - i - j + 1; k++) {
						l_bits[2] = k;
						if (l == 4) {
							l_bits[3] = 20 - i - j - k;
							update(l, &global_min, l_bits, l_bits_return);
						}
						else {
							for (m = 1; m < 20 - i - j - k + 1; m++) {
								l_bits[3] = m;
								l_bits[4] = 20 - i - j - k - m;
								update(l, &global_min, l_bits, l_bits_return);
							}
						}
					}
				}
				// printf("%d\n", j);
				// printf("bien\n");

			}
		}
		
	}
	// return l_bits_return;
}


// typedef struct TLB {
// 	int address = -1;
// 	int 
// }

typedef struct Row {
	struct Row** table;
	int frame;
	int valid;
	int extra_bit2;
	int extra_bit3;
} row;

row* create_row() {
	row* r = malloc(sizeof(row));
	r -> frame = -1;
	r -> valid = 0;
	r -> extra_bit2 = 0;
	r -> extra_bit3 = 0;
	return r;
}

row* create_and_assign(row** t) {
	row* r = create_row();
	r -> table = t;
	return r;
}

void free_tree(row** root, int levels, int *levels_bits) {
	row** t2;
	row** t3;
	row** t4;
	row** t5;
	row* rl1;
	row* rl2;
	row* rl3;
	row* rl4;
	for (i = 0; i < pow(2, levels_bits[0]); i++) {
		rl1 = root[i];
		if (levels > 1) {
			// printf("in if\n");
			// printf("%d\n", rl1 -> valid);
			// printf("%d\n", rl1 -> table);
			t2 = rl1 -> table;
			// printf("in if\n");
			for (j = 0; j < pow(2, levels_bits[1]); j++) {
				// printf("in for 2\n");
				rl2 = t2[j];
				if (levels > 2) {
					t3 = rl2 -> table;
					for (k = 0; k < pow(2, levels_bits[2]); k++) {
						rl3 = t3[k];
						if (levels > 3) {
							t4 = rl3 -> table;
							for (m = 0; m < pow(2, levels_bits[3]); m++) {
								rl4 = t4[m];
								if (levels > 4) {
									t5 = rl4 -> table;
									for (n = 0; n < pow(2, levels_bits[4]); n++) {
										printf("deep\n");
										free(t5[j]);
										printf("deep**\n");
									}
									free(t5);
								}
								free(rl4);
							}
							free(t4);
						}
						free(rl3);
					}
					free(t3);
				}
				// printf("free rl2\n");
				free(rl2);
			}
			// printf("free t2\n");
			free(t2);
		}
		// printf("free rl1\n");
		free(rl1);
	}
	printf("free root\n");
	free(root);
}

row* leaf(row** root, int l, int *input) {
	row** current = root;
	for (i = 0; i < (l - 1); i++) {
		current = current[input[i]] -> table;
	}
	return current[input[l - 1]];
}

unsigned long int_to_int(unsigned long k) {
    if (k == 0) return 0;
    if (k == 1) return 1;                       /* optional */
    return (k % 2) + 10 * int_to_int(k / 2);
}  // at https://stackoverflow.com/questions/5488377/converting-an-integer-to-binary-in-c

char* long_to_binary(unsigned long k)

{
        static char c[28];
        c[0] = '\0';

        unsigned long val;
        for (val = 1UL << (sizeof(unsigned long)*8-1); val > 0; val >>= 1)
        {   
            strcat(c, ((k & val) == val) ? "1" : "0");
        }
        return c;
}

int* dir_off_split(int addr) {
	printf("%d\n", addr);
	char str[256];
	sprintf(str, "%d", addr);  // to char bin
	char dir[256];
	strncpy(dir, str, 20);  // slice firsts
	dir[10] = 0; // null terminate destination
	printf("%s\n", dir);
	int cut = strtol(dir, NULL, 2);
	printf("%d\n", cut);
	printf("%d\n", addr - (cut * pow(10, 8)));
}


int main(int argc, char const *argv[])
{
	if(argc < 3)
    {
      printf("Mal uso de parámetros.\n");
      exit(2);
    }

    int levels = atoi(argv[1]);
    const char* filename = argv[2];

    int levels_bits[levels];
    // efficient_dir(levels, levels_bits);  //// DOOOOO

    if (levels == 1) {  // REMOOOOVE
    	levels_bits[0] = 20;
    }
    else if (levels == 2) {
    	levels_bits[0] = 10;
    	levels_bits[1] = 10;
    }
    else if (levels == 3) {
		levels_bits[0] = 7;
    	levels_bits[1] = 7;
    	levels_bits[2] = 6;
    }
    else if (levels == 4) {
		levels_bits[0] = 6;
    	levels_bits[1] = 5;
    	levels_bits[2] = 5;
    	levels_bits[3] = 4;
    }
    else if (levels == 5) {
		levels_bits[0] = 5;
    	levels_bits[1] = 4;
    	levels_bits[2] = 4;
    	levels_bits[3] = 6;
    	levels_bits[4] = 1;
    }


    //// construcción de niveles

    row** tn1 = calloc(pow(2, levels_bits[0]), sizeof(row*));
    
	for (i = 0; i < pow(2, levels_bits[0]); i++) {
		if (levels > 1) {
			row** tn2 = calloc(pow(2, levels_bits[1]), sizeof(row*));
			tn1[i] = create_and_assign(tn2);
			for (j = 0; j < pow(2, levels_bits[1]); j++) {
				if (levels > 2) {
					row** tn3 = calloc(pow(2, levels_bits[2]), sizeof(row*));
					tn2[j] = create_and_assign(tn3);
					// printf("here\n");
					// printf("%d\n", tn2[j] -> frame);
					for (k = 0; k < pow(2, levels_bits[2]); k++) {
						if (levels > 3) {
							row** tn4 = calloc(pow(2, levels_bits[3]), sizeof(row*));
							tn3[k] = create_and_assign(tn4);
							for (m = 0; m < pow(2, levels_bits[3]); m++) {
								if (levels > 4) {
									row** tn5 = calloc(pow(2, levels_bits[3]), sizeof(row*));
									tn4[m] = create_and_assign(tn5);
									for (n = 0; n < pow(2, levels_bits[4]); n++) {
										tn5[n] = create_row();
										tn5[n] -> frame = n;
									}
								}
								else {
									tn4[m] = create_row();
								}
							}
						}
						else {
							tn3[k] = create_row();
						}
					}
				}
				else {
					tn2[j] = create_row();
				}
			}
		}
		else {
			tn1[i] = create_row();
		}
	}
	//

	// TLB


	int q[levels];
	for (i = 0; i < levels; i++) {
		q[i] = 0;
	}
	// q[4] = 1;
	// printf("ok\n");
	// printf("%d\n", tn1[0] -> frame);

	// row* response = leaf(tn1, levels, q);

	// printf("ok\n");
	// printf("%d\n", response -> frame);
	// printf("ok\n");



	// int v_dir = int_to_int(268435455);
	// dir_off_split(v_dir);


	unsigned long addr = 268435455;
	printf("%ld\n", int_to_int(268435455));
	



	free_tree(tn1, levels, levels_bits);

	
	

	return 0;
}




