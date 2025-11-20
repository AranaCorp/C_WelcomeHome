/**
 * @file basic_prgm.c
 * @brief Emulate a basic program (Ctrl+C to stop)
 * 
 * @compilation:
 *      Win32: gcc basic_prgm.c -o basic_prgm.exe ; ./basic_prgm.exe 
 *             ./run.bat basic_prgm.c  
 *      Linux: gcc basic_prgm.c -o basic_prgm ; ./basic_prgm
 *                 ./run.sh basic_prgm.c 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// ==================== GLOBAL ====================
bool state = false;

// ==================== MAIN ====================
int main() {
    //----------------- Initialization -----------------
    char name[] = "PRGM";
    printf("[%s] Initialized\n",name);
    
    //----------------- Main loop -----------------
    uint16_t counter = 0;
    while (1) {
        // Simulate MCU tasks
        if (counter % 10 == 0) {
            state = !state;
            printf("[%s] Main loop running... Counter=%d : %d\n", name, counter, state);
        }
        counter++;
    }
    printf("[%s] Exited\n",name);
    return 0;
}