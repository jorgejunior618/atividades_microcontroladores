#include <p18f4520.h>
#include "LCD16X2.h"

#pragma config WDT = OFF
#pragma config PBADEN = OFF

#define RegTimerFlag    INTCONbits.TMR0IF
#define RegOdometroFlag INTCONbits.INT0IF
#define RegRotacaoFlag INTCON3bits.INT2IF

#define ODOMERTO        PORTBbits.RB0
#define ROTACAO         PORTBbits.RB2

#define SAIDA 0
#define ENTRADA 1
/**
 * Circunferencia media da roda do carro em metros
 */
#define circRoda 15

void tratamentoInterrupcao(void);

char contando = 0;
unsigned milisegundosDecorridos = 30;
unsigned milisegundosMostrar = 30;

unsigned velocidadeNum = 0;
char velocidadeStr[3] = {'0','0','0'};

unsigned rotacaoNum = 0;
char rotacaoStr[3] = {'0','0','0'};

unsigned quilometragemNum = 0;
char quilometragemStr[5] = {'0','0','0','0','0'};

#pragma code enderecoInterrupcao = 0x08

void interrupcaoTimer(void) {
    _asm goto tratamentoInterrupcao _endasm
}
#pragma code

#pragma interrupt tratamentoInterrupcao

void tratamentoInterrupcao(void) {
    if (RegTimerFlag == 1) {
        RegTimerFlag = 0;
        TMR0H = 0xfc;
        TMR0L = 0x2c;
        milisegundosDecorridos += 1;
    }
    if (RegOdometroFlag == 1) {
        // while(ODOMERTO == 1) {}
        RegOdometroFlag = 0;
    }
    if (RegRotacaoFlag == 1) {
        // while(ODOMERTO == 1) {}
        RegRotacaoFlag = 0;
        if (contando == 1) {
            contando = 0;
            milisegundosMostrar = milisegundosDecorridos;
            milisegundosDecorridos = 0;
            T0CONbits.TMR0ON = 0;
            quilometragemNum += 3;
        } else {
            T0CONbits.TMR0ON = 1;
            contando = 1;
        }
    }
}

void inicializaInterrupcoes(void) {
    INTCONbits.GIE      = 1;
    INTCONbits.TMR0IE   = 1;
    INTCON2bits.TMR0IP  = 1;
    RegTimerFlag        = 0;

    TRISBbits.TRISB0    = ENTRADA;
    INTCONbits.INT0IE   = 1;
    RegOdometroFlag     = 0;

    TRISBbits.TRISB2    = ENTRADA;
    INTCON3bits.INT2IE   = 1;
    RegRotacaoFlag      = 0;
}

void inicializaTimer(void) {
    T0CONbits.TMR0ON    = 0;
    T0CONbits.T08BIT    = 0; // Ativa o modo 16 bits
    T0CONbits.T0CS      = 0; // Relaciona o timer ao clock interno
    T0CONbits.PSA       = 1; // Ativa o Prescaler
    TMR0H = 0x00;
    TMR0L = 0x00;
}

void inicializarComputadorDeBordo(void) {
    enviarDadoLCD('0'); // 1
    enviarDadoLCD('0'); // 2
    enviarDadoLCD('0'); // 3
    enviarDadoLCD('K'); // 4
    enviarDadoLCD('m'); // 5
    enviarDadoLCD('/'); // 6
    enviarDadoLCD('h'); // 7

    enviarDadoLCD(' '); // 8
    enviarDadoLCD('|'); // 9
    enviarDadoLCD(' '); // 10

    enviarDadoLCD('0'); // 11
    enviarDadoLCD('0'); // 12
    enviarDadoLCD('0'); // 13
    enviarDadoLCD('R'); // 14
    enviarDadoLCD('P'); // 15
    enviarDadoLCD('M'); // 16

    enviarComandoLCD(0xc0);
    enviarDadoLCD('0'); // 2
    enviarDadoLCD('0'); // 3
    enviarDadoLCD('0'); // 4
    enviarDadoLCD('0'); // 5
    enviarDadoLCD('0'); // 6
    enviarDadoLCD(' '); // 7
    enviarDadoLCD('K'); // 8
    enviarDadoLCD('m'); // 9
}

void calculaVelocidade(void) {
    unsigned centena, dezena, unidade;
    velocidadeNum = (circRoda * 360)/ milisegundosMostrar;

    centena = velocidadeNum / 100;
    dezena  = (velocidadeNum / 10) - centena*10;
    unidade = velocidadeNum - (centena*100 + dezena*10);

    velocidadeStr[0] = centena + 48;
    velocidadeStr[1] = dezena + 48;
    velocidadeStr[2] = unidade + 48;
}

void calculaRotacao(void) {
    unsigned centena, dezena, unidade;
    rotacaoNum = 60000 / milisegundosMostrar;

    centena = rotacaoNum / 100;
    dezena  = (rotacaoNum / 10) - centena*10;
    unidade = rotacaoNum - (centena*100 + dezena*10);

    rotacaoStr[0] = centena + 48;
    rotacaoStr[1] = dezena + 48;
    rotacaoStr[2] = unidade + 48;
}

void calculaQuilometragem(void) {
    unsigned centenaMilhar, dezenaMilhar, milhar, centena, dezena, unidade, km;

    km = quilometragemNum / 1000;

    dezenaMilhar = km / 10000;
    milhar = km / 1000 - dezenaMilhar*10;
    centena = km / 100 - (dezenaMilhar*100 + milhar*10);
    dezena  = (km / 10) - (dezenaMilhar*1000 + milhar*100 + centena*10);
    unidade = km - (dezenaMilhar*10000 + milhar*1000 + centena*100 + dezena*10);

    quilometragemStr[0] = dezenaMilhar + 48;
    quilometragemStr[1] = milhar + 48;
    quilometragemStr[2] = centena + 48;
    quilometragemStr[3] = dezena + 48;
    quilometragemStr[4] = unidade + 48;
}

void mostraVelocidade(void) {
    char i;

    enviarComandoLCD(0x80);
    for (i = 0; i < 3; i++) {
        enviarDadoLCD(velocidadeStr[i]);
    }
}

void mostraRotacao(void) {
    char i;

    enviarComandoLCD(0x8a);
    for (i = 0; i < 3; i++) {
        enviarDadoLCD(rotacaoStr[i]);
    }
}

void mostraQuilometragem(void) {
    char i;

    enviarComandoLCD(0xc0);
    for (i = 0; i < 5; i++) {
        enviarDadoLCD(quilometragemStr[i]);
    }
}

void main(void) {
    ADCON1 = 0b00001111;
    TRISC = 0b00000000;
    TRISDbits.TRISD0 = SAIDA;
    TRISDbits.TRISD1 = SAIDA;

    inicializaInterrupcoes();
    inicializaTimer();
    inicializarLCD();

    inicializarComputadorDeBordo();

    while (1) {
        if (contando == 0) {
            calculaVelocidade();
            mostraVelocidade();
            calculaRotacao();
            mostraRotacao();
            calculaQuilometragem();
            mostraQuilometragem();
        }
    }
}