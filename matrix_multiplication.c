#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

//global variables

int nRows1=0;  //number of rows of first matrix
int nCols1=0;  //number of columns of first matrix
int matrix1[200][200];  //2d array for first matrix
int nRows2=0;
int nCols2=0;
int matrix2[200][200];

int result1[200][200];//result using method 1
int result2[200][200];//result using method 2

double time_spent1=0;
double time_spent2=0;

FILE *fp2;

typedef struct a
{
    int row;
    int column;
} a;

void *multiplyByRow(void * arg)
{
    int p=(int)arg; //get passed argument

    for(int j=0; j<nCols1; j++)
    {
        for(int k=0; k<nRows2; k++)
        {
            result1[p][j]+=matrix1[p][k]*matrix2[k][j];
        }
    }
}

void* multiplyByElement(void*arg)
{
    a *ind=(a*)arg;

    for(int k=0; k<nRows2; k++)
    {
        result2[ind->row][ind->column]+=matrix1[ind->row][k]*matrix2[k][ind->column];
    }

    free(arg);
}

void readFromFile(char* fileName)
{
    FILE *fp=fopen(fileName,"r");

    if(fp==NULL)//file not found
    {
        printf("Error, file not found\n");
        exit(1);
    }

    fscanf(fp,"%d",&nRows1);//get first no. in first line = no. of rows

    fscanf(fp,"%d",&nCols1);//get second no. in first line = no. of columns


    //read first input matrix from file
    for(int i=0; i<nRows1; i++)
    {
        for(int j=0; j<nCols1; j++)
        {
            fscanf(fp,"%d",&matrix1[i][j]);
        }
    }

    fscanf(fp,"%d",&nRows2);

    fscanf(fp,"%d",&nCols2);

    if(nCols1!=nRows2)//check if first matrix columns and second matrix rows are not equal
    {
        fp=fopen("outputMatrix.txt","w");
        fprintf(fp,"Invalid matrix dimensions!!");
        fclose(fp);
        exit(1);
    }

    //read second input matrix from file

    for(int i=0; i<nRows2; i++)
    {
        for(int j=0; j<nCols2; j++)
        {
            fscanf(fp,"%d",&matrix2[i][j]);
        }
    }

    fclose(fp);
}

void createThreadPerRow()
{
    int numOfThreads1=nRows1;//no. of threads = no. of output rows

    pthread_t th1[numOfThreads1];//create array of threads

    for (int i=0; i<numOfThreads1; i++)
        pthread_create(&th1[i],NULL,&multiplyByRow,(void*)i);//pass i as function argument for every thread

    for (int i=0; i<numOfThreads1; i++)
        pthread_join(th1[i],NULL);//wait for all threads to terminate
}

void createThreadPerElement()
{
    int numOfThreads2=nRows1*nCols2;  //no. of threads = no. of output elements

    pthread_t th2[numOfThreads2];

    int count=0;

    for(int i=0; i<nRows1; i++)
    {
        for(int j=0; j<nCols2; j++)
        {
            a* index=malloc(sizeof(a));//dynamically allocate space for struct pointer

            index->row=i;

            index->column=j;

            pthread_create(&th2[count++],NULL,multiplyByElement,(void *)index);//create element threads
        }
    }

    for(int i=0; i<numOfThreads2; i++)
    {
        pthread_join(th2[i],NULL);//wait for all threads to terminate
    }
}

void outputResults1()
{
    fp2=fopen("outputMatrix.txt","w");

    fprintf(fp2,"By Row\n");

    for(int i=0; i<nRows1; i++)
    {
        for(int j=0; j<nCols2; j++)
        {
            fprintf(fp2,"%d       ",result1[i][j]);
        }
        fprintf(fp2,"\n");
    }

    fprintf(fp2,"END1       %f\n\n",time_spent1);

    fclose(fp2);
}

void outputResults2()
{
    fp2=fopen("outputMatrix.txt","a");

    fprintf(fp2,"By Element\n");

    for(int i=0; i<nRows1; i++)
    {
        for(int j=0; j<nCols2; j++)
        {
            fprintf(fp2,"%d       ",result2[i][j]);
        }
        fprintf(fp2,"\n");
    }

    fprintf(fp2,"END2       %f\n",time_spent2);

    fclose(fp2);
}



int main(int argc,char* argv[])
{

    readFromFile(argv[1]); //read the two matrices from input file


    clock_t start = clock();

//case of row by row (thread for every row)

    createThreadPerRow();

    clock_t stop = clock();

    time_spent1 = (double)(stop - start) / CLOCKS_PER_SEC; //time in seconds


///-----------------------------------------------------------------------------

    start = clock();

//case of element by element
    createThreadPerElement();

    stop = clock();

    time_spent2 = (double)(stop - start) / CLOCKS_PER_SEC;

///------------------------------------------------------------------------------

//write results to file using both methods

    outputResults1();
    outputResults2();

    return 0;
}
