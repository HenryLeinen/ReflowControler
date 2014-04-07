/*
 * Parameter.c
 *
 * Created: 31.03.2014 12:59:35
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/eeprom.h>
#include "Parameter.h"

typedef struct {
	uint16_t Time;
	uint16_t Temp;
} ParamTable_t;



ParamTable_t  Parameters[2][5] EEMEM = 
{ 
	{{60, 120}, {240, 180}, {60, 230}, {60, 230}, {2, 50}},
	{{90, 150}, {90, 170}, {60, 240}, {45, 225}, {120, 40}}	
};

typedef struct {
	char BankName[10];
} BankDef_t;

uint8_t param_current_bank EEMEM = 0;
BankDef_t		param_bank_def[2] EEMEM =
{ 
	{
		"Bleifrei  "
	},
	{
		"Verbleit  "
	}
};

uint16_t param_activation_threshold EEMEM = 50;


uint16_t param_get_activation_threshold()
{
	return eeprom_read_word(&param_activation_threshold);
}

void param_set_activation_threshold(uint16_t thr)
{
	eeprom_write_word(&param_activation_threshold, thr);
}



void param_set_bank(uint8_t bank)
{
	eeprom_write_byte(&param_current_bank, bank);
}

uint8_t param_get_bank()
{
	return eeprom_read_byte(&param_current_bank);
}


void param_get_bank_name(char *pName)
{
	if (pName == NULL)
		return;
	
	eeprom_read_block(pName, &param_bank_def[param_get_bank()].BankName, 10);
}

void param_set_bank_name(char *pName)
{
	if (pName == NULL)
		return;
		
	eeprom_write_block(pName, &param_bank_def[param_get_bank()].BankName, 10);
}

uint16_t param_get_value(uint8_t p_no, ParameterName_t p_name)
{
	uint16_t *p_addr;
	if (p_name == Param_Temperature)
		p_addr = &Parameters[param_get_bank()][p_no].Temp;
	else
		p_addr = &Parameters[param_get_bank()][p_no].Time;
		
	return eeprom_read_word((const uint16_t *)p_addr);
}

void param_set_value(uint8_t p_no, ParameterName_t p_name, uint16_t p_value)
{
	uint16_t *p_addr;
	if (p_name == Param_Temperature)
		p_addr = &Parameters[param_get_bank()][p_no].Temp;
	else
		p_addr = &Parameters[param_get_bank()][p_no].Time;
		
	eeprom_write_word((uint16_t *)p_addr, p_value);
}