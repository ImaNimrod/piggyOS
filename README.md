# piggyOS ![GitHub all releases](https://img.shields.io/github/downloads/GlowInTheDark123/piggyOS/total) ![GitHub commit activity](https://img.shields.io/github/commit-activity/m/GlowInTheDark123/piggyOS?color=green) ![Lines of code](https://img.shields.io/tokei/lines/github/GlowInTheDark123/piggyOS)

piggyOS is a homemade operating system for i686 processors. This is a toy OS created purely
to learn the concepts of OS development and how to put them into practice. Right now, it 
is in the early phases of development and many features/fixes will be coming in the coming months.

## plan for version 1.0
- The goal right now is to have v1.0 finished by late November 2022.
- It will have no permanent storage and run off of an read-only ramdisk.
- It will have some basic commands and a shell (all loaded/run in usermode)
- Should be free of any major bugs

## build instructions
To build and run this project you first need to get a copy of the source code:
```bash
git clone https://github.com/GlowInTheDark123/piggyOS.git
cd piggyOS
```
Next you will need to build the project's toolchain. This is includes the binutils (v2.39) and gcc (v12.2.0). In order to build
these you need the standard coreutils as well as curl, tar, and all the libraries needed to build the software from source. You should 
check your package manager's documentation to find and install these dependencies. To download and compile the toolchain, run:
```bash
make toolchain
```
This will take a while, as it is patching and building the cross compiler from source.
Once that has finished, you can now build the operating system by running:
```bash
make
make cdrom
```
The first command will compile the kernel and userspace programs and the latter will create an cdrom with the kernel and an initrd image. You can now run 
the `piggyOS.iso` image using your x86 emulator of choice. The build system of this project comes with a command that uses qemu to run 
the project. Just type:
```bash
make run
```

## coming features
- [X] Multitasking (not fully featured but we're getting there)
- [X] VFS and devFS working
- [X] USTAR ramdisk support
- [x] Improved memory management system
- [ ] Switch to user mode
- [X] Syscalls
- [X] Loading ELF executables into memory and run them
- [ ] Redo / make better timer interface
- [X] Syncronization primatives (just spinlock right now)
- [ ] User mode shell with number of commands
- [ ] ACPI parsing
