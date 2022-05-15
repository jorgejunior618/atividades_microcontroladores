#ifndef TIMER_H_
#define TIMER_H_

#define CICLOUMSEG 13875
// Quantidade de iteracoes, num loop for para gerar um segundo
// com o Clock de 1MHz

void delay_secs(unsigned int secs) {
	unsigned int i;
	unsigned int j;

	for(i = 0; i < secs; i++) {
		for(j = 0; j < CICLOUMSEG; j++) {};
	};
}

void delay_millis(unsigned int millis) {
	unsigned int i;
	unsigned int j;

	for(i = 0; i < millis; i++) {
		for (j = 0; j < 12; j++){}
	}
}

#endif
