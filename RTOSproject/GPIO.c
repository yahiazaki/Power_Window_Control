#include "tm4c123gh6pm.h"
#include "GPIO.h"	



		/*PortF Initializations*/
		
void InitTask(){

		SYSCTL_RCGCGPIO_R |= 0x20; //enable clock for GPIOF
		GPIO_PORTF_LOCK_R |= 0x4C4F434B; 
		GPIO_PORTF_CR_R |= 0x1F;    
		GPIO_PORTF_DIR_R |= 0x0E;
		GPIO_PORTF_DIR_R |= (0<<4); //Configure SW1 as an input 
		GPIO_PORTF_DEN_R |= 0x1F;  //Configuring pins 0-4 as Digital pins
		GPIO_PORTF_PUR_R |= (1U << 4); //Configuring SW1 as a pull-up resistor  
	
}


		/*GPIOA Initializations*/

void GPIOA_INIT(){

		SYSCTL_RCGCGPIO_R |= 0x01; //enable clock for GPIOA
		GPIO_PORTA_DEN_R |= (1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7); //Configuring pins 2-7 as Digital pins
		GPIO_PORTA_DIR_R |= (1<<3)|(1<<2); //Configure SW1 as an output (DC motor terminals) 
		GPIO_PORTA_DIR_R &= (~(1<<4)); //Configure pin 4 as an input (Turn on Button)
		GPIO_PORTA_DIR_R &= (~(1<<5)); //Configure pin 5 as an input (Turn off Button)
		GPIO_PORTA_DIR_R &= (~(1<<6)); //Configure pin 6 as an input (AntiClockWise Button)
		GPIO_PORTA_DIR_R &= (~(1<<7)); //Configure pin 7 as an input (ClockWise Button)
		GPIO_PORTA_PUR_R |= (1U << 4); //Configuring pin 4 as a pull-up resistor
		GPIO_PORTA_PUR_R |= (1U << 5);
		GPIO_PORTA_PUR_R |= (1U << 6);  
		GPIO_PORTA_PUR_R |= (1U << 7);
		GPIO_PORTA_DATA_R |= (~(1<<2))|(~(1<<3)); //Initially setting the outputs to zero

}


void GPIOB_INIT(){

    SYSCTL_RCGCGPIO_R|=0x02; //enable clock for GPIOB
    GPIO_PORTB_DIR_R &= (~(1<<6)); //setting pins 6 to input pins
    GPIO_PORTB_DIR_R &= (~(1<<3)); //setting pins 3 to input pins
    GPIO_PORTB_DIR_R &= (~(1<<7));
		GPIO_PORTB_DIR_R &= (~(1<<1));
    GPIO_PORTB_DIR_R &= (~(1<<4));
		GPIO_PORTB_DIR_R &= (~(1<<0));	//setting pins 3 to input pins
    GPIO_PORTB_DEN_R|=(1<<6); //setting pins 6 to be digital pins
    GPIO_PORTB_DEN_R|=(1<<3);//setting pins 3 to be digital pins
    GPIO_PORTB_DEN_R|=(1<<7);
		GPIO_PORTB_DEN_R|=(1<<1);
	  GPIO_PORTB_DEN_R|=(1<<4);
		GPIO_PORTB_DEN_R|=(1<<0);
    GPIO_PORTB_PUR_R |= (1<<6); //Pullup resistance for pin 6
    GPIO_PORTB_PUR_R |= (1<<3); //Pullup resistance for pin 3

}


void Passenger_interrupt(){
		
		GPIO_PORTA_IS_R &= (~(1U<<7)) | (~(1U<<6)) |(~(1U<<5)) | (~(1U<<4));  // Set pins 4-7 as edge triggered
		GPIO_PORTA_IBE_R = 0; // Disable IBE bit for PA7,PA6,PA5,PA4 to work with IEV
		GPIO_PORTA_IEV_R &= (~(1U<<7)) | (~(1U<<6))| (~(1U<<4)) | (~(1U<<5)); // Set interrupt to trigger on falling edge 


		// Enable interrupt for Pins 4-7
		GPIO_PORTA_IM_R |= (1U<<7) | (1U<<6) |(1U<<4) | (1U<<5);

		// Enable interrupts globally
		NVIC_EN0_R |= 0x00000001; // Enable interrupt for Port A
		NVIC_PRI0_R = (NVIC_PRI0_R & 0xFFFFFF00) | 0x000000E0; //Setting priority 7 for GPIOA

}


void PORTB_interrupt(){

		GPIO_PORTB_IS_R &= (~(1U<<6)) |(~(1U<<3)) | (~(1U<<7))  | (~(1U<<1)) | (~(1U<<4))  | (~(1U<<0)); // Set pins 0,1,3,4,6,7 as edge triggered
		GPIO_PORTB_IBE_R |= 0; // Disable IBE bit for PA7,PA6 to work with IEV
		GPIO_PORTB_IEV_R &= (~(1U<<6))| (~(1U<<3)) | (~(1U<<7))| (~(1U<<1)) | (~(1U<<4))  | (~(1U<<0)) ; // Set interrupt to trigger on falling edge 
	
		// Enable interrupts for pins 0,1,3,4,6,7 
		GPIO_PORTB_IM_R |= (1U<<7) | (1U<<6) |(1U<<3) |(1U<<1) |(1U<<4) |(1U<<0);

		// Enable interrupts globally
		NVIC_EN0_R |= 0x00000002; // Enable interrupt for Port B
		NVIC_PRI0_R |= (NVIC_PRI0_R & 0xFFFF00FF) | 0x00006000; //Setting priority 3 for GPIOB

}

void PORTE_interrupt(){

			// Set edge-sensitive interrupt on PF4
		GPIO_PORTE_IS_R &= (~(1U<<1)) | (~(1U<<2));  // Set IS bit for PF4 as edge triggered
		GPIO_PORTE_IBE_R = 0; // Disable IBE bit for PF4 to work with IEV
		GPIO_PORTE_IEV_R &= (~(1U<<1)) | (~(1U<<2)); // Set interrupt to trigger on falling edge 


		// Enable interrupt for PF4
		GPIO_PORTE_IM_R |= (1U<<1) | (1U<<2);
			
		// Enable interrupts globally
		NVIC_EN0_R |= 0x00000010; // Enable interrupt for Port F
		NVIC_PRI1_R |= (NVIC_PRI1_R & 0xFFFF00FF) | 0x00006000; //prior 5	

}

void PORTF_interrupt(){

			// Set edge-sensitive interrupt on PF4
		GPIO_PORTF_IS_R &= ~(1U<<4);  // Set IS bit for PF4 as edge triggered
		GPIO_PORTF_IBE_R = 0; // Disable IBE bit for PF4 to work with IEV
		GPIO_PORTF_IEV_R &= ~(1<<4); // Set interrupt to trigger on falling edge 


		// Enable interrupt for PF4
		GPIO_PORTF_IM_R |= 0x10;
			
		// Enable interrupts globally
		NVIC_EN0_R |= 0x40000000; // Enable interrupt for Port F
		NVIC_PRI7_R |= (NVIC_PRI7_R & 0xFF00FFFF) | 0x00A00000; //prior 5	

}