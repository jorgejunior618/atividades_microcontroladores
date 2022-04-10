#include <p18f4520.h>
#include "timer.h"

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define MatrizEsq LATA
#define MatrizDir LATD
#define MostradorMatriz LATC

#define SAIDA 0
#define ENTRADA 1

char letras[] = {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,

	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	
	0b00000000,
	0b00000000,
	0b00000010,
	0b00000010,
	0b00100010,
	0b00100010,
	0b01000010,
	0b01000010,
	0b01001110,
	0b01001110,
	0b00110010,
	0b00110010,
	0b00000010,
	0b00000010,
	0b00000000,
	0b00000000,

	0b00000000,
	0b00000000,
	0b00011000,
	0b00011000,
	0b00100100,
	0b00100100,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01000010,
	0b00100100,
	0b00100100,
	0b00011000,
	0b00011000,
	0b00000000,
	0b00000000,

	0b00000000,
	0b00000000,
	0b01111110,
	0b01111110,
	0b00010010,
	0b00010010,
	0b00010010,
	0b00010010,
	0b00110010,
	0b00110010,
	0b01001100,
	0b01001100,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	
	0b00000000,
	0b00000000,
	0b00011000,
	0b00011000,
	0b00100100,
	0b00100100,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01010010,
	0b01010010,
	0b00110100,
	0b00110100,
	0b00000000,
	0b00000000,
	
	0b00000000,
	0b00000000,
	0b01111110,
	0b01111110,
	0b01001010,
	0b01001010,
	0b01001010,
	0b01001010,
	0b01001010,
	0b01001010,
	0b01000010,
	0b01000010,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

char display[] = {
	255 - 128,	255,
	255 - 64,	255,
	255 - 32,	255,
	255 - 16,	255,
	255 - 8,	255,
	255 - 4,	255,
	255 - 2,	255,
	255 - 1,	255,
};
unsigned char index, passo;
unsigned int contadorFrames;

void inicializarPic(void) {
	OSCCON = 0b01000100; // Configura a frequencia do oscilador interno para 1MHz
	TRISA = SAIDA;
	TRISC = SAIDA;
	TRISD = SAIDA;
}

void inicializarLetreiro(void) {
	index = 0;
	passo = 0;
	contadorFrames = 0;
	MostradorMatriz = 0;
}

void atualizarMatrix(void) {
// Realiza a varredura do display, coluna a coluna, mostrando o nome desejado

	index = 0;

	while (index < 16) {
		// Atualiza o valor a ser mostrado no display da esquerda
		if (index + passo >= 112) {
			MatrizEsq = ~letras[(index + passo) - 112];
		} else {
			MatrizEsq = ~letras[index + passo];
		}

		// Atualiza o valor a ser mostrado no display da esquerda
		if (index + passo + 16 >= 112) {
			MatrizDir = ~letras[(index + passo + 16) - 112];
		} else {
			MatrizDir = ~letras[index + passo + 16];
		}

		// Mostra o valor do display atualizado
		MostradorMatriz = ~display[index];

		index++;
		contadorFrames++;
		
		// Mostra o frame repetido 20 vezes até passar para o proximo, para um movimento fluido
		if (contadorFrames == 640) {
			contadorFrames = 0;
			passo += 2;

			// Passa um frame
			if (passo >= 112) {
				passo = 0;
			}
		}
	}
}

void main(void) {
	inicializarPic();
	inicializarLetreiro();
	
	while(1) {
		atualizarMatrix();
	}
}
