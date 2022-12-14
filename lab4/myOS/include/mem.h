#ifndef __MEM_H__
#define __MEM_H__

extern unsigned long kmalloc(unsigned long size);
extern unsigned long kfree(unsigned long start);

extern unsigned long dPartitionInit(unsigned long start, unsigned long size);
extern unsigned long dPartitionAlloc(unsigned long dp, unsigned long size);
extern unsigned long dPartitionFree( unsigned long dp, unsigned long start);

extern void dPartitionWalkByAddr(unsigned long dp);

extern unsigned long eFPartitionInit(unsigned long start, unsigned long perSize, unsigned long n);
extern unsigned long eFPartitionTotalSize(unsigned long perSize, unsigned long n);
extern unsigned long eFPartitionAlloc(unsigned long EFPHandler);
extern unsigned long eFPartitionFree(unsigned long EFPHandler,unsigned long mbStart);

extern void eFPartitionWalkByAddr(unsigned long efp);

extern void pMemInit(void);

extern unsigned long kernelMemHandler;
extern unsigned long userMemHandler;
#endif
