


	int n_bloques_leidos = 0;
int last_byte_leido = -1;

int cz_read(czFILE* file_desc, void* buffer, int nbytes){
 	// marcar pos_ultima_llamada_read
 	int num_bytes_leidos = 0;
 	// Primer error, pueden haber más...
 	if (file_desc -> mode != 'r'){
 		return -1;
 	}

 	int byte_actual_buffer = 0; 

 	unsigned char* cbuffer = (unsigned char*) buffer;

 	block* bloque_indice = get_block(file_desc -> index_block);

 	if (n_bloques_leidos >= 252) {
 		
 	}

 		int n_bloques_leidos = 
 		int entero_para_get_block = load_int(bloque_indice->kilobyte, 12 + 4*n_bloque_actual);
 		block* bloque_actual = get_block(entero_para_get_block);
 		
 	while (nbytes > 0){ // revisar condiciones... // && pos_ultima_llamada_read < file_desc -> size 
 		while (nbytes > 0 && byte_actual_bloque < 1024){
 			cbuffer[byte_actual_buffer] = bloque_actual->kilobyte[byte_actual_bloque];
 			byte_actual_buffer++;
 			byte_actual_bloque++;
 			nbytes--;
 			num_bytes_leidos++;
 		}
 		if (n_bloque_actual == 252){ // uso la indirección, equivale a cambiar el bloque índice
 			bloque_indice = get_block(load_int(bloque_indice->kilobyte, 1020));  // (bloque_indice->kilobyte[1020]<<24) | (bloque_indice->kilobyte[1021]<<16) | (bloque_indice->kilobyte[1022]<<8) | (bloque_indice->kilobyte[1023]));
 			byte_actual_bloque = 0; // reinicio el bloque a leer...
 			indice_bloque_actual = 0; // y parto del 0, no del 12
 		}
 		if (byte_actual_bloque == 1024){
 			indice_bloque_actual++; // a menos que me pase...
 			n_bloque_actual++;
 			entero_para_get_block = load_int(bloque_indice->kilobyte, 4*indice_bloque_actual);
 			bloque_actual = get_block(entero_para_get_block);
 			byte_actual_bloque = 0;
 		}
 	}
 	buffer = (void*) cbuffer; // devolver al buffer real
 	return num_bytes_leidos;
 }

int cz_read(czFILE* file_desc, void* buffer, int nbytes){
 	// marcar pos_ultima_llamada_read
 	int num_bytes_leidos = 0;
 	// Primer error, pueden haber más...
 	if (file_desc -> mode != 'r'){
 		return -1;
 	}

 	unsigned char* cbuffer = (unsigned char*) buffer;
 	block* bloque_indice = get_block(file_desc -> index_block);
 	int byte_actual_buffer = 0; 
 	// block* bloque_actual = blocks[bloque_indice->kilobyte[indice_bloque_actual]]; //get block, y usar 4
 	int entero_para_get_block = load_int(bloque_indice->kilobyte, 12 + 4*indice_bloque_actual);  // (bloque_indice->kilobyte[4*indice_bloque_actual]<<24) | (bloque_indice->kilobyte[4*indice_bloque_actual+1]<<16) | (bloque_indice->kilobyte[4*indice_bloque_actual+2]<<8) | (bloque_indice->kilobyte[4*indice_bloque_actual+3]);
 	block* bloque_actual = get_block(entero_para_get_block);
 	while (nbytes > 0){ // revisar condiciones... // && pos_ultima_llamada_read < file_desc -> size 
 		while (nbytes > 0 && byte_actual_bloque < 1024){
 			cbuffer[byte_actual_buffer] = bloque_actual->kilobyte[byte_actual_bloque];
 			byte_actual_buffer++;
 			byte_actual_bloque++;
 			nbytes--;
 			num_bytes_leidos++;
 		}
 		if (indice_bloque_actual == 253){ // uso la indirección, equivale a cambiar el bloque índice
 			bloque_indice = get_block(load_int(bloque_indice->kilobyte, 1020));  // (bloque_indice->kilobyte[1020]<<24) | (bloque_indice->kilobyte[1021]<<16) | (bloque_indice->kilobyte[1022]<<8) | (bloque_indice->kilobyte[1023]));
 			byte_actual_bloque = 0; // reinicio el bloque a leer...
 			indice_bloque_actual = 0; // y parto del 0, no del 12
 		}
 		if (byte_actual_bloque == 1024){
 			indice_bloque_actual++; // a menos que me pase...
 			entero_para_get_block = load_int(bloque_indice->kilobyte, 4*indice_bloque_actual);  // (bloque_indice->kilobyte[4*indice_bloque_actual]<<24) | (bloque_indice->kilobyte[4*indice_bloque_actual+1]<<16) | (bloque_indice->kilobyte[4*indice_bloque_actual+2]<<8) | (bloque_indice->kilobyte[4*indice_bloque_actual+3]);
 			bloque_actual = get_block(entero_para_get_block);
 			byte_actual_bloque = 0;
 		}
 	}
 	buffer = (void*) cbuffer; // devolver al buffer real
 	return num_bytes_leidos;
 }
