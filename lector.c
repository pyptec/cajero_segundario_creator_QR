
#include <lector.h> 
#include <string.h>
#include <reg51.h>


sbit lock = P1^7;					//Relevo 		
sbit busy = P3^3;  					//Entrada Interrupcion del Procesador principal			*
sbit ready = P3^2;					//Salida. solicitud envio Datos		
sbit port_clk = P3^4;				//Recepcion AUX			
sbit rx_in_data = P0^6;				//Indicador de Rx Transporte o Lectura Wiegand			*
sbit sel_com = P0^7;				//Micro switch	
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
#define SEQ_RTA_rdB4_Comercio	0x39
//estado de la lectura QR
#define SEQ_QR_WAIT						0x3a
#define SEQ_QR_INICIO					0x3b	
#define SEQ_QR_PTPRL					0x3c
#define SEQ_QR_WCMD						0x3d
#define SEQ_RECARGA_PASSWORD	0x3e


//ESTADOR RECEPCION SOFTWARE

#define ESPERA_RX				0
#define ESPERA_INICIO_RTA		1
#define RX_CONTADOR_H			2
#define RX_CONTADOR_L			3
#define SAVE_STR_SOF			4
#define RX_BCC					5

#define RX_QR						6

#define T_WAIT		20
#define TIMConsulta 1200
#define TIMWPoll 	100
#define 	TIME_RX						200		//
#define CTE_SEG 0X1C				

#define STX	0x02
#define ETX	0x03
#define CR	0x0d
#define LF	0x0a
#define ACK	0x06


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

extern unsigned char g_cEstadoComSoft;
extern unsigned int ValTimeOutCom;
extern unsigned char g_cEstadoComSeqMF;
extern unsigned char g_scArrRxComSoft[];
extern unsigned char ID1,ID2,ID3,ID4; 
extern unsigned char centena, decena, unidad;
extern unsigned char TipoCard;
extern unsigned char COD_PARK_R;
extern unsigned char ID_CLIENTE_R;
extern unsigned char xdata buffer_ticket[];
extern  unsigned char xdata sector6[];
extern  unsigned char xdata sector5[];
extern unsigned char Block;
extern const unsigned char ID_CLIENTE;		//					
extern const unsigned char  COD_PARK;
extern unsigned char  num_data;
extern unsigned char xdata g_scDATE[];
extern unsigned char Cod_Dcto;
extern unsigned char TipoVeh;
extern unsigned char Val_Horario;
extern unsigned char Val_PicoPlaca;
extern unsigned char ValDCTO_INPAGO;
extern unsigned char APB_CARD;
extern unsigned char g_scDATE_Liq[];
extern unsigned char TIME_ESPERA;
extern unsigned char NumLiq;
extern  unsigned year, month, day, hour, minut;
extern  unsigned char BufferWrite_MF[];
extern unsigned char RetryWrite_MF[];
extern unsigned char RetryWR;

extern unsigned char g_cContByteRx;



extern bit buffer_ready;
extern bit Desarmado;
extern bit Permite_Puerta;
extern bit Permite_Monedero;
extern bit Permite_Billetero;
extern bit FueraServicio;
extern  bit EmisionXperdida;
extern bit GrabaTarjeta;
extern bit MultiPark;
extern bit timeOut;
extern bit PrintLPR;
extern bit NotifyCofreB2B;
extern bit NotifyCofreCoin;
extern  bit NotifyPuerta;
extern bit AlarmaPrevia1;
extern bit AlarmaPrevia2;
extern bit AlarmaPrevia3;
extern bit alarma1;
extern bit alarma2;
extern bit alarma3;
extern bit onceAlarm;
extern bit ConsultaSoft;
extern bit Expulsa_Grabacion;
extern bit Comparacion;

extern const bit PuertoMF;
extern const bit PuertoDB9;
extern bit esQR;

extern void Get_Status(void)	;
extern void Debug_txt_Tibbo(unsigned char * str);
extern void Select_Card(void);
extern void tx_aux(unsigned char caracter);
extern void Ve_Hex(unsigned char valorhex);

extern void LoadPass(void);
extern void LoadPass2(void)	 ;
extern void Lea_S1_Block(unsigned char Bloque)	 ;
extern void Select_S2_Block(unsigned char Bloque)	;
extern void Graba_S1_BloqueSel(unsigned char Bloque)	;
extern void eject_card ();
extern void Eject_Card(unsigned char CodError);
extern bit Send_Port(unsigned char NumChar);
extern void tx_auxH(unsigned char caracter);
extern void Hex_Str(unsigned char valorhex);
extern unsigned char two_one (unsigned char byte_h,unsigned char byte_l);
extern void posee_in_out();
extern void ve_id(unsigned char id_h,unsigned char id_l);
extern void Debug_Dividir_texto();
extern void recibe_port(void);
extern unsigned char bcd_hex (unsigned char l_data);
extern void ErrorWR(unsigned char TipoERR);
extern void Info_Retry(void);
extern void wait_long (void);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char);
extern unsigned char num_num(unsigned char * p);	
extern unsigned char num_char(unsigned char * p,unsigned char chr);	
//*******************************************************************************************************************************

/*----------------------------------------------------------------------------------------------------
		RECIBE INFORMACION DEL PROCESADOR PRINCIPAL POR P2			
-----------------------------------------------------------------------------------------------------*/

bit Send_Port(unsigned char NumChar)
{
  	unsigned char j; 
	long int cont;
	
	port_clk=1;	 						//El que transmite debe fijar primero el Clk en 1
	rx_in_data=0;	 					//Led de visualizacion 	ON
	timeOut=0;
  ready=0;							//Genera interrupcion al Principal
	cont=50000;
 	while ((busy==1)&&(timeOut==0)) 	//Espera reconocimiento INT por entrada busy
	{
		cont--;
		if (cont==0)
		{
			timeOut=1;
		}
	}
	if ((timeOut==0)&&(busy==0))
	{
		for (j=0; j<NumChar; j++)
		{
   		P2=buffer_ticket[j];
			port_clk=0;
			wait_long();
			port_clk=1;
			wait_long();
		}
 	}
	P2=0XFF;
 	ready=1;
	port_clk=1;
	rx_in_data=1;					//Led de visualizacion 	OFF
  wait_long(); 	

	return timeOut;
}
void ProcesoLector(void)
{
 
 		unsigned char j;
		unsigned char temp,temp2;
		unsigned char *tipo_vehiculo;
		static unsigned char BID1=0,BID2=0,BID3=0,BID4=0,vehiculo,contador_QR=0;
		static unsigned char Ticket[10];
		static unsigned char  fecha[17]; 
 		switch (g_cEstadoComSeqMF)
		{
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_INICIO:									/*tiempo de espera*/
				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
					g_cEstadoComSeqMF=SEQ_ESPERA;
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=T_WAIT;
					buffer_ready=0;
				}
			break;
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_ESPERA:

				if (ValTimeOutCom==1)
				{
					g_cEstadoComSeqMF=SEQ_RTA_STATUS;
				   	Get_Status();												/*pregunta por el estado del lector*/
				}

			break;
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_RTA_STATUS:
			
				if (buffer_ready==1)
				{
					buffer_ready=0;
					if (g_scArrRxComSoft[5]=='Y')												/*tenemos presencia de tarjeta en la antena*/
					{
						Debug_txt_Tibbo((unsigned char *) "TARJETA LOCK\r\n");
						/*consulto el numero de la serie de la tarjeta*/
						Select_Card();																		
						g_cEstadoComSeqMF=SEQ_RTA_SELECT;
						Debug_txt_Tibbo(g_scArrRxComSoft);
					}
					else
					{
						g_cEstadoComSeqMF=SEQ_ESPERA;											/*no hay tarjeta probamos si hay QR*/
						g_cEstadoComSoft=ESPERA_RX;
							ValTimeOutCom=T_WAIT;
						/*Debug_txt_Tibbo(g_scArrRxComSoft);					
						sel_com=PuertoMF;	
						esQR=	1;					
						g_cEstadoComSeqMF=SEQ_QR_WAIT;											/*no hay tarjeta probamos si hay QR*/
						//g_cEstadoComSoft=RX_QR;
						//ValTimeOutCom=T_WAIT;																	/*TIME_RX;*/
					}
 				}
				else if (ValTimeOutCom==1)
				{	
					esQR=	0;	
					sel_com=PuertoDB9;
					g_cEstadoComSeqMF=SEQ_ESPERA;
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=T_WAIT;
				}


			break;
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_RTA_SELECT:
				if (buffer_ready==1)
				{
					buffer_ready=0;
					if (g_scArrRxComSoft[5]=='Y')
					{
						/*Y= tiene el numero de la tarjeta
							N= fallo el numero de tarjeta
							E= no hay tarjeta
						*/
						ID1=g_scArrRxComSoft[6];											/*numero de serie de la tarjeta enviado a tcp/ip*/
						ID2=g_scArrRxComSoft[7];
						ID3=g_scArrRxComSoft[8];
						ID4=g_scArrRxComSoft[9];

						tx_aux('C');
						tx_aux('A');
						tx_aux('R');
						tx_aux('D');
						tx_aux(':');
						tx_aux(' ');

						Ve_Hex(g_scArrRxComSoft[6]); 			// 
						tx_aux(decena);
						tx_aux(unidad);
						tx_aux(' ');
						
						Ve_Hex(g_scArrRxComSoft[7]); 			// 
						tx_aux(decena);
						tx_aux(unidad);
						tx_aux(' ');						

						Ve_Hex(g_scArrRxComSoft[8]); 			// 
						tx_aux(decena);
						tx_aux(unidad);
						tx_aux(' ');

						Ve_Hex(g_scArrRxComSoft[9]); 			// 
						tx_aux(decena);
						tx_aux(unidad);
					   	tx_aux(' ');

						tx_aux(CR);
						tx_aux(LF);														
						
						LoadPass();																			/*envia el password al lector */
						g_cEstadoComSeqMF=SEQ_RTA_LOADKEY;
						buffer_ready=0;						
												
					}
					else
					{
						Debug_txt_Tibbo((unsigned char *) "ERROR CARD\r\n");
							
						Eject_Card(ERR_SELECT);;
 					}
 
				}
				else if (ValTimeOutCom==1)
				{
					Debug_txt_Tibbo((unsigned char *) "SIN RTA. SELECT\r\n");
					Eject_Card(ERR_RD);

				}

			break;
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_RTA_LOADKEY:

				if (buffer_ready==1)
				{
 					buffer_ready=0;
					if (g_scArrRxComSoft[6]=='Y')					/*password ok*/
					{
						/*ok password*/
					 	Lea_S1_Block(1);										/*leo el bloque 1 de la tarjeta MF50*/
						g_cEstadoComSeqMF=SEQ_RTA_rdB5;
											
					}
					else
					{
						Debug_txt_Tibbo((unsigned char *) "ERROR KEY\r\n");
						Eject_Card(ERR_AUTH);
 					}


  				}
				else if (ValTimeOutCom==1)
				{
					Debug_txt_Tibbo((unsigned char *) "SIN RTA. KEY\r\n");
					Eject_Card(ERR_AUTH);

				}

		   break;
//------------------------------------------------------------------------------------------------------------------------------*
		   case SEQ_TEMPORAL:

			 	lock=1;

		  	break;
//------------------------------------------------------------------------------------------------------------------------------*
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_RTA_rdB5:
/*tengo los datos de la tarjeta sector y bloque 16 byte*/
				if (buffer_ready==1)
				{
					buffer_ready=0;
					if (g_scArrRxComSoft[7]=='Y')					/*lectura ok del bloque 1*/
					{
						//g_scArrRxComSoft[g_cContByteRx]=0;
					MultiPark=1;
						Debug_Dividir_texto()	;				
						Debug_txt_Tibbo((unsigned char *) "\r\ntrama del lector tarjetas\r\n");							/*la respuesta es desconocida*/
						DebugBufferMF(g_scArrRxComSoft,g_cContByteRx);																													/*imprimo la trama recibida*/
						Debug_txt_Tibbo((unsigned char *) "\r\n");
						Debug_Dividir_texto()	;	
						
						Debug_txt_Tibbo((unsigned char *) "tipo de tarjeta:");
						TipoCard=g_scArrRxComSoft[8];
						Ve_Hex(TipoCard);
							tx_aux(decena);
							tx_aux(unidad);
							tx_aux(' ');
							tx_aux(CR);
						  tx_aux(LF);	
						Debug_Dividir_texto()	;	
						Debug_txt_Tibbo((unsigned char *) "ID_CLIENTE_R:");
						ID_CLIENTE_R=g_scArrRxComSoft[9];
						Ve_Hex(ID_CLIENTE_R);
							tx_aux(decena);
							tx_aux(unidad);
							tx_aux(' ');
							tx_aux(CR);
						  tx_aux(LF);	
							Debug_Dividir_texto()	;	
							Debug_txt_Tibbo((unsigned char *) "ID_CLIENTE:");
							tx_auxH(ID_CLIENTE);
							Debug_txt_Tibbo((unsigned char *) "\r\n");
							Debug_Dividir_texto()	;	
						
				
						Debug_txt_Tibbo((unsigned char *) "COD_PARK_R:");
						COD_PARK_R=g_scArrRxComSoft[11];
						Ve_Hex(COD_PARK_R);
							tx_aux(decena);
							tx_aux(unidad);
							tx_aux(' ');	  								

						tx_aux(CR);
						tx_aux(LF);						
						
							Debug_Dividir_texto()	;	
							Debug_txt_Tibbo((unsigned char *) "COD_PARK:");
							tx_auxH(COD_PARK);
							Debug_txt_Tibbo((unsigned char *) "\r\n");
							Debug_Dividir_texto()	;	
						Debug_txt_Tibbo((unsigned char *) "SECTOR1_BLOQUE1: ");	
						for (j=0; j<16; j++)									  // GUARDO SECTOR 5 (STATUS CARD, COD PARK. ID PÁRK, FECHA VENCINIENTO MENSUAL)
						{
							sector5[j]=g_scArrRxComSoft[8+j];
							Ve_Hex(g_scArrRxComSoft[8+j]); 						// 
							tx_aux(decena);
							tx_aux(unidad);
							tx_aux(' ');
						}
						tx_aux(CR);
						tx_aux(LF);	
						
						if (((ID_CLIENTE_R==ID_CLIENTE)&&(MultiPark==1))||((ID_CLIENTE_R==ID_CLIENTE)&&(COD_PARK_R==COD_PARK))||((ID_CLIENTE==0)&&(COD_PARK==0)))
						{
							if ((TipoCard==CARD_ROTACION)||(TipoCard==EMITIDA_PERDIDA)||(TipoCard==CARD_MENSUALIDAD)||(TipoCard==CARD_LOCATARIO))
							{
								if ((Desarmado==0)&&(FueraServicio==0))
								{
									if (TipoCard==EMITIDA_PERDIDA)
									{
										EmisionXperdida=1;
									}
									else
									{
										EmisionXperdida=0;
									}
									g_cEstadoComSeqMF=SEQ_RTA_rdB6;
									g_scArrRxComSoft[7]=0xff;
									buffer_ready=0;

									Block=2;
									Lea_S1_Block(Block);						/*se leee el bloque 2 de la tarjeta MF50*/

								}
								else
								{
									Debug_txt_Tibbo((unsigned char *) "EN MTO. EJECT\r\n");
									
									Eject_Card(DESARM);	 					// Notifica que esta en Mantenimiento. No acepta tarjetas de rotacion
								}

							}

							else if (TipoCard==0x06)						// Tarjeta Servicio Operador
							{

							   	if ((Desarmado==0)&&(GrabaTarjeta==0))
								{
 									GrabaTarjeta=0;
									Permite_Puerta=1;
									Permite_Billetero=0;
									Permite_Monedero=0;
									Desarmado=1;
 								}

							 	buffer_ticket[0]=STX;								// Inicio de transmision
								buffer_ticket[1]='0';								// Dir 0
								buffer_ticket[2]=11;								// Dir 0
								buffer_ticket[3]='T';								// CMD T=Tarjeta Operativa
							    buffer_ticket[4]=':';								// CMD L=lectura
								buffer_ticket[5]='1';								// 1: Operador
								buffer_ticket[6]=ID1;
								buffer_ticket[7]=ID2;
								buffer_ticket[8]=ID3;
								buffer_ticket[9]=ID4;
								buffer_ticket[10]=ETX;								// Fin de trasmision
								Send_Port(11);										// ENVIA AL PRINCIPAL
								g_cEstadoComSeqMF=SEQ_WCMD;
								g_cEstadoComSoft=ESPERA_RX;
								ValTimeOutCom=TIMConsulta;

							}
							else if (TipoCard==0x07)								// Tarjeta Servicio Auditor
							{

 								if((Desarmado==0)&&(GrabaTarjeta==0))
								{
									GrabaTarjeta=0;
									Permite_Puerta=0;
									Permite_Billetero=0;
									Permite_Monedero=0;
							    	Desarmado=1;
								}
							 	buffer_ticket[0]=STX;								// Inicio de transmision
								buffer_ticket[1]='0';								// Dir 0
								buffer_ticket[2]=11;								// Dir 0
								buffer_ticket[3]='T';								// CMD L=lectura
							    buffer_ticket[4]=':';								// CMD L=lectura
								buffer_ticket[5]='2';								// 2: Auditor
								buffer_ticket[6]=ID1;
								buffer_ticket[7]=ID2;
								buffer_ticket[8]=ID3;
								buffer_ticket[9]=ID4;
								buffer_ticket[10]=ETX;								// Fin de trasmision
								Send_Port(11);										// ENVIA AL PRINCIPAL
								g_cEstadoComSeqMF=SEQ_WCMD;
								g_cEstadoComSoft=ESPERA_RX;
								ValTimeOutCom=TIMConsulta;

							}
							else if (TipoCard==0x08)		  						// Tarjeta Servicio Administrador
							{
								
								if ((Desarmado==0)&&(GrabaTarjeta==0))
								{
									GrabaTarjeta=0;
									Permite_Puerta=1;
									Permite_Billetero=1;
									Permite_Monedero=1;
									Desarmado=1;
 								}

							 	buffer_ticket[0]=STX;								// Inicio de transmision
								buffer_ticket[1]='0';								// Dir 0
								buffer_ticket[2]=11;								// Dir 0
								buffer_ticket[3]='T';								// CMD L=lectura
							    buffer_ticket[4]=':';								// CMD L=lectura
								buffer_ticket[5]='3';								// 3: admin
								buffer_ticket[6]=ID1;
								buffer_ticket[7]=ID2;
								buffer_ticket[8]=ID3;
								buffer_ticket[9]=ID4;
								buffer_ticket[10]=ETX;								// Fin de trasmision
								Send_Port(11);										// ENVIA AL PRINCIPAL
								if 	(timeOut==1)
								{
									Send_Port(0X07);
								}
								g_cEstadoComSeqMF=SEQ_WCMD;
								g_cEstadoComSoft=ESPERA_RX;
								ValTimeOutCom=TIMConsulta;

							}
								
							else if ((TipoCard==0xAA)&&(GrabaTarjeta==0))			//&&(Desarmado==0))		// // Tarjeta Servicio Auditor
							{

 								Permite_Puerta=0;
								Permite_Billetero=0;
								Permite_Monedero=0;
							  Desarmado=0;
								GrabaTarjeta=1;
							 	buffer_ticket[0]=STX;								// Inicio de transmision
								buffer_ticket[1]='0';								// Dir 0
								buffer_ticket[2]=11;								// Dir 0
								buffer_ticket[3]='T';								// CMD L=lectura
							  buffer_ticket[4]=':';								// CMD L=lectura
								buffer_ticket[5]='0';								// 2: Auditor
								buffer_ticket[6]=ID1;
								buffer_ticket[7]=ID2;
								buffer_ticket[8]=ID3;
								buffer_ticket[9]=ID4;
								buffer_ticket[10]=ETX;								// Fin de trasmision
								Send_Port(11);										// ENVIA AL PRINCIPAL
								g_cEstadoComSeqMF=SEQ_WCMD;
								g_cEstadoComSoft=ESPERA_RX;
								ValTimeOutCom=TIMConsulta;
							}
							else if ((TipoCard==CARD_COMPRA_DCTO)&&(GrabaTarjeta==0))			//&&(Desarmado==0))		// // Tarjeta Servicio Auditor
							{

								if ((Desarmado==0)&&(FueraServicio==0))
								{
									Debug_txt_Tibbo((unsigned char *) "CARD_COMPRA_DCTO\r\n");
	 								Permite_Puerta=0;
									Permite_Billetero=0;
									Permite_Monedero=0;
								  Desarmado=0;
									GrabaTarjeta=0;
								 	buffer_ticket[0]=STX;								// Inicio de transmision
									buffer_ticket[1]='0';								// Dir 0
									buffer_ticket[2]=11;								// Dir 0
									buffer_ticket[3]='T';								// CMD L=lectura
								  buffer_ticket[4]=':';								// CMD L=lectura
									buffer_ticket[5]='D';								// 2: Auditor
									buffer_ticket[6]=ID1;
									buffer_ticket[7]=ID2;
									buffer_ticket[8]=ID3;
									buffer_ticket[9]=ID4;
									buffer_ticket[10]=':';								// Fin de trasmision
							/*leo el nombre del comercio del descuento*/	
									Lea_S1_Block(0);									// Block(4)
									g_cEstadoComSeqMF=SEQ_RTA_rdB4_Comercio;
									g_cEstadoComSoft=ESPERA_RX;
									ValTimeOutCom=TIMConsulta;
									//Send_Port(11);										// ENVIA AL PRINCIPAL
									//g_cEstadoComSeqMF=SEQ_WCMD;
									//g_cEstadoComSoft=ESPERA_RX;
									//ValTimeOutCom=TIMConsulta;
  
								}
								else
								{
									Debug_txt_Tibbo((unsigned char *) "EN MTO. EJECT\r\n");
									
									Eject_Card(DESARM);	 					// Notifica que esta en Mantenimiento. No acepta tarjetas de rotacion
								}




							}
							else
							{
								
								Ve_Hex(TipoCard);
								tx_aux(decena);
								tx_aux(unidad);
								tx_aux(' ');
								Eject_Card(ERR_CARD_TIPO);	 						// No es tarjeta con STATUS valido
							}

						 }
						else
						{
							//OJO REVISAR
							Debug_txt_Tibbo((unsigned char *) "ERROR COD. PARK\r\n");
							Eject_Card(ERR_COD);
						}
 					}
					else
					{
						Debug_txt_Tibbo((unsigned char *) "SIN RTA. RD B5\r\n");
						Eject_Card(ERR_RD);
				
					}
			
  				}
				else if (ValTimeOutCom==1)
				{
					Debug_txt_Tibbo((unsigned char *) "SIN RTA. RD B5\r\n");
					Eject_Card(ERR_RD);
 				}
			break;
//------------------------------------------------------------------------------------------------------------------------------*
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_RTA_rdB6:

				if (buffer_ready==1)
				{
					buffer_ready=0;
				  	if (g_scArrRxComSoft[7]=='Y')								/*lectura ok del 2 bloque de la memoria MF50*/
					{
						Debug_txt_Tibbo((unsigned char *) "SEQ_RTA_rdB6\r\n");
						
						Debug_txt_Tibbo((unsigned char *) "SECTOR1_BLOQUE2: ");
						for (j=0; j<16; j++)
						{
							sector6[j]=g_scArrRxComSoft[8+j];
							tx_auxH(sector6[j]);				// Envia sector 6 leido
						}
						tx_aux(CR);
						tx_aux(LF);						
						
						
						
						if (GrabaTarjeta==1) 									//
						{

							if (g_scArrRxComSoft[18]==0)					  //APB inicializado...Solo Acepta tarjetas formateadas
							{
								buffer_ticket[0]=STX;							
								buffer_ticket[1]='0';							
								buffer_ticket[2]=0x08;						
								buffer_ticket[3]='U';
								buffer_ticket[4]=ID1;
								buffer_ticket[5]=ID2;
								buffer_ticket[6]=ID3;
								buffer_ticket[7]=ID4;
								buffer_ticket[8]=ETX;
								num_data=9;
	 							Send_Port(num_data);
								g_cEstadoComSeqMF=SEQ_WCMD;
							
							}
							else
							{
							 	Eject_Card(ROTACION);
							} 
 						}
						else
						{
							if ((g_scArrRxComSoft[18]==1))				// APB 1:Entrada 2:Salida
							{
								//posee_in_out();
								Debug_txt_Tibbo((unsigned char *) "POSEE IN\r\n");

							Hex_Str(g_scArrRxComSoft[8]); 			// Año Entrada
							g_scDATE[0]=decena;
							g_scDATE[1]=unidad;
								
							Hex_Str(g_scArrRxComSoft[9]); 			// Mes Entrada
							g_scDATE[2]=decena;
							g_scDATE[3]=unidad;
								
							Hex_Str(g_scArrRxComSoft[10]); 			// Dia Entrada
							g_scDATE[4]=decena;
							g_scDATE[5]=unidad;
								
							Hex_Str(g_scArrRxComSoft[11]); 			// Hora Entrada
							g_scDATE[6]=decena;
							g_scDATE[7]=unidad;
															
							Hex_Str(g_scArrRxComSoft[12]); 			// Minuto Entrada
							g_scDATE[8]=decena;
							g_scDATE[9]=unidad;
	
							Cod_Dcto=two_one(g_scArrRxComSoft[15],g_scArrRxComSoft[14]);
							Cod_Dcto=Cod_Dcto+0x30;
								
							Debug_txt_Tibbo((unsigned char *) "DCTO:");
							tx_aux(Cod_Dcto);
							tx_aux(CR);
							tx_aux(LF);

//------------------------------------------------------------------------------------------------
							TipoVeh=g_scArrRxComSoft[16]&(0x0f);
							Val_Horario=g_scArrRxComSoft[16]&(0xf0);
							Val_Horario>>=4;
//------------------------------------------------------------------------------------------------
					if ((TipoCard==CARD_MENSUALIDAD)||(TipoCard==CARD_COMPARTIDA))
					{
						Val_PicoPlaca=g_scArrRxComSoft[17]&0xf0;
						Val_PicoPlaca>>=4;
						ValDCTO_INPAGO=g_scArrRxComSoft[17]&0x0F;
				 }
				else
				{
					ValDCTO_INPAGO=g_scArrRxComSoft[17];
				}

					APB_CARD=g_scArrRxComSoft[18];
//------------------------------------------------------------------------------------------------
				Hex_Str(g_scArrRxComSoft[19]); 			// Año Liquidacion + T gracia
				g_scDATE_Liq[0]=decena;
				g_scDATE_Liq[1]=unidad;
								
				Hex_Str(g_scArrRxComSoft[20]); 			// Mes Liquidacion + T gracia
				g_scDATE_Liq[2]=decena;
				g_scDATE_Liq[3]=unidad;
								
				Hex_Str(g_scArrRxComSoft[21]); 			// Dia Liquidacion + T gracia
				g_scDATE_Liq[4]=decena;
				g_scDATE_Liq[5]=unidad;
							
				Hex_Str(g_scArrRxComSoft[22]); 			// Hora Liquidacion + T gracia
				g_scDATE_Liq[6]=decena;
				g_scDATE_Liq[7]=unidad;
					
				Hex_Str(g_scArrRxComSoft[23]); 			// Minuto Liquidacion + T gracia
				g_scDATE_Liq[8]=decena;
				g_scDATE_Liq[9]=unidad;
//------------------------------------------------------------------------------------------------								
											  
				Lea_S1_Block(0);									/* lee el bloque 0 de la tarjeta MF50 */
				g_cEstadoComSeqMF=SEQ_RTA_rdB4;
								
//-----------------------------------------------------------------------------------------------
							

							}
							else if  ((g_scArrRxComSoft[18]==2)&&((TipoCard==CARD_MENSUALIDAD)||(TipoCard==CARD_LOCATARIO)))
							{
								Debug_txt_Tibbo((unsigned char *) "MENSUAL NO POSEE IN OPCION RENOVAR\r\n");
									
								g_scDATE[0]='0';						// Año Entrada
								g_scDATE[1]='0';
								
		
								g_scDATE[2]='0';						// Mes Entrada
								g_scDATE[3]='0';
								
		
								g_scDATE[4]='0';					   	// Dia Entrada
								g_scDATE[5]='0';
								
		
								g_scDATE[6]='0';						// Hora Entrada
								g_scDATE[7]='0';
															
	
								g_scDATE[8]='0';						// Minuto Entrada
								g_scDATE[9]='0';
	
								Cod_Dcto='0';

//------------------------------------------------------------------------------------------------
								TipoVeh=g_scArrRxComSoft[16]&(0x0f);
								Val_Horario=g_scArrRxComSoft[16]&(0xf0);
								Val_Horario>>=4;
//------------------------------------------------------------------------------------------------

								Val_PicoPlaca=g_scArrRxComSoft[17]&0xf0;
								Val_PicoPlaca>>=4;
								ValDCTO_INPAGO=g_scArrRxComSoft[17]&0x0F;


								APB_CARD=g_scArrRxComSoft[18];
//------------------------------------------------------------------------------------------------
	
								g_scDATE_Liq[0]='0';				   // Año Liquidacion + T gracia
								g_scDATE_Liq[1]='0';
								
			
								g_scDATE_Liq[2]='0';				   // Mes Liquidacion + T gracia
								g_scDATE_Liq[3]='0';
								
		
								g_scDATE_Liq[4]='0';				   // Dia Liquidacion + T gracia
								g_scDATE_Liq[5]='0';
								
		
								g_scDATE_Liq[6]='0';				   // Hora Liquidacion + T gracia
								g_scDATE_Liq[7]='0';
						
		
								g_scDATE_Liq[8]='0';				  // Minuto Liquidacion + T gracia
								g_scDATE_Liq[9]='0';

								Lea_S1_Block(0);						// Block(4)
								g_cEstadoComSeqMF=SEQ_RTA_rdB4;



							}
							else
							{
								
									Debug_txt_Tibbo((unsigned char *) "SIN INGRESO\r\n");
									posee_in_out();										/*modificacion jp*/							
								//	Eject_Card(ERR_APB);





							}
						}
 					}
					else
					{
						Eject_Card(ERR_LECTOR);
					}

					buffer_ready=0;
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=TIMWPoll;						
  				}
				else if (ValTimeOutCom==1)
				{
					Debug_txt_Tibbo((unsigned char *) "SIN RTA. RD B6\r\n");
					Eject_Card(ERR_RD);
 				}

			break;
//------------------------------------------------------------------------------------------------------------------------------*
//------------------------------------------------------------------------------------------------------------------------------*
	case SEQ_RTA_rdB4_Comercio:

				if ((buffer_ready==1)||(ValTimeOutCom==1))
				{
						
					if (g_scArrRxComSoft[7]=='Y')
					{
						Debug_txt_Tibbo((unsigned char *) "SECTOR1_BLOQUE0: ");
						//for (j=0; j<16; j++)
						//{
						//	tx_auxH(g_scArrRxComSoft[8+j]);				// Envia sector 6 leido
					//	}
					//	tx_aux(CR);
					//	tx_aux(LF);		
						
						for (j=0; j<16; j++)
						{
							buffer_ticket[j+11]=g_scArrRxComSoft[8+j];
						}	

					}
					else
					{
 						for (j=11; j<27; j++)
						{
							buffer_ticket[j]=' ';
						}
	
					}
 					buffer_ticket[27]=ETX;
					
					for (j=0; j<28; j++)
					{
				   		tx_aux(buffer_ticket[j]);
					}
					tx_aux(CR);
					tx_aux(LF);						
					
					Send_Port(28);										// ENVIA AL PRINCIPAL
					g_cEstadoComSeqMF=SEQ_WCMD;
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=TIMConsulta;
					TIME_ESPERA=CTE_SEG*300;


				}
 
			break;		


			case SEQ_RTA_rdB4:

				if ((buffer_ready==1)||(TipoCard==CARD_MENSUALIDAD))
				{
					Debug_txt_Tibbo((unsigned char *) "SEQ_RTA_rdB4\n");
					
					if ((g_scArrRxComSoft[7]=='Y')||(TipoCard==CARD_MENSUALIDAD))		/*lectura del bloque 0 ok*/
					{


 						buffer_ready=0;

						buffer_ticket[0]=STX;								// Inicio de transmision
						buffer_ticket[1]='0';								// Dir 0
						buffer_ticket[3]='L';								// CMD L=lectura
						num_data=4;													// Posicion 2 = Numero de Caracteres enviados

						if ((TipoCard==CARD_MENSUALIDAD)||(TipoCard==CARD_LOCATARIO))
						{
							/*COVIERTE ID A DECIMAL*/
							ve_id(ID2,ID1);
 						}
						else
						{

							for (j=0; g_scArrRxComSoft[8+j]!='\0'; j++)			// Código hasta encontrar NULL
							{
						 		buffer_ticket[num_data++]=g_scArrRxComSoft[8+j];
							}
						}
						
  					/*datos de la memoria*/
						buffer_ticket[num_data++]=':';
						for (j=0; j<10; j++) 								// Fecha De Entrada
						{
							buffer_ticket[num_data++]=g_scDATE[j];
						}
						buffer_ticket[num_data++]=':';
						
						if (TipoVeh==0x00)	 								// Moto / Carro
						{
							buffer_ticket[num_data++]='C';				   	// Carro=0
						}
						else if (TipoVeh==0x01)
						{
							buffer_ticket[num_data++]='M';					// Moto=1
						}
						else if (TipoVeh==0x02)
						{
							buffer_ticket[num_data++]='B';					// Bicicleta=2
						}
						else if (TipoVeh==0x03)
						{
							buffer_ticket[num_data++]='T';					// Camion=3
						}
						else
						{
							buffer_ticket[num_data++]='C';
						}

						buffer_ticket[num_data++]=':';
//-------------------------------------------------------------------------------//
//						if (EmisionXperdida==1)
//						{
//						 	Cod_Dcto='0';
//							EmisionXperdida=0;
//						}
//-------------------------------------------------------------------------------//						
						buffer_ticket[num_data++]=Cod_Dcto;
						buffer_ticket[num_data++]=':';
//------------------------------------------------------------------------------// Fecha Liquidacion
						for (j=0; j<10; j++)		   			
						{
						 	buffer_ticket[num_data++]=g_scDATE_Liq[j];
						}
						buffer_ticket[num_data++]=':';
//------------------------------------------------------------------------------//
						if (TipoCard!=EMITIDA_PERDIDA)
						{
					   		buffer_ticket[num_data++]=TipoCard+0X30;			// Tipo de tarjeta 1=Rotacion Normal
						}
						else
						{
							
							buffer_ticket[num_data++]='P';						// Tipo de tarjeta Perdida
						}
						buffer_ticket[num_data++]=':';
//------------------------------------------------------------------------------//	LPR							
						if ((PrintLPR==1)&&(TipoCard!=CARD_MENSUALIDAD)&&(TipoCard!=CARD_COMPARTIDA)&&(TipoCard!=CARD_MULTIPLE))
						{
							Debug_txt_Tibbo((unsigned char *) "KEY2\r\n");
								
								LoadPass2();															/*envia la clave 2 a la tarjeta mf50*/
								g_cEstadoComSeqMF=SEQ_RTA_KEY2;	
								
						}
						else
						{
							buffer_ticket[num_data++]=' ';
					   		buffer_ticket[num_data++]=':';
//------------------------------------------------------------------------------//
							buffer_ticket[num_data++]=ETX;						// Fin de trasmision
						
							buffer_ticket[2]=num_data;							// Numero de datos (Despues de DIR antes de Comando)
//------------------------------------------------------------------------------//							
//							sel_com=PuertoMF;									// Datos enviados ap Principal
//							for (j=num_datos-9; j<num_datos+1; j++)
//							{
//								tx_chr(buffer_ticket[j]);
//							}
//							tx_chr(CR);
//							tx_chr(LF);
//							while (sendactive==1) 
//							{
//							}
//							sel_com=PuertDB9;
//------------------------------------------------------------------------------//										
							for (j=0; j<num_data; j++)
							{
								tx_aux(buffer_ticket[j]);
							}
							tx_aux(CR);
							tx_aux(LF);

//------------------------------------------------------------------------------// 							
							Send_Port(num_data);								// ENVIA AL PRINCIPAL
							if 	(timeOut==1)
							{
								Send_Port(num_data);
							}
  							g_cEstadoComSeqMF=SEQ_WCMD;
							TIME_ESPERA=CTE_SEG*30;							//6

						}
 					}
					else
					 {
						Eject_Card(ERR_LECTOR);
					}

					buffer_ready=0;	
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=TIMWPoll;									
  				}
				else if (ValTimeOutCom==1)
				{
					Debug_txt_Tibbo((unsigned char *) "SIN RTA. RD B4\r\n");
					Eject_Card(ERR_RD);
 				}


			break;
//------------------------------------------------------------------------------------------------------------------------------*
		   case SEQ_RTA_KEY2:
	
			if ((ValTimeOutCom==1)||(buffer_ready==1))
			{
				if ((buffer_ready==1)&&(g_scArrRxComSoft[6]=='Y'))	/*clave ok en el lector*/
				{
					Debug_txt_Tibbo((unsigned char *) "RD LPR\r\n");
					buffer_ready=0;
					Select_S2_Block(0);																	/*leo el contenido de la informacion en MF50 del mensual q esta en el sector2 bloque 0*/
					g_cEstadoComSeqMF=SEQ_LPR;
					//lee sector 2 bloque 0 : PLACA

				}
				else
				{
					buffer_ticket[num_data++]=' ';
					buffer_ticket[num_data++]=':';
					buffer_ticket[num_data++]=ETX;								// Fin de trasmision
					buffer_ticket[2]=num_data;
//------------------------------------------------------------------------------//										
					for (j=0; j<num_data; j++)
					{
						tx_aux(buffer_ticket[j]);
					}
					tx_aux(CR);
					tx_aux(LF);
//------------------------------------------------------------------------------// 							
					Send_Port(num_data);									// ENVIA AL PRINCIPAL
					if 	(timeOut==1)
					{
						Send_Port(num_data);
					}
   					g_cEstadoComSeqMF=SEQ_WCMD;
	 			}
			}

		   break;
//----------------------------------------------------------------------------------------------------------------------------*
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_LPR:

				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
					if ((buffer_ready==1)&&(g_scArrRxComSoft[7]=='Y'))
					{
						Debug_txt_Tibbo((unsigned char *) "PLACA:");
						
									
						buffer_ready=0;
						for (j=0; g_scArrRxComSoft[8+j]!='\0'; j++)				// cargo el arreglo  para transmitirlo al pto paralelo la placa
						{
							buffer_ticket[num_data++]=g_scArrRxComSoft[8+j];
							tx_aux(g_scArrRxComSoft[8+j]);
						}

						NumLiq = g_scArrRxComSoft[23];

						tx_aux(CR);
						tx_aux(LF);
					}
					else	 													// No hubo lectura sector LPR
					{
						buffer_ticket[num_data++]=' ';
					}
					buffer_ticket[num_data++]=':';
					buffer_ticket[num_data++]=ETX;								// Fin de trasmision
					buffer_ticket[2]=num_data;									// Numero de datos (Despues de DIR antes de Comando)
//------------------------------------------------------------------------------//							
//					sel_com=PuertoMF;											// Datos enviados ap Principal
//					for (j=num_datos-9; j<num_datos+1; j++)
//					{
//						tx_chr(buffer_ticket[j]);
//					}
//					tx_chr(CR);
//					tx_chr(LF);
//					while (sendactive==1) 
//					{
//					}
//					sel_com=PuertDB9;
//------------------------------------------------------------------------------//										
					Debug_Dividir_texto();				
					Debug_txt_Tibbo((unsigned char *) "Comunicacion pto paralelo\d\n");
					for (j=0; j<num_data; j++)
					{
						tx_aux(buffer_ticket[j]);
					}
					tx_aux(CR);
					tx_aux(LF);
					Debug_Dividir_texto();
					
//------------------------------------------------------------------------------// 							
					Send_Port(num_data);										// ENVIA AL PRINCIPAL
					if 	(timeOut==1)
					{
						Send_Port(num_data);
					}
						TIME_ESPERA=CTE_SEG*30;				/*el tiempo era 6 */
   					g_cEstadoComSeqMF=SEQ_WCMD;
	
				 }

			break;
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_RTA_EJECT:
				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
					ValTimeOutCom=TIMWPoll;
					buffer_ready=0;

					g_cEstadoComSeqMF=SEQ_ESPERA;
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=T_WAIT;
  				}
			break;

//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_STATUS_OUT:

				if (ValTimeOutCom==1)
				{
 					Get_Status();
					g_cEstadoComSeqMF=SEQ_RTA_STATUSout;
					buffer_ready=0;
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=TIMWPoll;				

  				}
			break;
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_RTA_STATUSout:
				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
					if ((buffer_ready==1)&&(g_scArrRxComSoft[3]=='P'))
					{
 						if (busy==0)
						{
							recibe_port();
			
							if (buffer_ticket[3]=='?')
							{
 								buffer_ticket[0]=STX;
								buffer_ticket[1]='0';
								buffer_ticket[2]=0x06;
								buffer_ticket[3]='?';
								buffer_ticket[4]=ACK;
								buffer_ticket[5]=ETX;
								Send_Port(6);					
   							}
							else if (buffer_ticket[3]=='o')
							{
								GrabaTarjeta=0;
								Desarmado=0;
								Permite_Puerta=0;
								Permite_Billetero=0;
								Permite_Monedero=0;
								NotifyCofreCoin=0;
								NotifyCofreB2B=0;
								NotifyPuerta=0;
			   				}
							else if (buffer_ticket[3]=='F')
							{
								FueraServicio=1;
			   				}
							else if (buffer_ticket[3]=='E')
							{
								FueraServicio=0;
								AlarmaPrevia1=0;
								AlarmaPrevia2=0;
								AlarmaPrevia3=0;
								GrabaTarjeta=0;
								Desarmado=0;
								lock=0;
								onceAlarm=0;
								alarma1=0;
								alarma2=0;
								alarma3=0;
								Permite_Puerta=0;
								Permite_Billetero=0;
								Permite_Monedero=0;
								NotifyCofreCoin=0;
								NotifyCofreB2B=0;
								NotifyPuerta=0;
			   				}

						}

						Get_Status();
						g_cEstadoComSeqMF=SEQ_RTA_STATUSout;
					}
					else
					{
						g_cEstadoComSeqMF=SEQ_INICIO;

					}
					buffer_ready=0;
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=TIMWPoll;
  				}

			break;
//------------------------------------------------------------------------------------------------------------------------------*
//------------------------------------------------------------------------------------------------------------------------------*
			case SEQ_WCMD:
				
			   	if (TIME_ESPERA==1)
				{
							Eject_Card(ERR_TIME_OUT);
					
//-------------------------------------------------------------------------------------------*
							Debug_txt_Tibbo((unsigned char *) "EXPULSA TIME OUT\r\n");
					
//-------------------------------------------------------------------------------------------*					

				}

 				else if (busy==0)																				/*recive informacion pto paralelo*/
				{
					recibe_port();

					if ((buffer_ticket[0]==ACK)&&(num_data==1))
					{
						TIME_ESPERA=0;

					}

//-------------------------------------------------------------------------------------------*
	
					if (buffer_ticket[3]=='O')				//eject
					{
						TIME_ESPERA=0;
//-------------------------------------------------------------------------------------------*
						/*solo ejecta MF*/
						if (num_data==5)								
						{
						   	if (Desarmado==1)
							{
								Eject_Card(DESARM);
							}
							else
							{
								Eject_Card(CARD_EJECT);
							}

							tx_aux('P');
							tx_aux('C');
							tx_aux(':');
							for (j=0; j<5; j++)
							{
								tx_aux(buffer_ticket[j]);	
							}
							
							Debug_txt_Tibbo((unsigned char *) "\r\nEXPULSA\r\n");
							
							for (j=0; j<60; j++)
							{
								buffer_ticket[j]=0;
							}
//-------------------------------------------------------------------------------------------*
						}
						else if (num_data>=16)
						{
	
//-------------------------------------------------------------------------------------------*
							Debug_txt_Tibbo((unsigned char *) "\r\nPC: comunicacion pto paralelo eject con fecha out\r\n");
							Debug_Dividir_texto();
							/*trama pto paralelo a tcp debuger*/
							for (j=0; j<num_data; j++)
							{
								tx_aux(buffer_ticket[j]);	
							}
							tx_aux(0X0d);
							tx_aux(0X0a);
							Debug_Dividir_texto();
//-------------------------------------------------------------------------------------------*
							if (buffer_ticket[3]=='O')
							{
			   				year=two_one(buffer_ticket[5],buffer_ticket[6]);
								year=bcd_hex(year);
						   
								month=two_one(buffer_ticket[7],buffer_ticket[8]);
								month=bcd_hex(month);
	
								day=two_one(buffer_ticket[9],buffer_ticket[10]);
								day=bcd_hex(day);
							 
								hour=two_one(buffer_ticket[11],buffer_ticket[12]);
								hour=bcd_hex(hour);
	
								minut=two_one(buffer_ticket[13],buffer_ticket[14]);
								minut=bcd_hex(minut);
	
								sector6[9]=ValDCTO_INPAGO|0x0f;		 	// Niblle Bajo == f (xxxx1111)	Pago proviene de Cajero
								
								sector6[11]=year;
							   	sector6[12]=month;
								sector6[13]=day;
							   	sector6[14]=hour;
								sector6[15]=minut;
	

//--------------------------------------------------------------------------------------------------
								Debug_txt_Tibbo((unsigned char *) "\r\nDatos MF \r\n");						
								Debug_Dividir_texto();
								/*informacion de la tarjeta mf50*/
								for (j=0; j<16; j++)
								{
								 	BufferWrite_MF[j]=sector6[j];
									tx_auxH(BufferWrite_MF[j]);				 // Envia sector 6 a BufferWrite_MF para grabar  !NO COMENTARIAR
								}
								tx_aux(CR);
								tx_aux(LF);
								Debug_Dividir_texto();
//--------------------------------------------------------------------------------------------------
								for (j=0; j<16; j++)					   	// Reintento
								{
								 	RetryWrite_MF[j]=sector6[j];
								}					

								ValTimeOutCom=500;	
								if (sector5[0]!=EMITIDA_PERDIDA	)					/*preguntamos si la tarjeta esta perdida*/
								{
									Debug_txt_Tibbo((unsigned char *) "\r\nWR_MF SEQ_RTA_STATUS_WR_B6: \r\n");
									Block=2;																/*graba fecha de salida*/
								  Get_Status();
								  buffer_ready=0;
									g_cEstadoComSeqMF=SEQ_RTA_STATUS_WR_B6;	
								}
								else
								{
																
									Debug_txt_Tibbo((unsigned char *) "\r\nWR_MF SEQ_RTA_STATUS_WR_B6_PeRdiDa_RTCN: \r\n");
									sector5[0]=01;													/*modifica el estado de perdida a rotacion*/										
									/*se muestra bloque1 sector1 informacion*/
									for (j=0; j<16; j++)
									{
								 	BufferWrite_MF[j]=sector5[j];
									tx_auxH(BufferWrite_MF[j]);				 // Envia sector 6 a BufferWrite_MF para grabar  !NO COMENTARIAR
									}
									tx_aux(CR);
									tx_aux(LF);
									Debug_Dividir_texto();
//--------------------------------------------------------------------------------------------------
									for (j=0; j<16; j++)					   	// Reintento
									{
								 	RetryWrite_MF[j]=sector5[j];
									}					
									
									TIME_ESPERA=0;
									Block=1;
									/*graba la informacion del tipo de tarjeta sector1_bloque 1*/
									//Graba_S1_BloqueSel(Block);
									/*para saber si esta la tarjeta*/
								  Get_Status();
								  buffer_ready=0;
									g_cEstadoComSeqMF=SEQ_RECARGA_PASSWORD;    //SEQ_RTA_STATUS_WR_B7;     //SEQ_RTA_WR_2;
									break;
								}
												/**/
//--------------------------------------------------------------------------------------------------
							}
							else if (buffer_ticket[3]=='M')
							{
							   	
			   					year=two_one(buffer_ticket[5],buffer_ticket[6]);	  				// FECHA DE VENCIMIENTO
								year=bcd_hex(year);
						   
								month=two_one(buffer_ticket[7],buffer_ticket[8]);
								month=bcd_hex(month);
	
								day=two_one(buffer_ticket[9],buffer_ticket[10]);
								day=bcd_hex(day);
							 
								sector5[8]=year;
								sector5[9]=month;
								sector5[10]=day;
//--------------------------------------------------------------------------------------------------
								Debug_Dividir_texto();
								Debug_txt_Tibbo((unsigned char *) "WR VENCIMIENTO:SEQ_RTA_STATUS_WR_B5 ");							

									
								for (j=0; j<16; j++)
								{
								 	BufferWrite_MF[j]=sector5[j];
									tx_auxH(BufferWrite_MF[j]);				 // Envia sector 6 a BufferWrite_MF para grabar  !NO COMENTARIAR
								}
								tx_aux(CR);
								tx_aux(LF);
								Debug_Dividir_texto();
//--------------------------------------------------------------------------------------------------
								for (j=0; j<16; j++)					   	// Reintento
								{
								 	RetryWrite_MF[j]=sector5[j];
								}					

								Block=1;
								Get_Status();
								buffer_ready=0;
								g_cEstadoComSeqMF=SEQ_RTA_STATUS_WR_B5;	  	//****************************aqui vamos
 //-------------------------------------------------------------------------------------------------- 
							}
							else
							{
								if (Desarmado==1)
								{
									Eject_Card(DESARM);
								}
								else
								{
									Eject_Card(CARD_EJECT);
								} 


							}



		 				}
						else
						{
						 	Eject_Card(ERR_COM);
						}
		
					}
					else if ((buffer_ticket[3]=='M')&&(buffer_ticket[4]==':'))				//&&(num_data==12))
					{
			   			
						TIME_ESPERA=0;

						year=two_one(buffer_ticket[5],buffer_ticket[6]);	  				// FECHA DE VENCIMIENTO
						year=bcd_hex(year);
					  
						month=two_one(buffer_ticket[7],buffer_ticket[8]);
						month=bcd_hex(month);
	
						day=two_one(buffer_ticket[9],buffer_ticket[10]);
						day=bcd_hex(day);
							 
						sector5[8]=year;
						sector5[9]=month;
						sector5[10]=day;
//--------------------------------------------------------------------------------------------------
						Debug_Dividir_texto();					
						Debug_txt_Tibbo((unsigned char *) "WR VENCIMIENTO 2 SEQ_RTA_STATUS_WR_B6:");		
						
						for (j=0; j<16; j++)
						{
						 	BufferWrite_MF[j]=sector5[j];
							tx_auxH(BufferWrite_MF[j]);				 // Envia sector 6 a BufferWrite_MF para grabar  !NO COMENTARIAR
						}
						tx_aux(CR);
						tx_aux(LF);
						Debug_Dividir_texto();
//--------------------------------------------------------------------------------------------------
						for (j=0; j<16; j++)					   	// Reintento
						{
						 	RetryWrite_MF[j]=sector5[j];
						}					
//--------------------------------------------------------------------------------------------------
						Get_Status();
						buffer_ready=0;
						g_cEstadoComSeqMF=SEQ_RTA_STATUS_WR_B5;
						g_cEstadoComSoft=ESPERA_RX;
						ValTimeOutCom=10;

					}
					else if (buffer_ticket[3]=='o')
					{
						TIME_ESPERA=0;
						
						GrabaTarjeta=0;
						Desarmado=0;
						Permite_Puerta=0;
						Permite_Billetero=0;
						Permite_Monedero=0;
						NotifyCofreCoin=0;
						NotifyCofreB2B=0;
						NotifyPuerta=0;
   					}
					else if (buffer_ticket[3]=='E')
					{
						TIME_ESPERA=0;

						GrabaTarjeta=0;
						Desarmado=0;
						lock=0;
						onceAlarm=0;
						alarma1=0;
						alarma2=0;
						alarma3=0;
						Permite_Puerta=0;
						Permite_Billetero=0;
						Permite_Monedero=0;
						NotifyCofreCoin=0;
						NotifyCofreB2B=0;
						NotifyPuerta=0;
   					}
					else if (buffer_ticket[3]=='F')
					{
						FueraServicio=1;
						TIME_ESPERA=0;
   					}
					else if (buffer_ticket[3]=='?')
					{
						TIME_ESPERA=0;
						Get_Status();
						g_cEstadoComSeqMF=SEQ_RTA_STATUS2;
						ConsultaSoft=1;
   					}
					else if ((buffer_ticket[3]=='U')&&(buffer_ticket[5]==ETX))
					{
						
						TIME_ESPERA=0;

						if ((buffer_ticket[4]>=0x36)&&((buffer_ticket[4]<=0x39)))
						{
 							buffer_ticket[3]=0X00;						//BORRA CMD
						/*modificacion jp*/
						
							BufferWrite_MF[0]=buffer_ticket[4]-0X30;	// NIVEL SATATUS
						
							/*modificacion jp*/
							BufferWrite_MF[1]=ID_CLIENTE;
							BufferWrite_MF[2]=0X00;
							BufferWrite_MF[3]=COD_PARK_R;
							BufferWrite_MF[4]=0X00;
							for (j=5; j<16; j++)
							{
								BufferWrite_MF[j]=0X00;
							}

							Block=1;
							Graba_S1_BloqueSel(Block); 					// B5
							g_cEstadoComSeqMF=SEQ_RTA_WR;

						   	Expulsa_Grabacion=1;
							
						}
						else
						{
							Eject_Card(ERR_CARD_TIPO);

						}
   					}
				}
				
				if	(ValTimeOutCom==1)
				{
//					Get_Status();
//					buffer_ready=0;
//					g_cEstadoComSeqMF=SEQ_RTA_STATUS2;
//					g_cEstadoComSoft=ESPERA_RX;
//					ValTimeOutCom=10;
				}

 			break;
//------------------------------------------------------------------------------------------------------------------------------*
//			GRABACION DE FECHA DE VENCIMIENTO
//------------------------------------------------------------------------------------------------------------------------------*
 			case SEQ_RTA_STATUS_WR_B5:

				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
					if ((buffer_ready==1))
					{
						buffer_ready=0;
						if (g_scArrRxComSoft[5]=='Y')			
						{
						  	
   						Expulsa_Grabacion=1;							
							RetryWR=4;									// Numero de RE-Intentos 
							Block=1;
							Graba_S1_BloqueSel(Block);					// B5  //Sector 1 bloque 1 = Block(5) >> FECHA VENCIMIENTO
							g_cEstadoComSeqMF=SEQ_RTA_WR;						
						}
						else
						{
							ErrorWR(NO_CARD);
							Eject_Card(ERR_NO_CARD);
							RetryWR=0;			
  						}
					}
					else
					{
						ErrorWR(NO_RTA_WR);
						Eject_Card(ERR_WR);
						RetryWR=0;
 					}
				 }

			break;
//------------------------------------------------------------------------------------------------------------------------------*
//			GRABACION DE FECHA MAXIMA DE SALIDA
//------------------------------------------------------------------------------------------------------------------------------*
 			case SEQ_RTA_STATUS_WR_B6:

				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
					if ((buffer_ready==1))
					{
						buffer_ready=0;
						if (g_scArrRxComSoft[5]=='Y')			
						{
							Debug_txt_Tibbo((unsigned char *) "GRABACION DE FECHA maxima de salida");	
   						Expulsa_Grabacion=1;							
							RetryWR=4;									// Numero de RE-Intentos 
							LoadPass();									// Sector 1 (bloque 2 = Block(6)) >> FECHA MAX DE SALIDA
							g_cEstadoComSeqMF=SEQ_SEL_B6w;
						}
						else
						{
							ErrorWR(NO_CARD);
							Eject_Card(ERR_NO_CARD);
							RetryWR=0;			
  						}
					}
					else
					{
						ErrorWR(NO_RTA_WR);
						Eject_Card(ERR_WR);
						RetryWR=0;
 					}
				 }

 
//--------------------------------------------------------------------------------------------------
			break;
//------------------------------------------------------------------------------------------------------------------------------*
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/		 

			case SEQ_RTA_STATUS_WR_B7:

				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
					

					if ((buffer_ready==1))
					{
						buffer_ready=0;
						if (g_scArrRxComSoft[5]=='Y')			
						{
   						Debug_txt_Tibbo((unsigned char *) "Graba tipo de tarjeta perdida por rotacion\r\n");					
							RetryWR=4;									// Numero de RE-Intentos 
							Block=1;
							/*se graba tipo de tarjeta*/
							Graba_S1_BloqueSel(Block);					// B5  //Sector 1 bloque 1 = Block(5) >> FECHA VENCIMIENTO
							ValTimeOutCom=TIMWPoll;
							g_cEstadoComSeqMF=SEQ_RTA_WR_2;						
						}
						else
						{
							ErrorWR(NO_CARD);
							Eject_Card(ERR_NO_CARD);
							RetryWR=0;			
  						}
					}
					else
					{
						ErrorWR(NO_RTA_WR);
						Eject_Card(ERR_WR);
						RetryWR=0;
 					}
				 }

			break;	


			case SEQ_RTA_STATUS2:

				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
//--------------------------------------------------------------------					
//	 				sel_com=PuertoMF;
//					for (j=0; j<g_cContByteRx; j++)
//					{
//						tx_chr(g_scArrRxComSoft[j]);	
//					}
//					tx_chr(0xd);
//					tx_chr(0xa);
//					while (sendactive==1) 
//					{
//					}
//					sel_com=PuertoDB9;
//--------------------------------------------------------------------	
					if ((buffer_ready==1))
					{
						if (g_scArrRxComSoft[5]=='Y')			
						{
   							g_cEstadoComSeqMF=SEQ_WCMD;
							g_cEstadoComSoft=ESPERA_RX;
							ValTimeOutCom=500;
							if (ConsultaSoft==1)
							{
							 	ConsultaSoft=0;
 								buffer_ticket[0]=STX;
								buffer_ticket[1]='0';
								buffer_ticket[2]=0x06;
								buffer_ticket[3]='?';
								buffer_ticket[4]=ACK;
								buffer_ticket[5]=ETX;
								Send_Port(6);
 							}
 						}
						else 
						{
				 			ErrorWR(NO_CARD);
							Eject_Card(ERR_NO_CARD);					//Eject_Card(ERR_WR);
							RetryWR=0;
							
							g_cEstadoComSoft=ESPERA_RX;
							ValTimeOutCom=TIMWPoll;
	
							if (ConsultaSoft==1)
							{
								buffer_ticket[0]=STX;
								buffer_ticket[1]='0';
								buffer_ticket[2]=0X06;
								buffer_ticket[3]='e';
								buffer_ticket[4]=0x40;				//Tarjeta Mal Insertada
								buffer_ticket[5]=ETX;
								Send_Port(6);
							}
							else
							{

								buffer_ticket[0]=STX;
								buffer_ticket[1]='0';
								buffer_ticket[2]=0X05;
								buffer_ticket[3]='X';
								buffer_ticket[4]=ETX;
								Send_Port(5);	 
	
								FueraServicio=0;
								GrabaTarjeta=0;
								Desarmado=0;
	
	   							Permite_Puerta=0;
								Permite_Billetero=0;
								Permite_Monedero=0;
								NotifyCofreCoin=0;
								NotifyCofreB2B=0;
								NotifyPuerta=0;
							}
						}
 					}
					else
					{
   							g_cEstadoComSeqMF=SEQ_WCMD;
							g_cEstadoComSoft=ESPERA_RX;
							ValTimeOutCom=500;
					}
					buffer_ready=0;			
				}
			break;
//------------------------------------------------------------------------------------------------------------------------------*
//			case SEQ_SEL_B5w:
//				if ((ValTimeOutCom==1))
//				{
//					if ((buffer_ready==1)&&(g_scArrRxComSoft[3]=='P'))	// 0:SIN ERROR
//					{
//						Block=1;
//						Graba_S1_BloqueSel(1); 							// B5
//						g_cEstadoComSeqMF=SEQ_RTA_WR;	
//					}
//					else
//					{
//						Eject_Card(ERR_SELECT);
//					}
//					buffer_ready=0;
//					g_cEstadoComSoft=ESPERA_RX;
//					ValTimeOutCom=TIMWPoll;
//				}
//			break;
//*******************************************************************************************************************************
			case SEQ_SEL_B6w:
				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
					if ((buffer_ready==1)&&(g_scArrRxComSoft[6]=='Y'))	// 0:SIN ERROR	  
					{
						buffer_ready=0;
						Debug_txt_Tibbo((unsigned char *) "Grabando...\r\n");		
								
					   	
						Block=2;
						Graba_S1_BloqueSel(Block);
						g_cEstadoComSeqMF=SEQ_RTA_WR;
						Expulsa_Grabacion=1;

						buffer_ready=0;
						g_cEstadoComSoft=ESPERA_RX;
						ValTimeOutCom=TIMWPoll;

					}
					else
					{
						Block=2;
						/*MSJ*/
						ErrorWR(SELECCION_SECTOR);
						g_cEstadoComSeqMF=SEQ_RETRY;						
					}
	
					buffer_ready=0;
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=TIMWPoll;
  				}

			break;
//*******************************************************************************************************************************
		 case SEQ_RTA_WR:
			if ((ValTimeOutCom==1)||(buffer_ready==1))
			{
  				
				if ((buffer_ready==1)&&(g_scArrRxComSoft[7]=='Y'))				/*datos ok grabados en MF50*/
				{
					buffer_ready=0;
					Debug_txt_Tibbo((unsigned char *) "Comparando...\r\n");		
									
//----------------------------
//----------------------------
					/*debug del dato grabado en la tarjeta sin verificacion*/
 					tx_aux('R');
					tx_aux('D');
					tx_aux(':');
					for (j=0; j<16; j++)				   	// Lectura 	sector 6
					{
						tx_auxH(g_scArrRxComSoft[8+j]);		// Envia sector 6 leido
					}
					tx_aux(CR);
					tx_aux(LF);

//					if ((BufferWrite_MF[11]==g_scArrRxComSoft[8])&&(BufferWrite_MF[12]==g_scArrRxComSoft[17])&&(BufferWrite_MF[13]==g_scArrRxComSoft[18])&&(BufferWrite_MF[14]==g_scArrRxComSoft[19])&&(BufferWrite_MF[15]==g_scArrRxComSoft[20]))
					
					Comparacion=1;
					for (j=0; j<16; j++)
					{
						if (BufferWrite_MF[j]!=g_scArrRxComSoft[8+j])
						{
							j=16;
							Comparacion=0;
						}
					}

					if (Comparacion==1)
					{
						if (Expulsa_Grabacion==1)
						{
							Debug_txt_Tibbo((unsigned char *) "CARD_EJECT_WR \r\n");
							Eject_Card(CARD_EJECT_WR);
							RetryWR=0;
							Expulsa_Grabacion=0;
						}
						else
						{
							Debug_txt_Tibbo((unsigned char *) "CARD_EJECT \r\n");
							Eject_Card(CARD_EJECT);
							RetryWR=0;
						}
						Debug_txt_Tibbo((unsigned char *) "GRABACION OK \r\n");		
						

						for (j=0; j<60; j++)
						{
							buffer_ticket[j]=0;
						}

						break;
					}
					else
					{
						Debug_txt_Tibbo((unsigned char *) "DIFERENCIA \r\n");		
						
	
						g_cEstadoComSeqMF=SEQ_RETRY;
						g_cEstadoComSoft=ESPERA_RX;
						ValTimeOutCom=20;
						buffer_ready=0;
					}

 //-----------------------------
//------------------------------

 				}
				else
				{
					if ((buffer_ready==1)&&(g_scArrRxComSoft[7]=='N'))
					{
						buffer_ready=0;
						ErrorWR(COMANDO_WR);
					}
					else if (buffer_ready==0)
					{
						ErrorWR(NO_RTA_WR);
					}
					buffer_ready=0;
					g_cEstadoComSeqMF=SEQ_RETRY;
				}
				g_cEstadoComSoft=ESPERA_RX;
				ValTimeOutCom=TIMWPoll;
			}
 		 break;
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/			
			
			case SEQ_RTA_WR_2:
			if ((ValTimeOutCom==1)||(buffer_ready==1))
			{
  				
				if ((buffer_ready==1)&&(g_scArrRxComSoft[7]=='Y'))				/*datos ok grabados en MF50*/
				{
					buffer_ready=0;
					Debug_txt_Tibbo((unsigned char *) "Reparando MF perdida...\r\n");		
									
					Debug_Dividir_texto();
//----------------------------
						for (j=0; j<16; j++)
						{
						 	BufferWrite_MF[j]=sector6[j];
							tx_auxH(BufferWrite_MF[j]);				 // Envia sector 6 a BufferWrite_MF para grabar  !NO COMENTARIAR
						}
							tx_aux(CR);
							tx_aux(LF);
						
//--------------------------------------------------------------------------------------------------
						for (j=0; j<16; j++)					   	// Reintento
						{
						 	RetryWrite_MF[j]=sector6[j];
						}								


					/*debug del dato grabado en la tarjeta sin verificacion*/
 					Debug_Dividir_texto();
				
					Block=2;																/*graba fecha de salida*/
					Get_Status();
					buffer_ready=0;
					g_cEstadoComSeqMF=SEQ_RTA_STATUS_WR_B6;	
					
					g_cEstadoComSoft=ESPERA_RX;
					ValTimeOutCom=20;
					
					
					
 //-----------------------------
//------------------------------

 				}
				else
				{
					if ((buffer_ready==1)&&(g_scArrRxComSoft[7]=='N'))
					{
						buffer_ready=0;
						/*error de escritura mf50*/
						ErrorWR(COMANDO_WR);
					}
					else if (buffer_ready==0)
					{
						ErrorWR(NO_RTA_WR);
					}
					LoadPass();	
					buffer_ready=0;
					g_cEstadoComSeqMF=SEQ_RETRY_KEY_2;							///SEQ_RTA_STATUS_WR_B7;				//SEQ_RETRY;
				}
				g_cEstadoComSoft=ESPERA_RX;
				ValTimeOutCom=TIMWPoll;
			}
 		 break;			
//------------------------------------------------------------------------------------------*
 		case SEQ_RETRY:
			if (ValTimeOutCom==1)
			{
				if (RetryWR!=0)
				{
					/*MSJ*/
			 		Info_Retry();
				}
				buffer_ready=0;
				g_cEstadoComSeqMF=SEQ_RETRY_ANTENA_OFF;
				g_cEstadoComSoft=ESPERA_RX;
				ValTimeOutCom=5;
			}
		break;
//------------------------------------------------------------------------------------------*
 		case SEQ_RETRY_ANTENA_OFF:

			if (ValTimeOutCom==1)
			{
				if (RetryWR!=0)
				{
					RetryWR--;
					Get_Status();												/*valido si hay tarjeta en el lector*/
					g_cEstadoComSeqMF= SEQ_LEE_ID_BACKUP;			//SEQ_RETRY_CARD_INSIDE;
				}
				else
				{
				 	ErrorWR(FALLA_WR);
					Eject_Card(ERR_WR);
				}
				buffer_ready=0;
				g_cEstadoComSoft=ESPERA_RX;
				ValTimeOutCom=TIMWPoll;			
			}

		break;
//------------------------------------------------------------------------------------------*
		case SEQ_RETRY_CARD_INSIDE:
			if ((ValTimeOutCom==1)||(buffer_ready==1))
			{
				if ((buffer_ready==1)&&(g_scArrRxComSoft[5]=='Y'))
				{
					Debug_txt_Tibbo((unsigned char *) "valida password SEQ_RETRY_KEY\r\n");
					LoadPass();																	/*valido el password 1 en MF50*/
					g_cEstadoComSeqMF=SEQ_RETRY_KEY;
				}
				else
				{
				 	ErrorWR(NO_CARD);
					Eject_Card(ERR_NO_CARD);
					RetryWR=0;
				}

				buffer_ready=0;
				g_cEstadoComSoft=ESPERA_RX;
				ValTimeOutCom=TIMWPoll;
  			}
		break;
//------------------------------------------------------------------------------------------*
 		case SEQ_RETRY_KEY:							/*envia cmd de grabar datos a MF50*/

			if ((ValTimeOutCom==1)||(buffer_ready==1))
			{
				if ((buffer_ready==1)&& (g_scArrRxComSoft[6]=='Y'))	// 0:SIN ERROR
				{
					Graba_S1_BloqueSel(Block);																								/*graba la MF50 con los datos recolectados*/
					g_cEstadoComSeqMF=SEQ_RTA_WR;
					Expulsa_Grabacion=1;
				  	buffer_ready=0;
				}
				else
				{
					Debug_txt_Tibbo((unsigned char *) "ERRR KER\r\n");		
				
					g_cEstadoComSeqMF=SEQ_RETRY;
				}
				buffer_ready=0;
				g_cEstadoComSoft=ESPERA_RX;
				ValTimeOutCom=TIMWPoll;
  			}

		break;
/*------------------------------------------------------------------------------------
leo el numero de serie de la tarjeta y lo almaceno para compararlo				
------------------------------------------------------------------------		---------*/
		case SEQ_LEE_ID_BACKUP:
		if ((ValTimeOutCom==1)||(buffer_ready==1))
			{
				if ((buffer_ready==1)&&(g_scArrRxComSoft[5]=='Y'))			/*Tarjeta en el lector*/
				{
					Debug_txt_Tibbo((unsigned char *) "TARJETA LOCK\r\n");
					Select_Card();																		/*consulto el numero de la serie de la tarjeta*/
					g_cEstadoComSeqMF=SEQ_BRTA_ID_BACKUP;

				}
				else
				{
				 	ErrorWR(NO_CARD);
					Eject_Card(ERR_NO_CARD);
					RetryWR=0;
				}

				buffer_ready=0;
				g_cEstadoComSoft=ESPERA_RX;
				ValTimeOutCom=TIMWPoll;
  			}
		break;	
		
/*------------------------------------------------------------------------------------
almaceno el numero del Id leido y lo comparo con el anterior			
------------------------------------------------------------------------		---------*/
		case 	SEQ_BRTA_ID_BACKUP:  //DEBE SER SEQ_RETRY_CARD_INSIDE
			if (buffer_ready==1)
				{
					buffer_ready=0;
					if (g_scArrRxComSoft[5]=='Y')
					{
						/*Y= tiene el numero de la tarjeta
							N= fallo el numero de tarjeta
							E= no hay tarjeta
						*/
						BID1=g_scArrRxComSoft[6];											/*numero de serie de la tarjeta enviado a tcp/ip*/
						BID2=g_scArrRxComSoft[7];
						BID3=g_scArrRxComSoft[8];
						BID4=g_scArrRxComSoft[9];
						Debug_txt_Tibbo((unsigned char *) "CARD_BACKUP: \r\n");
						
						Ve_Hex(g_scArrRxComSoft[6]); 			// 
						tx_aux(decena);
						tx_aux(unidad);
						tx_aux(' ');
						
						Ve_Hex(g_scArrRxComSoft[7]); 			// 
						tx_aux(decena);
						tx_aux(unidad);
						tx_aux(' ');						

						Ve_Hex(g_scArrRxComSoft[8]); 			// 
						tx_aux(decena);
						tx_aux(unidad);
						tx_aux(' ');

						Ve_Hex(g_scArrRxComSoft[9]); 			// 
						tx_aux(decena);
						tx_aux(unidad);
					  tx_aux(' ');

						tx_aux(CR);
						tx_aux(LF);														
						
						if ((ID1==BID1)&& (ID2==BID2)&& (ID3==BID3)&& (ID4==BID4)== 1)
						{
						Get_Status();	
						g_cEstadoComSeqMF=SEQ_RETRY_CARD_INSIDE;
						buffer_ready=0;		
						}
						else
						{
							Debug_txt_Tibbo((unsigned char *) "ERROR NO ES LA MISMA CARD\r\n");
							
							Eject_Card(ERR_SELECT);;
						}
						
						
												
					}
					else
					{
						Debug_txt_Tibbo((unsigned char *) "ERROR CARD\r\n");
							
						Eject_Card(ERR_SELECT);;
 					}
 
				}
				else if (ValTimeOutCom==1)
				{
					Debug_txt_Tibbo((unsigned char *) "SIN RTA. SELECT\r\n");
					Eject_Card(ERR_RD);

				}

			break;
		case SEQ_RETRY_KEY_2:							/*envia cmd de grabar datos a MF50*/

			if ((ValTimeOutCom==1)||(buffer_ready==1))
			{
				if ((buffer_ready==1)&& (g_scArrRxComSoft[6]=='Y'))	// 0:SIN ERROR
				{
					Graba_S1_BloqueSel(Block);																								/*graba la MF50 con los datos recolectados*/
					g_cEstadoComSeqMF=SEQ_RTA_WR_2;
					
				  	buffer_ready=0;
				}
				else
				{
					/*error clave no la recibe*/
					Debug_txt_Tibbo((unsigned char *) "ERRR KER\r\n");		
				
					g_cEstadoComSeqMF=SEQ_RETRY_KEY_2;
				}
				buffer_ready=0;
				g_cEstadoComSoft=ESPERA_RX;
				ValTimeOutCom=TIMWPoll;
  			}

		break;
				
				/* redirecciono el pto serie para leer en QR*/
		case SEQ_QR_WAIT:
			if ((ValTimeOutCom == 1)||(buffer_ready != 0))
			{
				Debug_txt_Tibbo((unsigned char *) "\r\nlector datalogic cod QR SEQ_QR_WAIT\r\n");	
				if ((g_cContByteRx=='E')|| (g_cContByteRx=='D'))
				{
					g_cEstadoComSeqMF=SEQ_QR_PTPRL;																										
					g_cEstadoComSoft=RX_QR;
					ValTimeOutCom=3;		
				}
				else
				{
				buffer_ready =0;
				sel_com=PuertoMF;																						/*pto serial redireccionado*/
				esQR=1;
				g_cEstadoComSeqMF=SEQ_QR_INICIO;														/*no hay tarjeta probamos si hay QR*/
				g_cEstadoComSoft=RX_QR;
				ValTimeOutCom=TIMConsulta;												//TIMWPoll;																			/*se espera el dato del qr*/
				g_cContByteRx=0;		
				}					/*contador de bits en cero*/
			}

			break;

			/*inicia la lectura del ticket*/
		case	SEQ_QR_INICIO:
			if ((ValTimeOutCom == 1)||(buffer_ready != 0))
			{
				if (buffer_ready == 1)
				{
				
					/*trama con codigo QR*/
					Debug_txt_Tibbo((unsigned char *) "\r\nQR SEQ_QR_INICIO\r\n");	
					Debug_txt_Tibbo((unsigned char *) "\r\ntrama del lector datalogic cod QR\r\n");							/*la respuesta es desconocida*/
					Debug_txt_Tibbo(g_scArrRxComSoft);																													/*imprimo la trama recibida*/
					Debug_txt_Tibbo((unsigned char *) "\r\n");
				
									
					g_cEstadoComSeqMF=SEQ_QR_PTPRL;																										
					g_cEstadoComSoft=RX_QR;
					ValTimeOutCom=3;				
				}
				else 
				{
					if (contador_QR==5)
					{	
					/*no hay datos QR se va a trajetas*/
						Debug_txt_Tibbo((unsigned char *) " HABILITO TARJETAS SEQ_QR_INICIO\r\n");
						contador_QR=0;
						ES = 1;			
						esQR=0;
						sel_com=PuertoDB9;
						buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
						g_cEstadoComSeqMF=SEQ_INICIO;											/*no hay tarjeta probamos si hay QR*/
						g_cEstadoComSoft=ESPERA_RX;
						ValTimeOutCom=T_WAIT;
					}
					else 
					{	
						contador_QR++;
						ES = 1;																												/*activo pto serie*/					
						esQR=1;																												/*habilito el pto serial del QR */
						sel_com=PuertoMF;																							/*PuertoMF*/
						buffer_ready=0;																								/* limpio el buffer*/
						g_cContByteRx=0	;						
						g_cEstadoComSeqMF=SEQ_QR_INICIO;																/*no hay tarjeta probamos si hay QR*/
						g_cEstadoComSoft=RX_QR;
						ValTimeOutCom=TIMConsulta;																					/*TIMConsulta=1200*/
					}
					
			}
		break;	


			/*tengo el dato del ticket QR */
		
		case SEQ_QR_PTPRL: 
			if ((ValTimeOutCom == 1)||(buffer_ready != 0))	
			{
			/*analizo los datos*/
			ES = 0;																											/*inactivo pto serie y analizo el dato*/	
			Debug_txt_Tibbo((unsigned char *) "cod SEQ_QR_PTPRL r\n");
			buffer_ticket[0]=STX;																				// Inicio de transmision TRAMA QR
			buffer_ticket[1]='0';																				// Direccion del pc
			buffer_ticket[2]='L';																				/*numero de digitos de la trama se modifica al final*/	
			buffer_ticket[3]='Q';																				/*cmd de cobro del cajero L tarjeta Q cobro por QR*/	
			buffer_ticket[4]=0;	
			num_data=4;
			
			Debug_Dividir_texto()	;																													// Posicion 2 = Numero de Caracteres enviados
			Debug_txt_Tibbo((unsigned char *) "numero de digitos de la tramar\n");
			tx_aux(g_cContByteRx);	
			Debug_txt_Tibbo((unsigned char *) "\r\n");									/*final de linea*/			
			Debug_Dividir_texto()	;			
			 /*se analiza la trama recibida */
			temp=num_num(g_scArrRxComSoft);															/*funcion que pregunta donde empieza el primer numero del ticket*/
			temp2=num_char(g_scArrRxComSoft+temp,'>');									/*busca un caracter en la trama en este caso el caracter final de la trama*/
			
			if ((tipo_vehiculo=strstr(g_scArrRxComSoft,"Carro"))!= 0)		/*pregunto el tipo de vehiculo grabado en el codigo QR*/
					{
					vehiculo='C';
					}
				else
					{
					vehiculo='M';
					}
					/*informacion del tipo de vehiculo y la longitud del ticket*/
				Debug_txt_Tibbo((unsigned char *) "tipo de vehiculo: ");		/*msj tipo de vehiculo */
				tx_aux(vehiculo);																						/*caracter del tipo de vehiculo*/
				Debug_txt_Tibbo((unsigned char *) "\r\n");									/*final de linea*/
				
				Debug_txt_Tibbo((unsigned char *) "longitud de la trama: ");/*msj longitud de la trama */
				tx_auxH(temp);																							/*numero de inicio del ticket*/
				Debug_txt_Tibbo((unsigned char *) "\r\n");									/*final de linea*/													
				Debug_txt_Tibbo((unsigned char *) "longitud del Ticket: ");
				tx_auxH(temp2);																							/*numero de caracteres del ticket*/
				Debug_txt_Tibbo((unsigned char *) "\r\n");									/*final de linea*/
		   	Ticket[0]=0;
					
				/*validamos la trama */
				if(temp== 0x0c)																							/*la trama debe iniciar en 0x0c*/
				{	
					strncpy(Ticket,g_scArrRxComSoft+temp,temp2);								/*copio el numero de ticket*/
					Ticket[temp2]=0;	
					num_data=strlen(Ticket);																		/*longitud del ticket*/
					strncat(buffer_ticket,Ticket,num_data); 										/*copio el numero de ticket al buffer del pto paralelo*/
					strcat(buffer_ticket,":");																	/*copio el separador de la trama*/	
				
					/*imprimo el numero del ticket*/
					Debug_txt_Tibbo((unsigned char *) "Numero del Ticket: ");				
					Debug_txt_Tibbo(Ticket);																		/*imprimo el numero de ticket*/
					Debug_txt_Tibbo((unsigned char *) "\r\n");									/*final de linea*/
					
					
					
					
					/*leo la fecha de ingreso del ticket en QR*/
				
					if ((tipo_vehiculo = strstr(g_scArrRxComSoft,"Fecha:"))!= 0);
					{
						strncpy(fecha,tipo_vehiculo+7,16);	
						/*imprimo en el lcd fechad e ingreso en el ticket QR*/
						Debug_txt_Tibbo((unsigned char *) "Fecha de ingreso: ");	/*trama de fecha*/
						Debug_txt_Tibbo(fecha);
						Debug_txt_Tibbo((unsigned char *) "\r\n");								/*final de linea*/
						/*copio al año*/
						strncat(buffer_ticket,fecha+2,2); 												/*copio el año a la trama*/
						/*copio el mes */
						strncat(buffer_ticket,fecha+5,2); 												/*copio el mes a la trama */
						/*copio el dia */
						strncat(buffer_ticket,fecha+8,2); 												/*copio el dia*/
						/*copio la hora */
						strncat(buffer_ticket,fecha+11,2);												/*copio la hora*/
						/*copio los minutos */
						strncat(buffer_ticket,fecha+14,2); 												/*copio los minutos*/
						
						strcat(buffer_ticket,":");



					if (vehiculo=='C')																					/*se escribe el tipo de vehiculo a la trama*/
						{
						strcat(buffer_ticket,"C");
						}	
						else
						{
						strcat(buffer_ticket,"M");
						}					
					strcat(buffer_ticket,":0:");																	/*separador de la trama, codigo del comercio y separador */		
				
					
				
					/*leo la fecha de salida del ticket en QR*/
				
				
				
						strcat(buffer_ticket,"0000000000:1::");															/*separador, tipo de tarjeta, separador,separador*/
						num_data=strlen(buffer_ticket);
						buffer_ticket[num_data]=0x03;																	/*el final de la trama ETX*/
						buffer_ticket[2]=num_data+1;
						Debug_txt_Tibbo((unsigned char *) "Trama pto paralelo\r\n ");/*trama del pto paralelo*/
						DebugBufferMF(buffer_ticket,num_data+1);											/*muestro la trama q se va enviar al pto paralelo*/
				
						/*envio pto paralelo*/
						Send_Port(num_data+1);								// ENVIA AL PRINCIPAL
							if 	(timeOut==1)
							{
								Send_Port(num_data+1);
							}
							TIME_ESPERA=CTE_SEG*30;		
				

							/*trama ok */
						
						
						ES = 1;																												/*activo pto serie*/					
						esQR=1;																												/*habilito el pto serial */
						sel_com=PuertoMF;		
						g_cContByteRx=0;							//PuertoDB9;PuertoMF
						buffer_ready=0;																							/* buffer del pto serie (0) inicia a esperar la trama*/
						g_cEstadoComSeqMF=SEQ_QR_WCMD;																/*no hay tarjeta probamos si hay QR*/
						g_cEstadoComSoft=RX_QR;
						ValTimeOutCom=T_WAIT;
					
					}
				
					
				}	
				else
				
				{ 
					/*error los caracteres no estan completas se espera volver a leer el dato*/		
					Debug_txt_Tibbo((unsigned char *) "ERROR caracteres incompletos QR\r\n ");/*trama del pto paralelo*/					
					ES = 1;				
					sel_com=PuertoMF;	
					g_cContByteRx=0;
					esQR=1;
					buffer_ready=0;	
					g_cEstadoComSeqMF=SEQ_QR_INICIO;											/*no hay tarjeta probamos si hay QR*/
					g_cEstadoComSoft=RX_QR;
					ValTimeOutCom=TIMWPoll;
				}
			}	
				
				
			break;
//------------------------------------------------------------------------------------------*
			case SEQ_QR_WCMD:
				
		
				   	if (TIME_ESPERA==1)
				{
					
//-------------------------------------------------------------------------------------------*
					Debug_txt_Tibbo((unsigned char *) " TIME OUT QR\r\n");
					if (contador_QR==5)
					{
						
						Debug_txt_Tibbo((unsigned char *) " HABILITO TARJETAS \r\n");
						TIME_ESPERA=0;
						esQR=0;
						sel_com=PuertoDB9;
						Get_Status();
							
						buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
						g_cEstadoComSeqMF=SEQ_INICIO;											/*no hay tarjeta probamos si hay QR*/
						g_cEstadoComSoft=ESPERA_RX;
						ValTimeOutCom=T_WAIT;
					}
					else
					{
						contador_QR++;
						TIME_ESPERA=0;	
						ES = 1;																												/*activo pto serie*/					
						esQR=1;																												/*habilito el pto serial del QR */
						sel_com=PuertoMF;																							/*PuertoMF*/
						buffer_ready=0;																								/* limpio el buffer*/
						g_cContByteRx=0	;						
						g_cEstadoComSeqMF=SEQ_QR_INICIO;																/*no hay tarjeta probamos si hay QR*/
						g_cEstadoComSoft=RX_QR;
						ValTimeOutCom=TIMConsulta;
					}
//-------------------------------------------------------------------------------------------*					

				}
				else 
				{	
					 if (busy==0)																				/*recive informacion pto paralelo*/
					{
						recibe_port();

						if ((buffer_ticket[0]==ACK)&&(num_data==1))
						{
						TIME_ESPERA=0;

						}

//-------------------------------------------------------------------------------------------*
	
					if (buffer_ticket[3]=='O')				//eject
					{
						TIME_ESPERA=0;
//-------------------------------------------------------------------------------------------*
				
						/*solo ejecta MF cancela  la transaccion */
						if (num_data==5)								
						{
						 Debug_txt_Tibbo((unsigned char *) "\r\nEXPULSA transaccion CANCELADA\r\n"); 

							tx_aux('P');
							tx_aux('C');
							tx_aux(':');
							for (j=0; j<5; j++)
							{
								tx_aux(buffer_ticket[j]);	
							}
						TIME_ESPERA=0;	
						ES = 1;																												/*activo pto serie*/					
						esQR=1;																												/*habilito el pto serial del QR */
						sel_com=PuertoMF;																							/*PuertoMF*/
						buffer_ready=0;																								/* limpio el buffer*/
						g_cEstadoComSeqMF=SEQ_QR_INICIO;																/*no hay tarjeta probamos si hay QR*/
						g_cEstadoComSoft=RX_QR;
						ValTimeOutCom=TIMConsulta;																			//T_WAIT;
							break;
						}	

							/*transaccion ok*/				
						 else if (num_data>=16)
						{
	
//-------------------------------------------------------------------------------------------*
							Debug_txt_Tibbo((unsigned char *) "\r\nPC: RX dato pto paralelo QR con fecha out\r\n");
							Debug_Dividir_texto();
							for (j=0; j<num_data; j++)
							{
								tx_aux(buffer_ticket[j]);	
							}
							tx_aux(0X0d);
							tx_aux(0X0a);
							Debug_Dividir_texto();
//-------------------------------------------------------------------------------------------*
							
								
								if (contador_QR==5)
								{	
								Debug_txt_Tibbo((unsigned char *) "\r\nHABILITO LECTURA DE TARJETA \r\n");	
								contador_QR=0;
								TIME_ESPERA=0;
								esQR=0;
								sel_com=PuertoDB9;
								Get_Status();
								
								buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
								g_cEstadoComSeqMF=SEQ_RTA_STATUS;											/*no hay tarjeta probamos si hay QR*/
								g_cEstadoComSoft=ESPERA_RX;
								ValTimeOutCom=T_WAIT;
								}
								else 
								{
									contador_QR++;
									TIME_ESPERA=0;	
									ES = 1;																												/*activo pto serie*/					
									esQR=1;																												/*habilito el pto serial del QR */
									sel_com=PuertoMF;																							/*PuertoMF*/
									buffer_ready=0;																								/* limpio el buffer*/
									g_cEstadoComSeqMF=SEQ_QR_INICIO;																/*no hay tarjeta probamos si hay QR*/
									g_cEstadoComSoft=RX_QR;
									ValTimeOutCom=TIMConsulta;		
								}
						}
												/**/
//--------------------------------------------------------------------------------------------------
					}
							
						
				
				}
			}
		}
					break;
		
			case SEQ_RECARGA_PASSWORD:

				if ((ValTimeOutCom==1)||(buffer_ready==1))
				{
					if ((buffer_ready==1))
					{
						buffer_ready=0;
						if (g_scArrRxComSoft[5]=='Y')			
						{
							Debug_txt_Tibbo((unsigned char *) "RECARGA EL PASSWORD\n");	
   						Expulsa_Grabacion=1;							
							RetryWR=4;									// Numero de RE-Intentos 
							LoadPass();
							/*SE REESCRIBE EL SECTOR1 Y BLOQUE 1*/ 
							g_cEstadoComSeqMF=SEQ_RTA_STATUS_WR_B7;
						}
						else
						{
							ErrorWR(NO_CARD);
							Eject_Card(ERR_NO_CARD);
							RetryWR=0;			
  						}
					}
					else
					{
						ErrorWR(NO_RTA_WR);
						Eject_Card(ERR_WR);
						RetryWR=0;
 					}
				 }

 			
			
	}
		
}
