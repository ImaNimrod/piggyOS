global spinlock_lock
global spinlock_unlock

spinlock_lock:
    mov ebx, [esp + 4]
    mov eax, [ebx + 4]
.loop:
    lock bts dword [eax], 0
    pause
    jc .loop
    ret

spinlock_unlock:
    mov ebx, [esp + 4]
    mov eax, [ebx + 4]
    mov dword [eax], 0
    ret
