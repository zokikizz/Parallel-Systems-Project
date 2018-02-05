#include <iostream>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

using namespace std;


void printArray(int *array, int start, int end);
void initArray(int *array, int length);
double averageTime(double *time, int length);
void check(int*array, int length);
//seqential
void devide(int *array, int start, int end);
void merge(int *array, int start,int middle, int end);
// parallel
void ParallelDevide(int *array, int number_of_threads, int length);



int main() 
{
    //array is used for parallel alg.
    int *array;
    //arraySeq is used for seqential alg.
    int* arraySeq;
    
    double seqTime;
    double parTime;
    
    //length of array
    int length = 1024;
    
    int number_of_tests = 100;
    //matix for saving execution time for every test (r[0][] - is for seq. and r[1][] is for parallel)
    double res[2][number_of_tests];

    int numberRes = 1;
    while( length <= 2000000)
    {	
        cout << endl << "Start with length: " << length << " To run " << number_of_tests << " tests." << endl;
	
        for(int counter = 0; counter < number_of_tests; counter++)
        {
            
            // allocating memory and init of array
            array = new int[length];
            initArray(array, length);
            
            
            //allocating memory and copying from array to arraySeq
            arraySeq = new int[length];
            for (int j = 0; j<length; j++) 
                arraySeq[j] = array[j];
            
           
            //seq. execution
            seqTime = omp_get_wtime();
            
            devide(arraySeq, 0, length);
            res[0][counter] = omp_get_wtime() - seqTime;
            
         
            //parallel execution
            parTime = omp_get_wtime();
            ParallelDevide(array, 4, length);
            res[1][counter] =  omp_get_wtime() - parTime;
    
            delete []array;
            delete []arraySeq;
        }
        
        //calculate avg. execution time for seqential alg.
        double seqExTime = averageTime(res[0], number_of_tests );
        
        //calculate avg. execution time for parallel alg.
        double parExTime = averageTime(res[1], number_of_tests );
        
        
        cout << numberRes++ <<". Speedup: "<< seqExTime / parExTime;
        
        //change length of array
        length*=2;
        
   }

   cout << endl;
   
   
}



///---Functions:----///

// print array 
void printArray(int *array, int start, int end)
{
    for(int i = start; i < end; i++)
    { 
        cout << array[i] << " " << std::flush;
    }
    cout << endl << std::flush;
}

//init
void initArray(int *array, int length)
{
    srand(time(NULL));   
    for(int i = 0; i < length; i++)
    {
        //a[i] = (n-1) - i;
        array[i] = rand() % 50;
    }
}

//merge right and left
void merge(int *array, int start,int middle, int end)
{
    int temp[end-start];
    int counterLeft = start, counterRight = middle, counterTotal = 0;
    while( counterLeft < middle && counterRight < end)
    {
        if(array[counterLeft] < array[counterRight])
            temp[counterTotal++] = array[counterLeft++];
        else
            temp[counterTotal++] = array[counterRight++];
    }
    
    while( counterLeft < middle)
    {
        temp[counterTotal++] = array[counterLeft++];
    }
    
    while( counterRight < end)
    {
        temp[counterTotal++] = array[counterRight++];
    }
    
    //copying result
    for(int i = 0; i < (end-start); i++)
        array[start+i] = temp[i];
    
    
}

//to devide array 
void devide(int *array, int start, int end)
{
    if( end - start == 1 )
    {
        return;
    }
    else 
    {
        int middle = (start+end)/2;
        devide(array, start, middle);
        devide(array, middle, end);
        merge(array, start, middle, end);

    }
}

//parallel
void ParallelDevide(int *array, int number_of_threads, int length)
{
    
    //devide array on number_of_threads parts 
    int count = length / number_of_threads;
    
    //and for every part do seqential alg.
    #pragma omp parallel for num_threads(number_of_threads)
    for(int i = 0; i < number_of_threads; i++)
    {
        devide(array,i*count,(i+1)*count);
    }
    //after this we have number_of_threads parts sorted and now we need to merge them
    
    
    //just merging, we again devide array on parts but now parts are bigger then in for loop
    number_of_threads = number_of_threads/2;
    
    while(number_of_threads)
    {
        count = length / number_of_threads;
        #pragma omp parallel for num_threads(number_of_threads) 
        for(int i = 0; i < number_of_threads; i++)
        {
            int start = omp_get_thread_num()*count;
            int end = (omp_get_thread_num()+1)*count;
            int middle = (start+end)/2;
            merge(array, start, middle, end);
        }
        number_of_threads = number_of_threads/2;
    }
}

//checking array
void check(int*array, int length)
{
    int result = 1;
    for(int i = 0; i < length-1; i++)
        if(array[i] > array[i+1])
                result = 0;

    if(result)
        cout << "OK." << endl;
    else
        cout << "Error." <<endl;
}

//average of array
double averageTime(double *time, int length)
{
    int i = 0;
    double sum = 0.0;
    for(; i < length; i++)
        sum+=time[i];
    
    return sum/length;
}

