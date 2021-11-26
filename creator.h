//*******************************************************************************************
//		LPR, Verificador Nuevo Transporte													*
//*******************************************************************************************
#include <at89c51xd2.h>             // define 8051 registers 	
	
#include <stdio.h>					//														*
#include <INTRINS.H>				//														*
#include <math.h> 
//*******************************************************************************************
//	DEFINICION DE IO DEL MICROCONTROLADOR													*
//*******************************************************************************************
sbit rx_IP = P0^0;					//														*
sbit audio_s2 = P0^1;				//
sbit Atascado = P0^3;				//
sbit led_err = P0^2;				//														*

sbit audio_s0 = P0^4;				//
sbit audio_s1 = P0^5;				//
sbit rx_in_data = P0^6;				//Indicador de Rx Transporte o Lectura Wiegand			*
sbit sel_com = P0^7;				//Micro switch											*

sbit txd2 = P1^0;					//Transmision Aux Datos	IP								*
sbit DataIn = P1^1;					//														*

sbit lock = P1^7;					//Relevo 												*

sbit sel_A = P3^5;					//Pulsador												*
sbit sel_B = P3^6;					//Entrada Sensor 2										*
sbit sel_C = P3^7;					//Entrada Sensor 1										*

sbit port_clk = P3^4;				//Recepcion AUX											*
sbit busy = P3^3;  					//Entrada Interrupcion del Procesador principal			*
sbit ready = P3^2;					//Salida. solicitud envio Datos							*

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************		
//		DEFINICIÓN DE TIPOS
//*******************************************************************************************
//*******************************************************************************************

 unsigned char xdata g_scDATE[10]; 
 unsigned char xdata sector6[16];
 unsigned char xdata sector5[16];
 unsigned char xdata buffer_ticket[60];
//------------------------------------------------------------------------------------------*
 unsigned char g_cCodSeqMF='0';
//------------------------------------------------------------------------------------------*
#define TAMANO_RX_COM_SOFT		50			//50
#define TAMANO_TX_COM_SOFT		50

//------------------------------------------------------------------------------------------*
#define SEQ_INICIO				0X00	
#define SEQ_TEMPORAL			0X01
#define SEQ_ESPERA				0X02
#define SEQ_RTA_STATUS			0X03
#define SEQ_RTA_EJECT			0X04
#define SEQ_RTA_LOADKEY  		0x05
#define SEQ_RTA_SELECT  		0x06
#define SEQ_LOCK		 		0x07
#define SEQ_RTA_rdB5	 		0x08
#define SEQ_RTA_rdB6	 		0x09
#define SEQ_RTA_rdB4	 		0x0a
//#define SEQ_RTA_wrB6	 		0x0b
#define SEQ_WCMD		 		0x0C
#define SEQ_RTA_RST		 		0x0d

#define SEQ_RTA_STATUS2			0X0f
#define SEQ_RTA_WR				0X10


#define SEQ_RTA_KEY2			0x19
#define SEQ_RTA_STATUSout		0x1e
#define SEQ_WAIT_EJECT			0x1F
#define SEQ_SEL_B6w				0x20

#define SEQ_STATUS_OUT			0x24
#define SEQ_RETRY				0x25

#define SEQ_RETRY_ANTENA_OFF	0x26
#define SEQ_RETRY_KEY			0x27

#define SEQ_RETRY_CARD_INSIDE	0X2D
#define SEQ_RTA_STATUS_WR_B6	0X2E

#define SEQ_RTA_LOADKEY2		0x2f

#define SEQ_LPR					0x31

#define SEQ_RTA_SELECT2			0x32
#define SEQ_RTA_STATUS_WR_B5	0X33
#define SEQ_LEE_ID_BACKUP		0x34
#define SEQ_BRTA_ID_BACKUP   0x35
#define SEQ_RTA_STATUS_WR_B7	0x36
#define SEQ_RTA_WR_2					0x37
#define SEQ_RETRY_KEY_2				0x38

//ESTADOR RECEPCION SOFTWARE

#define ESPERA_RX				0
#define ESPERA_INICIO_RTA		1
#define RX_CONTADOR_H			2
#define RX_CONTADOR_L			3
#define SAVE_STR_SOF			4
#define RX_BCC					5

#define CARD_INACTIVA		0x00
#define CARD_ROTACION 		0x01
#define CARD_MENSUALIDAD	0x02
#define CARD_LOCATARIO		0x05
#define CARD_MULTIPLE		0x0F
#define CARD_COMPARTIDA		0x0A
#define CARD_COMPRA_DCTO	0x0c
#define EMITIDA_PERDIDA		0X10


//ERRORES
#define SELECCION_SECTOR 		0
#define COMANDO_WR		 		1
#define NO_RTA_WR		 		2
#define COMANDO_RD		 		3
#define NO_RTA_RD		 		4
#define FALLA_WR 				5
#define NO_CARD 				6

//#define T_WBYTE		30
#define T_WAIT		20
#define TIMWPoll 	150
					
#define TIMConsulta 1200

//ESTADOR TRANSMICION SOFTWARE

#define SIN_LECTURA_TX	0x00
#define LECTURA_COD_TX	0x01

#define ENQ	0x05
#define EOT	0x04
#define ACK	0x06
#define STX	0x02
#define ETX	0x03
#define CR	0x0d
#define LF	0x0a
#define START_CHR 0x68

// Errores MIFARE  2:RequestMifare  3:Error de Autenticacion 4:Error de Lectura de BloquE

#define SIN_ERROR		0x30		// NO ERROR
#define ERR_AUTH		0x31		// ERROR AUTHENTICATION
#define ERR_RD			0x32		// NO SE PUDO LEER INFO
#define ERR_WR			0x33		//
#define ERR_EEPROMK		0x34 		// ERROR AL GRABAR EN LECTOR
#define ERR_LECTOR		0x35		// RESPUESTA INCORRECTA DEL LECTOR
#define ERR_EJECT		0x36		// ERROR EN EXPULSAR
#define ERR_APB			0x37		// NO POSEE INGRESO
#define ERR_COD			0x38		// TARJETA NO ES DE ESTE PARQUEADERO
#define DESARM			0x39 		// CAJERO DESARMADO / NO PUEDE LIQUIDAR TARJETA DE ROTACION
#define ROTACION		0x3A	  	// TARJETA DE ROTACION
#define ERR_CARD_TIPO	0X3B   		// ERROR TIPO DE TARJETA (STATUS)
#define ERR_COM			0X3c   		// ERROR TIPO DE TARJETA (STATUS)
#define CARD_EJECT		0X3D   		// ERROR TIPO DE TARJETA (STATUS)
#define CARD_EJECT_WR	0X3E   		// ERROR TIPO DE TARJETA (STATUS)
#define ERR_SELECT		0X3F   		// ERROR TIPO DE TARJETA (STATUS)
#define ERR_NO_CARD		0X40   		// ERROR NO HAY TARJETA PRESENTE
#define ERR_TIME_OUT	0X41   		// ERROR NO HAY TARJETA PRESENTE

#define CTE_SEG 0X1C						
#define g_cDirBoard 0x30

#define KeyA 0x60
#define KeyB 0x61
#define SECTOR 0X01


//#define 	RBUF_SIZE   				128



//-------------------------------------------------------------------------------------------*
// 	01: Central Parking
//
//-------------------------------------------------------------------------------------------*
const unsigned char ID_CLIENTE=	0;		//					
const unsigned char  COD_PARK=	0	;	//					
//-------------------------------------------------------------------------------------------*

//-------------------------------------------------------------------------------------------*

unsigned char g_scArrRxComSoft[TAMANO_RX_COM_SOFT];
 unsigned char g_scArrTxComSoft[TAMANO_TX_COM_SOFT];
 unsigned char BufferWrite_MF[16];
 unsigned char BufferRead_MF[16];
 unsigned char g_scDATE_Liq[10];

 unsigned char RetryWrite_MF[16];
 unsigned char COD_PARK_R;
 unsigned char ID_CLIENTE_R;

 unsigned int Tmin=1500;

 unsigned char g_cEstadoComSeqMF=SEQ_ESPERA;

 unsigned char g_cEstadoComSoft;
 unsigned char g_cContByteRx=0;
 unsigned int ValTimeOutCom=0;
 unsigned char Num_Datos=0;

 unsigned char num_data=0;
 unsigned char Block;

 unsigned char Cod_Dcto;
 unsigned char seg;
 unsigned char RetryWR=0;

 unsigned char ID1,ID2,ID3,ID4; 
 unsigned char centena, decena, unidad;
 unsigned char  num_data;
 unsigned year, month, day, hour, minut;
 unsigned char TipoVeh=0;
 unsigned char ValDCTO_INPAGO;
 unsigned char cod_alarm;
 unsigned char ERR;
 unsigned char TipoCard;

 unsigned char Val_Horario;
 unsigned char Val_PicoPlaca;
 unsigned char APB_CARD;
 unsigned char TIME_ESPERA=0;
 unsigned char NumLiq=0;

//------------------------------------------------------------------------------------------*

 bit buffer_ready=0;
 bit blink=0;
 bit cmd_comp=0;
 bit Permite_Puerta=0;
 bit Permite_Monedero=0;
 bit Permite_Billetero=0;
 bit timeOut=0;
 bit alarma1=0;
 bit alarma2=0;
 bit alarma3=0;
 bit onceAlarm=0;
 bit Desarmado=0;
 bit NotifyPuerta=0;
 bit NotifyCofreB2B=0;
 bit NotifyCofreCoin=0;
 bit NotifyP4=0;
 bit GrabaTarjeta=0;
 bit AlarmaPrevia1=0;
 bit AlarmaPrevia2=0;
 bit AlarmaPrevia3=0;
 bit FueraServicio=0;
 bit Expulsa_Grabacion=0;
 bit ConsultaSoft=0;
 bit Toggle=0;
 bit ErrorInCOM=0;
 bit sendactive=0;
 bit EmisionXperdida=0;
 bit Activo=0;
 bit MultiPark=0;
 bit ErrorWR_CARD=0;
 bit PrintLPR=0;
 bit TxENQ=0;
 bit Comparacion=0;
 bit esQR=0;															//puerto de creator en 0 y pto de qr 1

 const bit PuertoMF=0;
 const bit PuertoDB9=1;



#include <ptoprlelo.h> 
#include "crt_288.h"	
#include "debug.h"	
#include <uart.h> 
#include <lector.h> 