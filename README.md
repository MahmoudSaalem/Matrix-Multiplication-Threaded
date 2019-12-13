# Matrix Multiplication - Threaded

### A Multi-threaded version of matrix multiplication that computes the result in 2 ways
1. Each element of the result matrix is calculated in a thread
2. Each row of the result matrix is calculated in a thread

## To compile and run the program:
* Place your input file in the "Test Cases" directory
* Open the terminal
* Navigate to the program directory
* Issue the following commands in order
    ``` c
    gcc main.c -o main -lpthread
    ./main
    ```
* When the program runs it will ask for the input file name
    * Enter the input file name without extension
        * Input should be a .txt file

## Input file format:
    [number of rows of 1st matrix] [number of columns of 1st matrix]
    1st matrix entries
    [number of rows of 2nd matrix] [number of columns of 2nd matrix]
    2nd matrix entries
    
    Note: [] for clarity
    
    ex:
    3 5
    1 -2 3 4 5
    1 2 -3 4 5
    -1 2 3 4 5
    5 4
    -1 2 3 4
    1 -2 3 4
    1 2 -3 4
    1 2 3 -4
    -1 -2 -3 -4
    
    Then the program will output the result in a file entitled "output.txt" which is in the following format:
    
    result matrix entries
    END1	[elapsed time of procedure 1]
    result matrix entries
    END2	[elapsed time of procedure 2]
        
