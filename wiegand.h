sbit D0= P0^2;
sbit D1= P0^3;

 extern unsigned char nbitsW;
 extern unsigned char xdata buffer_wie[10];

 bit paridad;							//bandera para indicar tipo de paridad
 bit error_wie;	
 bit lect1_ok;

 unsigned char apunta;
 unsigned char byte_temp;
 unsigned char byte;
 unsigned char compara;

 unsigned char byte_wie; 
 unsigned char bcc;
 unsigned char dato_new;
 unsigned char type_lect;
 unsigned char shift;


//*******************************************************************************************
//	BORRA BUFFER																			*
//*******************************************************************************************

//*******************************************************************************************
void rote (void)		  					/*rota y almacena en xram datos				   */
{
	byte_wie=byte_wie|dato_new;
	byte--;									//byte contiene los 8 bits para armar byte wiegand
	nbitsW++;
	if (byte>0)								//Byte va decrementando
	{
		byte_wie=byte_wie<<1;	
	}
	if (byte==0) 							//Si byte llega a cero ya alamceno 8 bits en Byte Wiegand
	{
	    buffer_wie[apunta]=byte_wie;
		byte=0X08;
		byte_wie=0;
		apunta=apunta+1;
		
	}
}
//*******************************************************************************************
//*******************************************************************************************
//		RECIBE WIEGAND POR LECTOR 1															*
//******************************************************************************************
void  rx_wiegand (void) 			 			/* lectura wiegand LOGICA NEGADA POR ULN2004  */
{                   			
 

}


//*****************************************************************************************************************
void analiza_wiegand(void)
{

	if ((nbitsW==0x1a)|(nbitsW==0x19))
	{

 	}
	else
	{

	}
 
}

//****************************************************************************************************************
