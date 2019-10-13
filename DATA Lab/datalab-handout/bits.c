/* 
 * CS:APP Data Lab 
 * 
 * <Erika I. Bailon 108584251 erba0445>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 8.0.0.  Version 8.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2014, plus Amendment 1 (published
   2015-05-15).  */
/* We do not support C11 <threads.h>.  */


/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int evenBits(void) 
{
	int first = 0x55 <<24;	//to construct the first byte with 1's on the even positions(even-numbered)
	int second = 0x55 << 16;  //to construct the second byte with 1's on the even positions(even-numbered)
	int third = 0x55 << 8;	//to construct the third byte with 1's on the even positions(even-numbered)
	int last = 0x55;		//to construct the last byte with 1's on the even positions(even-numbered)
	
  return first|second|third|last;	//it will return all the even-numbered bits set to one because of the "or" operator comparing all of the bytes
}
/* 
 * minusOne - return a value of -1 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int minusOne(void) 
{
  int zero = 0x00; //from class we learn that -1 is all ones in the 4bytes so we just needed to return the oposite of 0 so they all could be 1
  return ~zero;
}
/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x) 
{
   x = x <<31;	//first you grab the LSB and get rid of everything on the left and add zeros to the right
   x = x >> 31; //then you grab the MSB (that was previously the LSB) and then you get rif of everything on the righ and copy the MSB on every bit
  return x;  
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) 
{
   int theSign = x >>31;	//first we force x to have the sign we need to do the arithmetic operation  
   int maskChange = ((theSign &1) << n) + theSign;	//from lecture with Miller we know that this is the 2's complement  
   int arithAnswer = (x + maskChange) >> n;	//When we shift to the right arithmetically we are dividing by two continuously and and it drops  
										// the decimals because we are working with integers
   return arithAnswer;   
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) 
{
	int convertValue = n <<3;		//we are going to use the int value instead of the representation to cut the bytes
	int shiftx = x>>convertValue;	//we cut the number of bytes we obtain with covertValue
	int theMask = 0xFF;				//we make a mask to obtain only the value of the bytes we want 
	int theResult = shiftx & theMask;		//when we compare we will only get 1's where we have the values of the bytes we want 
	return theResult; 
}
/* 
 * anyOddBit - return 1 if any odd-numbered bit in word set to 1
 *   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyOddBit(int x) 
{
   int theMask = 0xAA <<26;		//all this 4 masks are to create the master mask
   int theMask2 = 0xAA << 16;
   int theMask3 = 0xAA <<8;
   int lastMask = 0xAA;
   int ultimateMask = theMask|theMask2|theMask3|lastMask; //you can have a mask that has a 1 in the odd bits (101010101010101010101...)
   int theResult = x & ultimateMask; 		//to see if we have 1's on the odds with and & comparasion 
   return !!(theResult);  // !! returns True where there is a one (is like double negation because one ! returns true for 0)
}

/* 
 * isNegative - return 1 if x < 0, return 0 otherwise 
 *   Example: isNegative(-1) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNegative(int x) 
{
    x = x >> 31;		 
    return !!(x); //you shift the first value to the LSB to check what it is
}
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) 
{
	int assignedS = 1 << 31; //this will help with the addition
	int thirtyN = ~(assignedS|0x39);		//this will help set the bound for the 39 as suggested by my TA, to see where our x is
	int thirty = ~(0x30);					// also help set the lower bound which is 30
	
	//gonna check if our x is inse of the bounds, we need both of them because we cannot make conclusions
	// with jus tone bound
	int thNineBound = assignedS & (thirtyN +x) >>31;	
	int thZeroBound = assignedS &(thirty +1+x)>>31;
	
 return !(thNineBound | thZeroBound); 	//we use an or because we are comparing the two parameters
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int fitsBits(int x, int n) 
{
  int shifter = n + ~0;
  int isNegOrPos = x>> 31;
  x = x ^ isNegOrPos;
  x = x >> shifter;
  
  return !x; //TA-define negative and positive and find out how many bytes you need for an integer to be positive or negative 
		
}
/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) 
{
	int doSubst = ((x+1) + ~y);
	int sameSignCheck = x ^ y;
	int overFlowCheck = ~(doSubst ^ y);
	int result = ~(sameSignCheck & overFlowCheck);
	
  return (1&(result >> 31)); //overflow is when you put a l0t of bits where you don't need them
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) 
{
  int booleanX = ~(!!x) + 1;		
  int checkWithY = booleanX & y;
  int checkWithZ = ~booleanX & z;
  int result = checkWithY + checkWithZ;
  
  return result; // TA -if x is not 0 you return y .. make a variable for yourself that if you use and operation  . it'll tell you if x is 0 or one 
		// you need to check the equality of x to 0  
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) //couldn't figure it out, even with help of TA
{
return 0;
}
/* 
 * isNonZero - Check whether x is nonzero using
 *              the legal operators except !
 *   Examples: isNonZero(3) = 1, isNonZero(0) = 0
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4 
 */
int isNonZero(int x) 
{
  int n = ~x;	//we create the oposite of x 
  n = n+1;		//we add one because if x is 0, this will all be 0's, if is not 0 then it will have one's
  n = n >>31;	//we grab the MSB and duplicate it
  x = (x >> 31);	//same
  
  return (x | n) & 1; 	//we use and because it will only give us 1 if we have a 1 after the or.
  // TA -you can use the mask to find out if x is zero or not 
}
/* 
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) 
{
  int checkSign = x>>31;
  int mask = 1+ ~checkSign;
  x = x ^ checkSign;
  int element = x+mask;
  return element;
}
/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int isPower2(int x) //page 106
{
  int ones = ~0;
  x = ~x +1;
  int ifItIs = x ^ ones; 
  return !ifItIs;	//check the number of ones
  
}
