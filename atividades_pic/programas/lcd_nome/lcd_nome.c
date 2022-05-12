#include <p18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define DISPLAY LATC
#define RS LATDbits.LATD0
#define ENABLE LATDbits.LATD1

#define BITInstrucao 0
#define BITDado 1

unsigned char nomeJorge[] = {
  0b00000100,
  0b00001010,
  0b00000100,
  0b00001111,
  0b00000101,
  0b00000010,
  0b00000100,
  0b00000111,
  0b00000100,
  0b00000101,

  0b00000000,
  0b00000010,

  0b00000100,
  0b00001010,
  0b00000101,
  0b00000101,
  0b00000100,
  0b00001110,
  0b00000100,
  0b00001001,
  0b00000100,
  0b00001111,
  0b00000101,
  0b00000010,
};

void inicializarAmbiente(void) {
	TRISC =	0b11110000;
	TRISDbits.RD0 =	0;
	TRISDbits.RD1 =	0;
	
	OSCCON =  0b01000100; // Configura a frequencia do oscilador interno para 1MHz
}

void delay3ms(void) {
  int i;
  for (i = 0; i < 40; i++) {}
}

void delayInicialLCD(void) {
  int i;
  for (i = 0; i < 1380; i++) {}
}

void realizaClock(void) {
  ENABLE = 1;
  ENABLE = 0;
}

void limpaDisplay(void) {
  RS = BITInstrucao;

  DISPLAY = 0b00000000;
  realizaClock();
  DISPLAY = 0b00000001;
  realizaClock();
  delay3ms();
}

void inicializarLCD(void) {
	// DISPLAY = 0;
  delayInicialLCD();
  // Define modo 4bits, 1 linha, font 5x8
  RS = BITInstrucao;
  DISPLAY = 0b00000010;
  realizaClock();
  DISPLAY = 0b00000010;
  realizaClock();
  DISPLAY = 0b00001000;
  realizaClock();
  delay3ms();

  // Define o LCD ligado e cursor
  DISPLAY = 0b00000000;
  realizaClock();
  DISPLAY = 0b00001110;
  realizaClock();
  delay3ms();

  limpaDisplay();

  // Modo de Entrada
  DISPLAY = 0b00000000;
  realizaClock();
  DISPLAY = 0b00000110;
  realizaClock();
  delay3ms();
}

void exibirNome(void) {
  
}

void main(void) {
  int i = 0;
  inicializarAmbiente();
  inicializarLCD();

  while(1) {
    RS = BITDado;
    DISPLAY = nomeJorge[i];
    realizaClock();
    delayInicialLCD();
    i++;

    if (i > 23) {
      delayInicialLCD();
      limpaDisplay();
      delayInicialLCD();
      i = 0;
    }
  }
}