//******************************************************************************************
// 		RUTINAS DE RELOJ DS1302
//******************************************************************************************
sbit sck = P1^4 ;   							/* define I/O functions 		*/
sbit io  = P1^5 ;
sbit rst = P1^6 ;
//*********************************************************************************
extern void wait(void);
unsigned char data_clk;
//*********************************************************************************
void retar_clk ()
{
  int i;
  for (i=0; i<5; i++) 		
  {
  ;
  }
}
//*********************************************************************************
void p_ck (void)
{
 	  sck=1;
	  wait();
	  sck=0;
}
//*********************************************************************************
void eb_clk(char dir_clk) {
	int nbits;
	for (nbits=0; nbits<8; nbits++)
	{
	  dir_clk >>=1;
	  if (CY==1)
	  { 
	    io=1;
	  }
	  else
	  {
	    io=0;
	  }
	  p_ck();
	}
}
//*********************************************************************************
extern char lee_clk (unsigned char dir_clk) {            
int	nbits;
	rst=1;
	retar_clk();
	eb_clk(dir_clk);
	for (nbits=0; nbits<8; nbits++)
	{
	  if (io==1) 
	  {
		data_clk = data_clk | 0x80;
		if (nbits<=6) {
		 data_clk >>=1;
		 }
	  }
	  if (io==0)
	  {
	  	data_clk = data_clk & 0x7F;
		if (nbits<=6) 
		 {
		  data_clk >>=1;
		 }
 	  }
	  p_ck();
   	} 
	rst=0;
 	return data_clk;
}
//*********************************************************************************
void wr_clk (unsigned char dir_clk, unsigned char byte) {            
	rst=1;
	retar_clk();
	eb_clk(dir_clk);
	dir_clk=byte;
	eb_clk(dir_clk);
 	rst=0;
 	
 }
//****************************************************************************


