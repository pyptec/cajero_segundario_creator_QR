//******************************************************************************************
// 		RUTINAS DE EEPROM 24FC1025
//******************************************************************************************
sbit sda = P1^2;
sbit scl = P1^3;
//******************************************************************************************
char err_mem []= "ERROR DE MEMORIA" ;
bit memory_flag;
unsigned char byte_rd;
unsigned char l_data_eeprom;
unsigned char l_chr;

//*****************************************************************************************
void wait (void)  {                   			/* wait function 						*/
  ;                                   			
}
//*******************************************************************************************
//*********************************************************************************
void wait_long (void)  {                   		/* wait function 				*/
  	unsigned int j;
 	for (j=0; j<45; j++)    //50
	{
		wait();
	}   
}	
//*********************************************************************************
//*******************************************************************************************
void wait_wr_mem (void)  {                   	// wait function 						  	*	
	unsigned int j;
 	for (j=0; j<870; j++)   					//820
	{
		wait();
	}   
}	
//*******************************************************************************************
void ack(void){
	unsigned int i;
	memory_flag=1;
	scl=1;
	sda=1;
	for (i=0; i<1100; i++)					   //500
	{
	  if (sda==0)
	  {
	  	memory_flag=0;
		scl=0;

		break;
	  }
	}
	
	if (memory_flag==1)
	{ 

	}  
}	
//*******************************************************************************************
//*******************************************************************************************
void ack_lect(void){
	int i;
	scl=1;
	sda=1;
	memory_flag=1;
	for (i=0; i<650; i++)
	{
	  if (sda==0)
	  {
	  	memory_flag=0;
		scl=0;
		break;
	  }
	}
}	
//*******************************************************************************************
void p_me (void)
{
	  scl=1;
	  _nop_();
	  scl=0;
}
//*******************************************************************************************
void e_d_m (unsigned char a_serial_eeprom)
{
	unsigned char nbits;
  	for (nbits=0; nbits<8; nbits++)
	{
	  a_serial_eeprom <<=1;
	  if (CY==1)
	  { 
	    sda=1;
	  }
	  else
	  {
	    sda=0;
	  }
	  scl=1;
	  p_me();  				
	}
	return;
}
//********************************************************************************************
void start (void)
{
 	sda=1;
 	scl=1;
 	_nop_();
 	sda=0;
 	scl=0;
 	return;
}
//*******************************************************************************************
void stop (void)
{
 	scl=0;
 	sda=0;
 	scl=1;
 	sda=1;
 //	wait();
 //	wait();
 	return;
}
//*******************************************************************************************
unsigned char l_d_m (void)
{
	unsigned char nbits;
   	for (nbits=0; nbits<8; nbits++)
	{
		scl=0;
   		if (sda==1) 
	  	{
			l_data_eeprom = l_data_eeprom| 0x01;
			if (nbits<=6) 
			{
				l_data_eeprom<<=1;
				scl=1;
			}
	  	}
	   	if (sda==0)
	  	{
	  		l_data_eeprom = l_data_eeprom & 0xFE;
			if (nbits<=6) 
		 	{
		  		l_data_eeprom <<=1;
				scl=1;
		 	}
 	  	}
    }
	scl=0;
 	return l_data_eeprom;
}
//*******************************************************************************************
//*******************************************************************************************
//	ESCRIBE EN EEPROM																		*
//*******************************************************************************************
void wr_eeprom (unsigned char control,unsigned char dir_h,unsigned char dir_l,unsigned char data_eeprom)  
{
 	scl=0;
	sda=0;
// 	wait();
    start();
	e_d_m(control);
	ack();
	e_d_m(dir_h);
	ack();
	e_d_m(dir_l);
	ack();
	e_d_m(data_eeprom);
	ack();
	stop();
	wait_wr_mem();
	
	scl=1;
	sda=1;
}
//*******************************************************************************************
//	ESCRIBE EN EEPROM																		*
//*******************************************************************************************
void wrpage_eeprom (unsigned char control,unsigned char dir_h,unsigned char dir_l,unsigned char data_eeprom)  
{
 	unsigned int j;
	scl=0;
	sda=0;
 	wait();

	wait();

    start();
	e_d_m(control);
	ack();
	e_d_m(dir_h);
	ack();
	e_d_m(dir_l);
	ack();
 	for (j=0; j<=127; j++)
	{
		e_d_m(data_eeprom);
		ack();
	}
	stop();
	wait_wr_mem();
	scl=1;
	sda=1;
 return;
}
//*******************************************************************************************	
//*******************************************************************************************	
//	LEE EN EEPROM																			*
//*******************************************************************************************
unsigned char rd_eeprom (unsigned char control,unsigned char dir_h,unsigned char dir_l)  
{
 	scl=0;
	sda=0;
 //	wait();
 	start();
	e_d_m(control);
	ack_lect();
	e_d_m(dir_h);
  	ack_lect();
	e_d_m(dir_l);
 	ack_lect();
	start();
	e_d_m(control+0x01);
 	ack();
	scl=0;
 	l_d_m();
	stop();
  	scl=1;
	sda=1;
	return l_data_eeprom;
}