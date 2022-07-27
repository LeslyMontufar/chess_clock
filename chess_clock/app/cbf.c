#include <stdbool.h>
#include <stdint.h>
#include "cbf.h"

#define CBF_INC(v,mv) (((v+1) >= mv) ? 0 : (v+1)) // Faz o giro no buffer circular

cbf_status_t cbf_init(cbf_t *cb, uint8_t *area, uint16_t size){
	cb->buffer = area;
	cb->size = size;
	cb->prod = cb->cons = 0;

	return CBF_OK;
}

uint16_t cbf_bytes_available(cbf_t *cb){ // dados que ainda precisam ser lidos/consumidos do buffer
	if(cb->prod >= cb->cons)
		return cb->prod - cb->cons;
	else
		return cb->prod + (cb->size - cb->cons);
}

cbf_status_t cbf_flush(cbf_t *cb){
	cb->prod = cb->prod = 0; // não precisava igualar a zero
	return CBF_OK;
}

cbf_status_t cbf_get(cbf_t *cb, uint8_t *c){

	if(cb->cons == cb->prod) // tudo que foi produzido (dado criado no buffer) já foi consumido (já usado)
		return CBF_EMPTY;

	*c = cb->buffer[cb->cons]; //cb->cons + 1 tem o índice do próximo dado a ser lido, e o cb->prod + 1, a posição onde escrever o próximo dado
	cb->cons = CBF_INC(cb->cons, cb->size); // vê se será pos+1 ou volta para a pos==0 no buffer circular

	return CBF_OK;
}

cbf_status_t cbf_put(cbf_t *cb, uint8_t c){
	uint16_t next_prod = CBF_INC(cb->prod, cb->size);

	if(next_prod == cb->cons) // evitar de um dado sobreescrever um dado já existente no buffer circular, overrun
		return CBF_FULL;

	cb->buffer[cb->prod] = c; // Na posição do produtor atual, colocar o dado c.
	cb->prod = next_prod;

	return CBF_OK;
}


