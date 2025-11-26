/** 
 * @file    eeprom.c
 * @brief   Simule le comportement d'une EEPROM
 */
#include "eeprom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int8_t eeprom[EEPROM_SIZE] = {0};

void eeprom_load() {
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

void eeprom_save() {
    FILE *f = fopen(EEPROM_FILE, "wb");
    if (!f) return;
    fwrite(eeprom, sizeof(int8_t), EEPROM_SIZE, f);
    fclose(f);
}

void eeprom_write(uint8_t addr, int8_t value) {
    if (addr < EEPROM_SIZE) {
        eeprom[addr] = value;
        eeprom_save();
    }
}

int8_t eeprom_read(uint8_t addr) {
    if (addr < EEPROM_SIZE) return eeprom[addr];
    return 0;
}

void eeprom_write_int16(uint8_t addr, int16_t value) {
    if (addr < EEPROM_SIZE-1) {
        eeprom[addr]   = (value >> 8) & 0xFF;
        eeprom[addr+1] = value & 0xFF;
        eeprom_save();
    }
}

int16_t eeprom_read_int16(uint8_t addr) {
    if (addr < EEPROM_SIZE-1) {
        return ((uint8_t)eeprom[addr] << 8) | ((uint8_t)eeprom[addr+1]);
    }
    return 0;
}