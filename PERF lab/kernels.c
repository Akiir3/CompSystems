/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/*
 * Please fill in the following student struct:
 */
const team_t team = {
    "erba0445@colorado.edu", //Replace this with your email address.
    ""                   //Replace this with your partner's email address. Leave blank if working alone.
};

/***************
 * FLIP KERNEL
 ***************/

/******************************************************
 * Your different versions of the flip kernel go here
 ******************************************************/
 
/* 
 * naive_flip - The naive baseline version of flip 
 */
char naive_flip_descr[] = "naive_flip: Naive baseline implementation";
void naive_flip(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++){
            dst[RIDX_F(i, j, dim)].red   = src[RIDX(i, j, dim)].red;
            dst[RIDX_F(i, j, dim)].green = src[RIDX(i, j, dim)].green;
            dst[RIDX_F(i, j, dim)].blue  = src[RIDX(i, j, dim)].blue;
        }
    }
}

/* 
 * flip - Your current working version of flip
 * IMPORTANT: This is the version you will be graded on
 */


char flip_descr[] = "flip: Current working version";
void flip(int dim, pixel *src, pixel *dst) 
{
    int i, j, temp1, x;
    
    for (i = 0; i < dim; i++)
    {
        for (j = 0; j < dim; j++)
	{	 
            x = (i*dim) +(dim-1-j);

	    //temp1 = RIDX(i, j, dim);
	    //temp2 = RIDX(i, j, dim);
	    //temp3 = RIDX_F(i, j+1, dim);
	    temp1 = (i*dim) + j;
	    
            dst[x]  = src[temp1];
	    //dst[temp3] = src[x+1];
	   	 
        }
    }
}



char flip_descr3[] = "THIRD - flip: Current working version";
void flip3(int dim, pixel *src, pixel *dst) 
{
	int i, j, temp1, temp2, temp3, temp4;
    for (i = 0; i < dim; i++)
    {
        for (j = 0; j < dim; j+=2)
	{	 
            temp1 = RIDX_F(i, j, dim);
	    temp2 = RIDX(i, j, dim);
	    temp3 = RIDX_F(i, j+1, dim);
	    temp4 = RIDX(i, j+1, dim);

	    dst[temp1].red   = src[temp2].red;
	    dst[temp3].red   = src[temp4].red;
            dst[temp1].green = src[temp2].green;
	    dst[temp3].green = src[temp4].green;
            dst[temp1].blue  = src[temp2].blue;	
	    dst[temp3].blue  = src[temp4].blue; 
        }
    }
}

//to do it up to 16 and do it 15 more times
//n = array[1]

//n = *(arra+1)


/*********************************************************************
 * register_flip_functions - Register all of your different versions
 *     of the flip kernel with the driver by calling the
 *     add_flip_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_flip_functions() 
{
    add_flip_function(&flip, flip_descr);   
    add_flip_function(&naive_flip, naive_flip_descr); 
    //add_flip_function(&flip2, flip_descr2);   
    add_flip_function(&flip3, flip_descr3);
    /* ... Register additional test functions here */
}


/***************
 * CONVOLVE KERNEL
 **************/
 
/***************************************************************
 * Various typedefs and helper functions for the convolve function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute a pixel value */
typedef struct {
    float red;
    float green;
    float blue;
    float weight;
} pixel_sum;

/******************************************************
 * Your different versions of the convolve kernel go here
 ******************************************************/

/*
 * naive_convolve - The naive baseline version of convolve 
 */
char naive_convolve_descr[] = "naive_convolve: Naive baseline implementation";
void naive_convolve(int dim, pixel *src, pixel *dst) 
{
    int i, j, ii, jj, curI, curJ;
    pixel_sum ps;
    
    for (j = 0; j < dim; j++){
        for (i = 0; i < dim; i++){
            ps.red    = 0.0;
            ps.green  = 0.0;
            ps.blue   = 0.0;
            ps.weight = 0.0;
            for (jj = -2; jj <= 2; jj++)
      	    {
                for (ii = -2; ii <= 2; ii++)
                {
                    curJ = j+jj;
                    if(curJ<0 || curJ>=dim){
                        continue;
                    }
                    curI = i+ii;
                    if(curI<0 || curI>=dim){
                        continue;
                    }
                    ps.red   += src[RIDX(curI, curJ, dim)].red *   kernel[ii+2][jj+2];
                    ps.green += src[RIDX(curI, curJ, dim)].green * kernel[ii+2][jj+2];
                    ps.blue  += src[RIDX(curI, curJ, dim)].blue *  kernel[ii+2][jj+2];
                    ps.weight += kernel[ii+2][jj+2];
                }
            }
            dst[RIDX(i,j,dim)].red   = (unsigned short)(ps.red/ps.weight);
            dst[RIDX(i,j,dim)].green = (unsigned short)(ps.green/ps.weight);
            dst[RIDX(i,j,dim)].blue  = (unsigned short)(ps.blue/ps.weight);
        }
    }
}

//use code motion to initialize all the setting to 0 with one instruction
//kernel[ii+2][jj+2] make a variabe with it and replace each equation with it 
//switch to start with i and then the j loop 

/*
 * convolve - Your current working version of convolve. 
 * IMPORTANT: This is the version you will be graded on
 */
char convolve_descr[] = "convolve: Current working version";
void convolve(int dim, pixel *src, pixel *dst) 
{
    int i, j, ii, jj, curI, curJ, temp1;
    pixel_sum ps;
    
    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++){
	    temp1 = (i*dim) + j;
            ps.red    = 0.0;
            ps.green  = 0.0;
            ps.blue   = 0.0;
            ps.weight = 0.0;
            for (jj = -2; jj <= 2; jj++){
 		curJ = j+jj;
	        if(curJ<0 || curJ>=dim){
	                continue;
	            }
		for (ii = -2; ii <= 2; ii++){
		    
		    curI = i+ii;
		    if(curI<0 || curI>=dim){
			continue;
		    }
		    ps.red   += src[RIDX(curI, curJ, dim)].red *   kernel[ii+2][jj+2];
		    ps.green += src[RIDX(curI, curJ, dim)].green * kernel[ii+2][jj+2];
		    ps.blue  += src[RIDX(curI, curJ, dim)].blue *  kernel[ii+2][jj+2];
		    ps.weight += kernel[ii+2][jj+2];
                }
            }
            dst[temp1].red   = (unsigned short)(ps.red/ps.weight);
            dst[temp1].green = (unsigned short)(ps.green/ps.weight);
            dst[temp1].blue  = (unsigned short)(ps.blue/ps.weight);
        }
    }
}

/********************************************************************* 
 * register_convolve_functions - Register all of your different versions
 *     of the convolve kernel with the driver by calling the
 *     add_convolve_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_convolve_functions() {
    add_convolve_function(&convolve, convolve_descr);
    add_convolve_function(&naive_convolve, naive_convolve_descr);
    /* ... Register additional test functions here */
}

