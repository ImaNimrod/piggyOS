#include <sys/tasking.h>

list_t* task_list;
task_t* current_task;
task_t* prev_task;
tid_t next_tid = 0;
regs_t saved_context;

static void switch_context(regs_t* p_regs, context_t* n_regs) {
    if(prev_task) {
        prev_task->regs.eax = p_regs->eax;
        prev_task->regs.ebx = p_regs->ebx;
        prev_task->regs.ecx = p_regs->ecx;
        prev_task->regs.edx = p_regs->edx;
        prev_task->regs.esi = p_regs->esi;
        prev_task->regs.edi = p_regs->edi;
        prev_task->regs.ebp = p_regs->ebp;
        prev_task->regs.esp = p_regs->useresp;
        prev_task->regs.eflags = p_regs->eflags;
        prev_task->regs.eip = p_regs->eip;
        __asm__ volatile("mov %%cr3, %0" : "=r"(prev_task->regs.cr3));
    }

    if(((page_directory_t*) n_regs->cr3) != NULL) {
        switch_page_directory((page_directory_t*) n_regs->cr3, 1);
    }
    prev_task = current_task;
    user_regs_switch(n_regs);
}

void create_task(char* name, uintptr_t location) {
    task_t* task = kcalloc(sizeof(task_t), 1);

    strcpy(task->name, name);
    task->tid = next_tid++;
    task->regs.eip = (uint32_t) location;
    task->regs.eflags = 0x0206;

    task->regs.esp = LOAD_MEMORY_ADDRESS;

    task->page_dir = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
    memset(task->page_dir, 0, sizeof(page_directory_t));
    copy_page_directory(task->page_dir, kernel_page_dir);
    allocate_region(task->page_dir, LOAD_MEMORY_ADDRESS - 4 * 1024, LOAD_MEMORY_ADDRESS, 0, 0, 1);
    task->regs.cr3 = (uint32_t) virt2phys(kernel_page_dir, task->page_dir);

    task->state = TASK_STATE_ALIVE;

    if(!current_task)
        current_task = task;
    task->self = list_insert_front(task_list, task);
    kprintf("%s created\n", task->name);
}

void task_schedule(void) {
    if (!list_size(task_list))
        return;

    task_t* next;
    if (!current_task) {
        current_task = list_peek_front(task_list);
        prev_task = NULL; 
        switch_context(NULL, &current_task->regs);
    }

    list_node_t* nextnode = (current_task->self)->next;
    if(current_task->state == TASK_STATE_ZOMBIE) {
        list_remove_node(task_list, current_task->self);
        prev_task = NULL;
        free(current_task->page_dir);
        free(current_task);
        kprintf("Grim Reaper killed %s (%d). One less zombie.\n", current_task->name, current_task->tid);
    }

    if(!nextnode)
        next = list_peek_front(task_list);
    else
        next = nextnode->value;

    current_task = next;
    if(current_task == NULL)
        klog(LOG_ERR, "No tasks left to run... something has gone very wrong\n");

    switch_context(&saved_context, &next->regs);
}

void tasking_init(void) {
    klog(LOG_OK, "Multitasking initialized\n");
    task_list = list_create();
}
