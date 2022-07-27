/*
 * hw.c
 * Abstrai as funções do HAL e CMSIS
 *
 *  Created on: July 10, 2022
 *      Author: lesly
 */

#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "app.h"
#include "cbf.h"
#include "hw.h"

extern UART_HandleTypeDef huart2; //extern: linker sabe que essa var já existe, e usa essa msm variável
static cbf_t *hw_uart_cbf = 0; // buffer circular

void hw_uart_disable_interrupts(void){
	HAL_NVIC_DisableIRQ(USART2_IRQn);
}

void hw_uart_enable_interrupts(void){
	HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
	HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void hw_uart_init(cbf_t *cbf){
	hw_uart_disable_interrupts();

	huart2.Instance->CR3 |= USART_CR3_EIE;
	huart2.Instance->CR1 |= USART_CR1_PEIE | USART_CR1_RXNEIE;

	hw_uart_cbf = cbf;
	hw_uart_enable_interrupts();
}

// INTERRUPÇÃO - recepção - importante!
void hw_uart2_interrupt(void){
	uint8_t c;
	uint32_t sr;
	USART_TypeDef *h = huart2.Instance;

	sr = h->SR;
	while(sr & (UART_FLAG_ORE | UART_FLAG_PE | UART_FLAG_FE | UART_FLAG_NE)){
		sr = h->SR;
		c = h->DR;
	}

	// se sem erros e com dado recebido ... pegar o dado e colocar no buffer
	if(sr & UART_FLAG_RXNE){
		c = h->DR;

		if(hw_uart_cbf)
			cbf_put(hw_uart_cbf,c);
	}

	// led 3 que liga e desliga a cada caractere
//	if(hw_led_n_state_get(3))
//		hw_led_n_state_set(3,false);
//	else
//		hw_led_n_state_set(3,true);
}

// POOLING - transmissão
static void hw_uart_tx_byte(uint8_t c){
	USART_TypeDef *h = huart2.Instance; // USART_PORT

	// garante que o shift register esteja vazio
	while(!(h->SR & UART_FLAG_TXE)){}
	h->DR = c;
}

void hw_uart_tx(uint8_t *buffer, uint32_t size){
	for(size_t pos = 0; pos < size; pos++)
		hw_uart_tx_byte(buffer[pos]);
}

// ---------------------------------------------------- //

bool hw_button_state_get(void){
	GPIO_PinState button_state = HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);

	if(button_state == GPIO_PIN_RESET)
		return true;
	else
		return false;
}

bool hw_button_n_state_get(uint8_t n){
	GPIO_PinState button_state;
	switch(n){
		case 1:
			button_state = HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);
			break;
		case 2:
			button_state = HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin);
			break;
		default:
			button_state = HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);
			break;
	}
	if(button_state == GPIO_PIN_RESET)
		return true;
	else
		return false;
}

void hw_led_toggle(void){
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void hw_led_state_set(bool state){
	GPIO_PinState led_state = state ? GPIO_PIN_RESET : GPIO_PIN_SET;
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, led_state);
}

void hw_led_n_state_set(uint8_t n, bool state){
	GPIO_PinState led_state = state ? GPIO_PIN_RESET : GPIO_PIN_SET;
	switch(n){
		case 1:
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, led_state);
			break;
		case 2:
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, led_state);
			break;
		case 3:
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, led_state);
			break;
		default:
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, led_state);
			break;
	}
}

bool hw_led_n_state_get(uint8_t n){
	GPIO_PinState led_state;
	switch(n){
		case 1:
			led_state =  HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin);
			break;
		case 2:
			led_state =  HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin);
			break;
		case 3:
			led_state =  HAL_GPIO_ReadPin(LED3_GPIO_Port, LED3_Pin);
			break;
		default:
			led_state =  HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin);
			break;
	}
	if(led_state == GPIO_PIN_RESET)
		return true;
	else
		return false;
}

void hw_delay_ms(uint32_t time_ms){
	HAL_Delay(time_ms);
}

void hw_cpu_sleep(){
	__WFI();
}

uint32_t hw_tick_ms_get(void){
	return HAL_GetTick();
}
