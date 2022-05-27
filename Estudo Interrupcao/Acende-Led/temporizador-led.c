#include <p18f4520.h>

#define LED LATCbits.LATC0
#define RegTimerFlag INTCONbits.TMR0IF

void tratamentoTimer(void);

#pragma code enderecoIntTimer = 0x08

void interrupcaoTimer(void) {
    _asm goto tratamentoTimer _endasm
}
#pragma code

#pragma interrupt tratamentoTimer

void tratamentoTimer(void) {
    if (RegTimerFlag == 1) {
        RegTimerFlag = 0;
        TMR0H = 0x0B;
        TMR0L = 0xDC;
        LED = ~LED;
    }
}

void inicializaInterrupcoes(void) {
    INTCONbits.GIE      = 1;
    INTCONbits.TMR0IE   = 1;
    INTCON2bits.TMR0IP  = 1;
    RegTimerFlag        = 0;
}

void inicializaTimer(void) {
    T0CONbits.TMR0ON    = 1; // Ativa o timer
    T0CONbits.T08BIT    = 0; // Ativa o modo 16 bits
    T0CONbits.T0CS      = 0; // Relaciona o timer ao clock interno
    T0CONbits.PSA       = 0; // Ativa o Prescaler
    // Divisao do prescaler
    T0CONbits.T0PS0     = 0;
    T0CONbits.T0PS1     = 0;
    T0CONbits.T0PS2     = 1;
    TMR0H = 0x0B;
    TMR0L = 0xDC;
    TRISCbits.TRISC0    = 0;
    LED = 0;
}

void main(void) {
    inicializaInterrupcoes();
    inicializaTimer();
    
    while (1) {}
}
