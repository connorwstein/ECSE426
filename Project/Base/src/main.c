/**
  ******************************************************************************
  * @file    main.c
  * @author  Connor Stein (connor.stein@mail.mcgill.ca), Kevin Musgrave (takeshi.musgrave@mail.mcgill.ca),
						 Faisal Al-Kabariti (faisal.al-kabariti@mail.mcgill.ca), & Kamil Ahmad (kamil.ahmad@mail.mcgill.ca)
  * @version V1.0.0
  * @date    12-01-2015
  * @brief   LCD board main file
  ****************************************************************************** 
*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include <stdio.h>
#include <string.h>
#include "cc2500.h"
#include "math.h"
#include "Timers_and_interrupts.h"
#include "LCD_functions.h"

//maximum possible path length received from mobile
#define MAX_PATH_LENGTH 500

//one byte for pktlen and one for address
#define NUMBER_OF_NON_DATA_BYTES 2

//the array to which the FIFO contents will be written
uint8_t fifo_contents[SIZE_OF_FIFO];

//this will contain fifo contents casted to 16 bits
uint16_t path_data[MAX_PATH_LENGTH];

//length of the path that has been received so far
uint16_t length_of_path;

uint8_t number_of_packets_received = 0;

void receiving(void const *argument);
void draw_path(void const *argument);

osThreadDef(receiving, osPriorityNormal, 1, 400);
osThreadId receiving_thread;

osThreadDef(draw_path, osPriorityNormal, 1, 2000);
osThreadId draw_path_thread;

osMutexId path_data_mutex;
osMutexDef(path_data_mutex);


/**
  * @brief  casting fifo contents to 16 bit so that the full range of the LCD display can be used
  * @param  output_array : the 16 bit array
	          input_array : fifo contents
						size_of_array : length of path currently
  * @retval None
  */
void eight_to_sixteen(uint16_t* output_array, uint8_t* input_array,uint16_t size_of_array){
	
	for(int i=0;i<size_of_array;i++){
		output_array[i] = (uint16_t)input_array[i];
	}
	
}

/**
  * @brief  receives data when GDO0 interrupt goes low (i.e. when a packet has been received)
  * @param  None
  * @retval None
  */
void receiving(void const *argument){
	uint8_t num_bytes_to_read = 0;
	path_data_mutex = osMutexCreate(osMutex(path_data_mutex));

	while(1){
		osSignalWait(0x01,osWaitForever);
		
		//get data from FIFO
		num_bytes_to_read = cc2500_Receive_Data(fifo_contents);
		
		printf("num_bytes_to_read %d\n",num_bytes_to_read);
		
		//if there is actual data (no overflow) 
		if(num_bytes_to_read>0){
			number_of_packets_received++;
			
			printf("number_of_packets_received %d\n",number_of_packets_received);
			
			//wait for draw_path to finish copying data from path_data
			osMutexWait(path_data_mutex,osWaitForever);
			//cast to 16 bits
			eight_to_sixteen(path_data+length_of_path,fifo_contents+NUMBER_OF_NON_DATA_BYTES,num_bytes_to_read-NUMBER_OF_NON_DATA_BYTES);
			osMutexRelease(path_data_mutex);
			
			length_of_path += num_bytes_to_read-NUMBER_OF_NON_DATA_BYTES;

			//start drawing the path
			osSignalSet(draw_path_thread,0x01);
		}
	}
}


/**
  * @brief  draws the path recevied in the fifo
  * @param  None
  * @retval None
  */
void draw_path(void const *argument){	
	
	uint16_t final_path_data[MAX_PATH_LENGTH];
	memset(&final_path_data, 0, sizeof(final_path_data));

	//to indicate number of packets received
	uint8_t packet_receive_string[] = {' ',' ','P','K','T',' ','R','E','C','E','I','V','E','D'};
	
	
	while(1){
		osSignalWait(0x01,osWaitForever);
		osMutexWait(path_data_mutex,osWaitForever);
		//copy the path data to a different array to minimize time holding mutex.
		memcpy(final_path_data,path_data,sizeof(path_data));
		osMutexRelease(path_data_mutex);
		
//		for(int i=0;i<length_of_path;i++){
//			printf("path_data[%d] is %d\n", i,final_path_data[i]);
//		}
		
		LCD_Clear(LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_BLACK);
		
		//scale data so that the path is visible regardless of path length
		scale_data_to_screen(final_path_data,length_of_path);
		//reverse the image so that it is easy to understand on the display
		image_reverse_and_zero_offset(final_path_data,length_of_path);
		
		//draw lines connecting points of path
		for (int i=0;i<=length_of_path-4;i+=2){
			LCD_DrawUniLine(final_path_data[i],final_path_data[i+1],final_path_data[i+2],final_path_data[i+3]);
		}
		
		//draw circle at start of path, and cross at end of path
		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DrawCircle(final_path_data[0],final_path_data[1],CROSS_SIZE);
		LCD_SetTextColor(LCD_COLOR_RED);
		draw_cross(final_path_data[length_of_path-2], final_path_data[length_of_path-1]);
		
		//display number of packets received
		packet_receive_string[0] = number_of_packets_received + '0';
		LCD_DisplayStringLine(0,packet_receive_string);


	}
}


/**
  * @brief  GDO0 interrupt handler. Sets the receiver thread signal
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET){
			EXTI_ClearITPendingBit(EXTI_Line4); // Clear the interrupt pending bit
			printf("inside interrupt handler\n");
			osSignalSet(receiving_thread,0x01);
	}
}


/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	memset(&fifo_contents, 0, sizeof(fifo_contents));
	memset(&fifo_contents, 0, sizeof(path_data));
	length_of_path = 0;
	
	/* LCD initiatization */
  LCD_Init();
  
  /* LCD Layer initiatization */
  LCD_LayerInit();
    
  /* Enable the LTDC controler */
  LTDC_Cmd(ENABLE);
  
  /* Set LCD foreground layer as the current layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
	LCD_Clear(LCD_COLOR_WHITE);

	/* wireless chipset initialization */
	cc2500_start_up_procedure();
	
	uint8_t test = 0;
	cc2500_Read_Status_Register(&test,CC2500_PARTNUM);
	
	printf("after reset and initializaion\n");
	printf("partnum %d\n",test);

	receiving_thread = osThreadCreate(osThread(receiving), NULL);
	draw_path_thread = osThreadCreate(osThread(draw_path), NULL);
	
	/* EXTI interrupts for CC250 Initialize*/
	init_EXTI_interrupt();

	EXTI_GenerateSWInterrupt(EXTI_Line4);

	//display READY so that we know the board is ready to display the path
	uint8_t ready_string[] = {'R','E','A','D','Y'};
	LCD_DisplayStringLine(0,ready_string);

	osKernelStart ();                         // start thread execution 
	
}







	
	









