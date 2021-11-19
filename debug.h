/*********************************************
Diseñado Ing. Jaime Pedraza
Marzo 05 de 2019
*********************************************/
/*
@file		eeprom.h
@brief	header file for delay.c
*/

#ifndef _DEBUG_H_
#define _DEBUG_H_
/*pines del micro*/

/*funcion prototipo*/


void time_bit();
void tx_aux(unsigned char caracter);
void tx_auxH(unsigned char caracter);
void Debug_txt_Tibbo(unsigned char * str);
void Debug_Dividir_texto();
void ErrorWR(unsigned char TipoERR);
void Ve_Hex(unsigned char valorhex);
void DebugBufferMF(unsigned char *str,unsigned char num_char);
	
#endif	/*  */