#include "kernel.h"
#include "utils.h"
#include "char.h"

#define MAX_INPUT_LENGHT 200
#define custom_sizeof(type) ((uint32)(sizeof(type)))

uint32 vga_index;
static uint32 next_line_index = 1;
uint8 g_fore_color = WHITE, g_back_color = BLUE;
int digit_ascii_codes[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

/*
assembly code for vga_print_char

vga_print_char:
  mov di, word[VGA_INDEX]
  mov al, byte[VGA_CHAR]

  mov ah, byte[VGA_BACK_COLOR]
  sal ah, 4
  or ah, byte[VGA_FORE_COLOR]

  mov [es:di], ax

  ret

*/
uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) 
{
  uint16 ax = 0;
  uint8 ah = 0, al = 0;

  ah = back_color;
  ah <<= 4;
  ah |= fore_color;
  ax = ah;
  ax <<= 8;
  al = ch;
  ax |= al;

  return ax;
}

void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)
{
  uint32 i;
  for(i = 0; i < BUFSIZE; i++){
    (*buffer)[i] = vga_entry(NULL, fore_color, back_color);
  }
  next_line_index = 1;
  vga_index = 0;
}

void init_vga(uint8 fore_color, uint8 back_color)
{
  vga_buffer = (uint16*)VGA_ADDRESS;
  clear_vga_buffer(&vga_buffer, fore_color, back_color);
  g_fore_color = fore_color;
  g_back_color = back_color;
}

void print_new_line()
{
  if(next_line_index >= 55){
    next_line_index = 0;
    clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
  }
  vga_index = 80*next_line_index;
  next_line_index++;
}

void print_char(char ch)
{
  vga_buffer[vga_index] = vga_entry(ch, g_fore_color, g_back_color);
  vga_index++;
}

void print_string(char *str)
{
  uint32 index = 0;
  while(str[index]){
    print_char(str[index]);
    index++;
  }
}

/*
void print_int(int num)
{
  char str_num[digit_count(num)+1];
  itoa(num, str_num);
  print_string(str_num);
}
*/
uint8 inb(uint16 port)
{
  uint8 ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
  return ret;
}

void outb(uint16 port, uint8 data)
{
  asm volatile("outb %0, %1" : "=a"(data) : "d"(port));
}

char get_input_keycode()
{
  char ch = 0;
  while((ch = inb(KEYBOARD_PORT)) != 0){
    if(ch > 0)
      return ch;
  }
  return ch;
}

void wait_for_io(uint32 timer_count)
{
  while(1){
    asm volatile("nop");
    timer_count--;
    if(timer_count <= 0)
      break;
    }
}

void M_memset(void *ptr, uint8 value, uint32 num_bytes)
{
  uint8 *byte_ptr = (uint8 *)ptr;
  for(uint32 i = 0; i < num_bytes; i++)
  {
    *byte_ptr = value;
    byte_ptr++;
  }
}

void sleep(uint32 timer_count)
{
  wait_for_io(timer_count);
}

int strcmp(const char *str1, const char *str2)
{
  while(*str1 && *str2 && *str1 == *str2)
  {
    str1++;
    str2++;
  }
  return *str1 - *str2;
}

void test_input() {
    char ch = 0;
    char keycode = 0;
    char command_buffer[MAX_INPUT_LENGHT];
    uint32 command_index = 0;

    do {
        keycode = get_input_keycode();

        if (keycode == KEY_ENTER) {
            print_new_line();

            // Null-terminate the command buffer
            command_buffer[command_index] = '\0';

            // Process the command
            if (strcmp(command_buffer, "CLEAR") == 0) {
                clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
                next_line_index = 1;
                print_string("MoyeOS");
                print_new_line();
            } else if (strcmp(command_buffer, "HELLO") == 0) {
                print_string("Welcome to MoyeOS!");
                print_new_line();
            } else {
                // Unrecognized command
                print_string("Unknown command: ");
                print_string(command_buffer);
                print_new_line();
            }

            // Clear the command buffer for the next input
            command_index = 0;
            M_memset(command_buffer, 0, sizeof(command_buffer));
        } else if (keycode != 0) {
            ch = get_ascii_char(keycode);

            if (command_index < MAX_INPUT_LENGHT - 1) {
                // Append the character to the command buffer
                command_buffer[command_index] = ch;
                command_index++;

                // Display the character on the screen
                print_char(ch);
            }
        }
        sleep(0x02FFFFFF);
    } while (ch > 0);
}

void kernel_entry()
{
  init_vga(WHITE, BLUE);
  print_string("MoyeOS Terminal"); 
  print_new_line();
  test_input();

}

