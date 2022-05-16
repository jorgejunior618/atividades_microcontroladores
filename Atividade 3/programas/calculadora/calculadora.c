#include <p18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define SAIDA		0
#define ENTRADA		1
#define RESULT		20
#define SUBTRAI		21
#define MULTIPLICA	22
#define DIVIDE		23
#define SOMA		24

#define MatCol1 LATDbits.LATD0
#define MatCol2 LATDbits.LATD1
#define MatCol3 LATDbits.LATD2
#define MatCol4 LATDbits.LATD3
#define MatLinA PORTDbits.RD4
#define MatLinB PORTDbits.RD5
#define MatLinC PORTDbits.RD6
#define MatLinD PORTDbits.RD7

#define SAIDA_DISPLAY LATC
#define SAIDA_VARREDURA LATA

int operador1, operador2, operacao, valoresDisplay[6] = {-1,-1,-1,-1,-1,-1};
char mostraResultado = 0, numeros_display[11] = {
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
  0b01111111,
};

void inicializarAmbiente(void) {
	TRISC =	0b10000000;
	TRISA =	0b11000000;

	TRISDbits.RD0 = SAIDA;
	TRISDbits.RD1 = SAIDA;
	TRISDbits.RD2 = SAIDA;
	TRISDbits.RD3 = SAIDA;
	TRISDbits.RD4 = ENTRADA;
	TRISDbits.RD5 = ENTRADA;
	TRISDbits.RD6 = ENTRADA;
	TRISDbits.RD7 = ENTRADA;

	OSCCON =  0b01110111; // Configura a frequencia do oscilador interno para 1MHz
}

void inicializarRelogio(void) {
	operador1	= 0;
	operador2	= 0;
	operacao	= RESULT;

	MatCol1 = 1;
	MatCol2 = 1;
	MatCol3 = 1;

	SAIDA_DISPLAY = numeros_display[11];
	SAIDA_VARREDURA = 0b00111111;
}

void realizaVarredura(void) {
/** Configura o display com o ultimo valor do horario atualizado
 * Realizando a varredura nos Seis displays
 */

	unsigned int i;
	char byteVarredura = 0b00000001;
	char casasNumericas[6] = {
    valoresDisplay[0] == -1 ? 10 : valoresDisplay[0],
    valoresDisplay[1] == -1 ? 10 : valoresDisplay[1],
    valoresDisplay[2] == -1 ? 10 : valoresDisplay[2],
    valoresDisplay[3] == -1 ? 10 : valoresDisplay[3],
    valoresDisplay[4] == -1 ? 10 : valoresDisplay[4],
    valoresDisplay[5] == -1 ? 10 : valoresDisplay[5],
  };
	
	for (i = 0; i < 6; i++) {
		SAIDA_VARREDURA = 0b00000000;
		SAIDA_DISPLAY = numeros_display[casasNumericas[i]];
		SAIDA_VARREDURA = byteVarredura;
		byteVarredura = byteVarredura<<1;
	}
}

char lerTeclado(void) {
	// Varre o teclado numerico e retorna o valor correspondente a tecla clicada

	char valorTeclado = 19, i;
	char matriz_numeros_teclado[4][4] = {
		{7,		8,	9,		DIVIDE},
		{4,		5,	6,		MULTIPLICA},
		{1,		2,	3,		SUBTRAI},
		{-1,	0,	RESULT,	SOMA},
	};
	
	for (i = 0; i < 5; i++) {
		realizaVarredura();

		switch (i) {
			case 0: 
				MatCol1 = 0;
				MatCol2 = 1;
				MatCol3 = 1;
				MatCol4 = 1;
				break;

			case 1:
				MatCol1 = 1;
				MatCol2 = 0;
				MatCol3 = 1;
				MatCol4 = 1;
				break;

			case 2:
				MatCol1 = 1;
				MatCol2 = 1;
				MatCol3 = 0;
				MatCol4 = 1;
				break;

			case 3:
				MatCol1 = 1;
				MatCol2 = 1;
				MatCol3 = 1;
				MatCol4 = 0;
				break;

			default:
				MatCol1 = 1;
				MatCol2 = 1;
				MatCol3 = 1;
				MatCol4 = 1;
				break;
		}
		if (i == 4) {}
		else if(MatLinA == 0) {
			while (MatLinA == 0) {
				realizaVarredura();
			}
			mostraResultado = 0;
			return matriz_numeros_teclado[0][i];
		}
		else if(MatLinB == 0) {
			while (MatLinB == 0) {
				realizaVarredura();
			}
			mostraResultado = 0;
			return matriz_numeros_teclado[1][i];
		}
		else if(MatLinC == 0) {
			while (MatLinC == 0) {
				realizaVarredura();
			}
			mostraResultado = 0;
			return matriz_numeros_teclado[2][i];
		}
		else if(MatLinD == 0) {
			while (MatLinD == 0) {
				realizaVarredura();
			}
			mostraResultado = 0;
			return matriz_numeros_teclado[3][i];
		}
	}
	return valorTeclado;
}

void realizaCalculo(void) {
/** 
 *
 */

	char leitura, ultimaLeitura = 0, i;
	int aux;

	while(1) {
		if (mostraResultado == 0) {
			aux = ultimaLeitura == 0 ? operador1 : operador2;

			for (i = 5; i >= 0; i--) {
				valoresDisplay[i] = aux == 0 ? -1 : aux % 10;
				aux = aux / 10;
			}
		}

		leitura = lerTeclado();
		realizaVarredura();

		if (leitura == 19) {}
		else if (leitura == RESULT) {
			if (ultimaLeitura == 1) {
				break;
			}
		} else if (leitura > RESULT) {
			if (ultimaLeitura == 0) {
				operacao		= leitura;
				ultimaLeitura	= 1;
			}
		} else {
			if (ultimaLeitura == 0 && operador1 < 100000) {
				operador1 = (operador1 * 10) + leitura;
			} else if (ultimaLeitura == 1 && operador2 < 100000) {
				operador2 = (operador2 * 10) + leitura;
			}
		}
	}

	mostraResultado = 1;
	switch (operacao) {
		case SOMA:
			aux = operador1 + operador2;
			break;

		case SUBTRAI:
			aux = operador1;
			aux = aux - operador2;
			break;

		case MULTIPLICA:
			aux = operador1 * operador2;
			break;

		case DIVIDE:
			aux = operador1 / operador2;
			break;

		default:
			break;
	}

	operador1 = 0;
	operador2 = 0;

	for (i = 5; i >= 0; i--) {
		valoresDisplay[i] = aux == 0 ? -1 : aux % 10;
		aux = aux / 10;
	}
	// while (1) {
	// 	realizaVarredura();
	// }
}

void main(void) {
	inicializarAmbiente();
	inicializarRelogio();
	

	while (1) {
		// realizaVarredura();
		realizaCalculo();
	}
}
