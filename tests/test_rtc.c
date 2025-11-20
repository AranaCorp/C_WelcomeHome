/**
 * @file test_rtc.c
 * @brief Emulate a real time clock (Ctrl+C to stop)
 * 
 * @compilation:
 *      Win32: gcc  src\*.c -Iinclude tests\test_rtc.c -o test_rtc.exe -lws2_32
 *      Linux: gcc  src\*.c -Iinclude tests\test_rtc.c -o test_rtc.exe
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "eeprom.h"
#include "rtc.h"
// ==================== GLOBAL ====================
bool state = false;
RTC rtc = {0};

void clock_status(void) {
    RTC_GetDateTime(&rtc);
            printf("\rDATE= %s - delay %d",rtc.timestr,rtc.delta);

}
// ==================== MAIN ====================
int main() {
    //----------------- Initialization -----------------
    char name[] = "PRGM";
    printf("[%s] Initialized\n",name);
    
    load_eeprom(); //with eeprom

    RTC_Init(&rtc);
    clock_status();printf("\n");
    RTC_SyncWithSystem(&rtc);printf("synchronized\n");
    //----------------- Main loop -----------------
    uint16_t counter = 0;
    while (1) {
        if (counter % 10000 == 0) {
            clock_status();
        }
        counter++;
    }
    printf("[%s] Exited\n",name);
    return 0;
}