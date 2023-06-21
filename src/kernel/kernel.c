void kernel_main(void) {
    *(char*)0xb8000 = 'Q';
    return;
}
