#include <debug.h> 
#include <reg51.h>

//ERRORES
#define SELECCION_SECTOR 		0
#define COMANDO_WR		 		1
#define NO_RTA_WR		 		2
#define COMANDO_RD		 		3
#define NO_RTA_RD		 		4
#define FALLA_WR 				5
#define NO_CARD 				6

extern unsigned char centena, decena, unidad;
sbit txd2 = P1^0;					//Transmision Aux Datos	IP				
extern void          _nop_     (void);
//***************************************************************************************************
void time_bit()
{
unsigned char j;

	for (j=0; j<=7; j++) 				//18 para 19200  ...   41 Para 9600	 //42 142us //7 a 9600 transmision
	{
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		
		_nop_();
	}
}
//***************************************************************************************************/
//***************************************************************************************************
void tx_aux(unsigned char caracter)
{
	unsigned char j, temporal, bitTX;

	txd2=0;
	time_bit();
  	temporal=caracter;
	bitTX=caracter&0x01;
	if (bitTX==0x00)
	{
	 	txd2=0;
	}
	else
	{
	   	txd2=1;
	}
	time_bit();
	for (j=1; j<=7; j++)
	{
		temporal>>=1;
		bitTX=temporal&(0x01);
		if (bitTX==0x00)
		{
	 		txd2=0;
		}
		else
		{
	   		txd2=1;
		}
		time_bit();
	}
	txd2=1;
	time_bit();
	time_bit(); 
	time_bit();
}
//***************************************************************************************************
void tx_auxH(unsigned char caracter)
{
	unsigned char temporal;
	
	temporal=caracter;
	temporal>>=4;
	if (temporal<=0x09)
	{
		tx_aux(temporal|0x30);
	}
	else
	{
		tx_aux(temporal+0x37);	
	}

	temporal=caracter;	
	temporal=temporal&0x0f;
	if (temporal<=0x09)
	{
		tx_aux(temporal|0x30);
	}
	else
	{
		tx_aux(temporal+0x37);	
	}		
	tx_aux(' ');
}

/*------------------------------------------------------------------------------
imprime la trama hasta el caracter null
------------------------------------------------------------------------------*/
void Debug_txt_Tibbo(unsigned char * str)
{
	unsigned char i;
	i=0;
	
	
		for (i=0; str[i] != '\0'; i++)
		{
 	  		tx_aux(str[i]);
		}
		
	
}

void Debug_Dividir_texto()
{
	Debug_txt_Tibbo((unsigned char *) "/*---------------------------------------*/\r\n");
}

void ErrorWR(unsigned char TipoERR)
{
 	Debug_txt_Tibbo((unsigned char *) "Error: ");
	
	if (TipoERR==SELECCION_SECTOR)
	{
		Debug_txt_Tibbo((unsigned char *) "SEL. SECTOR\r\n");
		
	}
	else if	(TipoERR==COMANDO_WR)
	{
		Debug_txt_Tibbo((unsigned char *) "RTA. WR N.\r\n");
		
	}
	else if	(TipoERR==COMANDO_RD)
	{
		Debug_txt_Tibbo((unsigned char *) "RTA. RD N.\r\n");
		
	}
	else if	(TipoERR==NO_RTA_WR)
	{
		Debug_txt_Tibbo((unsigned char *) "SIN RTA WR\r\n");
		
	}
	else if	(TipoERR==NO_RTA_RD)
	{
		Debug_txt_Tibbo((unsigned char *) "SIN RTA RD\r\n");
		
	}
	else if	(TipoERR==FALLA_WR)
	{
		Debug_txt_Tibbo((unsigned char *) "GRABACION.\r\n");
		
	}
	else if	(TipoERR==NO_CARD)
	{
		Debug_txt_Tibbo((unsigned char *) "TARJETA RETIRADA\r\n");
		
	}
	
}
void DebugBufferMF(unsigned char *str,unsigned char num_char)
{
  unsigned char j;
 
  
  
		for (j=0; j<num_char; j++)
		{
		tx_auxH(*str);
		str++;
		}
		tx_aux('\r');
		tx_aux('\n');
  

}

