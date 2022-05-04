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
#define SAIDA_DISPLAY LATC
#define SAIDA_VARREDURA LATA

unsigned char hora, minuto, segundo, horaUltimoAlarme;
unsigned char numeros_display[10] = {
  0b00000001,
  0b01001111,
  0b00010010,
  0b00000110,
  0b01001100,
  0b00100100,
  0b00100000,
  0b00001111,
  0b00000000,
  0b00000100,
};

void inicializarAmbiente(void) {
	TRISC =	0b10000000;
	TRISA =	0b11000000;
	TRISDbits.RD0 =	ENTRADA;
	TRISDbits.RD1 =	ENTRADA;
	TRISDbits.RD2 =	ENTRADA;

	OSCCON =  0b01000100; // Configura a frequência do oscilador interno para 1MHz
}

void inicializarRelogio(void) {
	hora			 = 0;
	minuto			 = 0;
	segundo			 = 0;
	horaUltimoAlarme = 0;

	SAIDA_DISPLAY = numeros_display[0];
	SAIDA_VARREDURA = 0b00111111;
	BuzzerAlarme = 1;
}

void atualizaRelogio(void) {
	// Incrementa as variaveis do relogio em 1 (Um)
	// seguindo as regras do formato hh:mm:ss

  segundo += 1;

	if (segundo == 60) {
		segundo = 0;
    minuto += 1;
		
    if (minuto == 60) {
			minuto = 0;
      hora += 1;

			if (hora == 24) {
				hora = 0;
			}
		}
	}
}

void realizaUmaVarredura(void) {
	// Configura o display com o ultimo valor do horario atualizado
	// Realizando a varredura nos Seis displays

  unsigned int i;
	unsigned char byteVarredura = 0b00100000;
	unsigned char unidadesTempo[6] = {
    segundo - ((segundo / 10) * 10),
    segundo / 10,
    minuto - ((minuto / 10) * 10),
    minuto / 10,
    hora - ((hora / 10) * 10),
    hora / 10,
  };
	
	for (i = 0; i < 6; i++) {
		SAIDA_VARREDURA = 0b00000000;
		SAIDA_DISPLAY = numeros_display[unidadesTempo[i]];
		SAIDA_VARREDURA = byteVarredura;
		byteVarredura = byteVarredura>>1;
	}

}

void mostrarRelogioVarredura(void) {
	// Configura o display com o ultimo valor do horario atualizado
	// Realizando a varredura nos Seis displays

  unsigned int i;
  for (i = 0; i < 286; i++) { // Numero de repeticoes para completar Um segundo
    realizaUmaVarredura();
  }
}

void incrementaRelogioHora(void) {
	// Incrementa em 1 (um) o valor da variavel da hora,
	// e atualiza o display com o novo valor

  hora += 1;
	if (hora == 24) {
		hora = 0;
	}
	horaUltimoAlarme = hora;
}

void incrementaRelogioMinuto(void) {
	// Incrementa em 1 (um) o valor da variavel do minuto,
	// e atualiza o display com o novo valor

  minuto += 1;
	if (minuto == 60) {
		minuto = 0;
	}
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

	while(1) {
		realizaUmaVarredura();

		if(BotaoModo == 1){break;}
		if(AjusteHora == 1) {
			incrementaRelogioHora();
			while (AjusteHora == 1) {
				realizaUmaVarredura();
			}
		}
		if(AjusteMinuto == 1) {
			incrementaRelogioMinuto();
			while (AjusteMinuto == 1) {
				realizaUmaVarredura();
			}
		}
	}
}

unsigned char verificaModoEdicao(void) {
	// Verifica se o botao de modo de edicao foi pressionado
	// E inicia a contagem ate se passarem 2 segundos, ou
	// ate o botao deixar de ser pressionado

	unsigned int i;
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
	}
	if (cont == 2) {
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

		mostrarRelogioVarredura();
		atualizaRelogio();

		if (PORTBbits.RB0 == 1) {
			atualizaRelogio();
			atualizaRelogio();
			atualizaRelogio();
			atualizaRelogio();
			atualizaRelogio();
			atualizaRelogio();
			atualizaRelogio();

		}

		if (verificaModoEdicao() == 1) {
			editaRelogio();
		}

		verificarAlarme();
	}
}
