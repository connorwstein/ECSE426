#include "cc2500.h"
#include "stdio.h"
#include "osObjects.h" 


#define SIZE_OF_LONG_TRANSMIT 500
uint8_t test_data[] = {0,0,0,100,100,100,100,0,0,0,100,0,0,100,100,200,200,100,100,0};
uint8_t long_transmit_data[SIZE_OF_LONG_TRANSMIT];
	
	
void test_one_command_strobe(uint8_t correct_response, uint8_t register_address){
	osDelay(100);
	uint8_t command_strobe_response;
	cc2500_Send_Command_Strobe(&command_strobe_response,register_address);
	//printf("response = %d\n",command_strobe_response>>4);
	
	if(command_strobe_response>>4 == correct_response){
		printf("command strobe register 0x%x pass\n",register_address);
	}
	else{
		printf("command strobe register 0x%x FAIL\n",register_address);
	}		
	
}
	

void test_basic_command_strobes(){
	printf("\n\n*****test_basic_command_strobes*****\n\n");
	test_one_command_strobe(0,CC2500_SIDLE);
	test_one_command_strobe(5,CC2500_SRX);
	test_one_command_strobe(1,CC2500_SNOP);
	test_one_command_strobe(5,CC2500_STX);
	test_one_command_strobe(2,CC2500_SNOP);
}


void read_partnum(){
	printf("\n\n*****read_partnum*****\n\n");
	
	uint8_t command_strobe_response;
	
	cc2500_Read_Status_Register(&command_strobe_response,CC2500_PARTNUM);
	
	//printf("partnum = %d\n", command_strobe_response);
	if(command_strobe_response == 128){
		printf("partnum read pass\n");
	}
	else{
		printf("partnum read FAIL\n");
	}
	
}


void test_one_write_and_read(uint8_t test_write_value){
	uint8_t command_strobe_response;
	
	cc2500_Write(&test_write_value,CC2500_PKTLEN,1);
	
	//printf("wrote %d to pktlen\n",test_write_value);
	
	cc2500_Read(&command_strobe_response,CC2500_PKTLEN,1);
	
	//printf("current pktlen = %d\n",command_strobe_response);

	if(command_strobe_response == test_write_value){
		printf("read write pass\n");
	}
	else{
		printf("read write FAIL\n");
	}
	
}

void test_write_and_read(){
	printf("\n\n*****test_write_and_read*****\n\n");
	test_one_write_and_read(7);
	test_one_write_and_read(VAL_CC2500_PKTLEN);
}


void test_frequency(){
	printf("\n\n*****test_frequency*****\n\n");
	
	uint8_t temp_value;
	uint32_t frequency_value = 0;
	uint8_t reserved_value = 1<<6;
	
	cc2500_Read(&temp_value,CC2500_FREQ2,1);
	
	frequency_value += temp_value;
	
	cc2500_Read(&temp_value,CC2500_FREQ1,1);
	
	frequency_value <<=8;
	frequency_value += temp_value;
	
	cc2500_Read(&temp_value,CC2500_FREQ0,1);
	
	frequency_value <<=8;
	frequency_value += temp_value;
	
	float final_frequency = ((float)frequency_value/(float)(1<<16))*26;
	
	if(final_frequency - 2433.016000 < 0.001){
		printf("frequency test pass\n");
	}
	else{
		printf("frequency test FAIL\n");		
	}
	
}


void test_transmit(){
	cc2500_Transmit_Data(test_data,20);
	

	
}

void long_transmit(){
	for(int i=0;i<SIZE_OF_LONG_TRANSMIT;i++){
		if(i%2==0){
			long_transmit_data[i] = i%100;			
		}
		else{
			long_transmit_data[i] = i/10;			
		}
	}
	
	cc2500_Transmit_Data(long_transmit_data,SIZE_OF_LONG_TRANSMIT);
}




//int main(void){
//	
//	cc2500_start_up_procedure();
//	
//	read_partnum();
//	
//	test_write_and_read();
//	
//	test_basic_command_strobes();
//	
//	test_frequency();
//	
//	//test_transmit();
//	
//	long_transmit();
//	
//	printf("\n\n*****done testing*****\n\n");
//}


