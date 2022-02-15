#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

int array_size=0;
int array[50];

typedef struct y
{
    int l;
    int r;
} y;



void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1; //number of elements in left half
    int n2 = r - m; //number of elements in right half

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + j+ 1];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}


void* mergeSort(void* arg)
{
    y * subarr=(y *)arg; //retrieve passed arguments (struct) again
    if (subarr->l < subarr->r)//if no. of elements > 1
    {
        int mid = (subarr->l + subarr->r - 1) / 2; //get midpoint of array indices

        //new left subarray = 1st half of array
        y* lArr=malloc(sizeof(y));
        lArr->l=subarr->l;
        lArr->r=mid;

        //new right subarray = 2nd half of array
        y* rArr=malloc(sizeof(y));
        rArr->l=mid+1;
        rArr->r=subarr->r;


        pthread_t th_l,th_r;  //new left and right threads

        //recursive mergeSort calls
        pthread_create(&th_l, NULL, &mergeSort, (void*)lArr);//create left thread and pass left subarray
        pthread_create(&th_r, NULL, &mergeSort, (void*)rArr);//create right thread and pass right subarray

        //CREATING THE NEW THREADS WITH THEIR RESPECTIVE NODES

        pthread_join(th_l, NULL);
        pthread_join(th_r, NULL);


        merge(array, subarr->l, mid, subarr->r);
    }
}


void outputArray(int A[])
{
    FILE* fp2=fopen("sortOutput.txt","w");

    fprintf(fp2,"Sorted array: ");

    for (int i = 0; i < array_size; i++)
        fprintf(fp2,"%d ", A[i]);

    fclose(fp2);
}


void readFromFile(char* fileName)
{
    FILE* fp1=fopen(fileName,"r");
    if(fp1==NULL)//file not found
    {
        printf("Error, file not found\n");
        exit(1);
    }

    fscanf(fp1,"%d",&array_size);


    for(int j=0; j<array_size; j++)
    {
        fscanf(fp1,"%d",&array[j]);
    }


    fclose(fp1);
}



int main(int argc,char* argv[])
{
    readFromFile(argv[1]);  //read data from input file

    y* q=malloc(sizeof(struct y)); //dynamically allocate space for mergeSort function argument

    //pass the whole array first
    q->l=0;
    q->r=array_size-1;


    pthread_t th;

    pthread_create(&th,NULL,&mergeSort,(void*)q);//create one thread initially and pass the input array

    pthread_join(th,NULL);//wait for thread to terminate

    outputArray(array);

    return 0;
}
