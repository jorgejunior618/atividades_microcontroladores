#include <p18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config PBADEN = OFF

#define DISPLAY LATC
#define RS LATDbits.LATD0
#define ENABLE LATDbits.LATD1
#define MatCol1 LATBbits.LATB1
#define MatCol2 LATBbits.LATB2
#define MatCol3 LATBbits.LATB3
#define MatLinA PORTBbits.RB4
#define MatLinB PORTBbits.RB5
#define MatLinC PORTBbits.RB6
#define MatLinD PORTBbits.RB7

#define BITInstrucao 0
#define BITDado 1
#define SAIDA 0
#define ENTRADA 1

unsigned char logado = 0, senhaSalva[4] = {
  '1','2','3','4',
},senhaDigitada[4];

void inicializarAmbiente(void) {
	TRISC =	0b11110000;
	TRISDbits.RD0 =	0;
	TRISDbits.RD1 =	0;

  TRISBbits.RB1 = SAIDA;
	TRISBbits.RB2 = SAIDA;
	TRISBbits.RB3 = SAIDA;
	TRISBbits.RB4 = ENTRADA;
	TRISBbits.RB5 = ENTRADA;
	TRISBbits.RB6 = ENTRADA;
	TRISBbits.RB7 = ENTRADA;
	
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

void exibirNome(unsigned char texto[], int tamanhoTexto) {
  // Itera sobre o array de Caracteres definido para o nome a ser mostrado
  unsigned char i;

  for (i = 0; i < tamanhoTexto; i++) {
    enviarDadoLCD(texto[i]>>4);
    enviarDadoLCD(texto[i]);
  }
}

unsigned char lerTeclado(void) {
	// Varre o teclado numerico e retorna o valor correspondente a tecla clicada

	unsigned char valorTeclado = 200, i, teste;
	unsigned char matriz_numeros_teclado[4][3] = {
		{'1', '2',  '3'	},
		{'4', '5',	'6'	},
		{'7', '8',	'9'	},
		{200, '0',	200	},
	};

	for (i = 0; i < 4; i++) {
		switch (i) {
			case 0:
				MatCol2 = 1;
				MatCol3 = 1;
				MatCol1 = 0;
				break;

			case 1:
				MatCol1 = 1;
				MatCol3 = 1;
				MatCol2 = 0;
				break;

			case 2:
				MatCol1 = 1;
				MatCol2 = 1;
				MatCol3 = 0;
				break;

			default:
				MatCol1 = 1;
				MatCol2 = 1;
				MatCol3 = 1;
				break;
		}
		if (i > 3) {}
		else if(MatLinA == 0) {
			while (MatLinA == 0) {}
			return matriz_numeros_teclado[0][i];
		}
		else if(MatLinB == 0) {
			while (MatLinB == 0) {}
			return matriz_numeros_teclado[1][i];
		}
		else if(MatLinC == 0) {
			while (MatLinC == 0) {}
			return matriz_numeros_teclado[2][i];
		}
		else if(MatLinD == 0) {
			while (MatLinD == 0) {}
			return matriz_numeros_teclado[3][i];
		}
	}
	return valorTeclado;
}

void pedirSenha(void) {
  unsigned char mensagens[2][11] = {
    {'O','L','A',',',' ','J','O','R','G','E','!'},
    {'S','E','N','H','A',':',' ',' ',' ',' ',' '},
  };

  limpaDisplay();
  exibirNome(mensagens[0], 11);
  enviarComandoLCD(0b00001100);
  enviarComandoLCD(0b00000000);
  exibirNome(mensagens[1], 7);
}

void avisarSenhaIncorreta(void) {
  unsigned char mensagens[2][16] = {
    {'S','E','N','H','A',' ','I','N','C','O','R','R','E','T','A','!'},
    {'T','E','N','T','E',' ','N','O','V','A','M','E','N','T','E','!'},
  };

  limpaDisplay();
  exibirNome(mensagens[0], 16);
  enviarComandoLCD(0b00001100);
  enviarComandoLCD(0b00000000);
  exibirNome(mensagens[1], 16);
}

void avisarSenhaCorreta(void) {
  unsigned char mensagem[] = {
    'S','E','J','A',' ','B','E','M',' ','V','I','N','D','O','!',
  };

  limpaDisplay();
  exibirNome(mensagem, 15);
  while (1) {}
}

unsigned char realizaLogin(void) {
  unsigned char leituraTeclado, cont = 0;

  while (cont < 4) {
    leituraTeclado = lerTeclado();
    if (leituraTeclado != 200) {
      enviarDadoLCD(leituraTeclado>>4);
      enviarDadoLCD(leituraTeclado);
      senhaDigitada[cont] = leituraTeclado;
      cont += 1;
    }
  }
  cont = 0;

  while (cont < 4) {
    if (senhaDigitada[cont] != senhaSalva[cont]) {
      return 0;
    }
    cont += 1;
  }
  return 1;
}

void main(void) {

  inicializarAmbiente();
  inicializarLCD();

  while(1) {
    pedirSenha();
    logado = realizaLogin();

    if (logado) {
      break;
    } else {
      avisarSenhaIncorreta();
      delayMs(5000);
    }
  }
  avisarSenhaCorreta();
}