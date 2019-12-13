#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// STRUCTURES
typedef struct
{
    int *arr;
    int low;
    int high;
} block;

// PROTOTYPES
void endProgram(char *);
void checkFiles(FILE *);
int readSize(FILE *);
void readData(FILE *, int *, int);
void printData(int *, int);
void *threadedMergeSort(void *);
void merge(int *, int, int, int);

int main()
{
    FILE *fptr;
    fptr = fopen("input.txt", "r");
    checkFiles(fptr); // If the file does not exist, then exit program
    // Read data input from file
    int size;
    size = readSize(fptr);
    int arr[size];
    readData(fptr, arr, size);

    pthread_t threadID;
    block array = {arr, 0, size - 1}; // Passing multiple arguments to thread through struct
    pthread_create(&threadID, NULL, threadedMergeSort, &array);
    pthread_join(threadID, NULL);

    printData(arr, size);

    fclose(fptr);
    return 0;
}

// UTILITY FUNCTIONS
void endProgram(char *msg)
{
    printf("%s", msg);
    printf("Press 'Enter' to exit..\n");
    getchar();
    exit(-1);
}

void checkFiles(FILE *fptr)
{
    if (!fptr)
        endProgram("Missing file 'input.txt'\n");
}

int readSize(FILE *fptr)
{
    int size;
    fscanf(fptr, "%d\n", &size);
    if (size <= 0)
    {
        exit(0);
    }
    return size;
}

void readData(FILE *fptr, int arr[], int size)
{
    int i; // Loop counter
    for (i = 0; i < size - 1; i++)
        fscanf(fptr, "%d ", &arr[i]);
    fscanf(fptr, "%d", &arr[i]);
}

void printData(int arr[], int size)
{
    int i; // Loop counter
    for (i = 0; i < size - 1; i++)
        printf("%d ", arr[i]);
    printf("%d\n", arr[i]);
}

// MAIN FUNCTIONS
void *threadedMergeSort(void *vargp)
{
    block *arr = (block *)vargp;
    int l = arr->low,
        h = arr->high;
    // Base condition [single element]
    if (l == h)
        return NULL;
    // Creating a thread for each half
    pthread_t threadID[2];
    int mid = (l + h) / 2, i;
    block left = {arr->arr, l, mid};
    block right = {arr->arr, mid + 1, h};
    pthread_create(&threadID[0], NULL, threadedMergeSort, &left);
    pthread_create(&threadID[1], NULL, threadedMergeSort, &right);
    pthread_join(threadID[0], NULL);
    pthread_join(threadID[1], NULL);
    // Merging the 2 halves together
    merge(arr->arr, l, mid, h);
    return NULL;
}

void merge(int arr[], int low, int mid, int high)
{
    // Sizes of left and right halves of the array
    int leftSize = mid - low + 1,
        rightSize = high - mid;
    // Initialization of temporary arrays
    int leftArr[leftSize],
        rightArr[rightSize];
    // Copying the left half and the right half of the array to the temp arrays
    memcpy(leftArr, &arr[low], leftSize * sizeof(*arr));
    memcpy(rightArr, &arr[mid + 1], rightSize * sizeof(*arr));
    // Merging the temp arrays
    int i = 0,   // Left index
        j = 0,   // Right index
        k = low; // Main index
    // Merging the two arrays till one of them is finished
    while (i < leftSize && j < rightSize)
    {
        if (leftArr[i] < rightArr[j])
            arr[k] = leftArr[i++];
        else
            arr[k] = rightArr[j++];
        k++;
    }
    // Copying the remaining elements to the main array [if exist]
    while (i < leftSize)
        arr[k++] = leftArr[i++];
    while (j < rightSize)
        arr[k++] = rightArr[j++];
}