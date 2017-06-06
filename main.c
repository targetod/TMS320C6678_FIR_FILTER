
#include <stdlib.h>
#include <math.h>
#include <ti/dsplib/dsplib.h>

// бібліотека з коефіцієнтами фільтру з МатЛабу
#include "fdacoefs.h"

/* Команда вибору пам*яті для даних */
//#pragma DATA_SECTION(in_s , " .EXT_RAM")


/* Global definitions */
/* Розмір масиву для відліків сигналу */
#define N 256
/* Розмір блоку сигналу для фільтрації */
#define NF 64
/* блоку для фільтрації N/NF  */
#define NB 4

/* вирівнювання даних має бути подвійне слово  4 / або 8 */
#define AL 8

/* масив відліків вхідного сигналу та вихідного */
#pragma DATA_ALIGN(in_signal,AL) // вирівнювання
float   in_signal [N]; // для заповнення сигналом
#pragma DATA_ALIGN(out_signal,AL) // вирівнювання
float   out_signal [N]; // після блочної фільтрації

#pragma DATA_ALIGN(in_s, AL) // вирівнювання по 4 байти - 2 слова
float   in_s [NF+BL-1]; // блок для фільтрації

/* масив відліків блоку вихідного сигналу */
#pragma DATA_ALIGN(out_s,AL)
float   out_s [NF];

/*
    Функція генерує сигнал певної частоти і записує відліки в масив
*/
void generateInput ();

/*
    Функція робить зворотній масив коефіцієнтів
*/
void reverseCoef ();



int main(void) {
	int i;
	/* Generate the input data */
	generateInput();

	/* Call FIR routine
	Ф-ія DSPF_sp_fir_gen виконує фільтрацію по вхідному масиву x з масивом коефіцієнтів h.
	Результат зберігається в вихідному масиві y.

	Прототип функції:
	void DSPF_sp_fir_gen(const float * restrict x, const float * restrict h,
						float * restrict y, int nh, int ny)

	Parameters
			x 	Вказівник на вхідний масив. Масив має мати  ny + nh - 1 елементів.
			h 	Вказівник на масив коефіцієнтів фільтра. Масив має мати nh елементів
			 	 заданих в зворотному порядку: {h(nh - 1), ..., h(1), h(0)}.
			 	 Вирівняні по подвійному слову (double-word aligned).
			y 	Вихідний масив. Має мати ny елементів.
			nh 	К-ть елементів у масиві коефіцієнтів.
				Повинно ділитися на 4 і бути більшим 0.
			ny 	К-ть елементів у вихідному масиві коефіцієнтів.
			    Повинно ділитися на 4 і бути більшим 0.
	 */
	/* Функція обертає масив коефіцієнтів*/
	reverseCoef();

	// реалізація блочної фільтрації
	for (i = 0; i<NB;++i ){
	// заповнюємо блок відліками сигналу на позицію від BL-1. (М-1 по протоколу)
		memcpy(in_s+BL-1, in_signal +i*NF, NF*sizeof(float) );

		// відправляємо блок на фільтрацію
		DSPF_sp_fir_gen(in_s, B, out_s, BL, NF);

		// забираємо дані з фільтрації
		memcpy(out_signal + i*NF, out_s, NF*sizeof(float) );

		// заповнюємо перших М-1 елементів відліками з блоку in_s
		memcpy(in_s, in_s +NF, (BL-1)*sizeof(float) );
	}
	i=0;// breakpoint

	return 0;
}


void generateInput () {
    int   i;
    float FreqSample, sinWaveMag;
    int FreqSignal;

    /* частота  дискретизації Hz*/
    FreqSample = 48000;

    /* Амплітуда сигналу */
    sinWaveMag = 2;

    /* частота  сигналу Hz*/
    FreqSignal = 2000;

    /* генерування сигналу */
	for (i = 0; i < N; i++) {
		in_signal[i]  = sinWaveMag * cos(2*3.14*i * FreqSignal /FreqSample)+
				sinWaveMag * cos(2*3.14*i*  (4*FreqSignal) /FreqSample) ;
	}

}


void reverseCoef () {
    int   i;
    float ftmp;

	for (i = 0; i < BL/2; ++i) {
		ftmp = B[i];
		B[i] = B[BL-i-1];
		B[BL-i-1] = ftmp;
	}

}

