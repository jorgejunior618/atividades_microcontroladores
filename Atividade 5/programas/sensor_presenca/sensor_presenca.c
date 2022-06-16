#include <p18f4520.h>

#pragma config WDT = OFF
#pragma config PBADEN = OFF

#define SAIDA 0
#define ENTRADA 1

void tratamentoInterrupcao(void);

#pragma code enderecoInterrupcao = 0x08

void interrupcao(void) {
    _asm goto tratamentoInterrupcao _endasm
}
#pragma code

#pragma interrupt tratamentoInterrupcao

void tratamentoInterrupcao(void) {
    
}

void inicializaPortas(void) {
    ADCON1 = 0b00001111;
    OSCCON = 0b01111100;
    TRISAbits.TRISA0 = ENTRADA;
    TRISAbits.TRISA1 = ENTRADA;
    TRISAbits.TRISA2 = ENTRADA;
    TRISAbits.TRISA3 = ENTRADA;
    TRISDbits.TRISD0 = SAIDA;
    TRISDbits.TRISD1 = SAIDA;
}

void inicializaPWM(void) {
    TRISCbits.TRISC1 = SAIDA; // rc2/ccp1
    TRISCbits.TRISC2 = SAIDA; // rc1/ccp2

    CCP2CON = 0b00111100;
}

void inicializaInterrupcoes(void) {
    INTCONbits.GIE      = 0;
}

void geraTensao(void) {
    char i;

    for (i=0;i<18;i++){
        LATDbits.LATD0 = 1;
    }
    for (i=0;i<1;i++){
        LATDbits.LATD0 = 0;
    }
}

void main(void) {
    inicializaPortas();
    // inicializaInterrupcoes();

    while (1) {
        geraTensao();
    }
}