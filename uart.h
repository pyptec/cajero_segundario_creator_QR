/*********************************************
Diseñado Ing. Jaime Pedraza
Marzo 05 de 2019
*********************************************/
/*
@file		eeprom.h
@brief	header file for delay.c
*/

#ifndef _UART_H_
#define _UART_H_
/*pines del micro*/

/*funcion prototipo*/


void EscribirCadenaSoft(unsigned char tamano_cadena);
void tx_chr (unsigned char data_com);
void com_initialize (void);


#endif	/*  */