#include <p18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define DISPLAY LATC
#define RS LATDbits.LATD0
#define ENABLE LATDbits.LATD1

#define BITInstrucao 0
#define BITDado 1

unsigned char nomeJorge[] = {
  'J','O','R','G','E',' ','J','U','N','I','O','R'
};

void inicializarAmbiente(void) {
	TRISC =	0b11110000;
	TRISDbits.RD0 =	0;
	TRISDbits.RD1 =	0;
	
	OSCCON =  0b01000100; // Configura a frequencia do oscilador interno para 1MHz
}

void delayMs(int delay) {
  int i, j;
  for (i = 0; i < delay; i++){
    for (j = 0; j < 10; j++) {}
  }
}

void delayInicialLCD(void) {
  int i;
  for (i = 0; i < 1380; i++) {}
}

void pulsoEnable(void) {
  ENABLE = 1;
  ENABLE = 0;
  delayMs(1);
}

void enviarComandoLCD(unsigned char comando) {
  RS = BITInstrucao;
  DISPLAY = comando;
  
  pulsoEnable();
}

void enviarDadoLCD(unsigned char dado) {
  RS = BITDado;
  DISPLAY = dado;
  
  pulsoEnable();
}

void limpaDisplay(void) {
  RS = BITInstrucao;

  DISPLAY = 0b00000000;
  pulsoEnable();
  DISPLAY = 0b00000001;
  pulsoEnable();
  delayMs(2);
}

void inicializarLCD(void) {
	// DISPLAY = 0;
  delayInicialLCD();
  // Define modo 4bits, 1 linha, font 5x8
  // Define modo 4bits, 1 linha, font 5x8
  enviarComandoLCD(0b00000010);
  enviarComandoLCD(0b00000010);
  enviarComandoLCD(0b00001000);
  delayMs(2);

  // Define o LCD ligado e cursor
  enviarComandoLCD(0b00000000);
  enviarComandoLCD(0b00001110);
  delayMs(2);

  limpaDisplay();
  delayMs(2);

  // Modo de Entrada
  enviarComandoLCD(0b00000000);
  enviarComandoLCD(0b00000110);
  delayMs(2);
}

void exibirNome(void) {
  unsigned char i;

  for (i = 0; i < 12; i++) {
    enviarDadoLCD(nomeJorge[i]>>4);
    enviarDadoLCD(nomeJorge[i]);
    delayMs(70);
  }
}

void main(void) {
  inicializarAmbiente();
  inicializarLCD();

  while(1) {
    exibirNome();
    delayMs(1000);
    limpaDisplay();
  }
}