#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5;//  global variable
int addr6; // global variable 

int foo()
{
    return -1;
}
void point_at(void *p); // declarion of the function
void foo1();
char g = 'g';
void foo2();

int secondary(int x)
{
    int addr2;  // stack
    int addr3;
    char *yos = "ree";  
    int *addr4 = (int *)(malloc(50));  // heap
	int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3]; 
	int iarray2[] = {1,2,3};
    char carray2[] = {'a','b','c'};
    int* iarray2Ptr;
    char* carray2Ptr; 
    
	printf("- &addr2: %p\n", &addr2);  // stack
    printf("- &addr3: %p\n", &addr3);  // the diffrence is 4 byte
    printf("- foo: %p\n", &foo); // the text segment
      printf("- &addr5: %p\n", &addr5);
	printf("Print distances:\n");
    point_at(&addr5);

    printf("Print more addresses:\n");
    printf("- &addr6: %p\n", &addr6);
    printf("- yos: %p\n", yos);
    printf("- gg: %p\n", &g);
    printf("- addr4: %p\n", addr4);
    printf("- &addr4: %p\n", &addr4);

    printf("- &foo1: %p\n", &foo1);
    printf("- &foo1: %p\n", &foo2);
    
    printf("Print another distance:\n");
    printf("- &foo2 - &foo1: %ld\n", (long) (&foo2 - &foo1));

   
    printf("Arrays Mem Layout (T1b):  herrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr b \n");
    /* task 1 b here */
     printf("iarray: %p\n", (void *)iarray);  // int array
    printf("iarray+1: %p\n", (void *)(iarray+1));
    printf("farray: %p\n", (void *)farray);    // float array 
    printf("farray+1: %p\n", (void *)(farray+1));
    printf("darray: %p\n", (void *)darray);
    printf("darray+1: %p\n", (void *)(darray+1));  ///double array
    printf("carray: %p\n", (void *)carray);
    printf("carray+1: %p\n", (void *)(carray+1));  /// char array
  
    
    printf("Pointers and arrays (T1d):  herrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr   d  \n ");

    /* task 1 d here */


    iarray2Ptr = iarray2; // Initialize pointer to first cell of iarray2
    carray2Ptr = carray2; // Initialize pointer to first cell of carray2

    printf("Values of iarray2:\n");
    for (int i = 0; i < 3; ++i) {
        printf("%d ", *(iarray2Ptr + i)); 
    }
    printf("\n");

    printf("Values of carray2:\n");
    for (int i = 0; i < 3; ++i) {
        printf("%c ", *(carray2Ptr + i)); 
    }
    printf("\n");

    int *p; \
    printf("Value of uninitialized pointer p: %p\n", (void *)p); // Print the value of the uninitialized pointer
    //: The value of p is indeterminate and could be any random memory address since it has not been initialized.

}


int main(int argc, char **argv)
{ 

    printf("Print function argument addresses:\n");

    printf("- &argc %p\n", &argc);
    printf("- argv %p\n", argv);
    printf("- &argv %p\n", &argv);
	
	secondary(0);
    
    printf("Command line arg addresses (T1e):  herrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr   e  \n");
    /* task 1 e here */
       printf("- &argc: %p, value: %d\n", (void *)&argc, argc);

    for (int i = 0; i < argc; ++i) {
        printf("- argv[%d]: address: %p, content: %s\n", i, (void *)&argv[i], argv[i]);
    }

    return 0;
}

void point_at(void *p)
{
    int local;
    static int addr0 = 2;
    static int addr1;

    long dist1 = (size_t)&addr6 - (size_t)p;
    long dist2 = (size_t)&local - (size_t)p;
    long dist3 = (size_t)&foo - (size_t)p;   // nice

    printf("- dist1: (size_t)&addr6 - (size_t)p: %ld\n", dist1);
    printf("- dist2: (size_t)&local - (size_t)p: %ld\n", dist2);
    printf("- dist3: (size_t)&foo - (size_t)p:  %ld\n", dist3);
    
    printf("Check long type mem size (T1a): herrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr   a  \n");
    /* part of task 1 a here */   
    

   printf("- sizeof(long)   %p\n", sizeof(long));    // 8 bytes

    printf("- addr0: %p\n", &addr0);  // they are diffrence by 8 bytes because they are static and the static integer takes 8  bytes
    printf("- addr1: %p\n", &addr1);


    ///  yes beeacuse we are working in a 64 bit memory
}

void foo1()
{
    printf("foo1\n");
}

void foo2()
{
    printf("foo2\n");
}