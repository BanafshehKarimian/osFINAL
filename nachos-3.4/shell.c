#include "syscall.h"
#define SIZE 100

void* p(int a){

	//printf("pid%d\n",a);

}
int
main()
{
/*
    int array[SIZE], i, sum=0;

    for (i=0; i<SIZE; i++) array[i] = i;
    for (i=0; i<SIZE; i++) sum += array[i];
    PrintString("Total sum: ");
    PrintInt(sum);
    PrintChar('\n');*/
int x = Fork(p);
	if (x == 0) {

		int x = Fork(p);
		//Exec("../test/shell");
	}
	else {
		Join(x);
		Exit(0);

	}
}
