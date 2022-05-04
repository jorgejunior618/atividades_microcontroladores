#include <p18f4520.h>
#include "timer.h"

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define SAIDA 0
#define ENTRADA 1
#define BotaoModo PORTDbits.RD0
#define MatCol1 LATDbits.LATD1
#define MatCol2 LATDbits.LATD2
#define MatCol3 LATDbits.LATD3
#define MatLinA PORTDbits.RD4
#define MatLinB PORTDbits.RD5
#define MatLinC PORTDbits.RD6
#define MatLinD PORTDbits.RD7

#define BuzzerAlarme LATBbits.LATB0

#define SAIDA_DISPLAY LATC
#define SAIDA_VARREDURA LATA

unsigned char hora, minuto, segundo, horaAlarme, minutoAlarme, alarmando;
unsigned char numeros_display[10] = {
  0b00000001,
  0b01001111,
  0b00010010,
  0b00000110,
  0b01001100,
  0b00100100,
  0b00100000,
  0b00001111,
  0b00000000,
  0b00000100,
};
unsigned char matriz_numeros_teclado[4][3] = {
	{1,		2,	3	},
	{4,		5,	6	},
	{7,		8,	9	},
	{200,	0,	200	},
};

void inicializarAmbiente(void) {
	TRISC =	0b10000000;
	TRISA =	0b11000000;
	TRISBbits.RB0 = SAIDA;

	TRISDbits.RD0 = ENTRADA;
	TRISDbits.RD1 = SAIDA;
	TRISDbits.RD2 = SAIDA;
	TRISDbits.RD3 = SAIDA;
	TRISDbits.RD4 = ENTRADA;
	TRISDbits.RD5 = ENTRADA;
	TRISDbits.RD6 = ENTRADA;
	TRISDbits.RD7 = ENTRADA;

	OSCCON =  0b01000100; // Configura a frequencia do oscilador interno para 1MHz
}

void inicializarRelogio(void) {
	hora			= 0;
	minuto			= 0;
	segundo			= 0;
	horaAlarme		= 6;
	minutoAlarme	= 0;
	alarmando		= 0;

	MatCol1 = 1;
	MatCol2 = 1;
	MatCol3 = 1;
	BuzzerAlarme = 1;

	SAIDA_DISPLAY = numeros_display[0];
	SAIDA_VARREDURA = 0b00111111;
}

void realizaUmaVarredura(unsigned char mostraAlarme) {
/** Configura o display com o ultimo valor do horario atualizado
	* Realizando a varredura nos Seis displays
	*/

  unsigned int i;
	unsigned char byteVarredura = 0b00100000;
	unsigned char unidadesTempo[6] = {
    mostraAlarme ==  1 ? 0 : segundo - ((segundo / 10) * 10),
    mostraAlarme ==  1 ? 0 : segundo / 10,
    mostraAlarme ==  1 ? minutoAlarme - ((minutoAlarme / 10) * 10) : minuto - ((minuto / 10) * 10),
    mostraAlarme ==  1 ? minutoAlarme / 10 : minuto / 10,
    mostraAlarme ==  1 ? horaAlarme - ((horaAlarme / 10) * 10) : hora - ((hora / 10) * 10),
    mostraAlarme ==  1 ? horaAlarme / 10 : hora / 10,
  };
	
	for (i = 0; i < 6; i++) {
		SAIDA_VARREDURA = 0b00000000;
		SAIDA_DISPLAY = numeros_display[unidadesTempo[i]];
		SAIDA_VARREDURA = byteVarredura;
		byteVarredura = byteVarredura>>1;
	}
}

void mostrarRelogioVarredura(void) {
	// Configura o display com o ultimo valor do horario atualizado
	// Realizando a varredura nos Seis displays

  unsigned int i;
  for (i = 0; i < 286; i++) { // Numero de repeticoes para completar Um segundo
    realizaUmaVarredura(0);
  }
}

void atualizaRelogio(void) {
/** Incrementa as variaveis do relogio em 1 (Um)
	* seguindo as regras do formato hh:mm:ss
	*/

  segundo += 1;

	if (segundo == 60) {
		segundo = 0;
    minuto += 1;
		
    if (minuto == 60) {
			minuto = 0;
      hora += 1;

			if (hora == 24) {
				hora = 0;
			}
		}
	}
}

unsigned char lerTeclado(void) {
	// Varre o teclado numerico e retorna o valor correspondente a tecla clicada

	unsigned char valorTeclado = 200, i;
	for (i = 0; i < 4; i++) {
		switch (i) {
			case 0:
				MatCol2 = 1;
				MatCol3 = 1;
				MatCol1 = 0;
				break;

			case 1:
				MatCol1 = 1;
				MatCol3 = 1;
				MatCol2 = 0;
				break;

			case 2:
				MatCol1 = 1;
				MatCol2 = 1;
				MatCol3 = 0;
				break;

			default:
				MatCol1 = 1;
				MatCol2 = 1;
				MatCol3 = 1;
				break;
		}
		if (i > 4) {}
		else if(MatLinA == 0) {
			while (MatLinA == 0) {}
			valorTeclado = matriz_numeros_teclado[0][i];
			break;
		}
		else if(MatLinB == 0) {
			while (MatLinB == 0) {}
			valorTeclado = matriz_numeros_teclado[1][i];
			break;
		}
		else if(MatLinC == 0) {
			while (MatLinC == 0) {}
			valorTeclado = matriz_numeros_teclado[2][i];
			break;
		}
		else if(MatLinD == 0) {
			while (MatLinD == 0) {}
			valorTeclado = matriz_numeros_teclado[3][i];
			break;
		}
	}
	if (valorTeclado != 200) {
		return valorTeclado;
	}
	return 200;
}

void editaRelogio(void) {
/** Entra no modo de edicao do Relogio, iniciando pela edicao dos minutos.
	*	Inicia a leitura da dezena do minuto, logo apos a unidade, e passa para as horas
	*	Edicao das horas idem
	*	Ao Botao de modo ser apertado:
	*		Se estiver editando os minutos, a edicao passa para as horas
	*		Se estiver editando as horas, finaliza o modo de edicao
	*	Ao finalizar a leitura da dezena e unidade das horas, finaliza a edicao
	*/

	unsigned char aux, ultimaLeitura = 0;

// Previne o fim da funcao ao entrar no modo de edicao com o Botao de modo pressionado
	while(BotaoModo == 1) {}

	while(1) {
		if (ultimaLeitura >= 4) {
			realizaUmaVarredura(1);
		} else {
			realizaUmaVarredura(0);
		}

		if (ultimaLeitura > 7) {break;}

		if (BotaoModo == 1) {
			while (BotaoModo == 1) {}
			if (ultimaLeitura < 2) {
				ultimaLeitura = 2;
			} else if (ultimaLeitura < 4) {
				ultimaLeitura = 4;
			} else if (ultimaLeitura < 6) {
				ultimaLeitura = 6;
			} else if (ultimaLeitura > 5) {
				break;
			}
		}
		
		aux = lerTeclado();
		if (aux == 200) {}

		else if (ultimaLeitura == 0) {
			minuto = aux * 10;
			ultimaLeitura += 1;
		}
		else if (ultimaLeitura == 1) {
			minuto += aux;
			ultimaLeitura += 1;
		}
		else if (ultimaLeitura == 2) {
			hora = aux * 10;
			ultimaLeitura += 1;
		}
		else if (ultimaLeitura == 3) {
			hora += aux;
			ultimaLeitura += 1;
		}
		else if (ultimaLeitura == 4) {
			minutoAlarme = aux * 10;
			ultimaLeitura += 1;
		}
		else if (ultimaLeitura == 5) {
			minutoAlarme += aux;
			ultimaLeitura += 1;
		}
		else if (ultimaLeitura == 6) {
			horaAlarme = aux * 10;
			ultimaLeitura += 1;
		}
		else if (ultimaLeitura == 7) {
			horaAlarme += aux;
			ultimaLeitura += 1;
		}
	}
}

unsigned char verificaModoEdicao(void) {
/** Verifica se o botao de modo de edicao foi pressionado
	* E inicia a contagem ate se passarem 2 segundos, ou
	* ate o botao deixar de ser pressionado
	*/

	unsigned int i;
	unsigned int cont = 0;

	if (BotaoModo == 1) {
		while (cont < 2) {
			for(i = 0; i < CICLOUMSEG; i++) {
				if(BotaoModo == 0) {
					cont = 10;
					break;
				}
			};
			cont++;
		}
	}
	if (cont == 2) {
		return 1;
	}
	return 0;
}

void verificarAlarme(void) {
// Compara se a hora do alarme programada esta igual a do relogio e ativa o buzzer em caso positivo

	if (alarmando == 1) {
		BuzzerAlarme = 0;
	} else {
		BuzzerAlarme = 1;
	}

	if (horaAlarme == hora && minutoAlarme == minuto && segundo < 10) {
		if (alarmando != 3) {
			alarmando = 1;
		}
	} else {
		alarmando = 0;
	}
}

void main(void) {
	inicializarAmbiente();
	inicializarRelogio();

	while (1) {
		mostrarRelogioVarredura();
		atualizaRelogio();

		if (BotaoModo == 1 && alarmando == 1) {
			alarmando = 3;
		}
		verificarAlarme();


		if (verificaModoEdicao() == 1) {
			editaRelogio();
		}

	}
}
