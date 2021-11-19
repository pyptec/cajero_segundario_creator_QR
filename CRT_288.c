#include <crt_288.h> 

#define STX	0x02
#define ETX	0x03

#define ESPERA_RX				0
#define TIMWPoll 	150

#define B_0000    0
#define B_0001    1
#define B_0010    2
#define B_0011    3
#define B_0100    4
#define B_0101    5
#define B_0110    6
#define B_0111    7
#define B_1000    8
#define B_1001    9
#define B_1010    a
#define B_1011    b
#define B_1100    c
#define B_1101    d
#define B_1110    e
#define B_1111    f

#define _B2H(bits)    B_##bits
#define B2H(bits)    _B2H(bits)
#define _HEX(n)        0x##n
#define HEX(n)        _HEX(n)
#define _CCAT(a,b)    a##b
#define CCAT(a,b)   _CCAT(a,b)

#define MEMORY(a,b)        HEX( CCAT(B2H(a),B2H(b)) )
#define SEQ_RTA_EJECT			0X04

extern unsigned char g_scArrTxComSoft[];
extern unsigned char g_cEstadoComSoft;
extern unsigned int ValTimeOutCom;
extern unsigned char BufferWrite_MF[];
extern unsigned char g_cEstadoComSeqMF;
extern unsigned char ID1,ID2,ID3,ID4; 

/*funciones de prototipo*/
extern void EscribirCadenaSoft(unsigned char tamano_cadena);
extern void Debug_txt_Tibbo(unsigned char * str);
/*---------------------------------------------------------------------
Procedimeinto que expulsa la tarjeta del lector MF
Host 02,00,02,32,30,etx,bcc
Reader return stx,00,etx,32,30,status,etx,bcc
status=Y=0x59 expulso la tarjeta
status=N=0x4e fallo la expulsion
status=E=0x45 no hay tarjeta en el lector

----------------------------------------------------------------------*/
void eject_card ()
{
	//Debug_txt_Tibbo((unsigned char *) "EJECT CARD \r\n");		
	g_scArrTxComSoft[0]=STX; 			// Caracter de Inicio
	g_scArrTxComSoft[1]=0x00;			// Longitud (2 Bytes)
	g_scArrTxComSoft[2]=0x02;			//
	g_scArrTxComSoft[3]='2';			// Comando	
	g_scArrTxComSoft[4]='0';
	g_scArrTxComSoft[5]=ETX;		   	//
	g_scArrTxComSoft[6]=0x01;		   	// BCC
	EscribirCadenaSoft(7);
//--------------------------------------------------------------------
	g_cEstadoComSoft=ESPERA_RX;
	g_cEstadoComSeqMF=SEQ_RTA_EJECT;
	ValTimeOutCom=TIMWPoll;
	ID1=ID2=ID3=ID4=0;
}

/*--------------------------------------------------------------------------
procedimiento
se envia el cmd check status desde el mc
	el lector creator  debe  responder ASK de recibido 
	y el MC de confirmar con un ENQ
	al tener este dato genera la respuesta 02,00,03,31,30,status(S),03,bcc
	s=N (0x4e) no hay tarjeta en el lector
	s=Y (0x59) tarjeta ene el lector
	-------------------------------------------------------------------------*/
void Get_Status(void)	 
{

	g_scArrTxComSoft[0]=STX; 			// Caracter de Inicio
	g_scArrTxComSoft[1]=0x00;			// Longitud (2 Bytes)
	g_scArrTxComSoft[2]=0x02;			//
	g_scArrTxComSoft[3]='1';			// Comando	
	g_scArrTxComSoft[4]='0';		   	// 	
	g_scArrTxComSoft[5]=ETX;		   	//
	g_scArrTxComSoft[6]=0x02;		   	// BCC
	EscribirCadenaSoft(7);
	g_cEstadoComSoft=ESPERA_RX;
	ValTimeOutCom=TIMWPoll;
}
//***********************************************************************************************************
/*capture S/n el numero de la serie of mifare 1 card 02,00,02,35,31,03 bcc*/
void Select_Card(void)	  				
{
	g_scArrTxComSoft[0]=STX; 			// Caracter de Inicio
	g_scArrTxComSoft[1]=0x00;			// Longitud (2 Bytes)
	g_scArrTxComSoft[2]=0x02;			//
	g_scArrTxComSoft[3]='5';		   	// 	
	g_scArrTxComSoft[4]='1';		   	//
	g_scArrTxComSoft[5]=ETX;		   	//
	g_scArrTxComSoft[6]=0x07;		   	// BCC
	EscribirCadenaSoft(7);
	g_cEstadoComSoft=ESPERA_RX;
	ValTimeOutCom=TIMWPoll;
}

/*-----------------------------------------------------------------
check password se envia 02,00,09,35,32,sector,6byte de password, 03,bcc
se programa el pasword en la tarjeta si es ok  debe responder
02,00,03,35,32,sector,status,03,bcc
status=Y=0x59 que es el ok del password
status=N=0x4e error en el password
status=E=0x45 no hay tarjeta en el lector

-------------------------------------------------------------------*/
void LoadPass(void)	  			
{

  g_scArrTxComSoft[0]=STX; 			// Caracter de Inicio
	g_scArrTxComSoft[1]=0x00;			// Longitud (2 Bytes)
	g_scArrTxComSoft[2]=0x09;			//
	g_scArrTxComSoft[3]='5';		   	// 	
	g_scArrTxComSoft[4]='2';		   	//
	g_scArrTxComSoft[5]=0x01;		   	// SECTOR 1	 

	g_scArrTxComSoft[6]=MEMORY(0011,0011);
	g_scArrTxComSoft[7]=MEMORY(0101,0110);
	g_scArrTxComSoft[8]=MEMORY(0011,0000);
	g_scArrTxComSoft[9]=MEMORY(0111,0000);
	g_scArrTxComSoft[10]=MEMORY(0011,0100);
	g_scArrTxComSoft[11]=MEMORY(0111,0010);

	g_scArrTxComSoft[12]=ETX;		   	//
	g_scArrTxComSoft[13]=0x6d;		   	// BCC
	EscribirCadenaSoft(14);

	g_cEstadoComSoft=ESPERA_RX;
	ValTimeOutCom=TIMWPoll;
}
//***********************************************************************************************************
/*---------------------------------------------------------------------------------------------------------
envio el segundo password de la tarjeta MF50
-----------------------------------------------------------------------------------------------------------*/
void LoadPass2(void)	  			
{

  g_scArrTxComSoft[0]=STX; 			// Caracter de Inicio
	g_scArrTxComSoft[1]=0x00;			// Longitud (2 Bytes)
	g_scArrTxComSoft[2]=0x09;			//
	g_scArrTxComSoft[3]='5';		   	// 	
	g_scArrTxComSoft[4]='2';		   	//
	g_scArrTxComSoft[5]=0x02;		   	// SECTOR 2	 

	g_scArrTxComSoft[6]=MEMORY(0100,0001);
	g_scArrTxComSoft[7]=MEMORY(0110,0011);
	g_scArrTxComSoft[8]=MEMORY(0101,0011);
	g_scArrTxComSoft[9]=MEMORY(0100,0101);
	g_scArrTxComSoft[10]=MEMORY(0111,0110);
	g_scArrTxComSoft[11]=MEMORY(0101,0000);


	g_scArrTxComSoft[12]=ETX;		   	//
	g_scArrTxComSoft[13]=0x1F;		   	// BCC
	EscribirCadenaSoft(14);

	g_cEstadoComSoft=ESPERA_RX;
	ValTimeOutCom=TIMWPoll;
}
/*----------------------------------------------------------------------------------------------------------
read  data on sector
se lee el bloque de la tarjeta Mf 
host 02,00,04,35,33,sector,bloque,etx,bcc
respuesta
02,00,15,35,33,sector,bloque,status,16 byte hex,03,bcc
status=Y=0x59 que es el ok de lectura de la tarjeta mf50
falla
02,00,05,35,33,sector,bloque,status,03,bcc
status=N=0x4e error en la lectura
status=E=0x45 no hay tarjeta en el lector

------------------------------------------------------------------------------------------------------------*/
void Lea_S1_Block(unsigned char Bloque)	 
{
	/*read data on sector 02,00,04,35,33,sector,bloque,etx,bcc*/
	unsigned char BCC, j;
	BCC=0;

	g_scArrTxComSoft[0]=STX; 			// Caracter de Inicio
	g_scArrTxComSoft[1]=0x00;			// Longitud (2 Bytes)
	g_scArrTxComSoft[2]=0x04;			//
	g_scArrTxComSoft[3]='5';			// Comando	
	g_scArrTxComSoft[4]='3';			// 	
	g_scArrTxComSoft[5]=0x01;			// SECTOR 1 				
	g_scArrTxComSoft[6]=Bloque;			// 
	g_scArrTxComSoft[7]=ETX;		   	//
	
	for (j=0; j<8; j++)
	{
	 	BCC=BCC^g_scArrTxComSoft[j];
	}
	
	g_scArrTxComSoft[8]=BCC;		   	// BCC
	EscribirCadenaSoft(9);

   	g_cEstadoComSoft=ESPERA_RX;
	ValTimeOutCom=TIMWPoll;
}

//***********************************************************************************************************

//***********************************************************************************************************
void Select_S2_Block(unsigned char Bloque)	 
{
	unsigned char BCC, j;
	BCC=0;

	g_scArrTxComSoft[0]=STX; 			// Caracter de Inicio
	g_scArrTxComSoft[1]=0x00;			// Longitud (2 Bytes)
	g_scArrTxComSoft[2]=0x04;			//
	g_scArrTxComSoft[3]='5';			// Comando	
	g_scArrTxComSoft[4]='3';			// 	
	g_scArrTxComSoft[5]=0x02;			// SECTOR 2 				
	g_scArrTxComSoft[6]=Bloque;			// 
	g_scArrTxComSoft[7]=ETX;		   	//
	
	for (j=0; j<8; j++)
	{
	 	BCC=BCC^g_scArrTxComSoft[j];
	}
	
	g_scArrTxComSoft[8]=BCC;		   	// BCC
	EscribirCadenaSoft(9);

   	g_cEstadoComSoft=ESPERA_RX;
	ValTimeOutCom=TIMWPoll;
}
/*----------------------------------------------------------------------------------------------------------
rutina que escribe datos en un sector
host stx,null,0x14,35,34,sector,bloque,16 byte hex data , etx,bcc
reader
stx,null,0x15,35,34,sector no,bloque No,P,16 byte hex data, etx,bcc

P='Y' 0(0x59) ok
P='N'=(0x4e) error
P='E'=(0x45) no hay tarjeta en el lector
------------------------------------------------------------------------------------------------------------*/

void Graba_S1_BloqueSel(unsigned char Bloque)	 
{
	unsigned char BCC, j;

	BCC=0;

	g_scArrTxComSoft[0]=STX; 			// Caracter de Inicio
	g_scArrTxComSoft[1]=0x00;			// Longitud (2 Bytes)
	g_scArrTxComSoft[2]=0x14;			//
	g_scArrTxComSoft[3]='5';			// Comando	
	g_scArrTxComSoft[4]='4';			// 	
	g_scArrTxComSoft[5]=0x01;			// SECTOR 1 				
	g_scArrTxComSoft[6]=Bloque;			// 

	for (j=0; j<16; j++)
	{
	 	g_scArrTxComSoft[j+7]=BufferWrite_MF[j];
	}

 	g_scArrTxComSoft[23]=ETX;		   	//
	
	for (j=0; j<24; j++)
	{
	 	BCC=BCC^g_scArrTxComSoft[j];
	}
	
	g_scArrTxComSoft[24]=BCC;		   	// BCC
	EscribirCadenaSoft(25);

   	g_cEstadoComSoft=ESPERA_RX;
	ValTimeOutCom=TIMWPoll;
}
/*FUNCIONES DEL QR*/
/*---------------------------------------------------------------------------------
funcion que debuelve la posicion del inicio del primer caracter de numerico de 0 a 9
-----------------------------------------------------------------------------------*/
unsigned char num_num(unsigned char * p)	
{
unsigned char contador=0;
	while ((*p < 0x30)|| (*p > 0x39))
	{
		
		p++;
		contador++;
	}
	return contador;
}		
/*---------------------------------------------------------------------------------
definiciones de la pantalla
-----------------------------------------------------------------------------------*/
unsigned char num_char(unsigned char * p,unsigned char chr)	
{
unsigned char contador=0;
	while (*p !=chr)
	{
		
		p++;
		contador++;
	}
	return contador;
}	