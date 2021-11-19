#include <uart.h> 
#include <reg51.h>

sbit sel_com = P0^7;				//Micro switch											*

//ESTADOR RECEPCION SOFTWARE

#define ESPERA_RX				0
#define ESPERA_INICIO_RTA		1
#define RX_CONTADOR_H			2
#define RX_CONTADOR_L			3
#define SAVE_STR_SOF			4
#define RX_BCC					5
#define RX_QR						6


#define ENQ	0x05
#define EOT	0x04
#define ACK	0x06
#define STX	0x02
#define ETX	0x03
#define CR	0x0d
#define LF	0x0a
#define START_CHR 0x68

#define TAMANO_RX_COM_SOFT		50			//50
#define TAMANO_TX_COM_SOFT		50
#define RBUF_SIZE   128
extern unsigned char g_scArrRxComSoft[];
extern unsigned char g_scArrTxComSoft[];
extern unsigned char g_cEstadoComSoft;
extern unsigned char g_cContByteRx;



extern bit sendactive;
extern bit buffer_ready;
extern bit TxENQ;
extern const bit PuertoMF;
extern bit esQR;
/*funciones externas*/
extern void tx_auxH(unsigned char caracter);

//------------------------------------------------------------------------------------------*
//*******************************************************************************************
//																							*
// Function to initialize the serial port and the UART baudrate.							*
//																							*
//*******************************************************************************************
void com_initialize (void) 
{
  	sendactive = 0; 			        // transmitter is not active  						*
  	SCON = 0x50;			         	// serial port MODE 1, enable serial receiver		*
	PCON |= 0x80; 			         	// 0x80=SMOD: set serial baudrate doubler			*
  	TMOD |= 0x20; 			         	// put timer 1 into MODE 2							*
 	TCON = 0x40;    					// TCON 											*
//------------------------------------------------------------------------------------------*
// 	TH1 =  0xFF;    					// TH1 	115000 Bps @ 22.148MHZ						*
// 	TL1 =  0xFF;    					// TH1 												*
//------------------------------------------------------------------------------------------*
 	TH1 =  0xF4;    					// TH1 9600 Bps @ 22.148MHZ							*					 
	TL1 =  0xF4;    					// TH1 												*
//------------------------------------------------------------------------------------------*
// 	TH1 =  0xFa;    					// TH1 12000 Bps @ 22.148MHZ						*					 
//	TL1 =  0xFa;    					// TH1 												*
//------------------------------------------------------------------------------------------*
//  TH1 = (unsigned char) (256 - (XTAL / (16L * 12L * baudrate)));							*
//------------------------------------------------------------------------------------------*
   	TR1 = 1; 							// start timer 1
  	ES = 1; 							// enable serial interrupts
}


void EscribirCadenaSoft(unsigned char tamano_cadena)
{
unsigned char i;
 
	for(i=0;i<tamano_cadena;i++)
   	{
        tx_chr(g_scArrTxComSoft[i]);
    }

}

//***************************************************************************************************
// 	Transmision de un caracter																		*
//***************************************************************************************************
 void tx_chr (unsigned char data_com)
 {

  SBUF=data_com;
	sendactive=1;
	while (sendactive==1) 
	{
	}
 }
 
 //***************************************************************************************************
static void com_isr (void) interrupt 4 using 1
{
 unsigned char cDatoRx;
	if (RI) 
	{
	  	cDatoRx = SBUF; 			          		// read character
	  	RI = 0; 													// clear interrupt request flag
	
			
		switch (g_cEstadoComSoft)
		{
//---------------------------------------------------------------------------------------------------
			case ESPERA_RX:
			
				if(cDatoRx==ACK)
				{
					TxENQ=1;
				}
				else 
				{
					esQR=1;
					sel_com=PuertoMF;
					g_cContByteRx=0;
					g_scArrRxComSoft[g_cContByteRx]=cDatoRx;
					g_cContByteRx++;
					g_scArrRxComSoft[g_cContByteRx]=0;
					
					g_cEstadoComSoft=RX_QR	;
				}

			break;
//------------------------------------------------------------------------------------------------
			case ESPERA_INICIO_RTA:

				if(cDatoRx==STX)
				{
					g_cContByteRx=0;
					buffer_ready=0;
					g_scArrRxComSoft[g_cContByteRx++]=cDatoRx;
					g_cEstadoComSoft=RX_CONTADOR_H;
				}
				
	
			break;
//------------------------------------------------------------------------------------------------
			case RX_CONTADOR_H:

				g_scArrRxComSoft[g_cContByteRx++]=cDatoRx;
				g_cEstadoComSoft=RX_CONTADOR_L;

	
			break;
//------------------------------------------------------------------------------------------------
			case RX_CONTADOR_L:

				g_scArrRxComSoft[g_cContByteRx++]=cDatoRx;
				g_cEstadoComSoft=SAVE_STR_SOF;
		
			break;
//------------------------------------------------------------------------------------------------
			case SAVE_STR_SOF:

				g_scArrRxComSoft[g_cContByteRx++]=cDatoRx;

				if(g_cContByteRx>TAMANO_RX_COM_SOFT)
				{
					g_cEstadoComSoft=ESPERA_RX;
				}
				else if(cDatoRx==ETX)
				{
					g_cEstadoComSoft=RX_BCC;
				}
			break;
//-------------------------------------------------------------------------------------------------
			case RX_BCC:

				g_scArrRxComSoft[g_cContByteRx++]=cDatoRx;
				buffer_ready=1;
	
			break;
			case RX_QR:
				
					
				if (g_cContByteRx < RBUF_SIZE)
				{
					if(cDatoRx=='<')
					{
						buffer_ready=1;
						g_cContByteRx++;
						g_scArrRxComSoft[g_cContByteRx]=0;
						break;
					}
					else 
					{
						g_scArrRxComSoft[g_cContByteRx]=cDatoRx;		/*recivo los caracteres del qr*/
						g_cContByteRx++;
						g_scArrRxComSoft[g_cContByteRx]=0;
						g_cEstadoComSoft=RX_QR	;
						
					}
				}
				else
				{
					g_cContByteRx=0;
					g_cEstadoComSoft=ESPERA_RX;
				}
		break;
			
//-------------------------------------------------------------------------------------------------
			default:
				g_cEstadoComSoft=ESPERA_RX;
			break;
		} //switch (g_cEstadoComSoft) 	  
	}
	
//--------------------------------------------------------------------------------------------------*
// Transmitted data interrupt. 																		*
//--------------------------------------------------------------------------------------------------*
  	if (TI != 0) 
	{
    	TI = 0; 								// clear interrupt request flag
		sendactive=0;
  	}
}

