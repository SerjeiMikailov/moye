# Shell file to compile code

# assemble boot.s file
as --32 boot.s -o ./build/boot.o

# compile kernel.c file
gcc -m32 -c kernel.c -o ./build/kernel.o -std=gnu99 -ffreestanding -O1 -Wall -Wextra

gcc -m32 -c utils.c -o ./build/utils.o -std=gnu99 -ffreestanding -O1 -Wall -Wextra

gcc -m32 -c char.c -o ./build/char.o -std=gnu99 -ffreestanding -O1 -Wall -Wextra

# linking all the object files to Kernel_Keyboard_OS.bin
ld -m elf_i386 -T linker.ld ./build/kernel.o ./build/utils.o ./build/char.o ./build/boot.o -o ./build/Kernel_Keyboard_OS.bin -nostdlib

# check Kernel_Keyboard_OS.bin file is x86 multiboot file or not
grub-file --is-x86-multiboot ./build/Kernel_Keyboard_OS.bin

# building the iso file
mkdir -p isodir/boot/grub
cp ./build/Kernel_Keyboard_OS.bin isodir/boot/Kernel_Keyboard_OS.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o Kernel_Keyboard_OS.iso isodir

# run it in qemu
qemu-system-x86_64 -cdrom Kernel_Keyboard_OS.iso
