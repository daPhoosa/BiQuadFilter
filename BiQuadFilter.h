/*
  BiQuadFilter.h
  Copyright (c) 2016 Phillip Schmidt.  All right reserved.

 */

#ifndef BiQuadFilter_h

   #define BiQuadFilter_h

   #include "Arduino.h"

   class BiQuadFilter
   {

      public:
         BiQuadFilter(int cutOffFreq, int sampleFreq);
         int in(int value);
         int out();

      private:
         int32_t mul(int16_t a, int16_t b); // integer mltiplication: int16_t * int16_t to int32_t

         int a0, a1, a2, b1, b2;	// coefficients
         int IN_1 = 0, IN_2 = 0, OUT_1 = 0, OUT_2 = 0; // delayed data

         union TypeConverter
         {
            long L;
            int  I[2];
         } result;

   };

#endif
