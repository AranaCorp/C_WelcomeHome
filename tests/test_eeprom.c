/**
 * @file test_eeprom.c
 * @brief Emulate a eeprom (Ctrl+C to stop)
 * 
 * @compilation:
 *      Win32: gcc  src\*.c -Iinclude tests\test_eeprom.c -o test_eeprom.exe -lws2_32
 *      Linux: gcc  src\*.c -Iinclude tests\test_eeprom.c -o test_eeprom.exe
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "eeprom.h"
#include "rtc.h"
// ==================== GLOBAL ====================
bool state = false;
uint16_t var16_w = 54321;
uint16_t var16_r = 0;
int8_t var8_w = -125;
int8_t var8_r = 0;

void write_var(void) {
    printf("[EEPROM] Writing var16=%d at addr 0x10\n",var16_w);
    WRITE_EEPROM_INT16(0x10,var16_w);
    printf("[EEPROM] Writing var8=%d at addr 0x20\n",var8_w);
    WRITE_EEPROM(0x20,var8_w);
}

void read_var(void) {
    var16_r = READ_EEPROM_INT16(0x10);
    printf("[EEPROM] Read var16=%d from addr 0x10\n",var16_r);
    var8_r = READ_EEPROM(0x20);
    printf("[EEPROM] Read var8=%d from addr 0x20\n",var8_r);
}
// ==================== MAIN ====================
int main() {
    //----------------- Initialization -----------------
    char name[] = "PRGM";
    printf("[%s] Initialized\n",name);
    
    load_eeprom(); //with eeprom

    
    //----------------- Main loop -----------------
    uint16_t counter = 0;
    while (1) {
        var16_r = 0;
        var8_r = 0;
        if (counter % 1000 == 0) {
            write_var();
        }
        if (counter % 1000 == 10) {
            read_var();
        }
        counter++;
    }
    printf("[%s] Exited\n",name);
    return 0;
}