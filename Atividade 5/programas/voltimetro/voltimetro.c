#include <p18f4520.h>
#include "LCD16X2.h"

#pragma config WDT = OFF
#pragma config PBADEN = OFF

#define AnalogChannel   ADCON0bits.CHS0
#define ADConversorFlag PIR1bits.ADIF

#define SAIDA 0
#define ENTRADA 1

void calculaTensaoV1(void);
void mostraTensaoV1(void);
void calculaTensaoV2(void);
void mostraTensaoV2(void);
void tratamentoInterrupcao(void);

unsigned long tensaoV1Num = 0;
char tensaoV1Str[4] = {'0','.','0','0'};

unsigned long tensaoV2Num = 0;
char tensaoV2Str[4] = {'0','.','0','0'};

char calculandoV1 = 1;

#pragma code enderecoInterrupcao = 0x08

void interrupcao(void) {
    _asm goto tratamentoInterrupcao _endasm
}
#pragma code

#pragma interrupt tratamentoInterrupcao

void tratamentoInterrupcao(void) {
    if (ADConversorFlag == 1) {
        ADConversorFlag = 0;
        if (calculandoV1 == 0) {
            calculandoV1 = 1;
            calculaTensaoV1();
            mostraTensaoV1();
        } else {
            calculandoV1 = 0;
            calculaTensaoV2();
            mostraTensaoV2();
        }
        AnalogChannel = ~AnalogChannel;
        ADCON0bits.GO       = 1;
    }
}

/**
 * ADCON0, ADCON1, ADCON2 - Registradores de configuracao das entradas A/D
 * ADRESH, ADRESL - Registradores de conversao das entradas A/D
 * Tensao = (ADRESH * 256) + ADRESL - Obtendo resultado da conversao
 */
void inicializaPortas(void) {
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001101;
    ADCON2 = 0b10111110;
    TRISAbits.TRISA0 = ENTRADA;
    TRISAbits.TRISA1 = ENTRADA;
    TRISC = 0b00000000;
    TRISDbits.TRISD0 = SAIDA;
    TRISDbits.TRISD1 = SAIDA;
}

void inicializaInterrupcoes(void) {
    INTCONbits.GIE      = 1;
    INTCONbits.PEIE     = 1;
    PIE1bits.ADIE       = 1;
    IPR1bits.ADIP       = 1;
    ADConversorFlag     = 0;
    ADCON0bits.GO       = 1;
}

void inicializarVoltimetro(void) {
	int i;
    enviarDadoLCD('V');
    enviarDadoLCD('1');
	for (i=0;i<50;i++) {}
    enviarDadoLCD(':');
    enviarDadoLCD(' ');
    enviarDadoLCD('0');
    enviarDadoLCD('.');
    enviarDadoLCD('0');
    enviarDadoLCD('0');

    enviarComandoLCD(0xc0);
    enviarDadoLCD('V');
    enviarDadoLCD('2');
    enviarDadoLCD(':');
    enviarDadoLCD(' ');
    enviarDadoLCD('0');
    enviarDadoLCD('.');
    enviarDadoLCD('0');
    enviarDadoLCD('0');
}

void calculaTensaoV1(void) {
    unsigned long tensao, und, d1, d2;
    tensaoV1Num = (ADRESH * 256) + ADRESL;
    
    tensao = (tensaoV1Num * 488) / 100 + 6;

    und =   tensao / 1000;
    d1 =    tensao / 100 - und * 10;
    d2 =    tensao / 10  - (und * 100 + d1 * 10);

    tensaoV1Str[0] = und + 48;
    tensaoV1Str[2] = d1 + 48;
    tensaoV1Str[3] = d2 + 48;
}

void calculaTensaoV2(void) {
    unsigned long tensao, und, d1, d2;
    tensaoV2Num = (ADRESH * 256) + ADRESL;
    
    tensao = (tensaoV2Num * 488) / 100 + 6;

    und =   tensao / 1000;
    d1 =    tensao / 100 - und * 10;
    d2 =    tensao / 10  - (und * 100 + d1 * 10);

    tensaoV2Str[0] = und + 48;
    tensaoV2Str[2] = d1 + 48;
    tensaoV2Str[3] = d2 + 48;
}

void mostraTensaoV1(void) {
    char i;

    enviarComandoLCD(0x84);
    for (i = 0; i < 4; i++) {
        enviarDadoLCD(tensaoV1Str[i]);
    }
}

void mostraTensaoV2(void) {
    char i;

    enviarComandoLCD(0xc4);
    for (i = 0; i < 4; i++) {
        enviarDadoLCD(tensaoV2Str[i]);
    }
}

void main(void) {
    inicializaPortas();
    inicializarLCD();
    inicializarVoltimetro();
    inicializaInterrupcoes();

    while (1) {}
}