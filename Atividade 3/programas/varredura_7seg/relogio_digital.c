#include <p18f4520.h>
#include "timer.h"

#pragma config OSC = INTIO67
#pragma config WDT = OFF

#define SAIDA 0
#define ENTRADA 1
#define BotaoModo PORTDbits.RD0
#define AjusteHora PORTDbits.RD1
#define AjusteMinuto PORTDbits.RD2

#define BuzzerAlarme LATDbits.LATD4
#define HORAS LATA
#define MINUTOS LATB
#define SEGUNDOS LATC

unsigned char hora, minuto, segundo, horaUltimoAlarme;

void inicializarAmbiente(void) {
	TRISA = 0x00;
	TRISB = 0x00;
	TRISC = 0x00;
	TRISDbits.RD0 = ENTRADA;
	TRISDbits.RD1 = ENTRADA;
	TRISDbits.RD2 = ENTRADA;
	TRISDbits.RD4 = SAIDA;

	OSCCON = 0b01000100; // Configura a frequência do oscilador interno para 1MHz
}

void inicializarRelogio(void) {
	hora			 = 0;
	minuto			 = 0;
	segundo			 = 0;
	horaUltimoAlarme = 0;

	HORAS = 	hora;
	MINUTOS = 	minuto;
	SEGUNDOS = 	segundo;
	BuzzerAlarme = 1;
}

void atualizaRelogio(void) {
	// Incrementa as vari�veis do relogio em 1 (Um)
	// seguindo as regras do formato hh:mm:ss

	if ((segundo + 7) % 96 == 0) {
		segundo = 0;
	}
	else if ((segundo + 7) % 16 == 0) {
		segundo += 7;
	} else {
		segundo += 1;
	}

	if (segundo == 0) {
		if ((minuto + 7) % 96 == 0) {
			minuto = 0;
		}
		else if ((minuto + 7) % 16 == 0) {
			minuto = minuto + 7;
		} else {
			minuto = minuto + 1;
		}

		if (minuto == 0) {
			if ((hora + 1) % 36 == 0) {
				hora = 0;
			}
			else if ((hora + 7) % 16 == 0) {
				hora = hora + 7;
			} else {
				hora = hora + 1;
			}
		}
	}
}

void mostrarHorario(void) {
	// Configura o display com o ultimo valor do hor�rio atualizado

	SEGUNDOS = segundo;
	MINUTOS = minuto;
	HORAS = hora;
}

void incrementaRelogioHora(void) {
	// Incrementa em 1 (um) o valor da vari�vel da hora,
	// e atualiza o display com o novo valor

	if ((hora + 1) % 36 == 0) {
		hora = 0;
	}
	else if ((hora + 7) % 16 == 0) {
		hora = hora + 7;
	} else {
		hora = hora + 1;
	}
	HORAS = hora;
	horaUltimoAlarme = hora;
}

void incrementaRelogioMinuto(void) {
	// Incrementa em 1 (um) o valor da vari�vel do minuto,
	// e atualiza o display com o novo valor

	if ((minuto + 7) % 96 == 0) {
		minuto = 0;
	}
	else if ((minuto + 7) % 16 == 0) {
		minuto = minuto + 7;
	} else {
		minuto = minuto + 1;
	}
	MINUTOS = minuto;
}

void editaRelogio(void) {
	// Entra no modo de edicao do Relogio.
	// Ao Botao de modo ser apertado novamente, sai do modo de edicao
	// Ao Botao de ajuste de hora ser apertado, incrementa o valor da hora
	// Ao Botao de ajuste do minuto ser apertado, incrementa o valor do minuto

	unsigned int i;

	// Previne o fim da funcao ao entrar no modo de edicao
	// com o Botao de modo pressionado
	while(BotaoModo == 1) {}

	while(1){
		if(BotaoModo == 1){break;}
		if(AjusteHora == 1) {
			incrementaRelogioHora();
			delay_millis(400);
		}
		if(AjusteMinuto == 1) {
			incrementaRelogioMinuto();
			delay_millis(400);
		}
	}
}

unsigned char verificaModoEdicao(void) {
	// Verifica se o botao de modo de edicao foi pressionado
	// E inicia a contagem at� se passarem 2 segundos, ou
	// at� o bot�o deixar de ser pressionado

	unsigned int i, modoEdicao;
	unsigned int cont = 0;

	if (BotaoModo == 1) {
		while (cont < 2) {
			for(i = 0; i < CICLOUMSEG; i++) {
				if(BotaoModo == 0) {
					cont = 10;
					break;
				}
			};
			cont++;
		}
		modoEdicao = cont;
	}
	if (modoEdicao == 2) {
		return 1;
	}
	return 0;
}

void verificarAlarme(void) {
	// Verifica se a unid. Hora do rel�gio foi alterada
	// Caso sim, ativa o buzzer, e no proximo segundo desativa
	if (horaUltimoAlarme != hora) {
		BuzzerAlarme = 0;
	} else { // No proximo loop as duas vari�veis se igualam
		BuzzerAlarme = 1;
	}
}

void main(void) {
	inicializarAmbiente();

	inicializarRelogio();

	while (1) {
		// Sincroniza a hora do alarme apos o delay e incremento de tempo
		horaUltimoAlarme = hora;

		if (verificaModoEdicao() == 1) {
			editaRelogio();
		}

		delay_secs(1);

		atualizaRelogio();
		mostrarHorario();

		verificarAlarme();
	}
}
