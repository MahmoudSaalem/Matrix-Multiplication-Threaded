#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

// STRUCTURES
typedef struct
{
    int aRows, aCols,
        bRows, bCols;
    int **matA,
        **matB;
} block;
typedef struct
{
    block data;
    int **res;
    int i, j;
} element;

// PROTOTYPES
void readFileName(char *);
void endProgram(char *);
void checkFiles(FILE *);
void checkDim(int, int);
int **readMat(FILE *, int *, int *);
int **allocateMat(int, int);
void printMat(int **, int, int, double, int);
void deallocateMat(int **);
void multiply(element);
void *calElement(void *);
void *calRow(void *);

int main()
{
    FILE *fptr;
    char *fileBuffer = (char *)malloc(255 * sizeof(char));
    readFileName(fileBuffer);
    puts(fileBuffer);
    fptr = fopen(fileBuffer, "r");
    checkFiles(fptr); // If the file does not exist, then exit program
    free(fileBuffer);

    // Matrices dimensions and declaration
    int aRows, aCols,
        bRows, bCols;
    int **matA,
        **matB,
        **res;
    // Reading matrices from file and allocating space
    matA = readMat(fptr, &aRows, &aCols);
    matB = readMat(fptr, &bRows, &bCols);
    res = allocateMat(aRows, bCols);
    // Reading is done, close file
    fclose(fptr);
    // Checking if dimensions do not match
    checkDim(aCols, bRows);
    // Multiply matrices

    block data = {aRows, aCols, bRows, bCols, matA, matB};
    element toCalElement = {data, res};
    multiply(toCalElement);
    // printMat(res, aRows, bCols);

    // Free memory
    deallocateMat(matA);
    deallocateMat(matB);
    deallocateMat(res);

    puts("Multiplication done!");
    puts("Check output in 'output.txt'");

    return 0;
}

// UTILITY FUNCTIONS
void readFileName(char *input)
{
    char file[255] = "Test Cases/";
    char fileBuffer[241];
    puts("Please enter the name of the test case file..");
    fgets(fileBuffer, 241, stdin);
    fileBuffer[strlen(fileBuffer) - 1] = '\0';
    strcat(file, fileBuffer);
    strcat(file, ".txt");
    strcpy(input, file);
}

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
    FILE *fp = fopen("output.txt", "w");
    fclose(fp);
}

int **allocateMat(int rows, int cols)
{
    int **arr = (int **)malloc(rows * sizeof(int *));
    int *arr_data = malloc(rows * cols * sizeof(int));
    for (int i = 0; i < rows; i++)
        arr[i] = arr_data + i * cols;
    return arr;
}

int **readMat(FILE *fptr, int *rows, int *cols)
{
    fscanf(fptr, "%d %d\n", rows, cols);
    int **arr = allocateMat(*rows, *cols);
    for (int i = 0, j; i < *rows; i++)
    {
        for (j = 0; j < *cols - 1; j++)
            fscanf(fptr, "%d ", &arr[i][j]);
        fscanf(fptr, "%d\n", &arr[i][j]);
    }
    return arr;
}

void printMat(int **mat, int rows, int cols, double time, int turn)
{
    FILE *fptr = fopen("output.txt", "a");
    for (int i = 0, j; i < rows; i++)
    {
        for (j = 0; j < cols - 1; j++)
        {
            fprintf(fptr, "%d ", mat[i][j]);
        }
        fprintf(fptr, "%d\n", mat[i][j]);
    }
    fprintf(fptr, "END%d\t%lf\n", turn, time);
    fclose(fptr);
}

void deallocateMat(int **arr)
{
    free(arr[0]);
    free(arr);
}

void checkDim(int aCols, int bRows)
{
    if (aCols != bRows)
        endProgram("Dimensions error!\n");
}

// MAIN FUNCTIONS
void multiply(element resData)
{
    struct timespec start, end;
    long time;
    int aRows = resData.data.aRows;
    int bCols = resData.data.bCols;
    // int **res = allocateMat(aRows, bCols);
    // resData.res = res;
    int threadNum = aRows * bCols;
    element arrayElement[threadNum];
    pthread_t threadElementIDs[threadNum];
    element arrayRow[aRows];
    pthread_t threadRowIDs[aRows];
    int countElement = 0;
    int countRow = 0;
    for (int i = 0; i < aRows; i++)
    {
        arrayRow[countRow++].i = i;
        for (int j = 0; j < bCols; j++)
        {
            arrayElement[countElement].i = i;
            arrayElement[countElement++].j = j;
        }
    }
    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < threadNum; i++)
    {
        arrayElement[i].data = resData.data;
        arrayElement[i].res = resData.res;
        int error = pthread_create(&threadElementIDs[i], NULL, calElement, &arrayElement[i]);
        if (error)
        {
            exit(-1);
        }
    }
    for (int i = 0; i < threadNum; i++)
    {
        pthread_join(threadElementIDs[i], NULL);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    time = (end.tv_nsec - start.tv_nsec) / 1000;
    printMat(resData.res, aRows, bCols, time, 1);
    // printf("END1\t%ld\n", time);
    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < aRows; i++)
    {
        arrayRow[i].data = resData.data;
        arrayRow[i].res = resData.res;
        int error = pthread_create(&threadRowIDs[i], NULL, calRow, &arrayRow[i]);
        if (error)
        {
            exit(-1);
        }
    }
    for (int i = 0; i < aRows; i++)
    {
        pthread_join(threadRowIDs[i], NULL);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    time = (end.tv_nsec - start.tv_nsec) / 1000;
    printMat(resData.res, aRows, bCols, time, 2);
    // printf("END2\t%ld\n", time);
}

void *calElement(void *vargp)
{
    element *resData = (element *)vargp;
    int aCols = resData->data.aCols;
    int i = resData->i;
    int j = resData->j;
    int sum = 0;
    for (int k = 0; k < aCols; k++)
    {
        sum += resData->data.matA[i][k] * resData->data.matB[k][j];
    }
    resData->res[i][j] = sum;
    return NULL;
}

void *calRow(void *vargp)
{
    element *resData = (element *)vargp;
    int bCols = resData->data.bCols;
    int aCols = resData->data.aCols;
    int i = resData->i;
    for (int j = 0, sum; j < bCols; j++)
    {
        sum = 0;
        for (int k = 0; k < aCols; k++)
        {
            sum += resData->data.matA[i][k] * resData->data.matB[k][j];
        }
        resData->res[i][j] = sum;
    }
    return NULL;
}