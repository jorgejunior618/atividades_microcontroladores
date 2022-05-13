#include <p18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define DISPLAY LATC
#define RS LATDbits.LATD0
#define ENABLEPORTUGUES LATDbits.LATD1
#define ENABLEINGLES LATDbits.LATD2

#define BITInstrucao 0
#define BITDado 1

unsigned char alfabeto[] = {
  0b00000100,
  0b00000001, // A
  0b00000100,
  0b00000010, // B
  0b00000100,
  0b00000011, // C
  0b00000100,
  0b00000100, // D
  0b00000100,
  0b00000101, // E
  0b00000100,
  0b00000110, // F
  0b00000100,
  0b00000111, // G
  0b00000100,
  0b00001000, // H
  0b00000100,
  0b00001001, // I
  0b00000100,
  0b00001010, // J
  0b00000100,
  0b00001011, // K
  0b00000100,
  0b00001100, // L
  0b00000100,
  0b00001101, // M
  0b00000100,
  0b00001110, // N
  0b00000100,
  0b00001111, // O
  0b00000101,
  0b00000001, // P
  0b00000101,
  0b00000010, // Q
  0b00000101,
  0b00000011, // R
  0b00000101,
  0b00000100, // S
  0b00000101,
  0b00000101, // T
  0b00000101,
  0b00000110, // U
  0b00000101,
  0b00000111, // V
  0b00000101,
  0b00001001, // W
  0b00000101,
  0b00001010, // X
  0b00000101,
  0b00001011, // Y
  0b00000101,
  0b00001100, // Z
};

unsigned char tamanhoPortugues = 18, tamanhoIngles = 14;
unsigned char nomePortugues[18];
unsigned char nomeIngles[14];

void inicializarAmbiente(void) {
	TRISC =	0b11110000;
	TRISDbits.RD0 =	0;
	TRISDbits.RD1 =	0;
	TRISDbits.RD2 =	0;
	
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

void clockPortugues(void) {
  ENABLEPORTUGUES = 1;
  ENABLEPORTUGUES = 0;
}

void clockIngles(void) {
  ENABLEINGLES = 1;
  ENABLEINGLES = 0;
}

void limpaDisplay(void) {
  RS = BITInstrucao;

  DISPLAY = 0b00000000;
  clockPortugues();
  clockIngles();
  DISPLAY = 0b00000001;
  clockPortugues();
  clockIngles();
}

void inicializarLCD(void) {
  nomePortugues[0] = alfabeto[10];
  nomePortugues[1] = alfabeto[11];
  nomePortugues[2] = alfabeto[0];
  nomePortugues[3] = alfabeto[1];
  nomePortugues[4] = alfabeto[4];
  nomePortugues[5] = alfabeto[5];
  nomePortugues[6] = alfabeto[38];
  nomePortugues[7] = alfabeto[39];
  nomePortugues[8] = alfabeto[22];
  nomePortugues[9] = alfabeto[23];
  nomePortugues[10] = alfabeto[6];
  nomePortugues[11] = alfabeto[7];
  nomePortugues[12] = alfabeto[0];
  nomePortugues[13] = alfabeto[1];
  nomePortugues[14] = alfabeto[6];
  nomePortugues[15] = alfabeto[7];
  nomePortugues[16] = alfabeto[8];
  nomePortugues[17] = alfabeto[9];
  
  nomeIngles[0] = alfabeto[4];
  nomeIngles[1] = alfabeto[5];
  nomeIngles[2] = alfabeto[28];
  nomeIngles[3] = alfabeto[29];
  nomeIngles[4] = alfabeto[22];
  nomeIngles[5] = alfabeto[23];
  nomeIngles[6] = alfabeto[22];
  nomeIngles[7] = alfabeto[23];
  nomeIngles[8] = alfabeto[8];
  nomeIngles[9] = alfabeto[9];
  nomeIngles[10] = alfabeto[12];
  nomeIngles[11] = alfabeto[13];
  nomeIngles[12] = alfabeto[8];
  nomeIngles[13] = alfabeto[9];
	// DISPLAY = 0;
  delayInicialLCD();
  // Define modo 4bits, 1 linha, font 5x8
  RS = BITInstrucao;
  DISPLAY = 0b00000010;
  clockPortugues();
  clockIngles();
  DISPLAY = 0b00000010;
  clockPortugues();
  clockIngles();
  DISPLAY = 0b00001000;
  clockPortugues();
  clockIngles();
  delay3ms();

  // Define o LCD ligado e cursor
  DISPLAY = 0b00000000;
  clockPortugues();
  clockIngles();
  DISPLAY = 0b00001110;
  clockPortugues();
  clockIngles();
  delay3ms();

  limpaDisplay();
  delay3ms();

  // Modo de Entrada
  DISPLAY = 0b00000000;
  clockPortugues();
  clockIngles();
  DISPLAY = 0b00000110;
  clockPortugues();
  clockIngles();
  delay3ms();
}

void delayExibicao(void) {
  int i;
  for (i = 0; i < 6000; i++) {}
}

void exibirPalavras(void) {
  unsigned char i = 0;
  RS = BITDado;

  while(i < tamanhoPortugues || i < tamanhoIngles) {
    if (i < tamanhoPortugues){
      DISPLAY = nomePortugues[i];
      clockPortugues();
    }
    if (i < tamanhoIngles){
      DISPLAY = nomeIngles[i];
      clockIngles();
    }
    i += 1;
  }
}

void limparPalavras(void) {
  delayExibicao();
  limpaDisplay();
  delayExibicao();
}

void main(void) {
  inicializarAmbiente();
  inicializarLCD();

  while(1) {
    exibirPalavras();

    limparPalavras();
  }
}