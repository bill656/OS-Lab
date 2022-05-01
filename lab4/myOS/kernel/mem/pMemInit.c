#include "libio.h"
#include "mem.h"
unsigned long pMemStart;//可用的内存的起始地址
unsigned long pMemSize;//可用的大小
unsigned long pMemHandler;

void memTest(unsigned long start, unsigned long grainSize){
	/*本函数需要实现！！！*/
	/*功能：检测算法
		这一个函数对应实验讲解ppt中的第一大功能-内存检测。
		本函数的功能是检测从某一个地址开始的内存是否可用，具体算法就可以用ppt上说的写了读看是否一致。
        注意点两个：
        1、开始的地址要大于1M，需要做一个if判断。
        2、grainSize不能太小，也要做一个if判断
	*/
	if (start < (1 << 20)) { pMemSize = 0; return; }
    if (grainSize < 4) { pMemSize = 0; return; }
    unsigned short* p = (unsigned short*)start;
    pMemStart = start;
    while (1) {
        int flag = 1;
        unsigned short* p_end = p + grainSize - 1;
        unsigned short src = *p, src_end = *p_end;
        *p = 0xAA55;
        if (*p != 0xAA55) flag = 0;
        *p = 0x55AA;
        if (*p != 0x55AA) flag = 0;
        *p_end = 0xAA55;
        if (*p_end != 0xAA55) flag = 0;
        *p_end = 0x55AA;
        if (*p_end != 0x55AA) flag = 0;
        *p = src; *p_end = src_end;
        if (!flag) break;
        pMemSize += grainSize;
        p = p_end + 1;
    }
    myPrintk(0x7,"MemStart: %x  \n", pMemStart);
    myPrintk(0x7,"MemSize:  %x  \n", pMemSize);
}

extern unsigned long _end;
void pMemInit(void) {
	unsigned long _end_addr = (unsigned long) &_end;
	memTest(0x100000,0x1000);
	myPrintk(0x7,"_end:  %x  \n", _end_addr);
	if (pMemStart <= _end_addr) {
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}

	pMemHandler = dPartitionInit(pMemStart,pMemSize);	
}
