/*
 * @what: FLASH memory
 * @why: Flash support
 * @who: Marko Puric
 * @when: 09.01.2020
 *
 * Main functions are:
 *
 * S25FL164K_Write_Data (uint32_t address, uint8_t *buff, uint32_t byte_count);
 * S25FL164K_Read_Data (uint32_t address, uint8_t *buff, uint32_t byte_count);
 * S25FL164K_Modify_Sector_Data (uint32_t address, uint8_t *buff, uint32_t byte_count);
 *
 * Before writing or modifying data, memory locations must be erased with Erase Sector (4 kB) or Erase Block (64 kB) functions.
 * S25FL164K_Write_Data uses Page Program command so only S25FL164K_Write_Data should be used.
 * S25FL164K_Modify_Sector_Data is used for changing data within a sector.
 */

#include "FLASH_S25FL164K.h"
#include "S25FL164K_defs.h"
#include "../SPI_devices/SPI_Master.h"
#include "PE_Types.h"
#include "SPI_PDD.h"

/* Static variable mod_data used in 'S25FL164K_Modify_Sector_Data' */
static uint8_t mod_data[SECTOR_SIZE];

/* Keeps CS (SPI_FLASH_CS) signal in active state*/
#define SPI_CMD_FLASH(byte, cs)        SPI_Master(SPI0_PRPH_BASE_ADDRESS, ((byte) | SPI_PUSHR_PCS(cs) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT_MASK) )

/* Puts CS (SPI_FLASH_CS) signal in inactive state */
#define SPI_CMD_LAST_FLASH(byte, cs)   SPI_Master(SPI0_PRPH_BASE_ADDRESS, ((byte) | SPI_PUSHR_PCS(cs) | SPI_PUSHR_CTAS(1)) )

/*
 * @what: Enable Write Command
 * @why: Necessary for writing to Flash
 */
void S25FL164K_Write_Enable(void)
{
	/* Wait until BUSY bit is cleared... */
	while(S25FL164K_Read_BUSY_Bit());

	/* Write Enable Command */
	SPI_CMD_LAST_FLASH(WRITE_ENABLE, SPI_FLASH_CS);

    return;
}

/*
 * @what: Disable Write Command
 * @why: Necessary for disabling writing to Flash
 */
void S25FL164K_Write_Disable(void)
{
	/* Wait until BUSY bit is cleared... */
	while(S25FL164K_Read_BUSY_Bit());

	/* Write Disable Command */
	SPI_CMD_LAST_FLASH(WRITE_DISABLE, SPI_FLASH_CS);

	return;
}

/*
 * @what: Main function for writing data
 * @why:
 */
void S25FL164K_Write_Data(uint32_t address, uint8_t *buff, uint32_t byte_count)
{
	/* From 1-256 bytes */
	uint16_t partial_page_size = 0;

	/* Transfer size */
	uint16_t tr_size = 0;

	int32_t count = byte_count;

	while(count > 0)
	{
		/* Calculate leftover space on page */
		partial_page_size = PAGE_SIZE - (address & (PAGE_SIZE - 1U));

		/* Calculate transfer size */
		tr_size = count > partial_page_size ? partial_page_size : count;

		/* Send data */
		S25FL164K_Page_Program(address, buff, tr_size);

		/* Calculate new starting address */
		address = address + partial_page_size;

		/* Calculate new byte count */
		count = count - partial_page_size;
	}

	return;
}

/*
 * @what: Function to write data to a Page (max. 256 bytes)
 * @why:
 */
void S25FL164K_Page_Program(uint32_t address, uint8_t *buff, uint16_t byte_count)
{
	/* Enable Write */
	S25FL164K_Write_Enable();

	/* Wait until BUSY bit is cleared... */
	while(S25FL164K_Read_BUSY_Bit());

	/* Page Program Command */
	SPI_CMD_FLASH(PAGE_PROGRAM, SPI_FLASH_CS);

	/* Send address, MSB First*/
	SPI_CMD_FLASH((address >> 16U), SPI_FLASH_CS);
	SPI_CMD_FLASH((address >> 8U), SPI_FLASH_CS);
	SPI_CMD_FLASH(address, SPI_FLASH_CS);

	/* Send Data*/
	for(uint16_t i = 1U; i < (byte_count); i++)
	{
		SPI_CMD_FLASH( *buff, SPI_FLASH_CS);
		buff = buff + 1U;
	}

	SPI_CMD_LAST_FLASH( *buff, SPI_FLASH_CS);

	return;
}

/*
 * @what: Modify data of a Sector
 * @why:
 */
void S25FL164K_Modify_Sector_Data(uint32_t address, uint8_t *buff, uint32_t byte_count)
{
	uint32_t sector_address;

	/* Starting Sector address */
	sector_address = address & SECTOR_START_ADDR_MASK;

	/* Store Sector Data to local buffer 'mod_data' (4 kB) */
	S25FL164K_Read_Data(sector_address, mod_data, SECTOR_SIZE);

	/* Find write offset */
	uint32_t offset = address - sector_address;

	/* Write new data to local buffer mod_data */
	for(uint32_t i = 0U; i < byte_count; i++)
	{
		mod_data[offset] = *(buff + i);
	    offset = offset + 1U;
	}

	/* Erase Sector */
	S25FL164K_Erase_Sector(sector_address);

	/* Write mod_data to Sector */
	S25FL164K_Write_Data(sector_address, mod_data, SECTOR_SIZE);

	return;
}

/*
 * @what: Read data from 'address' to 'address + byteCount', entire memory can be accessed as long as the clock continues
 * @why:
 */
void S25FL164K_Read_Data(uint32_t address, uint8_t *buff, uint32_t byte_count)
{
	/* Send Read Command */
	SPI_CMD_FLASH(READ_DATA, SPI_FLASH_CS);

	/* Send Address, MSB First */
	SPI_CMD_FLASH((address >> 16U), SPI_FLASH_CS);
	SPI_CMD_FLASH((address >> 8U), SPI_FLASH_CS);
	SPI_CMD_FLASH(address, SPI_FLASH_CS);

	/* Read data */
	for(uint16_t i = 1U; i < (byte_count); i++)
	{
		*buff = SPI_CMD_FLASH(SEND_FF, SPI_FLASH_CS);
		buff = buff + 1U;
	}

	*buff = SPI_CMD_LAST_FLASH(SEND_FF, SPI_FLASH_CS);

	return;
}

/*
 * @what: Erase FLASH Sector, 4 kB
 * @why:
 */
void S25FL164K_Erase_Sector(uint32_t address)
{
	/* Enable Write */
	S25FL164K_Write_Enable();

	/* Wait until BUSY bit is cleared... */
	while(S25FL164K_Read_BUSY_Bit());

	/* Erase from Sector starting address XXX000h */
	address = address & SECTOR_START_ADDR_MASK;

	/* Command for Sector Erase */
	SPI_CMD_FLASH(SECTOR_ERASE, SPI_FLASH_CS);

	/* Send Sector Start Address, MSB First */
	SPI_CMD_FLASH((address >> 16U), SPI_FLASH_CS);
	SPI_CMD_FLASH((address >> 8U), SPI_FLASH_CS);
	SPI_CMD_LAST_FLASH(address, SPI_FLASH_CS);

	return;
}

/*
 * @what: Erase FLASH Block, 64 kB
 * @why:
 */
void S25FL164K_Erase_Block(uint32_t address)
{
	/* Enable Write */
	S25FL164K_Write_Enable();

	/* Wait until BUSY bit is cleared... */
	while(S25FL164K_Read_BUSY_Bit());

	/* Erase from Sector starting address XX0000h */
	address = address & BLOCK_START_ADDR_MASK;

	/* Block Erase Command */
	SPI_CMD_FLASH(BLOCK_ERASE, SPI_FLASH_CS);

	/* Send 24-bit Block Start Address, MSB First */
	SPI_CMD_FLASH((address >> 16U), SPI_FLASH_CS);
	SPI_CMD_FLASH((address >> 8U), SPI_FLASH_CS);
	SPI_CMD_LAST_FLASH(address, SPI_FLASH_CS);

	return;
}

/*
 * @what: Erase Flash (8 MB)
 * @why:
 */
void S25FL164K_Erase_Chip(void)
{
	/* Enable Write */
	S25FL164K_Write_Enable();

	/* Wait until BUSY bit is cleared... */
	while(S25FL164K_Read_BUSY_Bit());

	/* Command for Chip Erase */
	SPI_CMD_LAST_FLASH(CHIP_ERASE, SPI_FLASH_CS);

	return;
}

/*
 * @what: Reads BUSY bit in Status Register 1
 * @why:
 */
uint8_t S25FL164K_Read_BUSY_Bit(void)
{
	/* Send Command for Read Status Register 1 */
	SPI_CMD_FLASH(READ_STATUS_REGISTER_1, SPI_FLASH_CS);

	/* Store register data */
	uint8_t busy = SPI_CMD_LAST_FLASH(BUSY_BIT_MASK, SPI_FLASH_CS);

	/* Check BUSY bit */
	busy = busy & BUSY_BIT_MASK;

	return busy;
}

/*
 * @what: Erase Security Register at 'address' = start address
 * @why:
 */
void S25FL164K_Erase_Sec_Reg(uint32_t address)
{
	/* Enable write */
	S25FL164K_Write_Enable();

	/* Wait until BUSY bit is cleared... */
	while(S25FL164K_Read_BUSY_Bit());

	/* Command for Erase Security Register */
	SPI_CMD_FLASH(ERASE_SEC_REG, SPI_FLASH_CS);

	/* Send 24-bit Security Register start address */
	SPI_CMD_FLASH((address >> 16U), SPI_FLASH_CS);
	SPI_CMD_FLASH((address >> 8U), SPI_FLASH_CS);
	SPI_CMD_LAST_FLASH(address, SPI_FLASH_CS);

	return;
}

/*
 * @what: Write to Security Register at 'address' = start address
 * @why:
 */
// write 1 - 256 bytes
void S25FL164K_Write_Sec_Reg(uint32_t address, uint8_t *buff, uint16_t byte_count)
{
	/* Enable Write */
	S25FL164K_Write_Enable();

	/* Wait until BUSY bit is cleared... */
	while(S25FL164K_Read_BUSY_Bit());

	/* Page Program */
	SPI_CMD_FLASH(PROGRAM_SEC_REG, SPI_FLASH_CS);

	/* Send address */
	SPI_CMD_FLASH((address >> 16U), SPI_FLASH_CS);
	SPI_CMD_FLASH((address >> 8U), SPI_FLASH_CS);
	SPI_CMD_FLASH(address, SPI_FLASH_CS);

	/* Send Data*/
	for(uint16_t i = 1U; i < (byte_count); i++)
	{
		SPI_CMD_FLASH( *buff, SPI_FLASH_CS);
		buff = buff + 1U;
	}

	SPI_CMD_LAST_FLASH( *buff, SPI_FLASH_CS);

	return;
}

/*
 * @what: Read Security Register
 * @why:
 */
void S25FL164K_Read_Sec_Reg(uint32_t address, uint8_t *buff, uint16_t byte_count)
{
	/* Send Command for Fast Read mode */
	SPI_CMD_FLASH(READ_SEC_REG , SPI_FLASH_CS);

	/* Send Address, last byte is a dummy byte */
	SPI_CMD_FLASH((address >> 16U), SPI_FLASH_CS);
	SPI_CMD_FLASH((address >> 8), SPI_FLASH_CS);
	SPI_CMD_FLASH(address, SPI_FLASH_CS);
	SPI_CMD_FLASH(DUMMY_BYTE, SPI_FLASH_CS);

	/* Read data */
	for(uint16_t i = 1U; i < (byte_count); i++)
	{
		*buff = SPI_CMD_FLASH(SEND_FF, SPI_FLASH_CS);
		buff = buff + 1U;
	}

	*buff = SPI_CMD_LAST_FLASH(SEND_FF, SPI_FLASH_CS);

	return;
}

/*
 * @what: Reset Flash
 * @why: Restore the device to its initial power up state, by reloading volatile
 *		 registers from non-volatile default values
 */
void S25FL164K_Software_Reset(void)
{
	/* Reset Enable Command */
	SPI_CMD_FLASH(SOFTWARE_RESET_ENABLE , SPI_FLASH_CS);

	/* Software Reset Command */
	SPI_CMD_LAST_FLASH(SOFTWARE_RESET , SPI_FLASH_CS);

	return;
}
