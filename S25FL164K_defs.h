/*
 * @what: FLASH memory S25FL164K defs
 * @why: Flash support
 * @who: Marko Puric
 * @when: 09.01.2020
 */

#ifndef UTASKER_HARDWARE_SPI_DEVICES_S25FL164K_DEFS_H_
#define UTASKER_HARDWARE_SPI_DEVICES_S25FL164K_DEFS_H_

#define SEND_FF (0xFFU)
#define DUMMY_BYTE (0x00U)
#define SECTOR_START_ADDR_MASK (0x00FFF000U)
#define BLOCK_START_ADDR_MASK (0x00FF0000U)

/*
 * Flash info
 * All 4-kB sectors have the pattern XXX000h-XXXFFFh
 * All 64-kB blocks have the pattern XX0000h-XXFFFFh
 */
#define MAIN_FLASH_SIZE         (8388608U) 				/* 8 MB	(4*2048*1024) */
#define ADDRESS_BITS            (24U)
#define ADDRESS_BYTES           (ADDRESS_BITS / 8U)
#define SECTOR_SIZE             (4U * 1024U)       		/* 4 kB  */
#define BLOCK_SIZE              (64U * 1024U)      		/* 64 kB */
#define SECTOR_COUNT            (2048U)
#define PAGE_SIZE               (256U)     				/* 256 B */
#define FLASH_STARTING_ADDRESS  (0x00000000U)
#define FLASH_ENDING_ADDRESS    (0x007FFFFFU)

/*
 * Security Registers info
 * has size of 256-byte each
 */
#define SEC_REG_SIZE            (256U)     				/* 256 B */
#define SEC_REG_1_START_ADDR    (0x00001000U)
#define SEC_REG_1_END_ADDR      (0x000010FFU)
#define SEC_REG_2_START_ADDR    (0x00002000U)
#define SEC_REG_2_END_ADDR      (0x000020FFU)
#define SEC_REG_3_START_ADDR    (0x00003000U)
#define SEC_REG_3_END_ADDR      (0x000030FFU)

/*
 * Flash Commands
 */
#define READ_STATUS_REGISTER_1  (0x05U)
#define READ_STATUS_REGISTER_2  (0x35U)
#define READ_STATUS_REGISTER_3  (0x33U)
#define WRITE_ENABLE            (0x06U)
#define WRITE_DISABLE           (0x04U)
#define WRITE_STATUS_REG		(0x01U)
#define PAGE_PROGRAM            (0x02U)    				/* 1 - 256 B */
#define SECTOR_ERASE            (0x20U)    				/* 4 kB      */
#define BLOCK_ERASE             (0xD8U)    				/* 64 kB     */
#define CHIP_ERASE              (0xC7U)    				/* 8 MB      */

#define READ_DATA               (0x03U)
#define FAST_READ				(0x0BU)

#define SOFTWARE_RESET_ENABLE	(0x66U)
#define SOFTWARE_RESET			(0x99U)
#define CNT_READ_RESET			(0xFFU)

#define READ_JEDEC_ID           (0x9FU)
#define PROGRAM_SEC_REG         (0x42U)
#define ERASE_SEC_REG           (0x44U)
#define READ_SEC_REG            (0x48U)

/*
 * Status Registers info
 */
#define BUSY_BIT_MASK           (1U)

#endif /* UTASKER_HARDWARE_SPI_DEVICES_S25FL164K_DEFS_H_ */
