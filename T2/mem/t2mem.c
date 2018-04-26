#include <math.h>
#include <stdio.h>
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

typedef struct Row {
	struct Row** table;
	int frame;
	int valid;
} row;

row* create_row() {
	row* r = malloc(sizeof(row));
	r -> frame = -1;
	r -> valid = 0;
	return r;
}

void create_and_assign(row* r, row** t) {
	r = create_row();
	r -> table = t;
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

    if (levels == 1) {
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



    row** tn1 = calloc(pow(2, levels_bits[0]), sizeof(row*));
    
	for (i = 0; i < pow(2, levels_bits[0]); i++) {
		if (levels > 1) {
			row** tn2 = calloc(pow(2, levels_bits[1]), sizeof(row*));
			create_and_assign(tn1[i], tn2);
			for (j = 0; j < pow(2, levels_bits[1]); j++) {
				if (levels > 2) {
					row** tn3 = calloc(pow(2, levels_bits[2]), sizeof(row*));
					create_and_assign(tn2[j], tn3);
					for (k = 0; k < pow(2, levels_bits[2]); k++) {
						if (levels > 3) {
							row** tn4 = calloc(pow(2, levels_bits[3]), sizeof(row*));
							create_and_assign(tn3[j], tn4);
							for (m = 0; m < pow(2, levels_bits[3]); m++) {
								if (levels > 4) {
									row** tn5 = calloc(pow(2, levels_bits[3]), sizeof(row*));
									create_and_assign(tn4[j], tn5);
									for (n = 0; n < pow(2, levels_bits[4]); n++) {
										tn5[n] = create_row();
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

	return 0;
}




