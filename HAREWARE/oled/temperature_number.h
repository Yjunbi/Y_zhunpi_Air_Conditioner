#ifndef __TEMPERATURE_NUMBER_H
#define __TEMPERATURE_NUMBER_H

#define NUMBER_WRITH 16
#define NUMBER_HIGH 16

/* (16 X 32 , 仿宋 )*/
const unsigned char temperature_number[][16] = {
	{0x00,0x80,0xE0,0xF0,0x30,0x08,0x08,0x08,0x08,0x08,0x10,0x30,0xE0,0xC0,0x00,0x00},
    {0x00,0x03,0x0F,0x1F,0x10,0x20,0x20,0x20,0x20,0x20,0x20,0x18,0x1F,0x07,0x00,0x00},/*"0",0*/

    {0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xF0,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x3F,0x3F,0x20,0x20,0x20,0x00,0x00,0x00,0x00},/*"1",1*/

    {0x00,0x00,0x60,0x70,0x50,0x08,0x08,0x08,0x08,0x08,0x08,0xF0,0xF0,0x20,0x00,0x00},
    {0x00,0x20,0x30,0x30,0x28,0x28,0x24,0x24,0x22,0x21,0x21,0x20,0x30,0x18,0x00,0x00},/*"2",2*/

    {0x00,0x00,0x30,0x30,0x30,0x08,0x08,0x08,0x08,0x08,0x90,0xF0,0x60,0x00,0x00,0x00},
    {0x00,0x00,0x18,0x38,0x20,0x20,0x21,0x21,0x21,0x21,0x21,0x12,0x1E,0x0C,0x00,0x00},/*"3",3*/

    {0x00,0x00,0x00,0x00,0x00,0x80,0x40,0x20,0x10,0xF0,0xF8,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x04,0x06,0x05,0x05,0x04,0x04,0x24,0x24,0x3F,0x3F,0x24,0x24,0x04,0x04,0x00},/*"4",4*/

    {0x00,0x00,0x00,0xF8,0x08,0x88,0x88,0x88,0x88,0x88,0x88,0x08,0x08,0x00,0x00,0x00},
    {0x00,0x00,0x18,0x19,0x29,0x20,0x20,0x20,0x20,0x20,0x20,0x31,0x1F,0x0E,0x00,0x00},/*"5",5*/

    {0x00,0x00,0xC0,0xE0,0x10,0x10,0x88,0x88,0x88,0x88,0x88,0x18,0x10,0x00,0x00,0x00},
    {0x00,0x03,0x0F,0x1F,0x31,0x21,0x20,0x20,0x20,0x20,0x20,0x21,0x1F,0x0E,0x00,0x00},/*"6",6*/

    {0x00,0x00,0x30,0x18,0x08,0x08,0x08,0x08,0x08,0x88,0x48,0x28,0x18,0x08,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x3E,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"7",7*/

    {0x00,0x00,0x70,0xF0,0x90,0x88,0x88,0x08,0x08,0x08,0x08,0x90,0x70,0x20,0x00,0x00},
    {0x00,0x0C,0x1E,0x22,0x22,0x21,0x21,0x21,0x21,0x21,0x23,0x22,0x1E,0x0C,0x00,0x00},/*"8",8*/

    {0x00,0xC0,0xF0,0xB0,0x10,0x08,0x08,0x08,0x08,0x08,0x10,0x30,0xE0,0xC0,0x00,0x00},
    {0x00,0x00,0x11,0x33,0x32,0x22,0x22,0x22,0x22,0x22,0x11,0x1D,0x0F,0x07,0x00,0x00},/*"9",9*/

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x20,0x30,0x30,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*".",10*/
};

const unsigned char T_or_H_symbol[][16] = {
	{0x00,0x00,0x38,0x48,0x30,0xC0,0x60,0x10,0x10,0x08,0x08,0x10,0x10,0x70,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x0F,0x1C,0x30,0x20,0x20,0x20,0x20,0x20,0x10,0x00,0x00},/*"℃",0*/
	
	
	{0x60,0xF0,0x08,0x08,0x08,0xF0,0xF0,0x00,0x80,0x40,0x20,0x18,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x01,0x21,0x11,0x09,0x06,0x01,0x0C,0x1E,0x22,0x21,0x21,0x3E,0x1C,0x00},/*"%",0*/
	
	
};

#endif

