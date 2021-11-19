//********************************************************************************
//	RUTINAS DE DISPLAY DMC 2 LINEAS 20 CHR
//********************************************************************************
sbit RS = P1^0 ;   								/* define I/O functions 		*/
sbit E =  P1^1 ;							    /* P3.5 ZC550 					*/

//*********************************************************************************
void wait (void)  {                   			/* wait function 				*/
  ;                                   			
}
//*********************************************************************************
void wait_long (void)  {                   		/* wait function 				*/
  	unsigned int j;
 	for (j=0; j<45; j++)    //50
	{
		wait();
	}   
}	
//*********************************************************************************
void pulso ()  {                   
   	E=0;
  	wait_long();

	E=1;
	wait_long();

	RS=0;
	wait_long();
}
//********************************************************************************
void cont(unsigned char caracter)  { 			/* DIRECCION					*/                  
	pulso();
	P0 = caracter;
  	wait_long();
   	E=0;
 }
//*********************************************************************************
/* 							PROGRAMA EL DISPLAY									 */
//*********************************************************************************
void prg_disp (void)  	 						
 {
	pulso();	  	//Function Set Command: (8-Bit interface)
 	P0=0x3F;	  	//BF cannot be checked before this command.
  	E=0;

	wait_long();	//No data should be transferred to or from
					//the display during this time.

	pulso();	  	//Function Set Command: (8-Bit interface)
 	P0=0x3F;	  	//BF cannot be checked before this command.
  	E=0;

	wait();			//No data should be transferred to or from
					//the display during this time.

	pulso();	  	//Function Set Command: (8-Bit interface)
 	P0=0x3F;	  	//BF cannot be checked before this command.
  	E=0;

	wait();			//No data should be transferred to or from
					//the display during this time.

	pulso();
 	P0=0x38;
  	E=0;

	pulso();
 	P0=0x08;
	E=0;

	pulso();
	P0=0x01;
	E=0;

	pulso();
	P0=0x03;
	E=0;
 
	pulso();
 	P0=0x0C;	  		//borra cursor
  	E=0;
	 

 }
//********************************************************************************
void vdato (unsigned char caracter)  {                   					/* wait function 		*/

   	E=0;
  	wait();
	wait();
	wait();
	wait();

	E=1;
 	wait();
	wait();
	wait();
	wait();
	wait();

	RS=1;
	wait();
	wait();
	wait();
	wait();

   	P0= caracter;
	wait();
	wait();
	wait();
	E=0;

 }