#ifndef EEPROM_H
#define EEPROM_H
/** 
 * @file    eeprom.h
 * @brief   Simule le comportement d'une EEPROM
 */
#include <stdint.h>

#define EEPROM_SIZE 256
#define EEPROM_FILE "eeprom.bin"

void eeprom_load();
void eeprom_save();
void eeprom_clear();
void eeprom_show();

void eeprom_write(uint8_t addr, uint8_t value);
uint8_t eeprom_read(uint8_t addr);

void eeprom_write_int16(uint8_t addr, uint16_t value);
uint16_t eeprom_read_int16(uint8_t addr);

#endif