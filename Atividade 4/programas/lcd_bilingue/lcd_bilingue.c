#include <p18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define DISPLAY LATC
#define RS LATDbits.LATD0
#define ENABLEPORTUGUES LATDbits.LATD1
#define ENABLEINGLES LATDbits.LATD2

#define BITInstrucao 0
#define BITDado 1

unsigned char tamanhoPortugues = 9, tamanhoIngles = 7;
unsigned char nomePortugues[] = {
  'F','A','C','U','L','D','A','D','E',
};
unsigned char nomeIngles[] = {
  'C','O','L','L','E','G','E',
};

void inicializarAmbiente(void) {
	TRISC =	0b11110000;
	TRISDbits.RD0 =	0;
	TRISDbits.RD1 =	0;
	TRISDbits.RD2 =	0;
	
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

void clockPortugues(void) {
  // Realiza um pulso no Enable do LCD Portugues para receber a informação no barramento de dados
  ENABLEPORTUGUES = 1;
  ENABLEPORTUGUES = 0;
}

void clockIngles(void) {
  // Realiza um pulso no Enable do LCD Ingles para receber a informação no barramento de dados
  ENABLEINGLES = 1;
  ENABLEINGLES = 0;
}

void enviarComandoLCD(unsigned char comando, unsigned char portugues, unsigned char ingles) {
  // Envia o comando informado como instrução para o LCD
  // Os parametros [portugues] e [ingles] sao utilizados para identificas o(s) LCD(s) a receber o comando

  RS = BITInstrucao;
  DISPLAY = comando;
  
  if (portugues != 0) {
    clockPortugues();
  }
  if (ingles != 0) {
    clockIngles();
  }
  delayMs(1);
}

void enviarDadoLCD(unsigned char dado, unsigned char portugues, unsigned char ingles) {
  // Envia o comando informado como dado para o LCD
  // Os parametros [portugues] e [ingles] sao utilizados para identificas o(s) LCD(s) a receber o dado

  RS = BITDado;
  DISPLAY = dado;
  
  if (portugues != 0) {
    clockPortugues();
  }
  if (ingles != 0) {
    clockIngles();
  }
  delayMs(1);
}

void limpaDisplay(void) {
  // Limpa todos os caracteres que estejam nos displays
  enviarComandoLCD(0b00000000, 1, 1);
  enviarComandoLCD(0b00000001, 1, 1);
}

void inicializarLCD(void) {
  // Define modo 4bits, 1 linha, font 5x8
  enviarComandoLCD(0b00000010, 1, 1);
  enviarComandoLCD(0b00000010, 1, 1);
  enviarComandoLCD(0b00001000, 1, 1);
  delayMs(2);

  // Define o LCD ligado e cursor
  enviarComandoLCD(0b00000000, 1, 1);
  enviarComandoLCD(0b00001110, 1, 1);
  delayMs(2);

  limpaDisplay();
  delayMs(2);

  // Modo de Entrada
  enviarComandoLCD(0b00000000, 1, 1);
  enviarComandoLCD(0b00000110, 1, 1);
  delayMs(2);
}


void exibirPalavras(void) {
  // Itera sobre os arrays de caracteres para mostra los nos displays
  unsigned char i = 0;

  while(i < tamanhoPortugues || i < tamanhoIngles) {
    if (i < tamanhoPortugues){
      enviarDadoLCD(nomePortugues[i]>>4, 1, 0);
      enviarDadoLCD(nomePortugues[i], 1, 0);
    }
    if (i < tamanhoIngles){
      enviarDadoLCD(nomeIngles[i]>>4, 0, 1);
      enviarDadoLCD(nomeIngles[i], 0, 1);
    }
    delayMs(75);
    i += 1;
  }
}

void limparPalavras(void) {
  delayMs(600);
  limpaDisplay();
  delayMs(600);
}

void main(void) {
  inicializarAmbiente();
  inicializarLCD();

  while(1) {
    exibirPalavras();

    limparPalavras();
  }
}