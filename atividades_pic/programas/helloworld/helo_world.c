#include <p18f4520.h>

#define SAIDA 0
#define ENTRADA 1
#define LED PORTCbits.RC0
#define CHAVE PORTCbits.RC1

void delay_secs(unsigned int secs) {
	unsigned int i;
	unsigned int j;
	
	for(i = 0; i < secs; i++) {
		for(j = 0; j < 40000; j++) {};
	};
}

void delay_millis(unsigned int millis) {
	unsigned int i;
	unsigned int j;
	
	for(i = 0; i < millis; i++) {
		for (j = 0; j < 54; j++){}
	}
}

main() {
	TRISCbits.RC0 = SAIDA;
	TRISCbits.RC1 = ENTRADA;

	// TRISC = 0b01111111;	-> 0b - 		Define entrada em binário (pino a pino)
	// 						-> 01111111 -	Define a função de cada Pino (0 - Saída, 1 - entrada)
	// 						   01234567
	// 										Definir o pino como 1 é mais economico

	while (1) {
		if (CHAVE == 0) {
			LED = 1;
		}
		LED = ~LED;
		delay_secs(1);
	}
}
