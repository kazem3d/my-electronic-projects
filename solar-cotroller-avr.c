/*******************************************************
This program was created by the
CodeWizardAVR V3.14 Advanced
Automatic Program Generator
© Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 1/21/2024
Author  : 
Company : 
Comments: 


Chip type               : ATtiny13
AVR Core Clock frequency: 4.800000 MHz
Memory model            : Tiny
External RAM size       : 0
Data Stack size         : 16
*******************************************************/

#include <tiny13.h>

#include <delay.h>

// Declare your global variables here

// Bandgap Voltage Reference: Off
#define ADC_VREF_TYPE ((0<<REFS0) | (0<<ADLAR))
unsigned char counter = 0   ;
unsigned char batt;
unsigned char batt_threshold = 10;
unsigned char on_time = 0x55; //OCR0A or OCR0B register values


unsigned char read_adc(unsigned char adc_input)
{
ADMUX=adc_input | ADC_VREF_TYPE;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=(1<<ADSC);
// Wait for the AD conversion to complete
while ((ADCSRA & (1<<ADIF))==0);
ADCSRA|=(1<<ADIF);

return ((ADCW *5.0)/1024.0)*7.833 ;
}

void start_pwm_timer(void)
{

// Timer Period: 13.653 ms
// Output Pulse(s):
// OC0A Period: 13.653 ms Width: 4.264 ms
// OC0B Period: 13.653 ms Width: 4.26 ms
TCCR0A=(1<<COM0A1) | (0<<COM0A0) | (1<<COM0B1) | (1<<COM0B0) | (1<<WGM01) | (1<<WGM00);
TCCR0B=(0<<WGM02) | (1<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0A=on_time;
OCR0B=on_time;
}
void stop_timer(void)
{
TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
TCCR0B=(0<<WGM02) | (1<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;
PORTB=(0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);
}

void start_normal_timer(void)
{
// Timer Period: 39.893 ms
TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
TCCR0B=(0<<WGM02) | (1<<CS02) | (0<<CS01) | (1<<CS00);
TCNT0=0x45;
OCR0A=0x00;
OCR0B=0x00;
PORTB=(0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);
}
// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
 counter ++;
  if(counter >100)
 { 
    stop_timer();
    // solar = read_adc(1<<MUX1|1<<MUX0);
    batt = read_adc(1<<MUX1|0<<MUX0);
    if (batt > 14) {
        // ot protect battry from over charge
        on_time = 0xff;
    }else{
        on_time = 0x55;
    }
    
   if(batt > batt_threshold){ 
     start_pwm_timer(); 
   }
   
   counter = 0;
 }
 else if(counter >74)
 { 
   start_normal_timer();
 }
}

void main(void)
{
// Declare your local variables here

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port B initialization
// Function: Bit5=In Bit4=In Bit3=In Bit2=In Bit1=Out Bit0=Out 
DDRB=(0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (1<<DDB1) | (1<<DDB0);
// State: Bit5=T Bit4=T Bit3=T Bit2=T Bit1=0 Bit0=0 
PORTB=(0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

start_pwm_timer();

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (1<<TOIE0);

// External Interrupt(s) initialization
// INT0: Off
// Interrupt on any change on pins PCINT0-5: Off
GIMSK=(0<<INT0) | (0<<PCIE);
MCUCR=(0<<ISC01) | (0<<ISC00);

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIS1) | (0<<ACIS0);
ADCSRB=(0<<ACME);
// Digital input buffer on AIN0: On
// Digital input buffer on AIN1: On
DIDR0=(0<<AIN0D) | (0<<AIN1D);


// ADC initialization
// ADC Clock frequency: 37.500 kHz
// ADC Bandgap Voltage Reference: Off
// ADC Auto Trigger Source: Timer0 Overflow
// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
DIDR0|=(0<<ADC0D) | (0<<ADC2D) | (0<<ADC3D) | (0<<ADC1D);
ADMUX=ADC_VREF_TYPE;
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

WDTCR=(0<<WDTIF) | (0<<WDTIE) | (1<<WDP3) | (1<<WDCE) | (1<<WDE) | (0<<WDP2) | (0<<WDP1) | (0<<WDP0);

// Global enable interrupts
#asm("sei")

while (1)
      {
      #asm("WDR")
      }
}
