#include <sys/task.h>

task_t* current_task;
list_t* task_list;
static volatile pid_t next_pid = 1;

spinlock_t lock = 0;
spinlock_t lock2 = 0;

void kidle(void) {
    kprintf("idle task started\n");
    while (1)
        __asm__ volatile("hlt");
}

void task_create(char* name, uintptr_t func) {
    context_t* context;

    task_t* t = (task_t*) kmalloc(sizeof(task_t));

    t->kstack_mem = kmalloc(0x1000); 
    if (!t->kstack_mem) {
        kfree(t);
        return;
    }

    strncpy(t->name, name, 16);
    t->pid = next_pid++;

	__asm__ volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(t->page_dir)::"%eax");

    t->kstack = t->kstack_mem + 0x1000 - sizeof(context_t);
    context = (context_t*) t->kstack;

    context->eip = func;
    context->eflags = 0x202; 

    t->self = list_insert_back(task_list, t);

    schedule();
}

void schedule(void) {
    if (!task_list && !current_task)
        return;

    /* this only happens for the first task */
    if (!current_task) {
        task_t* next = list_peek_front(task_list);
        context_switch(next);
    }

    list_node_t* next_node = (current_task->self)->next;
    task_t* next;

    if (!next_node)
        next = list_peek_front(task_list); // we have reached the end of the list
    else
        next = next_node->value; // get the next task in the list

    if (!current_task) {
        kpanic("no tasks left. this is really bad\n");
    }
   
    context_switch(next);
}

void multitasking_init(char* name, uintptr_t func) {
    klog(LOG_OK, "Initializing multitasking\n");

    task_list = list_create();
    
    /* initial kernel idle task */
    task_create(name, func);
}
