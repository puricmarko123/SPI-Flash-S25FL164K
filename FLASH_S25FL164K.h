/*
 * @what: FLASH memory
 * @why: Flash support
 * @who: Marko Puric
 * @when: 09.01.2020
 */
#ifndef UTASKER_HARDWARE_SPI_DEVICES_FLASH_S25FL164K_H_
#define UTASKER_HARDWARE_SPI_DEVICES_FLASH_S25FL164K_H_

#include "syscnst.h"
#include "PE_Types.h"

void S25FL164K_Write_Enable(void);
void S25FL164K_Write_Disable(void);
uint8_t S25FL164K_Read_BUSY_Bit(void);

/* Main functions used by user */
void S25FL164K_Write_Data(uint32_t address, uint8_t *buff, uint32_t byte_count);
void S25FL164K_Read_Data(uint32_t address, uint8_t *buff, uint32_t byte_count);
void S25FL164K_Modify_Sector_Data(uint32_t address, uint8_t *buff, uint32_t byte_count);

/* Page write function */
void S25FL164K_Page_Program(uint32_t address, uint8_t *buff, uint16_t byte_count);

/* Reset functions */
void S25FL164K_Software_Reset(void);

/* Erase functions */
void S25FL164K_Erase_Sector(uint32_t address);
void S25FL164K_Erase_Block(uint32_t address);
void S25FL164K_Erase_Chip(void);

/* Security Registers functions */
void S25FL164K_Read_Sec_Reg(uint32_t address, uint8_t *buff, uint16_t byte_count);
void S25FL164K_Write_Sec_Reg(uint32_t address, uint8_t *buff, uint16_t byte_count);
void S25FL164K_Erase_Sec_Reg(uint32_t address);
void S25FL164K_Modify_Sec_Reg(uint32_t address, uint8_t *buff, uint16_t byte_count);

#endif /* UTASKER_HARDWARE_SPI_DEVICES_FLASH_S25FL164K_H_ */
