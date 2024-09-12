#include <stdio.h>
#include <stdlib.h>

void bubbleSort(int numbers[], int array_size)
{
    int i, j;
    int temp;  // Use an int instead of allocating memory with malloc
    for (i = (array_size - 1); i > 0; i--)
    {
        for (j = 0; j < i; j++)  // Loop should start from 0, not 1
        {
            if (numbers[j] > numbers[j + 1])
            {
                temp = numbers[j];
                numbers[j] = numbers[j + 1];
                numbers[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s num1 num2 ... numN\n", argv[0]);
        return 1;
    }

    char **arr = argv + 1;
    int i, n = argc - 1;
    int *numbers = (int *)calloc(n, sizeof(int));

  
    printf("Original array:");
    for (i = 0; i < n; ++i)  // Loop should be i < n
    {
        printf(" %s", arr[i]);
        numbers[i] = atoi(arr[i]);
    }
    printf("\n");

    bubbleSort(numbers, n);

    printf("Sorted array:");
    for (i = 0; i < n; ++i)
        printf(" %d", numbers[i]);
    printf("\n");

    free(numbers);  // Free the allocated memory
    return 0;
}
