/*
  BiQuadFilter.cpp
  Copyright (c) 2016 Phillip Schmidt.  All right reserved.
  
*/

#include "BiQuadFilter.h"

	

BiQuadFilter::BiQuadFilter(int cutOffFreq, int sampleFreq) // initialize filter
{	
	float Q = 0.7071f;		// should not be larger than 1.0
	float Fc = min(float(cutOffFreq), float((sampleFreq / 2) - 1));  // cutoff freq must not be above half of sample freq
	float Fs = float(sampleFreq);
	
	// generate fixed point coefficients
	float R = tan(3.1415927f * Fc / Fs);
	float Normal = 1.0f / (1.0f + R / Q + R * R);
	
	a0 = int(R * R * Normal * 16384.0f); // offset by 2^14 (max expected coefficient is +/-2.0)
	a1 = 2 * a0;
	a2 = a0;
	
	b1 = int( 2.0f * (R * R - 1.0f) * Normal * 16384.0f);
	b2 = int((1.0f - R / Q + R * R) * Normal * 16384.0f);

}


int BiQuadFilter::in(int value) // data input function
{  
	
	// multiply samples by coefficients -- accumulate data in a Long to reduce rounding errors
	result.L =	mul(value, a0) + 
				mul(IN_1,  a1) +
				mul(IN_2,  a2) -
				mul(OUT_1, b1) -
				mul(OUT_2, b2); 

	result.L = result.L << 2; // bitshift to make up for non 16bit coefficient offset
	
	// shift data
	IN_2  = IN_1;
	IN_1  = value;
	OUT_2 = OUT_1;
	OUT_1 = result.I[1];
	
	return result.I[1]; // the H16 part of the Long is the result
}


int BiQuadFilter::out() // return the value of the most recent result without submitting new data
{
	return result.I[1];
}


// **** Special Integer Multiplication ****
// signed16 * signed16 to signed32
// 22 cycles
// http://mekonik.wordpress.com/2009/03/18/arduino-avr-gcc-multiplication/
#define MultiS16X16to32(longRes, intIn1, intIn2) \
asm volatile ( \
"clr r26 \n\t" \
"mul %A1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"muls %B1, %B2 \n\t" \
"movw %C0, r0 \n\t" \
"mulsu %B2, %A1 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"mulsu %B1, %A2 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (intIn1), \
"a" (intIn2) \
: \
"r26" \
)


// multiplication wrapper function
int32_t inline BiQuadFilter::mul(int16_t a, int16_t b) {
	int32_t r;
	MultiS16X16to32(r, a, b);
	return r;
}