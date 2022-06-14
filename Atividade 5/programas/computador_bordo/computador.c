#include <p18f4520.h>
#include "LCD16X2.h"

#pragma config WDT = OFF
#pragma config PBADEN = OFF

#define RegTimerFlag    INTCONbits.TMR0IF
#define RegIntFlag      INTCONbits.INT0IF

#define ODOMERTO        PORTBbits.RB0
#define ROTACAO         PORTBbits.RB2

#define SAIDA 0
#define ENTRADA 1

void tratamentoTimer(void);
char inicializado = 0;

#pragma code enderecoIntTimer = 0x08

void interrupcaoTimer(void) {
    _asm goto tratamentoTimer _endasm
}
#pragma code

#pragma interrupt tratamentoTimer

void tratamentoTimer(void) {
    if (RegTimerFlag == 1) {
        RegTimerFlag = 0;
        TMR0H = 0xf8;
        TMR0L = 0x44;
        if (inicializado == 1)
        {
            enviarDadoLCD('b');
        }
    }
    if (RegIntFlag == 1) {
        // while(ODOMERTO == 1) {}
        RegIntFlag = 0;
        enviarDadoLCD('A');
    }
}

void inicializaInterrupcoes(void) {
    INTCONbits.GIE      = 1;
    INTCONbits.TMR0IE   = 1;
    INTCON2bits.TMR0IP  = 1;
    RegTimerFlag        = 0;

    TRISBbits.TRISB0    = ENTRADA;
    INTCONbits.INT0IE   = 1;
    RegIntFlag          = 0;
}

void inicializaTimer(void) {
    T0CONbits.TMR0ON    = 1; // Ativa o timer
    T0CONbits.T08BIT    = 0; // Ativa o modo 16 bits
    T0CONbits.T0CS      = 0; // Relaciona o timer ao clock interno
    T0CONbits.PSA       = 1; // Ativa o Prescaler
    TMR0H = 0xf8;
    TMR0L = 0x44;
}

void main(void) {
    ADCON1 = 0b00001111;
    TRISC = 0b00000000;
    TRISDbits.TRISD0 = SAIDA;
    TRISDbits.TRISD1 = SAIDA;

    inicializaInterrupcoes();
    inicializaTimer();
    inicializarLCD();

    inicializado = 1;

    while (1) {}
}
