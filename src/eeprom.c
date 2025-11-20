/** 
 * @file    eeprom.c
 * @brief   Simule le comportement d'une EEPROM
 */
#include "eeprom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int8_t eeprom[EEPROM_SIZE] = {0};

void load_eeprom() {
    FILE *f = fopen(EEPROM_FILE, "rb");
    if (f) {
        fread(eeprom, sizeof(int8_t), EEPROM_SIZE, f);
        fclose(f);
        printf("[EEPROM] Data loaded from %s\n", EEPROM_FILE);
    } else {
        memset(eeprom, 0, EEPROM_SIZE);
        printf("[EEPROM] Initialized at 0\n");
    }
}

void save_eeprom() {
    FILE *f = fopen(EEPROM_FILE, "wb");
    if (!f) return;
    fwrite(eeprom, sizeof(int8_t), EEPROM_SIZE, f);
    fclose(f);
}

void WRITE_EEPROM(uint8_t addr, int8_t value) {
    if (addr < EEPROM_SIZE) {
        eeprom[addr] = value;
        save_eeprom();
    }
}

int8_t READ_EEPROM(uint8_t addr) {
    if (addr < EEPROM_SIZE) return eeprom[addr];
    return 0;
}

void WRITE_EEPROM_INT16(uint8_t addr, int16_t value) {
    if (addr < EEPROM_SIZE-1) {
        eeprom[addr]   = (value >> 8) & 0xFF;
        eeprom[addr+1] = value & 0xFF;
        save_eeprom();
    }
}

int16_t READ_EEPROM_INT16(uint8_t addr) {
    if (addr < EEPROM_SIZE-1) {
        return ((uint8_t)eeprom[addr] << 8) | ((uint8_t)eeprom[addr+1]);
    }
    return 0;
}