#ifndef LCD16X2_H_
#define LCD16X2_H_

#define DISPLAY         LATC
#define RS              LATDbits.LATD0
#define ENABLE          LATDbits.LATD1
#define BITInstrucao 0
#define BITDado 1

void delayInstrucaoLCD(int delay) {
    // Realiza a espera do valor informado no parametro [delay] em milisegundos
    int i, j;
    for (i = 0; i < delay; i++){
        for (j = 0; j < 40; j++) {}
    }
}

void delayInicialLCD(void) {
    // Delay para garantir a inicialização correta do LCD (aprox. 100ms)
    int i;
    for (i = 0; i < 1700; i++) {}
}

void pulsoEnable(void) {
    // Realiza um pulso no Enable do LCD para receber a informação no barramento de dados
    ENABLE = 1;
    ENABLE = 0;
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
}

void inicializarLCD(void) {
    // DISPLAY = 0;
    delayInicialLCD();

    enviarComandoLCD(0b00111000);
    delayInstrucaoLCD(2);

    // Define o LCD ligado e cursor
    enviarComandoLCD(0b00001110);
    delayInstrucaoLCD(2);

    // Limpa o display
    enviarComandoLCD(0b00000001);
    delayInstrucaoLCD(2);

    // Modo de Entrada
    enviarComandoLCD(0b00000110);
    delayInstrucaoLCD(2);
}

#endif
