#pragma once

typedef enum cbf_status_s{
	CBF_OK = 0,
	CBF_FULL,
	CBF_EMPTY,
} cbf_status_t;

typedef struct cbf_s{
	volatile uint16_t prod;	// Indicador da posição de produção no buffer circular
	volatile uint16_t cons; // Indicador da posição de consumo no buffer circular
	uint16_t size;			// Tamanho total do buffer circular
	uint8_t *buffer;		// Ponteiro para a área de dados do buffer circular
} cbf_t;

cbf_status_t cbf_init(cbf_t *cb, uint8_t *area, uint16_t size);
uint16_t cbf_bytes_available(cbf_t *cb);
cbf_status_t cbf_flush(cbf_t *cb);
cbf_status_t cbf_get(cbf_t *cb, uint8_t *c);
cbf_status_t cbf_put(cbf_t *cb, uint8_t c);
