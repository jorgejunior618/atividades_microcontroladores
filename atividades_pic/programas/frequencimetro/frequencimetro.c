#include <p18f4520.h>

#pragma config WDT = OFF

#define Display LATC
#define FrequenciaEntrada PORTDbits.RD0

#define CICLOUMSEG 32000
#define SAIDA 0
#define ENTRADA 1

unsigned char contadorOndas;

void inicializarPic(void) {
  OSCCON = 0b01000100;
  TRISDbits.RD0 = ENTRADA;
  TRISC = SAIDA;
}

void inicializarFrequencimetro() {
  Display = 0;
  contadorOndas = 0;
}

void mostraFrequencia(void) {
  if (contadorOndas > 153 || contadorOndas < 10) {
    contadorOndas = 0xee;
  }
  Display = contadorOndas;
  contadorOndas = 0;
}

void contaFrequencia(void) {
  unsigned char estadoFrequenciaEntrada = FrequenciaEntrada, completouOnda = 0;
  unsigned long i;

  for(i = 0; i < 23238; i++) {
    if (contadorOndas > 153) {
      break;
    }
    if (estadoFrequenciaEntrada != FrequenciaEntrada) {
      estadoFrequenciaEntrada = FrequenciaEntrada;
      completouOnda += 1;

      if (completouOnda == 2) {
        if ((contadorOndas + 7) % 16 == 0) {
          contadorOndas += 7;
        } else {
          contadorOndas += 1;
        }
        completouOnda = 0;
      }
    }
  }
}

void main(void) {
  inicializarPic();

  while(1) {
    contaFrequencia();
    mostraFrequencia();
  }
}