//******************************************************************************
//******************************************************************************
//*  PIC 16F88 v1.0 I2C - RaspberryPI/Nvidia Jetson/Xavier NX
//*    
//*  04.06.2019 - start
//*  main clock 8MHz
//*            ________
//*  free     -|       |- free
//*  free     -|       |- free
//*  free     -|       |- free
//*  reset    -|       |- free
//*  gnd      -|       |- +5V
//*  free     -|       |- free
//*  SDA/RB1  -|       |- free
//*  rx       -|       |- tx
//*  free     -|       |- SCL/RB4
//*            --------
//* 1. Pomiar z jednego kana³u - dzia³a
//* 2. Pomiar z wielu kana³ów -
//* 3. Odebranie danych od mastera i wystawienie sygna³u -

#include <16F88.h>
#device adc=10 
#fuses HS,NOWDT,PUT,BROWNOUT,NOLVP 
#use delay(clock=8000000) 
#use i2c(SLAVE, SDA=PIN_B1, SCL=PIN_B4, address=0x12) 
#use RS232(baud = 19200,xmit=pin_B5,errors)



int8 buffer[12] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF}; 
int8 index; 
byte address;
long adc_result; 


#INT_SSP 
void ssp_interrupt() 
{ 
   int8 incoming, state;

   state = i2c_isr_state(); 
    
   if(state <= 0x80)     // Master is sending data 
   { 
      incoming = i2c_read();
      if(state == 1)                     //First received byte is address
         address = incoming;
      if(state == 2)                     //Second received byte is data
         buffer[address] = incoming;  
   } 
 
   if(state >= 0x80)   // Master is requesting data from slave 
   { 
      index = state & 15;  // Lower 3 bits of state = the index 
      i2c_write(buffer[index]); 
   } 

} 


//====================================== 
void main () 
{ 
setup_adc_ports(sAN0);
setup_adc_ports(sAN1);
setup_adc_ports(sAN2);
setup_adc_ports(sAN3);
setup_adc(ADC_CLOCK_DIV_8);
setup_comparator(NC_NC_NC_NC);
setup_vref(FALSE);
enable_interrupts(INT_SSP); 
enable_interrupts(GLOBAL); 
    
while(1) 
  { 
   set_adc_channel(0); 
   delay_us(20);
   adc_result = read_adc();
   buffer[0] = adc_result >> 8;
   buffer[1] = adc_result;
   set_adc_channel(1); 
   delay_us(20);
   adc_result = read_adc();
   buffer[2] = adc_result >> 8;
   buffer[3] = adc_result;
   set_adc_channel(2); 
   delay_us(20);
   adc_result = read_adc();
   buffer[4] = adc_result >> 8;
   buffer[5] = adc_result;
   set_adc_channel(3); 
   delay_us(20);
   adc_result = read_adc();
   buffer[6] = adc_result >> 8;
   buffer[7] = adc_result;
   delay_us(20);
   
   if(input_state(PIN_B0) == 1) buffer[8] = 1;
   else buffer[8] = 0;
   if(input_state(PIN_B3) == 1) buffer[9] = 1;
   else buffer[9] = 0;
   if(input_state(PIN_B6) == 1) buffer[10] = 1;
   else buffer[10] = 0;
   if(input_state(PIN_B7) == 1) buffer[11] = 1;
   else buffer[11] = 0;
   
 
   delay_ms(50); 
  
  } 

}




