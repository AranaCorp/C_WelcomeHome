/** 
 * @file    eeprom.c
 * @brief   Simule le comportement d'une EEPROM
 */
#include "eeprom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t eeprom[EEPROM_SIZE] = {0};

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
    fwrite(eeprom, sizeof(uint8_t), EEPROM_SIZE, f);
    fclose(f);
}

void eeprom_clear() {
    memset(eeprom, 0, EEPROM_SIZE);
    eeprom_save();
}

void eeprom_show() {
    printf("EEPROM CONTENT:\n");
    for (size_t i = 0; i < EEPROM_SIZE; i++) {
        if (i % 16 == 0) printf("%04X: ", (unsigned int)i);
        printf("%02X ", eeprom[i]);
        if (i % 16 == 15) printf("\n");
    }
    if (EEPROM_SIZE % 16 != 0) printf("\n");
}

void eeprom_write(uint8_t addr, uint8_t value) {
    if (addr < EEPROM_SIZE) {
        eeprom[addr] = value;
        eeprom_save();
    }
}

uint8_t eeprom_read(uint8_t addr) {
    if (addr < EEPROM_SIZE) return eeprom[addr];
    return 0;
}

void eeprom_write_int16(uint8_t addr, uint16_t value) {
    if (addr < EEPROM_SIZE-1) {
        eeprom[addr] = (value & 0xFF);
        eeprom[addr+1]   = (value >> 8) & 0xFF;
        eeprom_save();
    }
}

uint16_t eeprom_read_int16(uint8_t addr) {
    if (addr < EEPROM_SIZE-1) {
        return ((uint8_t)eeprom[addr+1] << 8) | ((uint8_t)eeprom[addr]);
    }
    return 0;
}