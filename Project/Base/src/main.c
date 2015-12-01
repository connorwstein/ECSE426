/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
#include "background16bpp.h"
#include <stdio.h>
#include <string.h>
#include "cc2500.h"
#include "math.h"
#include "Timers_and_interrupts.h"

#define MAX_PATH_LENGTH 100
#define SIZE_OF_TEST_DATA 100
#define X_ZERO_OFFSET 50
#define Y_ZERO_OFFSET 50
#define IMAGE_REVERSE_OFFSET 240
#define CROSS_SIZE 5
uint8_t fifo_contents[SIZE_OF_TEST_DATA];
uint16_t path_data[MAX_PATH_LENGTH];
uint16_t length_of_path;

void receiving(void const *argument);
void draw_path(void const *argument);

osThreadDef(receiving, osPriorityNormal, 1, 400);
osThreadId receiving_thread;

osThreadDef(draw_path, osPriorityNormal, 1, 400);
osThreadId draw_path_thread;

osMutexId path_data_mutex;
osMutexDef(path_data_mutex);


void eight_to_sixteen(uint16_t* output_array, uint8_t* input_array,uint16_t size_of_array){
	
	for(int i=0;i<size_of_array/2;i++){
		output_array[i] = (input_array[i*2] << 8) + input_array[(i*2)+1];
	}
	
}

void receiving(void const *argument){
	uint8_t num_bytes_to_read = 0;

	while(1){
		osSignalWait(0x01,osWaitForever);
		
		osDelay(1000);
		//printf("/IN RECEIVING ");
		num_bytes_to_read = cc2500_Receive_Data(fifo_contents);
		
		printf("num_bytes_to_read %d\n",num_bytes_to_read);
		
		osMutexWait(path_data_mutex,osWaitForever);
		eight_to_sixteen(path_data+length_of_path,fifo_contents,num_bytes_to_read);
		osMutexRelease(path_data_mutex);
		
		for(int i=0;i<num_bytes_to_read;i++){
			printf("fifo_contents[%d] is %d\n", i,fifo_contents[i]);
		}
		
		length_of_path += (num_bytes_to_read / 2);
		
		osSignalSet(draw_path_thread,0x01);
	}
}


void image_reverse_and_zero_offset(uint16_t* input_data, uint16_t size_of_array){
	for(int i=0;i<size_of_array;i++){
		if(i%2 == 0){
			input_data[i] += X_ZERO_OFFSET;
			input_data[i] = IMAGE_REVERSE_OFFSET - input_data[i];
		}
		else{
			input_data[i] += Y_ZERO_OFFSET;
		}
	}
}


void draw_cross(uint16_t x_point,uint16_t y_point){
	LCD_DrawUniLine(x_point-CROSS_SIZE,y_point-CROSS_SIZE,x_point+CROSS_SIZE,y_point+CROSS_SIZE);
	LCD_DrawUniLine(x_point-CROSS_SIZE,y_point+CROSS_SIZE,x_point+CROSS_SIZE,y_point-CROSS_SIZE);
}

void draw_path(void const *argument){	
	
	uint16_t final_path_data[MAX_PATH_LENGTH];
	memset(&final_path_data, 0, sizeof(final_path_data));

	uint16_t test_data[10]={0,0,25,100,50,200,75,180,100,150};
	image_reverse_and_zero_offset(test_data,10);
	
	
	//while(1){
	//osSignalWait(0x01,osWaitForever);
		//osMutexWait(path_data_mutex,osWaitForever);
		memcpy(final_path_data,path_data,length_of_path);
		//osMutexRelease(path_data_mutex);
	
		LCD_Clear(LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_BLUE2);
		for (int i=0;i<8;i+=2){
			LCD_DrawUniLine(test_data[i],test_data[i+1],test_data[i+2],test_data[i+3]);
		}
		draw_cross(test_data[0],test_data[1]);

		//osDelay(10);
			
	//}
}



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

	osKernelStart ();                         // start thread execution 
	
}







	
	









