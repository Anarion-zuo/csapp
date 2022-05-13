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
#include <stdlib.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

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
    // printf("A 0x%p B 0x%p\n", A, B);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

void trans_part_row(int M, int N, int A[N][M], int B[M][N], int rowBegin, int rowEnd, int colBegin, int colEnd) {
    int i, j, tmp;
    for (i = rowBegin; i < rowEnd; i++) {
        for (j = colBegin; j < colEnd; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

void trans_part_col(int M, int N, int A[N][M], int B[M][N], int rowBegin, int rowEnd, int colBegin, int colEnd) {
    int i, j, tmp;
    for (j = colBegin; j < colEnd; j++) {
        for (i = rowBegin; i < rowEnd; i++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

void blocked_divn(int n, int M, int N, int A[N][M], int B[M][N]) {
    int md = M / n, nd = N / n;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            trans_part_row(M, N, A, B, i * md, (i + 1) * md, j * nd, (j + 1) * nd);
        }
        trans_part_row(M, N, A, B, i * md, (i + 1) * md, n * nd, N);
    }
    for (int j = 0; j < n; ++j) {
        trans_part_row(M, N, A, B, n * md, N, j * nd, (j + 1) * nd);
    }
    trans_part_row(M, N, A, B, n * md, N, n * nd, M);
}

void trans_32_32(int A[32][32], int B[32][32]) {
    for (int i = 0; i < 32; i += 8) {
        for (int j = 0; j < 32; j += 8) {
            for (int k = i; k < i + 8; ++k) {
                int a0 = A[k][j + 0];
                int a1 = A[k][j + 1];
                int a2 = A[k][j + 2];
                int a3 = A[k][j + 3];
                int a4 = A[k][j + 4];
                int a5 = A[k][j + 5];
                int a6 = A[k][j + 6];
                int a7 = A[k][j + 7];
                B[j + 0][k] = a0;
                B[j + 1][k] = a1;
                B[j + 2][k] = a2;
                B[j + 3][k] = a3;
                B[j + 4][k] = a4;
                B[j + 5][k] = a5;
                B[j + 6][k] = a6;
                B[j + 7][k] = a7;
            }
        }
    }
}

void trans_64_64(int A[64][64], int B[64][64]) {
    int a0, a1, a2, a3, a4, a5, a6, a7;
    /*
    for (int i = 0; i < 64; i += 8) {
        for (int j = 0; j < 64; j += 8) {
            for (int k = 0; k < 4; ++k) {
                a0 = A[k + i][j + 0];
                a1 = A[k + i][j + 1];
                a2 = A[k + i][j + 2];
                a3 = A[k + i][j + 3];
                
                a4 = A[k + i][j + 4];
                a5 = A[k + i][j + 5];
                a6 = A[k + i][j + 6];
                a7 = A[k + i][j + 7];

                B[j + 0][k + i] = a0;
                B[j + 1][k + i] = a1;
                B[j + 2][k + i] = a2;
                B[j + 3][k + i] = a3;

                B[j + 0][k + 4 + i] = a4;
                B[j + 1][k + 4 + i] = a5;
                B[j + 2][k + 4 + i] = a6;
                B[j + 3][k + 4 + i] = a7;
            }
            for (int k = 0; k < 4; ++k) {
                a0 = A[i + 4][k + j];
                a1 = A[i + 5][k + j];
                a2 = A[i + 6][k + j];
                a3 = A[i + 7][k + j];

                a4 = B[i + 4][k + 4 + j];
                a5 = B[i + 5][k + 4 + j];
                a6 = B[i + 6][k + 4 + j];
                a7 = B[i + 7][k + 4 + j];

                int t;

                t = B[k + j][i + 4];
                B[k + j][i + 4] = a0;
                a0 = t;
                t = B[k + j][i + 5];
                B[k + j][i + 5] = a1;
                a1 = t;
                t = B[k + j][i + 6];
                B[k + j][i + 6] = a2;
                a2 = t;
                t = B[k + j][i + 7];
                B[k + j][i + 7] = a3;
                a3 = t;

                B[k + 4 + j][i + 4] = a4;
                B[k + 4 + j][i + 5] = a5;
                B[k + 4 + j][i + 6] = a6;
                B[k + 4 + j][i + 7] = a7;

                B[k + 4 + j][i + 0] = a0;
                B[k + 4 + j][i + 1] = a1;
                B[k + 4 + j][i + 2] = a2;
                B[k + 4 + j][i + 3] = a3;
            }
        }
    }
    */
   int i, j, k, tmp, M = 64, N = 64, block_size = 8;
   for (i = 0; i < N; i += block_size) {
        for (j = 0; j < M; j += block_size) {
            for (k = 0; k < block_size / 2; k++) {
                // A top left
                a0 = A[k + i][j];
                a1 = A[k + i][j + 1];
                a2 = A[k + i][j + 2];
                a3 = A[k + i][j + 3];

                // copy
                // A top right
                a4 = A[k + i][j + 4];
                a5 = A[k + i][j + 5];
                a6 = A[k + i][j + 6];
                a7 = A[k + i][j + 7];

                // B top left
                B[j][k + i] = a0;
                B[j + 1][k + i] = a1;
                B[j + 2][k + i] = a2;
                B[j + 3][k + i] = a3;

                // copy
                // B top right
                B[j + 0][k + 4 + i] = a4;
                B[j + 1][k + 4 + i] = a5;
                B[j + 2][k + 4 + i] = a6;
                B[j + 3][k + 4 + i] = a7;
            }
            for (k = 0; k < block_size / 2; k++) {
                // step 1
                a0 = A[i + 4][j + k]; a4 = A[i + 4][j + k + 4];
                a1 = A[i + 5][j + k]; a5 = A[i + 5][j + k + 4];
                a2 = A[i + 6][j + k]; a6 = A[i + 6][j + k + 4];
                a3 = A[i + 7][j + k]; a7 = A[i + 7][j + k + 4];
                // step 2
                tmp = B[j + k][i + 4]; B[j + k][i + 4] = a0; a0 = tmp;
                tmp = B[j + k][i + 5]; B[j + k][i + 5] = a1; a1 = tmp;
                tmp = B[j + k][i + 6]; B[j + k][i + 6] = a2; a2 = tmp;
                tmp = B[j + k][i + 7]; B[j + k][i + 7] = a3; a3 = tmp;
                // step 3
                B[j + k + 4][i + 0] = a0; B[j + k + 4][i + 4 + 0] = a4;
                B[j + k + 4][i + 1] = a1; B[j + k + 4][i + 4 + 1] = a5;
                B[j + k + 4][i + 2] = a2; B[j + k + 4][i + 4 + 2] = a6;
                B[j + k + 4][i + 3] = a3; B[j + k + 4][i + 4 + 3] = a7;
            }
        }
    }
}

#define min(x, y) \
((x) < (y) ? (x) : (y))

void trans_61_67(int A[61][67], int B[67][61]) {
    // blocked_divn(10, 61, 67, A, B);
    for (int i = 0; i < 61; i += 8) {
        int j;
        for (j = 0; j < 67; j += 8) {
            if (i + 8 <= 61 && j + 8 <= 67) {
                for (int k = j; k < j + 8; ++k) {
                    int a0 = A[i + 0][k];
                    int a1 = A[i + 1][k];
                    int a2 = A[i + 2][k];
                    int a3 = A[i + 3][k];
                    int a4 = A[i + 4][k];
                    int a5 = A[i + 5][k];
                    int a6 = A[i + 6][k];
                    int a7 = A[i + 7][k];
                    B[k][i + 0] = a0;
                    B[k][i + 1] = a1;
                    B[k][i + 2] = a2;
                    B[k][i + 3] = a3;
                    B[k][i + 4] = a4;
                    B[k][i + 5] = a5;
                    B[k][i + 6] = a6;
                    B[k][i + 7] = a7;
                }
            } else {
                int im = 61;
                if (im > i + 8) {
                    im = i + 8;
                }
                int jm = 67;
                if (jm > j + 8) {
                    jm = j + 8;
                }
                for (int ii = i; ii < im; ++ii) {
                    for (int jj = j; jj < jm; ++jj) {
                        B[jj][ii] = A[ii][jj];
                    }
                }
            }
            
        }
    }/*
    int N = 61, M = 67;
    int i, j, s, k;
    int a0, a1, a2, a3, a4, a5, a6, a7;
    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 23) {
            if (i + 8 <= N && j + 23 <= M) {
                for (s = j; s < j + 23; s++) {
                    a0 = A[i][s];
                    a1 = A[i + 1][s];
                    a2 = A[i + 2][s];
                    a3 = A[i + 3][s];
                    a4 = A[i + 4][s];
                    a5 = A[i + 5][s];
                    a6 = A[i + 6][s];
                    a7 = A[i + 7][s];
                    B[s][i + 0] = a0;
                    B[s][i + 1] = a1;
                    B[s][i + 2] = a2;
                    B[s][i + 3] = a3;
                    B[s][i + 4] = a4;
                    B[s][i + 5] = a5;
                    B[s][i + 6] = a6;
                    B[s][i + 7] = a7;
                }
            } else {
                for (k = i; k < min(i + 8, N); k++) {
                    for (s = j; s < min(j + 23, M); s++) {
                        B[s][k] = A[k][s];
                    }
                }
            }
        }
    }*/
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
    if (M == 32 && N == 32) {
        trans_32_32(A, B);
    } else if (M == 64 && N == 64) {
        trans_64_64(A, B);
    } else if (M == 67 && N == 61) {
        trans_61_67(A, B);
    } else {
        printf("unexpected input...\n");
        exit(1);
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
    // registerTransFunction(trans, trans_desc); 
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

