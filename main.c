
#include <stdlib.h>
#include <math.h>
#include <ti/dsplib/dsplib.h>

// �������� � ������������� ������� � �������
#include "fdacoefs.h"

/* ������� ������ ���*�� ��� ����� */
//#pragma DATA_SECTION(in_s , " .EXT_RAM")


/* Global definitions */
/* ����� ������ ��� ����� ������� */
#define N 256
/* ����� ����� ������� ��� ���������� */
#define NF 64
/* ����� ��� ���������� N/NF  */
#define NB 4

/* ����������� ����� �� ���� ������� �����  4 / ��� 8 */
#define AL 8

/* ����� ����� �������� ������� �� ��������� */
#pragma DATA_ALIGN(in_signal,AL) // �����������
float   in_signal [N]; // ��� ���������� ��������
#pragma DATA_ALIGN(out_signal,AL) // �����������
float   out_signal [N]; // ���� ������ ����������

#pragma DATA_ALIGN(in_s, AL) // ����������� �� 4 ����� - 2 �����
float   in_s [NF+BL-1]; // ���� ��� ����������

/* ����� ����� ����� ��������� ������� */
#pragma DATA_ALIGN(out_s,AL)
float   out_s [NF];

/*
    ������� ������ ������ ����� ������� � ������ ����� � �����
*/
void generateInput ();

/*
    ������� ������ �������� ����� �����������
*/
void reverseCoef ();



int main(void) {
	int i;
	/* Generate the input data */
	generateInput();

	/* Call FIR routine
	�-�� DSPF_sp_fir_gen ������ ���������� �� �������� ������ x � ������� ����������� h.
	��������� ���������� � ��������� ����� y.

	�������� �������:
	void DSPF_sp_fir_gen(const float * restrict x, const float * restrict h,
						float * restrict y, int nh, int ny)

	Parameters
			x 	�������� �� ������� �����. ����� �� ����  ny + nh - 1 ��������.
			h 	�������� �� ����� ����������� �������. ����� �� ���� nh ��������
			 	 ������� � ���������� �������: {h(nh - 1), ..., h(1), h(0)}.
			 	 ������� �� ��������� ����� (double-word aligned).
			y 	�������� �����. �� ���� ny ��������.
			nh 	�-�� �������� � ����� �����������.
				������� ������� �� 4 � ���� ������ 0.
			ny 	�-�� �������� � ��������� ����� �����������.
			    ������� ������� �� 4 � ���� ������ 0.
	 */
	/* ������� ������ ����� �����������*/
	reverseCoef();

	// ��������� ������ ����������
	for (i = 0; i<NB;++i ){
	// ���������� ���� ������� ������� �� ������� �� BL-1. (�-1 �� ���������)
		memcpy(in_s+BL-1, in_signal +i*NF, NF*sizeof(float) );

		// ����������� ���� �� ����������
		DSPF_sp_fir_gen(in_s, B, out_s, BL, NF);

		// �������� ��� � ����������
		memcpy(out_signal + i*NF, out_s, NF*sizeof(float) );

		// ���������� ������ �-1 �������� ������� � ����� in_s
		memcpy(in_s, in_s +NF, (BL-1)*sizeof(float) );
	}
	i=0;// breakpoint

	return 0;
}


void generateInput () {
    int   i;
    float FreqSample, sinWaveMag;
    int FreqSignal;

    /* �������  ������������� Hz*/
    FreqSample = 48000;

    /* �������� ������� */
    sinWaveMag = 2;

    /* �������  ������� Hz*/
    FreqSignal = 2000;

    /* ����������� ������� */
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

