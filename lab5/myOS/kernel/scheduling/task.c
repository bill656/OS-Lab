#include "kernel/scheduling/taskQueueFIFO.h"
#include "kernel/scheduling/scheduler.h"
#include "kernel/scheduling/task.h"
#include "kernel/mem/mem.h"
#include "lib/libmem.h"

unsigned long **prevTSK_StackPtrAddr;
unsigned long *nextTSK_StackPtr;

void context_switch(unsigned long **prevTskStkAddr, unsigned long *nextTskStk) {
    prevTSK_StackPtrAddr = prevTskStkAddr;
    nextTSK_StackPtr = nextTskStk;
    CTX_SW();
}

void stack_init(unsigned long **stk, void (*task)(void)) {
    *(*stk)-- = (unsigned long)0x08; // CS 
    *(*stk)-- = (unsigned long)tskEnd;
    *(*stk)-- = (unsigned long)task; // eip
    // pushf
    *(*stk)-- = (unsigned long)0x0202; // flag registers
    // pusha
    *(*stk)-- = (unsigned long)0xAAAAAAAA; // eax
    *(*stk)-- = (unsigned long)0xCCCCCCCC; // ecx
    *(*stk)-- = (unsigned long)0xDDDDDDDD; // edx
    *(*stk)-- = (unsigned long)0xBBBBBBBB; // ebx
    *(*stk)-- = (unsigned long)0x44444444; // esp
    *(*stk)-- = (unsigned long)0x55555555; // ebp
    *(*stk)-- = (unsigned long)0x66666666; // esi
    **stk = (unsigned long)0x77777777; // edi
}

task_list* task_list_head;

const unsigned long stack_size = 0x100000; // 1 M

unsigned tid_cnt = 1;
unsigned createTsk(void (*tskBody)(void)) {
    TCB tcb;
    tcb.tid = tid_cnt++;
    tcb.stack = (unsigned long*) (malloc(stack_size) + stack_size - 1);
    tcb.state = WAITING;
    tcb.params = (tskPara) {.priority = 0, .arrTime = 0, .exeTime = 0};
    stack_init(&tcb.stack, tskBody);
    task_list* tmp = (task_list*) kmalloc(sizeof(task_list));
    tmp->data = tcb;
    tmp->next = task_list_head->next;
    task_list_head->next = tmp;
    return tcb.tid;
}

void destroyTsk(unsigned tskIndex) {
    task_list* tmp = task_list_head->next, * prev = task_list_head;
    for (; tmp; tmp = tmp->next) {
        if (tmp->data.tid == tskIndex) {
            free((unsigned long)(tmp->data.stack) + 1 - stack_size);
            prev->next = tmp->next;
            kfree((unsigned long)tmp);
        }
        prev = tmp;
    }
}

unsigned current_tsk_index;
unsigned long* current_ysk_stack;
void tskStart(TCB* tsk) {
    tsk->state = READY;
    current_tsk_index = tsk->tid;
    current_ysk_stack = tsk->stack;
    qpush(&taskQueue, *tsk);
}

void tskEnd() {
    destroyTsk(current_tsk_index);
    qpop(&taskQueue);
    current_ysk_stack = 0;
    fifo_schedule();
}
