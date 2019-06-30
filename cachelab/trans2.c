#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

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
  

int sz, temp = 0, d = 0;

  
    if (N == (1<<5))
    {
        sz = 8;
        for(int col = 0; col < N; col += 8)
            for(int row = 0; row < N; row += 8)
                for(int r = row; r < row + 8; r++)
                    {
                        for(int c = col; c < col + 8; c++)
                        {
                            if(r != c)
                                B[c][r] = A[r][c];
                                                
                            else{
                                temp = A[r][c];
                                d = r;
                            }
                        }

                    if (row == col) 
                        B[d][d] = temp;
                    
                    }
            
        
    }

    else if (N == (1<<6))
    {   
        sz = (1<<2);
        for(int r = 0; r < N; r += sz)
        {
            for(int c = 0; c < M; c += sz)
            {
                int cur = A[r][c];
                int cur1 = A[r+1][c];
                int cur2 = A[r+2][c];
                int cur3 = A[r+2][c+1];
                int cur4 = A[r+2][c+2];
   
                B[c+3][r] = A[r][c+3];
                B[c+3][r+1] = A[r+1][c+3];
                B[c+3][r+2] = A[r+2][c+3];
   
                B[c+2][r] = A[r][c+2];
                B[c+2][r+1] = A[r+1][c+2];
                B[c+2][r+2] = cur4;
                cur4 = A[r+1][c+1];
   
                B[c+1][r] = A[r][c+1];
                B[c+1][r+1] = cur4;
                B[c+1][r+2] = cur3;
          
                B[c][r] = cur;
                B[c][r+1] = cur1;
                B[c][r+2] = cur2;
          
                B[c][r+3] = A[r+3][c];
                B[c+1][r+3] = A[r+3][c+1];
                B[c+2][r+3] = A[r+3][c+2];
                cur = A[r+3][c+3];
          
                B[c+3][r+3] = cur;
            }
        }
    }

    else 
    {
        sz = (1<<4);
        
        for (int col = 0; col < M; col += sz)
        {
            for (int row = 0; row < N; row += sz)
            {   

                for(int r = row; (r < N) && (r < row + sz); r++)
                {
                    for(int c = col; (c < M) && (c < col + sz); c++)
                    {

                        if (r != c)
                            B[c][r] = A[r][c];
                        

                        else{
                            temp = A[r][c];
                            d = r;
                        }
                    }
                    
 
                    if(row == col) 
                        B[d][d] = temp;
                    
                }
            }
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
    //Used only 1 function for all cases

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