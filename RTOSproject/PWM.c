
/*GPIOPCTL --> each GPIO pin has 4 bits for configuring some alternate functions as PWM, UART,....etc.
               PWM is bit 2 from the 4 bits so to enable PWM alternate function enable bit 2 for the required pin

                before enabling the required bit, we must clear all the bits of the required pin to ensure that
                any previous alternate function configurations are removed.
  pin 0 --> bits (0:3)
  pin 1 --> bits (4:7)
  pin 2 --> bits (8:11)
  pin 3 --> bits (12:15)
  pin 4 --> bits (16:19)
  pin 5 --> bits (20:23)
  pin 6 --> bits (24:27)
  pin 7 --> bits (28:31)*/

/*
PWM1_3_GENA_R --> decides when action is taken , when reload, when counter matches compare when count down or when count up and so on

when action is taken: 
bits 0:1 when counter reaches zero
bits 2:3 when reload
bits 4:5 when counter match compareA (when count up)
bits 6:7 when counter match CompareA (when count down)

Actions Done:
0x00 Do Nohing
0x01 Invert PWMA
0x10 Drive PWMA Low
0x11 Drive PWMA High 
 
*/

/*
PWMCMPA --> when count down the counter counts untill reaches zero, but when counter matches the compare value an action is taken
            based on the PWMGENA
            so in our case the PWMGENA is configured to make go high when reloading and go down when counter matches compareA
            therefore each time the counter reaches 2500 the signal goes down and when counter reloads (reaches 5000) signal goes up
            
            CompareA can have any number within the range of the load, but this value gives a 50% duty cycle if we increased the number
            the motor becomes faster and decreasing it will lead to slower rotation (all of this in case of count down)
*/



#include "tm4c123gh6pm.h"
#include "PWM.h"


void PWM_INIT(){

            //clock setting
  SYSCTL_RCGCPWM_R |= (1U<<1); //module 1
  SYSCTL_RCGCGPIO_R |= 0x20;
  SYSCTL_RCC_R |=  0x001E0000; //divisor 64
  
  GPIO_PORTF_AFSEL_R |= (1U<<2);
  GPIO_PORTF_PCTL_R &= ~0x00000F00;        // clears the bits for pin 2 to ensure that any previous alternate function configurations are removed.
  GPIO_PORTF_PCTL_R |= 0x00000500;         // configure pin PF2 for PWM output
  GPIO_PORTF_DEN_R |= (1U<<2);
  
  PWM1_3_CTL_R |= (0<<0);
  PWM1_3_CTL_R |= (0<<1); //Setting the counter to count down
  PWM1_3_GENA_R = 0x0000008C;
  PWM1_3_LOAD_R = 5000;
  PWM1_3_CMPA_R = 4000;
  PWM1_3_CTL_R |= (1<<0);
  PWM1_ENABLE_R = 0x40;
}



