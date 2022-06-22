#include <p18f4520.h>

#pragma config WDT = OFF
#pragma config PBADEN = OFF

#define SAIDA 0
#define ENTRADA 1

#define Zona1Flag INTCONbits.INT0IF
#define Zona2Flag INTCON3bits.INT1IF
#define Zona3Flag INTCON3bits.INT2IF
#define RegTimerFlag INTCONbits.TMR0IF

#define Transmissor LATCbits.LATC0

void tratamentoInterrupcao(void);
void delay1200bps(void);
void delayDefasagem(void);
void transmiteByte(char byte);
void incrementaSegundo(void);
char
    alarmando,
    zonaInvadida,
    enviarMensagem,
    dezHr,
    unHr,
    dezMin,
    unMin,
    dezSeg,
    unSeg;
long segundos;

#pragma code enderecoInterrupcao = 0x08

void interrupcao(void) {
    _asm goto tratamentoInterrupcao _endasm
}
#pragma code

#pragma interrupt tratamentoInterrupcao

void tratamentoInterrupcao(void) {
    if (Zona1Flag == 1) {
		Zona1Flag = 0;
        alarmando = 1;
        enviarMensagem = 1;
        zonaInvadida = '1';
    }
    if (Zona2Flag == 1) {
		Zona2Flag = 0;
        alarmando = 1;
        enviarMensagem = 1;
        zonaInvadida = '2';
    }
    if (Zona3Flag == 1) {
		Zona3Flag = 0;
        alarmando = 1;
        enviarMensagem = 1;
        zonaInvadida = '3';
    }
    if (RegTimerFlag == 1) {
        RegTimerFlag = 0;
        TMR0H = 0x0B;
        TMR0L = 0xDC;
        incrementaSegundo();
    }
}

void inicializaPic(void) {
    dezHr   = '0';
    unHr    = '0';
    dezMin  = '0';
    unMin   = '0';
    dezSeg  = '0';
    unSeg   = '0';

    segundos = 0;
    alarmando = 0;
    zonaInvadida = '0';
    enviarMensagem = 1;
}

void inicializaPortas(void) {
    ADCON1 = 0b00001111;
    OSCCON = 0b01110111;
    TRISBbits.TRISB4 = ENTRADA;
    TRISBbits.TRISB5 = ENTRADA;
    TRISBbits.TRISB6 = ENTRADA;
    TRISBbits.TRISB7 = ENTRADA;
    Transmissor = 1;
}

void inicializaPWM(void) {
    TRISCbits.TRISC0 = SAIDA; // rc2/ccp1
    TRISCbits.TRISC1 = SAIDA; // rc2/ccp1
    TRISCbits.TRISC2 = SAIDA; // rc1/ccp2

    CCP2CON = 0b00111100;
    PR2 = 100; // periodo
    CCPR2L = 50; // Duty cicle

    T2CON = 0b00000110;
}

void inicializaInterrupcoes(void) {
    INTCONbits.GIE = 1;

    INTCONbits.INT0IE = 1;
    INTCONbits.INT0IF = 0;

    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IP = 1;
    INTCON3bits.INT1IF = 0;

    INTCON3bits.INT2IE = 1;
    INTCON3bits.INT2IP = 1;
    INTCON3bits.INT2IF = 0;

    
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
}

void incrementaSegundo(void) {
    if (segundos++ > 86400) {
        segundos = 0;
    }
}

void delay1200bps(void) {
    // Funcao de delay para a recepcao dos dados de entrada bit a bit
    // Aguarda Aprox. 833us (1200Hz)
    unsigned i = 0;

    while (i++ < 107) {}
}

void transmiteByte(char byte) {
    // Transmite o caracter correspondente ao armazenado para o terminal
    // Recebe: byte-> char : correspondente ao que foi recebido
    unsigned int i = 0;
    int mascara = 0b000000001;
    int valor = 0;

    Transmissor = valor;
    delay1200bps();
    for(i = 0; i < 8; i++) {
        if (byte & mascara<<i) {
            valor = 1;
        }
        Transmissor = valor;
        delay1200bps();
        valor = 0;
    }
    Transmissor = 1;
    delay1200bps();
}

void calculaHorario(void) {
    long hora, minuto, segundo;

    hora = segundos / 3600;
    minuto = (segundos - (hora*3600)) / 60;
    segundo = segundos - (hora*3600 + minuto*60);

    dezHr = hora / 10;
    unHr = hora - dezHr * 10;

    dezMin = minuto / 10;
    unMin = minuto - dezMin * 10;

    dezSeg = segundo / 10;
    unSeg = segundo - dezSeg * 10;
    dezHr = dezHr+48;
    unHr = unHr+48;
    dezMin = dezMin+48;
    unMin = unMin+48;
    dezSeg = dezSeg+48;
    unSeg = unSeg+48;
}

void transmiteInvasao(void) {
    char i;
    char mensagem[15] = {
        'Z','O','N','A',' ', zonaInvadida, ' ',
        dezHr,unHr,':', dezMin,unMin,':', dezSeg,unSeg
    };

    enviarMensagem = 0;
    for (i = 0; i < 15; i++) {
        transmiteByte(mensagem[i]);
    }
    transmiteByte(13);
}

void desligarAlarme(void) {
    CCP2CON = 0b00110000;
}

void main(void) {
    unsigned char periodo = 100, crescendo = 1;
    int i;

    inicializaPic();
    inicializaPortas();
    inicializaInterrupcoes();
    inicializaPWM();
    inicializaTimer();

    delay1200bps();
    while (1) {
		if (alarmando == 1) {
            if (enviarMensagem == 1) {
                calculaHorario();
                transmiteInvasao();
            }
            CCP2CON = 0b00111100;
            PR2 = periodo;
            CCPR2L = periodo/2;

            if (periodo > 150) {
                crescendo = 0;
            } else if (periodo < 50) {
                crescendo = 1;
            }
            if (crescendo == 1) {
                periodo = periodo + 1;
            } else {
                periodo = periodo - 1;
            }
        for (i = 0; i < 350; i++) {}
        }
         else  {
            desligarAlarme();
         }
    }
}