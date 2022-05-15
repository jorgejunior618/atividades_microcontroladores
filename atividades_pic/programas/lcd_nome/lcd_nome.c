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
  // Realiza a espera do valor informado no parametro [delay] em milisegundos
  int i, j;
  for (i = 0; i < delay; i++){
    for (j = 0; j < 10; j++) {}
  }
}

void delayInicialLCD(void) {
  // Delay para garantir a inicialização correta do LCD (aprox. 100ms)
  int i;
  for (i = 0; i < 1380; i++) {}
}

void pulsoEnable(void) {
  // Realiza um pulso no Enable do LCD para receber a informação no barramento de dados
  ENABLE = 1;
  ENABLE = 0;
  delayMs(1);
}

void enviarComandoLCD(unsigned char comando) {
  // Envia o comando informado como instrução para o LCD
  RS = BITInstrucao;
  DISPLAY = comando;
  
  pulsoEnable();
}

void enviarDadoLCD(unsigned char dado) {
  // Envia o comando informada como dado para o LCD
  RS = BITDado;
  DISPLAY = dado;
  
  pulsoEnable();
}

void limpaDisplay(void) {
  // Limpa todos os caracteres que estejam no display
  enviarComandoLCD(0b00000000);
  enviarComandoLCD(0b00000001);
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
  // Itera sobre o array de Caracteres definido para o nome a ser mostrado
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