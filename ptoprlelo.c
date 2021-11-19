#include <ptoprlelo.h> 
#include <reg51.h>

sbit rx_in_data = P0^6;				//Indicador de Rx Transporte o Lectura Wiegand		
sbit port_clk = P3^4;				//Recepcion AUX											*
sbit busy = P3^3;  					//Entrada Interrupcion del Procesador principal			*
sbit ready = P3^2;					//Salida. solicitud envio Datos							*


extern unsigned char xdata buffer_ticket[];
extern unsigned char num_data;
extern bit timeOut;
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
 	ready=1;
	port_clk=1;
	rx_in_data=1;					//Led de visualizacion 	OFF
   	

	return timeOut;
}
*/ 
//***************************************************************************************************
void wait (void)  {                   			
  ;                                   			
}
//***************************************************************************************************
void wait_long (void)  {                   			
  	unsigned int j;
 	for (j=0; j<45; j++)    //4550
	{
		wait();
	}   
}	

//***********************************************************************************************************
//		RECIBE INFORMACION DEL PROCESADOR PRINCIPAL POR P2													*
//***********************************************************************************************************
void recibe_port(void)
{

	unsigned int count;
	unsigned char time_out,j;

	num_data=0;
	
//------------------------------------------// BORRA BUFFER
	for (j=0; j<60; j++)
	{
		buffer_ticket[j]=0xff;	
	}
//------------------------------------------	
	
	if (busy==0)
	{
  		rx_in_data=0;
		time_out=0;
		ready=0;
		j=0;	
		for (j=0; j<30; j++)
		{
			while ((port_clk==1)&&(busy==0))
			{
			
			}
			buffer_ticket[j]=P2;	 
			if ((port_clk==0)&&(time_out==0)&&(busy==0))
			{
				buffer_ticket[j]=P2;
				num_data++; 
				count=0;
				while ((port_clk==0)&&(time_out==0))
				{
					count++;
					if (count>30000)		 //65000
					{
						time_out=1;
						j=31;
					}				
				}
			}
			if (busy==1)
			{
				j=31;
			}
 			ready=1;
		 }
		rx_in_data=1;
 	}
	ready=1;
	P2=0xff;

}
