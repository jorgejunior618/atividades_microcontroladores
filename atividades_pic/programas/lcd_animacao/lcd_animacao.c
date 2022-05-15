#include <p18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define DISPLAY LATC
#define RS LATDbits.LATD0
#define ENABLE LATDbits.LATD1

#define BITInstrucao 0
#define BITDado 1

unsigned char caracteresEspeciais2[] = { // Desenho da animação
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

unsigned char caracteresEspeciais[] = { // Desenho da animação
  0b00001100,
  0b00000101,
  0b00011110,
  0b00000100,
  0b00001000,
  0b00010100,
  0b00001010,
  0b00011011,
  
  0b00001100,
  0b00000100,
  0b00011111,
  0b00000100,
  0b00000100,
  0b00001010,
  0b00001010,
  0b00011011,

  0b00001100,
  0b00010100,
  0b00001111,
  0b00000100,
  0b00000010,
  0b00000101,
  0b00001010,
  0b00011011,
  
  0b00001100,
  0b00000100,
  0b00011111,
  0b00000100,
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
  // Envia o comando informada como instrução para o LCD
  RS = BITInstrucao;
  DISPLAY = comando;
  
  pulsoEnable();
}

void enviarDadoLCD(unsigned char dado) {
  // Envia o comando informado como dado para o LCD
  RS = BITDado;
  DISPLAY = dado;
  
  pulsoEnable();
}

void limpaDisplay(void) {
  // Limpa todos os caracteres que estejam no display
  enviarComandoLCD(0b00000000);
  enviarComandoLCD(0b00000001);
}

void definirCaracteresEspeciais(void) {
  // Atribui a personalização para os caracteres especiais da Tabela ASCII
  // Utilizados para realizar a animacao
  char i;
  enviarComandoLCD(0b00000100);
  enviarComandoLCD(0b00000000);

  for (i= 0; i < 56; i++) {
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
  // Realiza a iteracao com os caracteres especiais para mostrar a animação no LCD
  unsigned char i;

  char caracteres[] = {
    0b00000000,
    0b00000001,
    0b00000010,
    0b00000011,
    0b00000100,
    0b00000101,
    0b00000110,
    0b00000111,
  };

  for (i = 0; i < 4; i++) {
    enviarDadoLCD(caracteres[0]);
    enviarDadoLCD(caracteres[i]);
    delayMs(100);
    cursorPosicaoInicial();
  }
}

void main(void) {
  inicializarAmbiente();
  inicializarLCD();

  while(1) {
    exibirAnimacao();
  }
}