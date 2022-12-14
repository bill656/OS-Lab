#include "kernel/scheduling/taskQueueFIFO.h"
#include "kernel/scheduling/scheduler.h"
#include "kernel/scheduling/task.h"

#include "kernel/mem/mem.h"

unsigned long BspContextBase[0x100];
unsigned long *BspContext;
void startMultitask(void) {
    BspContext = BspContextBase + 0x100 - 1;
    TCB firstTsk = qfront(&taskQueue);
    context_switch(&BspContext, firstTsk.stack, firstTsk.tid);
}

void idleTsk(void) {
    while (qempty(&taskQueue)) schedule();
}

unsigned long* idle_stack;
unsigned idle_id;

extern void main(void);

void init_tsk_manager(void) {
    task_list_head = (task_list*) kmalloc(sizeof(task_list));
    taskQueue.data = (TCB*) kmalloc(initial_size * sizeof(TCB));
    taskQueue.head = 0; taskQueue.tail = 1;
    taskQueue.size = initial_size;
    idle_id = createTsk(idleTsk);
    idle_stack = task_list_head->next->data.stack;
    unsigned init = createTsk(main);
    tskStart(init);
    startMultitask();
}

void fifo_schedule(void) {
    TCB next = qfront(&taskQueue);
    context_switch(&current_tsk_stack, next.stack, next.tid);
}

void schedule(void) {
    if (qempty(&taskQueue))
        context_switch(&current_tsk_stack, idle_stack, idle_id);
    fifo_schedule();
}
