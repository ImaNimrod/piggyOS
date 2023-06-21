# piggyOS ![GitHub all releases](https://img.shields.io/github/downloads/GlowInTheDark123/piggyOS/total) ![GitHub commit activity](https://img.shields.io/github/commit-activity/m/GlowInTheDark123/piggyOS?color=green) ![Lines of code](https://img.shields.io/tokei/lines/github/GlowInTheDark123/piggyOS)

piggyOS is a homemade operating system for i686 processors. This is a toy OS created purely
to learn the concepts of OS development and how to put them into practice. Right now, it 
is in the early phases of development and many features/fixes will be coming in the coming months.

## plan for version 1.0
- The goal right now is to have v1.0 finished by late November 2022.
- It will have no permanent storage and run off of an read-only ramdisk.
- It will have some basic commands and a shell (all loaded/run in usermode)
- Should be free of any major bugs

## coming features
- [ ] Multitasking (not fully featured as of 8/27/22)
- [X] VFS and devFS working
- [X] EXT2 ramdisk support
- [x] Improved memory management system
- [ ] Switch to user mode
- [X] Syscalls
- [ ] Loading ELF User Mode programs into memory and run them
- [ ] Redo / make better timer interface
- [X] Syncronization primatives (just spinlock right now)
- [ ] User mode shell with number of commands
- [X] ACPI parsing
