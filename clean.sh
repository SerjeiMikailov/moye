# shell file to clean compiled code

# remove all .o object files
rm ./build/*.o

# remove isodir
rm -r isodir

# remove Kernel_Keyboard_OS.bin
rm ./build/Kernel_Keyboard_OS.bin

# remove Kernel_Keyboard_OS.iso
rm Kernel_Keyboard_OS.iso
