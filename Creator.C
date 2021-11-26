#include "creator.h"	

void Eject_Card(unsigned char CodError)	;
bit Send_Port(unsigned char NumChar);
void Hex_Str(unsigned char valorhex);
unsigned char two_one (unsigned char byte_h,unsigned char byte_l);
void posee_in_out();
void ve_id(unsigned char id_h,unsigned char id_l);
unsigned char bcd_hex (unsigned char l_data);
void Info_Retry(void);
//***************************************************************************************************
void sel_Pulsa(void)
{
	sel_A=1;
	sel_B=1;
	sel_C=0;
}
//***************************************************************************************************
void sel_Sensor2(void)
{
	sel_A=0;
	sel_B=0;
	sel_C=1;
}
//***************************************************************************************************
void sel_Sensor1(void)
{
	sel_A=1;
	sel_B=0;
	sel_C=1;
}
//***************************************************************************************************
//void sel_Dir1(void)
//{
//	sel_A=0;
//	sel_B=1;
//	sel_C=1;
//}
//***************************************************************************************************
//void sel_Dir2(void)
//{
//	sel_A=1;
//	sel_B=1;
//	sel_C=1;
//}																								   

void Ve_Hex(unsigned char valorhex)
{
	unsigned char numero;


	decena=0;
	unidad=0;
 	numero=valorhex;
	numero=numero&0xf0;
	numero>>=4;
   	decena=numero;
	(decena>0x09)?(decena=decena+0x37):(decena=decena+0x30);
	
 	numero=valorhex;
	numero=numero&0x0f;
	unidad=numero;
	(unidad>0x09)?(unidad=unidad+0x37):(unidad=unidad+0x30);

}

//***************************************************************************************************


//***************************************************************************************************
 
//***************************************************************************************************
//***************************************************************************************************
//***************************************************************************************************
//	CONVIERTE DE 1BYTE HEXADECIMAL A DECIMAL 														*
//***************************************************************************************************
void Hex_Str(unsigned char valorhex)
{
	unsigned char numero;

	centena=0;
	decena=0;
	unidad=0;
 	numero=valorhex;

	while (numero>=0x064) 				// resto 100
	{
		numero=numero-0x64;
		centena=centena+1;
	}
	while (numero>=0x0a)				// resto 10
	{
		numero=numero-0x0a;
		decena=decena+1;
	}
	unidad=numero;

	decena=(decena|0x30);
	unidad=(unidad|0x30);
}
//***************************************************************************************************
//***************************************************************************************************
unsigned char two_one (unsigned char byte_h,unsigned char byte_l)
{

	unsigned char byte_out;

	byte_h=byte_h&0x0f;
	byte_h<<=4;
	byte_l=byte_l&0x0f;
	byte_out=byte_h|byte_l;

	return byte_out;
}
//**************************************************************************************************
//*******************************************************************************************
void ve_id(unsigned char id_h,unsigned char id_l)
{
	unsigned int valor,numero;
	unsigned char dmil, mil, centena, decena, unidad;
	valor=0;
	dmil=0;
	mil=0;
	centena=0;
	decena=0;
	unidad=0;


	 
	valor=id_h;
	valor<<=8;
	valor=valor|id_l;


	numero=valor;
 	while (numero>=0x2710)				// resto 10.000 
	{
		numero=numero-0x2710;
		dmil=dmil+1;
	}
	while (numero>=0x03e8) 				// resto 1.000
	{
		numero=numero-0x03e8;
		mil=mil+1;
	}
	while (numero>=0x064) 				// resto 100
	{
		numero=numero-0x64;
		centena=centena+1;
	}
	while (numero>=0x0a)				// resto 10
	{
		numero=numero-0x0a;
		decena=decena+1;
	}
	unidad=numero;

//	vdato(dmil|0x30);
//	vdato(mil|0x30);
//	vdato(centena|0x30);
//	vdato(decena|0x30);
//	vdato(unidad|0x30);

	if (dmil!=0)
	{
		 buffer_ticket[num_data++]=(dmil|0x30);
		 buffer_ticket[num_data++]=(mil|0x30);
		 buffer_ticket[num_data++]=(centena|0x30);
		 buffer_ticket[num_data++]=(decena|0x30);
		 buffer_ticket[num_data++]=(unidad|0x30);
	}
	else if (mil!=0)
	{
		 buffer_ticket[num_data++]=(mil|0x30);
		 buffer_ticket[num_data++]=(centena|0x30);
		 buffer_ticket[num_data++]=(decena|0x30);
		 buffer_ticket[num_data++]=(unidad|0x30);
	}
	else if (centena!=0)
	{
		 buffer_ticket[num_data++]=(centena|0x30);
		 buffer_ticket[num_data++]=(decena|0x30);
		 buffer_ticket[num_data++]=(unidad|0x30);
	}
	else if (decena!=0)
	{
		 buffer_ticket[num_data++]=(decena|0x30);
		 buffer_ticket[num_data++]=(unidad|0x30);
	}
	else
	{
		buffer_ticket[num_data++]=(unidad|0x30);
 	}

	
}
//******************************************************************************************
//**************************************************************************************************
 unsigned char bcd_hex (unsigned char l_data)
 {
 	unsigned char temp,j;
	temp=l_data;
	temp>>=4;
	temp=temp & 0x0f;
	if (temp!=0x00)
	{
		l_data=l_data & 0x0f;
	 	for (j=0;j<temp;j++)
	 	{
	  	 	l_data=l_data+0x0a;
	 	}	
	}
	return l_data;
 }

 /*----------------------------------------------------------------------------------------------------
		RECIBE INFORMACION DEL PROCESADOR PRINCIPAL POR P2			
-----------------------------------------------------------------------------------------------------*/
/*
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
	//P2=0XFF;
 	ready=1;
	port_clk=1;
	rx_in_data=1;					//Led de visualizacion 	OFF
  //wait_long(); 	

	return timeOut;
}


//***********************************************************************************************************
//***********************************************************************************************************
//	COMANDOS MIFARE TYRM 9000																				*
//***********************************************************************************************************
//***********************************************************************************************************


//***********************************************************************************************************
//***********************************************************************************************************
/*ojo jp
void LuzCajero (unsigned char CodLuz)	 
{
			
	buffer_ticket[0]=STX;
	buffer_ticket[1]='0';
	buffer_ticket[2]=0X03;
	buffer_ticket[3]='L';
	buffer_ticket[4]=CodLuz;
	buffer_ticket[5]=ETX;
	Send_Port(6);

}
*/
//--------------------------------------------------------------------
//***********************************************************************************************************
void Eject_Card(unsigned char CodError)	 
{
			
	if ((CodError!=SIN_ERROR)&&(CodError!=ERR_EJECT)&&(CodError!=DESARM))      
	{
		buffer_ticket[0]=STX;
		buffer_ticket[1]='0';
		buffer_ticket[2]=0X06;
		buffer_ticket[3]='e';
		buffer_ticket[4]=CodError;
		buffer_ticket[5]=ETX;
		Send_Port(6);
 	}
//--------------------------------------------------------------------
//	COMANDO EXPULSION
//--------------------------------------------------------------------
 eject_card ();
}

//***********************************************************************************************************
//***********************************************************************************************************

//*******************************************************************************************************************************
void Info_Retry(void)
{
//	sel_com=PuertoMF;
//	tx_chr('R');
//	tx_chr('E');
//	tx_chr('T');
//	tx_chr('R');
//	tx_chr('Y');
//	tx_chr(CR);
//	tx_chr(LF);
//	while (sendactive==1) 
//	{
//	}
//	sel_com=PuertoDB9;

	tx_aux('R');
	tx_aux('E');
	tx_aux('I');
	tx_aux('N');
	tx_aux('T');
	tx_aux('E');
	tx_aux('N');
	tx_aux('T');
	tx_aux('O');
	tx_aux(':');
	tx_aux(RetryWR|0x30);
	tx_aux(CR);
	tx_aux(LF);
}
void posee_in_out()
{
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
			ValTimeOutCom=TIMWPoll;
			buffer_ready=0;
			g_cEstadoComSoft=ESPERA_RX;
			g_cEstadoComSeqMF=SEQ_RTA_rdB4;
}			


//*******************************************************************************************

//*******************************************************************************************
//*******************************************************************************************
//																							*
//			CICLO DE EvP (SECUENCIA VEHICULAR IMPRESION/LECTURA								*
//																							*
//*******************************************************************************************


//*******************************************************************************************	
//	PROGRAMA PRINCIPAL																		*
//*******************************************************************************************
//*******************************************************************************************
void main (void)  
{
	unsigned int k;

//*******************************************************************************************	
  TF2=0; 								//  Bandera de Timer								*
	TH2=0X00;							//													*						
	TL2=0X00;							//													*
	TR2=1;								// Run Timer 0 										*
//*******************************************************************************************	
  	EA = 1;                             // Enable global interrupts 						*
  	com_initialize ();                  // Initialize interrupt driven serial I/O 			*
//*******************************************************************************************	
	TMOD=(TMOD & 0xf0) | 0x01;			//  Coloca el temporizador 0 y 1 en modo 1.  16bITS	*
 	TF0=0; 								//  Bandera de Timer								*
	TH0=0X00;							//													*						
	TL0=0X00;							//													*
	TR0=1;								// Run TM2											*
//******************************************************************************************* 
//*******************************************************************************************
	onceAlarm=0;
//*******************************************************************************************
	led_err=1;							// Error Impresora									*
	rx_in_data = 1;						// Indicador de Rx Transporte o Lectura Wiegand		*	
 	lock=0;								// Relevo 0											*
	Atascado = 0;						//
//------------------------------------------------------------------------------------------*
	seg=CTE_SEG;						//													*
	txd2=1;							   	//													*
	sel_com=PuertoDB9;					//													*
	esQR=0;
	g_cCodSeqMF='0'; 					//													*
//*******************************************************************************************
	Eject_Card(SIN_ERROR);
	
//------------------------------------------------------------------------------------------*
//	Antena_OFF();
//	for (k=0;k<5000;k++)
//	{
//  	wait_long();
//	}
//------------------------------------------------------------------------------------------* 
   	ErrorWR_CARD=0;
	buffer_ready=0;
	FueraServicio=0;
	RetryWR=0;
	GrabaTarjeta=0;
//------------------------------------------------------------------------------------------*
	g_cEstadoComSoft=ESPERA_RX;
	g_cEstadoComSeqMF=SEQ_INICIO;
 	ValTimeOutCom=TIMWPoll;				//													*
	TIME_ESPERA=0;

	for (k=0; k<60; k++)
	{
		buffer_ticket[k]=0;
	}
//*******************************************************************************************
//			LOOP PRINCIPAL		 															*
//*******************************************************************************************
	MultiPark=0;
	PrintLPR=1;
//*******************************************************************************************
	Debug_txt_Tibbo((unsigned char *) "inicio debug jp\r\n"); 
	Debug_Dividir_texto();
	while (1) 								// Loop Principal								*	
  {                         				
//------------------------------------------------------------------------------------------*
		P2=0xff;
		cod_alarm=P2;
		txd2=1;
		sel_com=PuertoDB9;
	//	if(esQR==1){sel_com=PuertoMF;}
//		else{	sel_com=PuertoDB9;}			//PuertoMF;
//------------------------------------------------------------------------------------------*
		ProcesoLector();
//------------------------------------------------------------------------------------------*	
		/*	while(1)
			{
						
						buffer_ticket[0]='h';
						buffer_ticket[1]='o';
						buffer_ticket[2]='l';
						buffer_ticket[3]='A';
						buffer_ticket[4]=' ';
						buffer_ticket[5]=ETX;
						Send_Port(6);
			}				
	*/		
//		if (buffer_ready==1)
//		{
//			buffer_ready=0;
//			sel_com=PuertoMF;						// Solo comn propositos de prueba PortMifare
//			for(k=0;k<g_cContByteRx;k++)
//	 		{
//	        	tx_chr(g_scArrRxComSoft[k]);
//	    	}
//			tx_chr(CR);
//			tx_chr(LF);
//			while (sendactive==1) 
//			{
//			}
//			sel_com=PuertoDB9;
//		}
//------------------------------------------------------------------------------------------*
//------------------------------------------------------------------------------------------*
		if(TF0==1)
		{
			TF0=0;
 
//--------------------------------------------------------- 
   			if (ValTimeOutCom!=0)
			{
				ValTimeOutCom--;
				if (TxENQ==1)																	/*si recibo un ASK significa q transmiti un dato espera la confirmacion q es un ENQ*/
				{
					if(esQR==0)																	/*significa q la prioridad es el lector creator*/
					{
					tx_chr(ENQ);
					TxENQ=0;
					ValTimeOutCom=TIMWPoll;
					g_cEstadoComSoft=ESPERA_INICIO_RTA;
					}
				}
				
			}
//---------------------------------------------------------
 			if (TIME_ESPERA!=0)
			{
				TIME_ESPERA--;
			}
//---------------------------------------------------------
 			seg--;
 			if (seg==0)
			{

			   	   	if (((alarma1==1)||(alarma2==1)||(alarma3==1))&&(onceAlarm==0))
					{
						onceAlarm=1;
				
						cod_alarm=0x30;
						if (alarma1==1)
						{
							cod_alarm++;
							AlarmaPrevia1=1;
						}
						if (alarma2==1)
						{
							cod_alarm++;
							cod_alarm++;
							AlarmaPrevia2=1;	
	 					}
						if (alarma3==1)
						{
							cod_alarm++;
							cod_alarm++;
							cod_alarm++;
							cod_alarm++;
							AlarmaPrevia3=1;
						}										
						
	
	
	
						buffer_ticket[0]=STX;
						buffer_ticket[1]='0';
						buffer_ticket[2]=0X06;
						buffer_ticket[3]='A';
						buffer_ticket[4]=cod_alarm;
						buffer_ticket[5]=ETX;
						Send_Port(6);
					}



   
				seg=CTE_SEG;
  				if (blink==0)
				{
				  led_err=1;
				  blink=1;
				}
				else
				{
 				  led_err=0;
				  blink=0;
				}
//				lock=0;
//------------------------------------------------------------------------------------------*
//				iData=TABLE_CRC16_R[j];
//				tx_chr2bytes(iData);
//				j++;
//------------------------------------------------------------------------------------------*
			}
		}											// if(TF0==1)
//------------------------------------------------------------------------------------------*
//		sel_Funcion();
//		if ((DataIn==0)&&(info==0))
//		{
//			if (busy==1)
//			{
//				info=1;
//				rx_in_data=0;
//				load_info();
//				send_info();
//				rx_in_data=1;
//			}
//		}
//		sel_Funcion();
//		if (DataIn==1)
//		{
//			info=0;
//		}
//------------------------------------------------------------------------------------------*
//		sel_Sensor1();
//		if (DataIn==0)
//		{
//		}
//		sel_Sensor2();
//		if (DataIn==0)
//		{
//		}
//-------------------------------------------------------------------------------------------*
/////////////////////////////////////////////////////////////////////////////////////////////

 		if ((g_cEstadoComSeqMF!=SEQ_WCMD)&&(g_cEstadoComSeqMF!=SEQ_RTA_STATUSout)&&(Expulsa_Grabacion==0))
		{
	 		if (busy==0)
			{
	   			recibe_port();
				if (buffer_ticket[3]=='O')
				{

					Eject_Card(CARD_EJECT);

					if (num_data==5)
					{
//						Eject_Card(CARD_EJECT);

					}
					else if (num_data==16)
					{
//-------------------------------------------------------------------------------------------*
							tx_aux('S');
							tx_aux('I');
							tx_aux('N');
							tx_aux(' ');
							tx_aux('T');
							tx_aux('A');
							tx_aux('R');
							tx_aux('J');
							tx_aux('E');
							tx_aux('T');
							tx_aux('A');
							tx_aux(' ');



							tx_aux('P');
							tx_aux('C');
							tx_aux(':');
							for (k=0; k<16; k++)
							{
								tx_aux(buffer_ticket[k]);	
							}
							tx_aux(CR);
							tx_aux(LF);
//-------------------------------------------------------------------------------------------*						
	 				}
	
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

				else if (buffer_ticket[3]=='?')
				{
					buffer_ticket[0]=STX;
					buffer_ticket[1]='0';
					buffer_ticket[2]=0X06;
					buffer_ticket[3]='e';
					buffer_ticket[4]=0x40;				//Tarjeta Mal Insertada
					buffer_ticket[5]=ETX;
					Send_Port(6);			
   				}
			}					
		}
															   
																						   
/////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************
//	  	ALARMAS
//*******************************************************************************************
//		sel_Sensor1();
//		if ((DataIn==1)&&(Permite_Puerta==0))
//		{
//		 	lock=1;
//			alarma1=1;
//		}
//-------------------------------------------------------
		Tmin=2500;
		Activo=0;
		sel_Sensor1();
		if (DataIn==1)
		{
			sel_Sensor1();
			if (DataIn==1)
			{
				while ((DataIn==1)&&(Activo==0))
				{
					Tmin--;
					if (Tmin==0)
					{
						Activo=1;
					}

				}
				if ((Activo==1)&&(Permite_Puerta==0))
				{
		 			lock=1;
					alarma1=1;
				}
			}
		}
//-------------------------------------------------------
//		sel_Sensor2();
//		if ((DataIn==1)&&(Permite_Billetero==0))
//		{
//		 	lock=1;
//			alarma2=1;
//		}
//--------------------------------------------------------
		Tmin=2500;
		Activo=0;
		sel_Sensor2();
		if (DataIn==1)
		{
			sel_Sensor2();
			if (DataIn==1)
			{
				while ((DataIn==1)&&(Activo==0))
				{
					Tmin--;
					if (Tmin==0)
					{
						Activo=1;
					}
				}
				if ((Activo==1)&&(Permite_Billetero==0))
				{
		 			lock=1;
					alarma2=1;
				}
			}
		}
//*********************************************************
//		sel_Pulsa();
//		if ((DataIn==1)&&(Permite_Monedero==0))
//		{
//		 	lock=1;
//			alarma3=1;
//		}
//---------------------------------------------------------
		Tmin=2500;
		Activo=0;
		sel_Pulsa();
		if (DataIn==1)
		{
			sel_Pulsa();
			if (DataIn==1)
			{
				while ((DataIn==1)&&(Activo==0))
				{
					Tmin--;
					if (Tmin==0)
					{
						Activo=1;
					}
				}
				if ((Activo==1)&&(Permite_Monedero==0))
				{
		 			lock=1;
					alarma3=1;
				}
			}
		}
//********************************************************				

		if ((Permite_Puerta==1)&&(Permite_Billetero==1)&&(Permite_Monedero==1))
		{
			lock=0;
			onceAlarm=0;
			alarma1=0;
			alarma2=0;
			alarma3=0;
		}
	
//*******************************************************************************************
//		Notificacion de Aperturas en Desarmado
//*******************************************************************************************
		if ((Desarmado==1))
		{
			sel_Sensor1();
			if ((DataIn==1)&&(NotifyPuerta==0)&&(AlarmaPrevia1==0))
			{
				NotifyPuerta=1;

				buffer_ticket[0]=STX;
				buffer_ticket[1]='0';
				buffer_ticket[2]=0X06;
				buffer_ticket[3]='S';
				buffer_ticket[4]='1';
				buffer_ticket[5]=ETX;
				Send_Port(6);
 
			}
			
			sel_Sensor2();
			if ((DataIn==1)&&(NotifyCofreB2B==0)&&(AlarmaPrevia2==0))
			{
				NotifyCofreB2B=1;

				buffer_ticket[0]=STX;
				buffer_ticket[1]='0';
				buffer_ticket[2]=0X06;
				buffer_ticket[3]='S';
				buffer_ticket[4]='2';
				buffer_ticket[5]=ETX;
				Send_Port(6);
			}						

			sel_Pulsa();
			if ((DataIn==1)&&(NotifyCofreCoin==0)&&(AlarmaPrevia3==0))
			{
				NotifyCofreCoin=1;

				buffer_ticket[0]=STX;
				buffer_ticket[1]='0';
				buffer_ticket[2]=0X06;
				buffer_ticket[3]='S';
				buffer_ticket[4]='3';
				buffer_ticket[5]=ETX;
				Send_Port(6);
			}

	   	}
/////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------*
//		if(TF2==1)
//		{
//			TF2=0;
//	   		seg2--;
//   		if (seg2==0)
//			{
//				seg2=CTE_SEG;
//			}
//		}
//------------------------------------------------------------------------------------------*
    }												//Cierra While Principal				* 
}													//Cierra main							*
//*******************************************************************************************

