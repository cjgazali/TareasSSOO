#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>

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

typedef struct TLBRow {
	int address;
	int frame;
	int valid;
	time_t timestamp;
} tlbrow;

tlbrow* create_tlb_row() {
	tlbrow* t = malloc(sizeof(tlbrow));
	t -> frame = -1;
	t -> valid = 0;
	return t;
}

typedef struct Row {  // de tabla de páginas
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

typedef struct RAMrow {
	char frame[256];
	int valid;
	int at_tlb;
	row* at_pt;
	time_t timestamp;
} ramrow;

ramrow* create_mem_row() {
	ramrow* m = malloc(sizeof(ramrow));
	m -> valid = 0;
	m -> at_tlb = -1;
	return m;
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
	// printf("free root\n");
	free(root);
}

row* leaf(row** root, int l, int *input) {
	row** current = root;
	for (i = 0; i < (l - 1); i++) {
		current = current[input[i]] -> table;
	}
	return current[input[l - 1]];
}

char* int_to_bin_char(int num, char* bin) {
	// char bin[] = "0000000000000000000000000000";
	// char* bin = malloc(sizeof(char)*28);
	for (i = 0; i < 28; i++) {
		bin[i] = '0';
	}
	int rest;
	int divint;
	i = 0;
	while (num != 0) {
		rest = num % 2;
		num = (num - rest) / 2;
		if (rest == 0) {
			bin[27 - i] = '0';
		}
		else {
			bin[27 - i] = '1';
		}
		i++;
	}
	bin[28] = 0;
	return bin;
}

void dir_off_split(char* left, char* right, char* all) {
	strncpy(left, all, 20);  // slice firsts
	left[20] = 0; // null terminate destination
	for (i = 0; i < 8; i++) {
		right[i] = all[20 + i];
	}
	right[8] = 0;
}

void split_in_levels(char* ad, int l, int* l_bits, int* quest) {
	
	// printf("%s\n", ad);

	if (l == 1) {
		quest[0] = strtol(ad, NULL, 2);
		// printf("%d\n", quest[0]);
		return;
	}

	//char l1[l_bits[0]];
	//strncpy(l1, ad, l_bits[0]);
	//printf("%s\n", l1);
	//quest[0] = strtol(l1, NULL, 2);
	//printf("%d\n", quest[0]);

	

	int at = 0;

	for (i = 0; i < l; i++) {
		char li[l_bits[i]];
		for (j = 0; j < l_bits[i]; j++) {
			li[j] = ad[at + j];
		}
		li[l_bits[i]] = 0;
		// printf("%d with %s\n", i, li);
		quest[i] = strtol(li, NULL, 2);
		// printf("%d with %d\n", i, quest[i]);
		at += l_bits[i];
	}

}

void update_tlb_row(tlbrow* r, int adn, int f) {
	r -> valid = 1;
	r -> frame = f;
	r -> timestamp = time(NULL);
	// int adn = strtol(ad, NULL, 2);
	r -> address = adn;
}

//void update_ram_row()

int get_LRU_frame(ramrow** m) {
	ramrow* current;
	for (i = 0; i < 256; i++) {
		current = m[i];
		if (current -> valid == 0) {
			return i;
		}
	}
	int lru = 0;
	time_t min_time = time(NULL) + 100;
	for (i = 0; i < 256; i++) {
		current = m[i];
		if (current -> timestamp < min_time) {
			min_time = current -> timestamp;
			lru = i;
		}
	}
	return lru;
}

int tlb_LRU_insert(tlbrow** buff, int adn, int f, ramrow** m) {
	tlbrow* current;
	for (i = 0; i < 64; i++) {
		current = buff[i];
		if (current -> valid == 0) {
			update_tlb_row(current, adn, f);
			return i;
		}
	}
	int lru = 0;
	time_t min_time = time(NULL) + 100;
	for (i = 0; i < 64; i++) {
		current = buff[i];
		if (current -> timestamp < min_time) {
			min_time = current -> timestamp;
			lru = i;
		}
	}
	m[buff[lru] -> frame] -> at_tlb = -1;
	update_tlb_row(buff[lru], adn, f);
	return lru;
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
	tlbrow** TLB = calloc(64, sizeof(tlbrow*));
	for (i = 0; i < 64; i++) {
		TLB[i] = create_tlb_row();
	}

	// memoria física
	ramrow** mem = calloc(256, sizeof(ramrow*));
	for (i = 0; i < 256; i++) {
		mem[i] = create_mem_row();
	}



	int q[levels];
	// for (i = 0; i < levels; i++) {
	// 	q[i] = 0;
	// }


	// q[4] = 1;
	// printf("ok\n");
	// printf("%d\n", tn1[0] -> frame);

	// row* response = leaf(tn1, levels, q);

	// printf("ok\n");
	// printf("%d\n", response -> frame);
	// printf("ok\n");


	// char* addr = malloc(sizeof(char)*28);
	// int_to_bin_char(35363, addr);

	// char* direct = malloc(sizeof(char)*20);
	// char* offset = malloc(sizeof(char)*8);
	//dir_off_split(direct, offset, addr);
	// printf("%s\n", addr);
	// printf("%s\n", direct);
	// printf("%s\n", offset);

	//split_in_levels(direct, levels, levels_bits, q);

	// free(addr);
	// free(direct);
	// free(offset);
	// free(divided);

	FILE* fp;
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	fp = fopen(filename, "r");

	int frm;
	int offst;
	int found;
	// int i_tlb;
	row* entry;

	int tlbhits = 0;
	int page_faults = 0;

	char buffer[256];  // unsigned?
	FILE* data;
	data = fopen("data.bin", "r");
	int page;

	while ((read = getline(&line, &len, fp)) != -1) {

		found = 0;

		printf("%d\n", atoi(line));

		char* addr = calloc(28, sizeof(char));
		int_to_bin_char(atoi(line), addr);

		char* direct = calloc(20, sizeof(char));
		char* offset = calloc(8, sizeof(char));
		dir_off_split(direct, offset, addr);

		printf("%s\n", addr);
		printf("%s\n", direct);
		printf("%s\n", offset);

		offst = strtol(offset, NULL, 2);

		int directn = strtol(direct, NULL, 2);

		for (i = 0; i < 64; i++) {
			tlbrow* tlbr = TLB[i];
			if (tlbr -> valid == 1 && tlbr -> address == directn) {
				frm = tlbr -> frame;
				tlbr -> timestamp = time(NULL);
				found = 1;
				// i_tlb = i;
				tlbhits += 1;
				continue;
			}
		}

		if (!found) {
			split_in_levels(direct, levels, levels_bits, q);
			entry = leaf(tn1, levels, q);
			if (entry -> valid == 1) {
				frm = entry -> frame;
				found = 1;
				mem[frm] -> at_tlb = tlb_LRU_insert(TLB, directn, frm, mem);
			}
		}

		if (!found) {
			printf("page fault\n");
			page_faults += 1;

			// lee data.bin
			page = directn * pow(2, 8);
			fseek(data, page, SEEK_SET);
			int n_bytes = fread(buffer, 1, 256, data);
			printf("read %d\n", n_bytes);
			
			// obtiene frame a usar con LRU si es necesario

			// frm = get_LRU_frame(mem);
			int do_lru = 1;
			ramrow* current;
			for (i = 0; i < 256; i++) {
				current = mem[i];
				if (current -> valid == 0) {
					frm = i;
					do_lru = 0;
					break;
				}
			}
			if (do_lru) {
				int lru = 0;
				time_t min_time = time(NULL) + 100;
				for (i = 0; i < 256; i++) {
					current = mem[i];
					if (current -> timestamp < min_time) {
						min_time = current -> timestamp;
						frm = i;
					}
				}
				// invalida entrada de tabla de páginas que usó ese frame
				row* prev_entry = mem[frm] -> at_pt;
				// char prev_direct[20];
				// int_to_bin_char(prev_pt, prev_direct);
				// printf("prev pt %d\n", prev_pt);
				// printf("prev direct %s\n", prev_direct);
				// split_in_levels(prev_direct, levels, levels_bits, q);  // CAMBIA Q!!!
				// row* prev_entry = leaf(tn1, levels, q);
				prev_entry -> valid = 0;
			}
			
			printf("frame %d\n", frm);

			ramrow* current_frame = mem[frm];
			current_frame -> valid = 1;
			current_frame -> timestamp = time(NULL);

			// actualiza entrada
			entry -> valid = 1;
			entry -> frame = frm;
			// referencia en frame
			current_frame -> at_pt = entry;

			// actualiza tlb y referencia en frame
			current_frame -> at_tlb = tlb_LRU_insert(TLB, directn, frm, mem);
			printf("at tlb %d\n", current_frame -> at_tlb);

			// current_frame -> frame
		}




		free(addr);
		free(direct);
		free(offset);
	}

	fclose(fp);
	fclose(data);

	free_tree(tn1, levels, levels_bits);

	return 0;
}




