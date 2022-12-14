#include "kernel/scheduling/scheduler.h"
#include "kernel/scheduling/task.h"
#include "kernel/mem/mem.h"
#include "lib/libmem.h"

unsigned long **prevTSK_StackPtrAddr;
unsigned long *nextTSK_StackPtr;

void context_switch(unsigned long **prevTskStkAddr, unsigned long *nextTskStk,
                    unsigned next_tsk_id) {
    prevTSK_StackPtrAddr = prevTskStkAddr;
    nextTSK_StackPtr = nextTskStk;
    current_tsk_index = next_tsk_id;
    current_tsk_stack = nextTskStk;
    CTX_SW();
}

extern void idleTsk(void);
void stack_init(unsigned long **stk, void (*task)(void)) {
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
extern void main(void);
extern unsigned long* initial_idle_stack;
unsigned createTsk(void (*tskBody)(void), unsigned prio, unsigned arr_time) {
    TCB tcb;
    tcb.tid = tid_cnt++;
    tcb.malloced_pos = malloc(stack_size);
    tcb.stack = (unsigned long*)(tcb.malloced_pos + stack_size) - 1;
    tcb.tsk = tskBody;
    tcb.state = WAITING;
    tcb.params = (tskPara) {.priority = prio, .arrTime = arr_time};
    stack_init(&tcb.stack, tskBody);
    if (tskBody == idleTsk) {
        idle = tcb;
        initial_idle_stack = idle.stack;
    }
    else if (tskBody == main) init = tcb;
    task_list* tmp = (task_list*) kmalloc(sizeof(task_list));
    tmp->data = tcb;
    if (task_list_head->next) {
        int flag = 0;
        task_list* p = task_list_head;
        for (; p->next; p = p->next) {
            if (tcb.params.arrTime < p->next->data.params.arrTime) {
                flag = 1;
                if (p == task_list_head) {
                    tmp->next = task_list_head->next;
                    task_list_head->next = tmp;
                    break;
                }
                else {
                    tmp->next = p->next;
                    p->next = tmp;
                    break;
                }
            }
        }
        if (flag == 0) {
            p->next = tmp;
            tmp->next = 0;
        }
    }
    else {
        task_list_head->next = tmp;
        tmp->next = 0;
    }
    return tcb.tid;
}

void destroyTsk(unsigned tskIndex) {
    task_list* tmp = task_list_head->next, * prev = task_list_head;
    for (; tmp; tmp = tmp->next) {
        if (tmp->data.tid == tskIndex) {
            free(tmp->data.malloced_pos);
            prev->next = tmp->next;
            kfree((unsigned long)tmp);
        }
        prev = tmp;
    }
}

unsigned current_tsk_index;
unsigned long* current_tsk_stack;
void tskStart(unsigned tskIndex) {
    task_list* tmp = task_list_head->next;
    for (; tmp; tmp = tmp->next) {
        if (tmp->data.tid == tskIndex) break;
    }
    if (!tmp) return;
    TCB tsk = tmp->data;
    tsk.state = READY;
    system_scheduler.enqueue(tsk);
}

void tskEnd(void) {
    destroyTsk(current_tsk_index);
    current_tsk_stack = 0;
    system_scheduler.dequeue();
    schedule();
}
