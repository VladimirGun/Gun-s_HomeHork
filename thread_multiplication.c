#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define ARG 2
#define printi(x) printf("%d\n\n", x)

//Оно заработало. Датает ускорение как раз в два раза. 

typedef struct matr
{
	int **vals;
	int cols;
	int rows;
} matrix;

typedef struct
{
	matrix first;
	matrix second;
	matrix answer 	;
	int th_n;
	int pos;
} ord;

matrix create_matrix (int rows, int cols);

matrix trans_matrix (matrix M);

matrix matrix_mult (matrix A, matrix B);

void free_matrix (matrix A);

void show_matrix (matrix M, char *name);

int scal_mult (int *a, int *b, int n);

void *thread_function(void *dP);

int main (int argc, char **argv)
{
	
	matrix A = create_matrix (atoi(argv[1]), atoi(argv[1]));
	matrix B = create_matrix (atoi(argv[1]), atoi(argv[1]));
	int NTHREADS = atoi(argv[2]);
	matrix BT = trans_matrix(B);
	matrix C = create_matrix (A.rows, B.cols);
	matrix AB = create_matrix (A.rows, B.cols);

	



	if (A.cols != B.rows)
	{
		printf ("Matrixes do not fit together\n");
		exit (EXIT_FAILURE);
	}

	printf("Threads: %d\n", NTHREADS);
/***********************************************/
	
	//printf("%lg\n", ((float)clock())/CLOCKS_PER_SEC);

	pthread_t thread_id[NTHREADS];

   ord orders[NTHREADS];
   for(int i=0; i < NTHREADS; i++)
   {
   		orders[i].first  = A; 
   		orders[i].second = BT;
   		orders[i].th_n = NTHREADS;
   		orders[i].pos = i;
   		orders[i].answer = C;
    	pthread_create( &(thread_id[i]), NULL, thread_function, (void*)(orders+i));
   }


   for(int i=0; i < NTHREADS; i++)
   {
      pthread_join( thread_id[i], NULL);
   }
    //AB = matrix_mult(A, B);
	//show_matrix (AB, "A*B");
    //show_matrix(A, "A");
  	//show_matrix(B, "B");
   	//show_matrix(C, "C");
	
/***************************************************************/


	free_matrix(A);
	free_matrix(B);
	free_matrix(C);
	free_matrix(AB);
	
	return 0;
}


void *thread_function(void *prms)
{
	ord order = *((ord*) prms);

	for (int i = order.pos; i < order.first.rows; i += order.th_n)
		for (int j = 0; j < order.second.rows; j++) 
		{						
			int s = scal_mult(order.first.vals[i], order.second.vals[j], order.first.cols);
			(order.answer).vals[i][j] = s;
		}
}


void free_matrix (matrix A)
{
	for (int i = 0; i < A.rows; i++)
		free(A.vals[i]);
	free(A.vals);
}

matrix matrix_mult (matrix A, matrix B)
{	
	matrix C = create_matrix(A.rows, B.cols);

	for (int i = 0; i < C.rows; i++)
		for (int j = 0; j < C.cols; j++)
			C.vals[i][j] = scal_mult(A.vals[i], trans_matrix(B).vals[j], A.cols);
	return C;
}


matrix create_matrix (int rows, int cols)
{
	matrix M;
    M.vals = (int**) malloc(rows*sizeof(int*));

    for(int i = 0; i < rows; i++)
        M.vals[i] = (int*) malloc(cols*sizeof(int));

    for (int i = 0; i < rows; i++) 
    	for (int j = 0; j < cols; j++)
    		M.vals[i][j] = rand() % 10;

    M.rows = rows;
    M.cols = cols;
    return M;
}

void show_matrix (matrix M, char *name)
{
	printf("%s matrix\n", name);

	for (int i = 0; i < M.rows; i++)
	{
		for (int j = 0; j < M.cols; j++)
			printf("%d\t", M.vals[i][j]);
		printf("\n");
	}
	printf("\n");
}

int scal_mult (int *a, int *b, int n)
{
	int s = 0;
	for (int i = 0; i < n; i++)
		s += a[i]*b[i];

	return s;
}

matrix trans_matrix (matrix M)
{
	matrix A = create_matrix(M.cols, M.rows);

	for (int i = 0; i < A.rows; i++)
		for (int j = 0; j < A.cols; j++)
			A.vals[i][j] = M.vals[j][i];
	return A;
}
