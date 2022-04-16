#include <p18f4520.h>
#include "timer.h"

#pragma config WDT = OFF

#define SAIDA 0
#define ENTRADA 1

#define Transmissor PORTCbits.RC0
#define Receptor PORTCbits.RC1

void inicializarPic(void) {
  OSCCON = 0b01110111;
  TRISCbits.RC0 = SAIDA;
  TRISCbits.RC1 = ENTRADA;
  Transmissor = 1;
}

void delay1200bps(void) {
  // Funcao de delay para a recepcao dos dados de entrada bit a bit
  // Aguarda Aprox. 833us (1200Hz)
  unsigned int i;

  for (i=0; i< 108; i++) {}
}

void delayDefasagem(void) {
  // Defasagem de proteção de meio tempo de um recebimento de um bit.
  // Utilizar apos receber o bit de start do Serial
  unsigned int i;

  for (i=0; i< 53; i++) {}
}

char recebeByte(void) {
  // Processa o recebimento de um byte de dado (1 caracter) e o retorna.
  // Retorna: Um char correspondente uma tecla pressionada
  unsigned int i = 0;
  char dado = 0b00000000;
  int valor = 0;

  while(Receptor) {} // Aguarda o bit de start (Receptor == 0) vindo da entrada
  delayDefasagem();
  if(Receptor) {
    // Testa se durante o recebimento do bit de start houve mudança (Interferencia)
    // Caso positivo retorna um caracter representando a Interferencia
    return 0xff;
  }

  delay1200bps();
  for(i = 0; i < 8; i++) {
    valor = (int) Receptor;
    dado = dado | valor<<i;
    delay1200bps();
  }
  return dado;
}

void transmiteByte(char byte) {
  // Transmite o caracter correspondente ao armazenado para o terminal
  // Recebe: byte-> char : correspondente ao que foi recebido
  unsigned int i = 0;
  int mascara = 0b00000001;
  int valor = 0;

  Transmissor = valor;
  delay1200bps();
  for(i = 0; i < 8; i++) {
    if (byte & mascara<<i) {
      valor = 1;
    }
    Transmissor = valor;
    delay1200bps();
    valor = 0;
  }
  Transmissor = 1;
  delay1200bps();
}

void main(void) {
  inicializarPic();
  while(1) {
    transmiteByte(recebeByte());
  }
}
