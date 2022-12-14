#include "../../myOS/include/userInterface.h"
#include "../include/shell.h"
#include "../../myOS/include/mem.h"

int testCase1(UNUSED int argc, UNUSED char (*argv)[32]){  
	//======for malloc===============================
	char*buf1 = (char*)malloc(19);
    //dPartitionWalkByAddr(userMemHandler);
	char*buf2 = (char*)malloc(24);
    //dPartitionWalkByAddr(userMemHandler);

	for(int i=0;i<17;i++) *(buf1+i) = '*';
	*(buf1+17) = '\n';
	*(buf1+18) = '\000';

	for(int i=0;i<22;i++) *(buf2+i) = '#';
	*(buf2+22) = '\n';
	*(buf2+23) = '\000';

	printf(0x5, "We allocated 2 buffers.\n");
	printf(0x5, "BUF1(size=19, addr=0x%x) filled with 17(*): ",(unsigned long)buf1);
	printf(0x7,buf1);
	printf(0x5, "BUF2(size=24, addr=0x%x) filled with 22(#): ", (unsigned long)buf2);
	printf(0x7,buf2);

	printf(0x7,"\n");
	
	free((unsigned long)buf1);
    //dPartitionWalkByAddr(userMemHandler);
	free((unsigned long)buf2);
    //dPartitionWalkByAddr(userMemHandler);

	return 0;
}

int testCase2(UNUSED int argc, UNUSED char (*argv)[32]){
	//======for malloc===============================
	char*buf1 = (char*)malloc(11);
	char*buf2 = (char*)malloc(21);

	for(int i=0;i<9;i++) *(buf1+i) = '+';
	*(buf1+9) = '\n';
	*(buf1+10) = '\000';

	for(int i=0;i<19;i++) *(buf2+i) = ',';
	*(buf2+19) = '\n';
	*(buf2+20) = '\000';

	printf(0x5, "We allocated 2 buffers.\n");
	printf(0x5, "BUF1(size=9, addr=0x%x) filled with 9(+): ",(unsigned long)buf1);
	printf(0x7,buf1);
	printf(0x5, "BUF2(size=19, addr=0x%x) filled with 19(,): ", (unsigned long)buf2);
	printf(0x7,buf2);

	printf(0x7,"\n");

	free((unsigned long)buf1);
	free((unsigned long)buf2);

	return 0;
}

int maxMallocSizeNow(UNUSED int argc, UNUSED char (*argv)[32]){
	int i=0x1000;
	unsigned long x;

	while(1){
		x = malloc(i);
		if(x) free(x);	
		else break;

		i += 0x1000;
	}
	printf(0x7, "MAX_MALLOC_SIZE: 0x%x (with step = 0x1000);\n",i);	
}

//======for dPartition===============================
int test_dP1(UNUSED int argc, UNUSED char (*argv)[32]){
	unsigned long x,x1,xHandler;
	int i, t_size = 0x100;
	x = malloc(t_size);
	if (x){
		printf(0x7, "We had successfully ");
		printf(0x5, "malloc()");
		printf(0x7, " a small memBlock (size=0x%x, addr=0x%x);\n", t_size, x);

		printf(0x7, "It is initialized as a very small dPartition;\n");
		xHandler = dPartitionInit(x, t_size);
		dPartitionWalkByAddr(x);

		i=0x10;
		while(1){
			x1 = dPartitionAlloc(xHandler,i); 
			printf(0x7, "Alloc a memBlock with size 0x%x, ", i);
			if(x1) {	
				printf(0x5, "success(addr=0x%x)!",x1);
				dPartitionFree(xHandler,x1);
				printf(0x7, "......Released;\n");
			} else {
				printf(0x5, "failed!\n");
				break;
			}
			
			i <<= 1;
		}

		printf(0x7,"Now, converse the sequence.\n");
		while(i >= 0x10){
			x1 = dPartitionAlloc(xHandler,i); 
			printf(0x7, "Alloc a memBlock with size 0x%x, ", i);
			if(x1) {	
				printf(0x5, "success(addr=0x%x)!",x1);
				dPartitionFree(xHandler,x1);
				printf(0x7, "......Released;\n");
			} else printf(0x5, "failed!\n");
			
			i >>= 1;
		}
		
		free(x);
	} else printf(0x7,"MALLOC FAILED, CAN't TEST dPartition\n");	
}



int test_dP2(UNUSED int argc, UNUSED char (*argv)[32]){
	unsigned long x,x1,x2,x3,xHandler;
	int t_size = 0x100;
	x = malloc(t_size); 
	if (x){
		printf(0x7, "We had successfully ");
		printf(0x5, "malloc()");
		printf(0x7, " a small memBlock (size=0x%x, addr=0x%x);\n", t_size, x);

		printf(0x7, "It is initialized as a very small dPartition;\n");
		xHandler = dPartitionInit(x, t_size);
		dPartitionWalkByAddr(x);

		printf(0x7,"Now, A:B:C:- ==> -:B:C:- ==> -:C- ==> - .\n");

		x1 = dPartitionAlloc(xHandler,0x10); 
		printf(0x7, "Alloc memBlock A with size 0x10: ");
		if(x1) printf(0x5, "success(addr=0x%x)!\n",x1);
		else printf(0x5, "failed!\n");
		*(unsigned long*)x1 = 0xAAAAAAAA;		
		dPartitionWalkByAddr(xHandler);

		x2 = dPartitionAlloc(xHandler,0x20); 
		printf(0x7, "Alloc memBlock B with size 0x20: ");
		if(x2) printf(0x5, "success(addr=0x%x)!\n",x2);
		else printf(0x5, "failed!\n");
		*(unsigned long*)x2 = 0xBBBBBBBB;		
		dPartitionWalkByAddr(xHandler);

		x3 = dPartitionAlloc(xHandler,0x30); 
		printf(0x7, "Alloc memBlock C with size 0x30: ");
		if(x3) printf(0x5, "success(addr=0x%x)!\n",x3);
		else printf(0x5, "failed!\n");
		*(unsigned long*)x3 = 0xCCCCCCCC;		
		dPartitionWalkByAddr(xHandler);
		
		printf(0x7,"Now, release A.\n");
		dPartitionFree(xHandler,x1); 
		dPartitionWalkByAddr(xHandler);
		
		printf(0x7,"Now, release B.\n");
		dPartitionFree(xHandler,x2); 
		dPartitionWalkByAddr(xHandler);

		printf(0x7,"At last, release C.\n");
		dPartitionFree(xHandler,x3); 
		dPartitionWalkByAddr(xHandler);
		
		free(x);
	} else printf(0x7,"MALLOC FAILED, CAN't TEST dPartition\n");	
}

int test_eFP(UNUSED int argc, UNUSED char (*argv)[32]) {
	unsigned long x,x1,x2,x3,x4,x5,xHandler;
	unsigned long p_size = 31;//int
	unsigned long n = 4;//int
	unsigned long t_size;//int
	//======for eFPartition===============================
	
	t_size = eFPartitionTotalSize(p_size, n);
	x = malloc(t_size); printf(0x7, "X:0x%x:%d \n", x, t_size);
	if (x) {
		printf(0x7, "We had successfully ");
		printf(0x5, "malloc()");
		printf(0x7, " a small memBlock (size=0x%x, addr=0x%x);\n", t_size, x);

		printf(0x7, "It is initialized as a very small ePartition;\n");
		xHandler = eFPartitionInit(x, p_size, n);

		eFPartitionWalkByAddr(xHandler);

		x1 = eFPartitionAlloc(xHandler);
		if(x1){
			*(unsigned long *)x1 = 0xAAAAAAAA;
			printf(0x7,"Alloc memBlock A, start = 0x%x: 0x%x \n",x1,*(unsigned long *)x1);
		} else printf(0x7,"Alloc memBlock A, failed!\n");
		eFPartitionWalkByAddr(xHandler);

		x2 = eFPartitionAlloc(xHandler);
		if(x2){
			*(unsigned long *)x2 = 0xBBBBBBBB;
			printf(0x7,"Alloc memBlock B, start = 0x%x: 0x%x \n",x2,*(unsigned long *)x2);
		} else printf(0x7,"Alloc memBlock B, failed!\n");
		eFPartitionWalkByAddr(xHandler);

		x3 = eFPartitionAlloc(xHandler);
		if(x3){
			*(unsigned long *)x3 = 0xCCCCCCCC;
			printf(0x7,"Alloc memBlock C, start = 0x%x: 0x%x \n",x3,*(unsigned long *)x3);
		} else printf(0x7,"Alloc memBlock C, failed!\n");
		eFPartitionWalkByAddr(xHandler);

		x4 = eFPartitionAlloc(xHandler);
		if(x4){
			*(unsigned long *)x4 = 0xDDDDDDDD;
			printf(0x7,"Alloc memBlock D, start = 0x%x: 0x%x \n",x4,*(unsigned long *)x4);
		} else printf(0x7,"Alloc memBlock D, failed!\n");
		eFPartitionWalkByAddr(xHandler);

		x5 = eFPartitionAlloc(xHandler);
		if(x5){
			*(unsigned long *)x5 = 0xEEEEEEEE;
			printf(0x7,"Alloc memBlock E, start = 0x%x: 0x%x \n",x5,*(unsigned long *)x5);
		} else printf(0x7,"Alloc memBlock E, failed!\n");
		eFPartitionWalkByAddr(xHandler);

		printf(0x7,"Now, release A.\n");
		eFPartitionFree(xHandler,x1);
		eFPartitionWalkByAddr(xHandler);

		printf(0x7,"Now, release B.\n");
		eFPartitionFree(xHandler,x2);
		eFPartitionWalkByAddr(xHandler);

		printf(0x7,"Now, release C.\n");
		eFPartitionFree(xHandler,x3);
		eFPartitionWalkByAddr(xHandler);

		printf(0x7,"Now, release D.\n");
		eFPartitionFree(xHandler,x4);		
		eFPartitionWalkByAddr(xHandler);
	} else printf(0x7,"TSK2: MALLOC FAILED, CAN't TEST eFPartition\n");

	return 0;
}

int test_dP3(UNUSED int argc, UNUSED char (*argv)[32]){
	unsigned long x,x1,x2,x3,xHandler;
	int t_size = 0x100;
	x = malloc(t_size);
	if (x){
		printf(0x7, "We had successfully ");
		printf(0x5, "malloc()");
		printf(0x7, " a small memBlock (size=0x%x, addr=0x%x);\n", t_size, x);

		printf(0x7, "It is initialized as a very small dPartition;\n");
		xHandler = dPartitionInit(x, t_size);
		dPartitionWalkByAddr(x);

		printf(0x7,"Now, A:B:C:- ==> -:B:C:- ==> -:C- ==> - .\n");

		x1 = dPartitionAlloc(xHandler,0x10); 
		printf(0x7, "Alloc memBlock A with size 0x10: ");
		if(x1) printf(0x5, "success(addr=0x%x)!\n",x1);
		else printf(0x5, "failed!\n");
		*(unsigned long*)x1 = 0xAAAAAAAA;		
		dPartitionWalkByAddr(xHandler);

		x2 = dPartitionAlloc(xHandler,0x20); 
		printf(0x7, "Alloc memBlock B with size 0x20: ");
		if(x2) printf(0x5, "success(addr=0x%x)!\n",x2);
		else printf(0x5, "failed!\n");
		*(unsigned long*)x2 = 0xBBBBBBBB;		
		dPartitionWalkByAddr(xHandler);

		x3 = dPartitionAlloc(xHandler,0x30); 
		printf(0x7, "Alloc memBlock C with size 0x30: ");
		if(x3) printf(0x5, "success(addr=0x%x)!\n",x3);
		else printf(0x5, "failed!\n");
		*(unsigned long*)x3 = 0xCCCCCCCC;		
		dPartitionWalkByAddr(xHandler);
		
		printf(0x7,"At last, release C.\n");
		dPartitionFree(xHandler,x3); 
		dPartitionWalkByAddr(xHandler);

		printf(0x7,"Now, release B.\n");
		dPartitionFree(xHandler,x2); 
		dPartitionWalkByAddr(xHandler);

		printf(0x7,"Now, release A.\n");
		dPartitionFree(xHandler,x1); 
		dPartitionWalkByAddr(xHandler);
		
		free(x);
	} else printf(0x7,"MALLOC FAILED, CAN't TEST dPartition\n");		
}

void memTestCaseInit(void){	
	addNewCmd("testMalloc1\0", testCase1, NULL, "Malloc, write and read.\0");
	addNewCmd("testMalloc2\0", testCase2, NULL, "Malloc, write and read.\0");		
	addNewCmd("maxMallocSizeNow\0", maxMallocSizeNow, NULL, "MAX_MALLOC_SIZE always changes. What's the value Now?\0");

    addNewCmd("testdP1\0", test_dP1, NULL, "Init a dPartition(size=0x100) [Alloc,Free]* with step = 0x20\0");
    addNewCmd("testdP2\0", test_dP2, NULL, "Init a dPartition(size=0x100) A:B:C:- ==> -:B:C:- ==> -:C:- ==> - .\0");
    addNewCmd("testdP3\0", test_dP3, NULL, "Init a dPartition(size=0x100) A:B:C:- ==> A:B:- ==> A:- ==> - .\0");

    addNewCmd("testeFP\0", test_eFP, NULL, "Init a eFPartition. Alloc all and Free all.\0");
}
