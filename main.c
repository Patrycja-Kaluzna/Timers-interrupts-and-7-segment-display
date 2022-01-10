#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#ifndef _BV
#define _BV(bit)				(1<<(bit))
#endif

#ifndef sbi
#define sbi(reg,bit)		reg |= (_BV(bit))
#endif

#ifndef cbi
#define cbi(reg,bit)		reg &= ~(_BV(bit))
#endif

#ifndef tbi
#define tbi(reg,bit)		reg ^= (_BV(bit))
#endif

volatile int i;

/*
//ZADANIE 1
//Inicjalizuje Timer1 do wywolywania przerwania z czêstotliwoœci¹ 2 Hz
void TimerInit () {
	//Wybranie trybu pracy CTC z TOP OCR1A
	sbi(TCCR1B, WGM12);
	//Wybranie dzielnika czestotliwosci
	sbi(TCCR1B, CS12);
	//Zapisanie do OCR1A wartosci odpowiadajacej 0,5 s
	OCR1A = 15625;
	//Uruchomienie przerwania OCIE1A
	sbi(TIMSK, OCIE1A);
}
*/

//ZADANIE 2
char cyfra[10] = {0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011,
		0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011};

//ZADANIE 2
//Inicjalizuje porty do obs³ugi wyswietlacza 7 segmentowego
void seg7Init () {
	//Inicjalizacja segmentu
	DDRC = 0xff; //Ustawia wyprowadzenia jako wyjœcia
	PORTC = 0x00; //Ustawia wyprowadzenia w stan niski
}

//ZADANIE 2
//Wyswietla na wyswietlaczu 7 segmentowym cyfre z argumentu
void seg7ShowCyfra (uint8_t cyfraDoWyswietlenia) {
	PORTC = cyfra[cyfraDoWyswietlenia]; //Ustawia wyprowadzenia portu C odpowiednio w stan niski lub wysoki
										//w zale¿noœci od wartoœci zapisanych w elementach tablicy
										//znakow char cyfra[10]
}

//ZADANIE 3
void TimerInit () {
	//Wybranie trybu pracy CTC z TOP OCR1A
	sbi(TCCR1B, WGM12);
	//Wybranie dzielnika czestotliwosci
	sbi(TCCR1B, CS12);
	//Zapisanie do OCR1A wartosci odpowiadajacej 1 s
	OCR1A = 31250;
	//Uruchomienie przerwania OCIE1A
	sbi(TIMSK, OCIE1A);
}

int main () {
	/*
	//ZADANIE 1

	//Przerwanie polega na przerwaniu wykonywania g³ownego programu i uruchomieniu specjalnej funkcji.
	//Po zakonczeniu dzialania tej funkcji nastepuje powrot do przerwanego glownego programu i
	//wznowienie jego dzialania od momentu, w ktorym zostalo ono przerwane. Przerwania dzielimy na
	//wewnetrzne, np. wywolane ukladami licznikowymi i zewnetrzne, np. wywolane wcisnieciem przycisku.
	//Wykorzystuje sie je w celu m. in. oszczedzania energii lub czasu pracy mikrokontrolera czy tez
	//zmniejszenia komplikacji kodu programu.

	TimerInit(); //Inicjalizuje Timer1 do wywolywania przerwania z czêstotliwoœci¹ 2 Hz
	sei(); //Uruchamia globalne przerwania
	sbi(DDRB, PB0); //Ustawia wyprowadzenie jako wyjœcie
	while (1) {
		_delay_ms(500);
	}
	*/

	/*
	//ZADANIE 2

	//Do obs³ugi wyswietlacza 7-segmentowego potrzeba 7 linii mikrokontrolera.

	seg7Init(); //Inicjalizuje porty do obs³ugi wyswietlacza 7 segmentowego
	while (1) {
		for (uint8_t i = 0; i < 10; i++) {
			seg7ShowCyfra(i); //Wyswietla na wyswietlaczu 7-segmentowym cyfre z argumentu
			_delay_ms(500);
		}
	}
	*/


	//ZADANIE 3
	int flag = 0; //Informuje o liczbie wcisniesc przycisku
	int digit = 9; //Przechowuje cyfre do wyswietlenia po zatrzymaniu odliczania
	TimerInit(); //Inicjalizuje Timer1 do wywolywania przerwania z czêstotliwoœci¹ 1 Hz
	seg7Init(); //Inicjalizuje porty do obs³ugi wyswietlacza 7 segmentowego
	sbi(DDRB, PB0); //Ustawia wyprowadzenie jako wyjœcie
	cbi(DDRD, PD0); //Ustawia wyprowadzenie jako wejscie
	sbi(PORTD, PD0); //Ustawia wyprowadzenie jako pull-UP
	while (1) {
		while (flag == 0) { //Dopoki nie wcisnieto przycisku
			if (bit_is_clear(PIND, PD0)) { //Jesli wcisnieto przycisk
				flag++;
				while (bit_is_clear(PIND, PD0)) { //Dopoki przycisk jest wcisniety
				}
			}
		}
		while (flag == 1) { //Dopoki wcisnieto przycisk 1 raz
			i = 10; //Bo odliczamy od cyfry 9, ktora ma indeks 10 w tablicy char cyfry[10]
			sei(); //Uruchamia globalne przerwania
			while (i > 0) {
				seg7ShowCyfra(i); //Wyswietla na wyswietlaczu 7-segmentowym cyfre z argumentu
				if (bit_is_clear(PIND, PD0)) { //Jesli wcisnieto przycisk
					digit = i;
					flag++;
					cli(); //Wylacza globalne przerwania
					while (bit_is_clear(PIND, PD0)) { //Dopoki przycisk jest wcisniety
					}
					goto label;
				}
			}
			i = 5;
			sbi(PORTB, PB0); //Ustawia wyprowadzenie w stan wysoki (wlacza diode)
			while (i > 0) {
				if (bit_is_clear(PIND, PD0)) { //Jesli wcisnieto przycisk
					digit = 0;
					flag++;
					cli(); //Wylacza globalne przerwania
					while (bit_is_clear(PIND, PD0)) { //Dopoki przycisk jest wcisniety
					}
					goto label;
				}
			}
			cbi(PORTB, PB0); //Ustawia wyprowadzenie w stan niski (wylacza diode)
			cli(); //Wylacza globalne przerwania
			flag = 0;
			PORTC = 0x00; //Wylacza wyswietlacz 7-segmentowy
		}
		label:
			while (flag == 2) { //Dopoki wcisnieto przycisk 2 razy
				if (bit_is_clear(PIND, PD0)) { //Jesli wcisnieto przycisk
				flag = 0;
				PORTC = 0x00; //Wylacza wysweitlacz 7-segmentowy
				cbi(PORTB, PB0); //Ustawia wyprowadzenie w stan niski (wylacza diode)
				while (bit_is_clear(PIND, PD0)) { //Dopoki przycisk jest wcisniety
				}
				} else {
					seg7ShowCyfra(digit); //Wyswietla na wyswietlaczu 7-segmentowym cyfre z argumentu
				}
			}
	}


	return 0;
}

/*
//ZADANIE 1
//Funkcja uruchamiana z przerwaniem po przepelnieniu licznika w timer1
ISR (TIMER1_COMPA_vect) {
	tbi(PORTB, PB0); //Ustawia wyprowadzenie w stan przeciwny do aktualnego
}
*/

//ZADANIE 3
ISR (TIMER1_COMPA_vect) {
	i--;
}
