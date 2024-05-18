/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <stdbool.h>
#include "tm4c123gh6pm.h"

/* Driver Includes */
#include "GPIO.h"
#include "PWM.h"

/* Declaring semaphore variables of type xSemaphoreHandle*/
xSemaphoreHandle Button6;
xSemaphoreHandle Button7;
xSemaphoreHandle Button3B;

/* Declaring Mutex variables of type xSemaphoreHandle*/
xSemaphoreHandle xMutex1;
xSemaphoreHandle xMutex2;

/*Global flags for the interrupts*/
bool flag = false;
bool Limit1 = true;
bool Limit2 = true;
bool IN6 = false;
bool IN3 = false;
bool IN7 = false;
bool IN6A = false;
bool Protection = true;



/* The tasks to be created. */
void vClockWise(void* pvParameter);
void vAntiClockWise(void* pvParameter);
void vDriverDirections(void* pvParameter);

/*Interrupts Handlers*/
void GPIOA_Handler();
void GPIOB_Handler();
void GPIOF_Handler();
	
void delayMs (int n);


int main()
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                {

	/* Before a semaphore is used it must be explicitly created.  In the following lines
		 binary semaphores are created. */
		vSemaphoreCreateBinary( Button6 );
		vSemaphoreCreateBinary( Button7 );
		vSemaphoreCreateBinary( Button3B );

	/* Mutex Creation*/
	 xMutex1 = xSemaphoreCreateMutex();
	 xMutex2 = xSemaphoreCreateMutex();

		PWM_INIT();
		InitTask();
		GPIOA_INIT();
		GPIOB_INIT();
		Passenger_interrupt();
		PORTB_interrupt();
		PORTF_interrupt();

		__asm("CPSIE i");

		/*Creating the task that will control the clock-wise direction for the passenger*/
		xTaskCreate( vClockWise,"clock_wise_passenger",100, NULL, 1,NULL);
		
		/*Creating the task that will control the anti clock-wise direction for the passenger*/
		xTaskCreate( vAntiClockWise,"anti_clock_wise_passeneger",100, NULL, 1,NULL);
		
		/*Creating the task that will control the directions for the driver*/
	  xTaskCreate( vDriverDirections,"driver_directions",100, NULL, 2,NULL);

	 /* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();

		// The following line should never be reached.  
		//Failure to allocate enough memory from the heap could be a reason.
		for (;;);

}


void delayMs (int n)
{
    int i, j; 
    for(i=0; i < n; i++) 
    {
        for(j=0; j < 3180; j++)
        {
        }
    }
 }

 
/* The directions functions uses mutex to control access to shared resource */
void ClockWiseDirection(){

		xSemaphoreTake( xMutex1, portMAX_DELAY );

		GPIO_PORTA_DATA_R |=(1<<2);
		GPIO_PORTA_DATA_R &=~(1<<3);

		xSemaphoreGive( xMutex1 );
	
}

void AntiClockWiseDirection(){

		xSemaphoreTake( xMutex2, portMAX_DELAY );

		GPIO_PORTA_DATA_R &=~ (1<<2);
		GPIO_PORTA_DATA_R |= (1<<3);

		xSemaphoreGive( xMutex2 );

}

void StopRotation(){

		GPIO_PORTA_DATA_R &=~ (1<<2);
		GPIO_PORTA_DATA_R &=~(1<<3);

}



void GPIOA_Handler (){

		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	
		 /* Giving the semaphore may have unblocked a task - if it did and the
				unblocked task has a priority equal to or above the currently executing
				task then xHigherPriorityTaskWoken will have been set to pdTRUE and
				portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
				higher priority task.*/

		
	//Checking which button triggered the interrupt
		if (GPIO_PORTA_RIS_R & (1U<<7)){

			xSemaphoreGiveFromISR(Button7, &xHigherPriorityTaskWoken );	
			IN7 = true;
			IN6A = false;
			GPIO_PORTA_ICR_R |= (1U<<7);  //Clearing the interrupt flag

		}

		else if(GPIO_PORTA_RIS_R & (1U<<6)){

			xSemaphoreGiveFromISR( Button6, &xHigherPriorityTaskWoken );	
			IN7 = false;
			IN6A = true;
			GPIO_PORTA_ICR_R |= (1U<<6);

		}

		else if(GPIO_PORTA_RIS_R & (1U<<4)){
			
			flag = true;
			GPIO_PORTA_ICR_R |= (1U<<4);
			
		}

		else if(GPIO_PORTA_RIS_R & (1U<<5)){
			
			flag = false;
			StopRotation();
			GPIO_PORTA_ICR_R |= (1U<<5);
			
		}


		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

} 

void GPIOB_Handler(){
	
		portBASE_TYPE xHigherPriorityTaskWoken2 = pdFALSE;

		if(GPIO_PORTB_RIS_R &(1<<6)){
			xSemaphoreGiveFromISR(Button3B, &xHigherPriorityTaskWoken2 );	
			IN6 = true;
			IN3 = false;
			GPIO_PORTB_ICR_R |= (1U<<6);

		}
		else if(GPIO_PORTB_RIS_R &(1<<3)){

			xSemaphoreGiveFromISR(Button3B, &xHigherPriorityTaskWoken2 );
			IN6 = false;
			IN3 = true;		
			GPIO_PORTB_ICR_R |= (1U<<3);

		}
		else if(GPIO_PORTB_RIS_R &(1<<1)){
			Limit1 = false; //If the Limit1 switch triggered an interrupt it sets the flag to false to block the running tasks (NO)
			GPIO_PORTB_ICR_R |= (1U<<1);

		}
		else if(GPIO_PORTB_RIS_R &(1<<7)){
			Limit1 = true;	//If the switch is released it turns the flag to true (NC)
			StopRotation();
			GPIO_PORTB_ICR_R |= (1U<<7);
		}
		else if(GPIO_PORTB_RIS_R &(1<<4)){
			Limit2 = false; //If the Limit2 switch triggered an interrupt it sets the flag to false to block the running tasks (NO)
			GPIO_PORTB_ICR_R |= (1U<<4);

		}
		else if(GPIO_PORTB_RIS_R &(1<<0)){
			Limit2 = true;	//If the switch is released it turns the flag to true (NC)
			StopRotation();
			GPIO_PORTB_ICR_R |= (1U<<0);
		}

		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken2 );

}

//Protection button simulates the jam protection function
void GPIOF_Handler(){

	if(GPIO_PORTF_RIS_R &(1<<4)){
		Protection = false;
		GPIO_PORTF_ICR_R |= (1U<<4);
	}

}


void vClockWise(void* pvParameter){

	
	/*Take the semaphore once to start with so the semaphore is empty before the
    infinite loop is entered.*/
		xSemaphoreTake( Button7, 0 ); 

		for(; ;){

	/* Use the semaphore to wait for the event.The task blocks until
     semaphore has been successfully obtained*/
			xSemaphoreTake( Button7, portMAX_DELAY );

			
	 /* Checking if the button is still pressed so it can be used
			as a manuall button and stops when releasing the button*/
			delayMs(2000);
			if( ( (GPIO_PORTA_DATA_R &(1<<7)) == 0 ) && flag && Limit1 ) 
				{

					while(((GPIO_PORTA_DATA_R &(1<<7))==0) && flag)
						{

							ClockWiseDirection();
							if(!Limit1)
								{
									break;
								}	
						
						}
						
				/*Limit reached*/
				if(!Limit1)
					{
						
						StopRotation();
						GPIO_PORTF_DATA_R = 0x0E;
						vTaskDelay(250/portTICK_RATE_MS);
						
				  }
				else //Button is released
					{
						StopRotation();
				  }
					
			  }
			
	 /* Checking if button is not pressed continuously then the button
			is used as an automatic button that works continuously untill
			Limit1 is reached*/
			else if (flag && Limit1 && IN7)
				{
				
					//delayMs(2000);
					while( ((GPIO_PORTA_DATA_R &(1<<7)) !=0 ) && flag )
						{
						
							ClockWiseDirection();
							if(!Limit1 || !IN7)
								{
									break;
								}	
						
				 	  }
					
					/*Limit reached*/					 
					if(!Limit1)
						 {
						 
								StopRotation();
								GPIO_PORTF_DATA_R = 0x0E;
								vTaskDelay(250/portTICK_RATE_MS);
							 
						 }
					 
					else if (!flag) //flag is false which means that the Driver is using the button 
						 {
								StopRotation(); 
						 }
				 
			  }
			
	 }

}


/*Same as the previous function but with opposite direction*/
void vAntiClockWise(void* pvParameter){

		xSemaphoreTake( Button6, 0 );

		for(; ;){

			xSemaphoreTake( Button6, portMAX_DELAY );


			delayMs(2000);
			if( ((GPIO_PORTA_DATA_R & (1<<6)) == 0 ) && flag && Limit2 )
				{

					while( ((GPIO_PORTA_DATA_R & (1<<6)) == 0 ) && flag )
						{
							
							AntiClockWiseDirection();
							if(!Limit2)
								{
									break;
								}
								
						}
						
					if(!Limit2)
						{
						
							StopRotation();
							GPIO_PORTF_DATA_R = 0x0E;
							vTaskDelay(250/portTICK_RATE_MS);
						
						}
					else
						{
							StopRotation();
						}
			  }
			
			else if(flag && Limit2 && IN6A)
				{
					
					//delayMs(2000);
					while( ( (GPIO_PORTA_DATA_R & (1<<6)) !=0 ) && Protection && flag )
						{
								
							AntiClockWiseDirection();
							if(!Limit2 || !IN6A)
								{
									break;
								}

						}
					
					if(!Limit2)
						{
							
							StopRotation();
							GPIO_PORTF_DATA_R = 0x0E;
							vTaskDelay(250/portTICK_RATE_MS);
							
						}
					else if (!Protection) //If objected detected on the window stop rotation
						{
							
							StopRotation();
							Protection = true;
							
						}
						else if (!flag){
						StopRotation();
						}
		  	}
		}

}

void vDriverDirections(void* pvParameter){

		xSemaphoreTake( Button3B, 0 );

		for(;;){

			xSemaphoreTake(Button3B,portMAX_DELAY);

			
/*If long press detected on button 6 port B then use manuall control*/
			delayMs(2000);
			if( ((GPIO_PORTB_DATA_R &(1<<6)) == 0) && Limit1 )
				
				{

					while( (GPIO_PORTB_DATA_R &(1<<6)) ==0 )
						{
							
							ClockWiseDirection();
							flag = false; //Since Driver has the control, therefore disable passenger if trying to use the buttons at the same time
							if(!Limit1)
								{
									break;
								}
								
					}
					
					if(!Limit1)
						{
							
							StopRotation();
							GPIO_PORTF_DATA_R = 0x0E;
							vTaskDelay(250/portTICK_RATE_MS);
							
						}
					else
						{
						StopRotation();
						}	
						
		  	}
			/*If Limit1 switch is not reached and button 6 is not pressed continuously then use automatic control*/
			else if(IN6 && Limit1) 
				{
					
				//	delayMs(2000);
					while( (GPIO_PORTB_DATA_R &(1<<6)) !=0 )
						{
							
							flag = false;
							ClockWiseDirection();
							if(!Limit1 || !IN6)
								{
									break;
								}
								
						}
						
					if(!Limit1)
						{
							
							StopRotation();
							GPIO_PORTF_DATA_R = 0x0E;
							vTaskDelay(250/portTICK_RATE_MS);
							
						}
						
			  }


				

/*If long press detected on button 3 port B then use manuall control*/
			delayMs(2000);
			if((GPIO_PORTB_DATA_R &(1<<3))==0)
				{

					while( ((GPIO_PORTB_DATA_R &(1<<3)) ==0 ) && Limit2) 
						{
							AntiClockWiseDirection();
							flag = false;
							if(!Limit1)
								{
									break;
								}
								
						}

					if(!Limit2)
						{
							
							StopRotation();
							GPIO_PORTF_DATA_R = 0x0E;
							vTaskDelay(250/portTICK_RATE_MS);
							
						}
					else
						{
							StopRotation();
						}
			 }
			
			/*If Limit1 switch is not reached and button 3 is not pressed continuously then use automatic control*/
			else if(IN3 && Limit2)
				{
					
					//delayMs(2000);
					while( ((GPIO_PORTB_DATA_R &(1<<3)) != 0 ) && Protection )
						{
							
							AntiClockWiseDirection();
							flag = false;
							if(!Limit2 || !IN3){
									break;
								}

					  }
				 if(!Limit2)
					 {
						 
							StopRotation();
							GPIO_PORTF_DATA_R = 0x0E;
							vTaskDelay(250/portTICK_RATE_MS);
						 
					 }
				 else if (!Protection)
					 {
						 
						 StopRotation();
						 Protection = true;
						 
				   }
			 
			}
			
	}
}

