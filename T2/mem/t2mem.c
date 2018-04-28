#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
// #include <time.h>

int i, j, k, m, n, o;


void update(int l, int *global_min, int *l_bits, int *l_bits_return) {
	int local_min = 0;
	for (o = 0; o < l - 1; o++) {
		local_min += pow(2, l_bits[o]) * l_bits[o + 1];
	}
	local_min += pow(2, l_bits[l - 1]) * 11;
	if (local_min < *global_min) {
		// printf("update!\n");
		*global_min = local_min;
		for (o = 0; o < l; o++) {
			l_bits_return[o] = l_bits[o];
			// printf("%d ", l_bits[o]);
		}
		// printf("\n");
	}
}

int efficient_dir(int l, int *l_bits_return) {

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

typedef struct TLBRow {
	int address;
	int frame;
	int valid;
	int timestamp;
} tlbrow;

tlbrow* create_tlb_row() {
	tlbrow* t = malloc(sizeof(tlbrow));
	t -> frame = -1;
	t -> valid = 0;
	t -> timestamp = 0;
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
	unsigned char frame[256];
	int valid;
	int at_tlb;
	row* at_pt;
	int timestamp;
} ramrow;

ramrow* create_mem_row() {
	ramrow* m = malloc(sizeof(ramrow));
	m -> valid = 0;
	m -> at_tlb = -1;
	m -> timestamp = 0;
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
										free(t5[n]);
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

char* int_to_bin_char(int num, char* bin, int length) {
	for (i = 0; i < length; i++) {
		bin[i] = '0';
	}
	int rest;
	int divint;
	i = 0;
	while (num != 0) {
		rest = num % 2;
		num = (num - rest) / 2;
		if (rest == 0) {
			bin[length - 1 - i] = '0';
		}
		else {
			bin[length - 1 - i] = '1';
		}
		i++;
	}
	bin[length] = 0;
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
	r -> timestamp = 0;
	// int adn = strtol(ad, NULL, 2);
	r -> address = adn;
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
	int max_time = 0;
	for (i = 0; i < 64; i++) {
		current = buff[i];
		if (current -> timestamp > max_time) {
			max_time = current -> timestamp;
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
    float mem_use = efficient_dir(levels, levels_bits);
    float bytes_used = mem_use / 8;
    for (i = 0; i < levels; i++) {
    	printf("BITS NIVEL %d: %d\n", i + 1, levels_bits[i]);
    }
    printf("ESPACIO UTILIZADO: %fB\n\n", bytes_used);


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

	// preparar simulación

	int q[levels];

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

	float tlbhits = 0.0;
	float page_faults = 0.0;
	float total = 0.0;

	char buffer[256];  // unsigned?
	FILE* data;
	data = fopen("data.bin", "r");
	int page;

	// SIMULACIÓN

	while ((read = getline(&line, &len, fp)) != -1) {

		total += 1;

		found = 0;

		// printf("%d\n", atoi(line));

		char* addr = calloc(28, sizeof(char));
		int_to_bin_char(atoi(line), addr, 28);

		char* direct = calloc(20, sizeof(char));
		char* offset = calloc(8, sizeof(char));
		dir_off_split(direct, offset, addr);

		// printf("%s\n", addr);
		// printf("%s\n", direct);
		// printf("%s\n", offset);

		offst = strtol(offset, NULL, 2);

		int directn = strtol(direct, NULL, 2);

		for (i = 0; i < 64; i++) {
			tlbrow* tlbr = TLB[i];
			if (tlbr -> valid == 1 && tlbr -> address == directn) {
				frm = tlbr -> frame;
				tlbr -> timestamp = 0;
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
			// printf("page fault\n");
			page_faults += 1;

			// lee data.bin
			page = directn * pow(2, 8);
			fseek(data, page, SEEK_SET);
			int n_bytes = fread(buffer, 1, 256, data);
			// printf("read %d\n", n_bytes);
			
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
				int max_time = 0;
				for (i = 0; i < 256; i++) {
					current = mem[i];
					if (current -> timestamp > max_time) {
						max_time = current -> timestamp;
						frm = i;
					}
				}
				// invalida entrada de tabla de páginas que usó ese frame
				row* prev_entry = mem[frm] -> at_pt;
				prev_entry -> valid = 0;
			}
			
			// printf("frame %d\n", frm);

			ramrow* current_frame = mem[frm];
			current_frame -> valid = 1;
			current_frame -> timestamp = 0;

			// actualiza entrada
			entry -> valid = 1;
			entry -> frame = frm;
			// referencia en frame
			current_frame -> at_pt = entry;

			// actualiza tlb y referencia en frame
			current_frame -> at_tlb = tlb_LRU_insert(TLB, directn, frm, mem);
			// printf("at tlb %d\n", current_frame -> at_tlb);

			// guarda en memoria la página leída
			// save_page(new_frame, buffer);
			unsigned char* new_frame = current_frame -> frame;
			for (i = 0; i < 256; i++) {
				new_frame[i] = buffer[i];
			}
		}

		for (i = 0; i < 256; i++) {
			mem[i] -> timestamp += 1;
		}
		for (i = 0; i < 64; i++) {
			TLB[i] -> timestamp += 1;
		}

		// resultados
		printf("-%d-\n", atoi(line));
		printf("DIRECCIÓN FÍSICA: %d\n", (frm * 256) + offst);
		unsigned char* final_frame = mem[frm] -> frame;
		unsigned char final_char = final_frame[offst];
		printf("CONTENIDO: %d\n", final_char);

		free(addr);
		free(direct);
		free(offset);
	}

	fclose(fp);
	fclose(data);

	// resultados
	printf("PORCENTAJE_PAGE_FAULTS = %f%%\n", 100.0 * page_faults / total);
	printf("PORCENTAJE_TLB_HITS = %f%%\n", 100.0 * tlbhits / total);
	printf("TLB\n");
	printf("i\t");
	for (i = 0; i < levels; i++) {
		printf("n%d_number\t", i + 1);
	}
	printf("frame_number\n");
	for (o = 0; o < 64; o++) {
		printf("%d\t", o);
		tlbrow* current = TLB[o];
		if (current -> valid == 1) {
			char row_address[20];
			int_to_bin_char(current -> address, row_address, 20);
			split_in_levels(row_address, levels, levels_bits, q);
			for (j = 0; j < levels; j++) {
				printf("%d\t\t", q[j]);
			}
			printf("%d\n", current -> frame);
		}
		else {
			for (j = 0; j < levels; j++) {
				printf("-\t\t");
			}
			printf("-\n");
		}
	}

	free_tree(tn1, levels, levels_bits);
	for (i = 0; i < 64; i++) {
		free(TLB[i]);
	}
	free(TLB);
	for (i = 0; i < 256; i++) {
		free(mem[i]);
	}
	free(mem);

	return 0;
}
