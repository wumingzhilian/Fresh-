/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

char transpose_1[] = "Transpose 1";
void trans_1(int M, int N, int A[N][M], int B[M][N])
{
	int ii, jj, i, val1, val2, val3, val4, val5, val6, val7, val0;
	for(jj = 0; jj < 32; jj += 8)
		for(ii = 0; ii < 32; ii += 8)
		{
			for(i = ii; i < ii + 8; i++)
			{
				val0 = A[i][jj];
				val1 = A[i][jj + 1];
				val2 = A[i][jj + 2];
				val3 = A[i][jj + 3];
				val4 = A[i][jj + 4];
				val5 = A[i][jj + 5];
				val6 = A[i][jj + 6];
				val7 = A[i][jj + 7];
				B[jj][i] = val0;
				B[jj + 1][i] = val1;
				B[jj + 2][i] = val2;
				B[jj + 3][i] = val3;
				B[jj + 4][i] = val4;
				B[jj + 5][i] = val5;
				B[jj + 6][i] = val6;
				B[jj + 7][i] = val7;
			}
		}
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    return trans_1(32,32,A,B);
}



/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
//    registerTransFunction(trans, trans_desc);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

