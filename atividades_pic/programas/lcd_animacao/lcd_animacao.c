#include <p18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define DISPLAY LATC
#define RS LATDbits.LATD0
#define ENABLE LATDbits.LATD1

#define BITInstrucao 0
#define BITDado 1

unsigned char caracteresEspeciais[] = {
  0b00001100,
  0b00000100,
  0b00001110,
  0b00010101,
  0b00000100,
  0b00001010,
  0b00001010,
  0b00011011,

  0b00001100,
  0b00000100,
  0b00011111,
  0b00000100,
  0b00000100,
  0b00001010,
  0b00010001,
  0b00010001,

  0b00001100,
  0b00010101,
  0b00001110,
  0b00000100,
  0b00000100,
  0b00001010,
  0b00010001,
  0b00010001,

  0b00001100,
  0b00000100,
  0b00011111,
  0b00000100,
  0b00000100,
  0b00001010,
  0b00010001,
  0b00010001,

  0b00001100,
  0b00000100,
  0b00001110,
  0b00010101,
  0b00000100,
  0b00001010,
  0b00001010,
  0b00011011,
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

void definirCaracteresEspeciais(void) {
  char i;
  enviarComandoLCD(0b00000100);
  enviarComandoLCD(0b00000000);

  for (i= 0; i < 40; i++) {
    enviarDadoLCD(caracteresEspeciais[i]>>4);
    enviarDadoLCD(caracteresEspeciais[i]);
  }
}

void cursorPosicaoInicial(void) {
  enviarComandoLCD(0b00001000);
  enviarComandoLCD(0b00000000);
}

void inicializarLCD(void) {
	// DISPLAY = 0;
  delayInicialLCD();
  // Define modo 4bits, 1 linha, font 5x8
  enviarComandoLCD(0b00000010);
  enviarComandoLCD(0b00000010);
  enviarComandoLCD(0b00001000);
  delayMs(2);

  // Define o LCD ligado e cursor
  enviarComandoLCD(0b00000000);
  enviarComandoLCD(0b00001100);
  delayMs(2);

  limpaDisplay();
  delayMs(2);

  // Modo de Entrada
  enviarComandoLCD(0b00000000);
  enviarComandoLCD(0b00000110);
  delayMs(2);

  definirCaracteresEspeciais();
  cursorPosicaoInicial();
}

void exibirAnimacao(void) {
  unsigned char i;

  char caracteres[] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000001,
    0b00000000,
    0b00000010,
    0b00000000,
    0b00000011,
    0b00000000,
    0b00000100,
  };

  for (i = 0; i < 10; i++) {
    enviarDadoLCD(caracteres[i]);
    
    if (i % 2 == 1) {
      delayMs(150);
      cursorPosicaoInicial();
    }
  }
}

void main(void) {
  inicializarAmbiente();
  inicializarLCD();

  while(1) {
    exibirAnimacao();
  }
}