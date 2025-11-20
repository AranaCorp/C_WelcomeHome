#ifndef EEPROM_H
#define EEPROM_H
/** 
 * @file    eeprom.h
 * @brief   Simule le comportement d'une EEPROM
 */
#include <stdint.h>

#define EEPROM_SIZE 256
#define EEPROM_FILE "eeprom.bin"

void load_eeprom();
void save_eeprom();

void WRITE_EEPROM(uint8_t addr, int8_t value);
int8_t READ_EEPROM(uint8_t addr);

void WRITE_EEPROM_INT16(uint8_t addr, int16_t value);
int16_t READ_EEPROM_INT16(uint8_t addr);

#endif