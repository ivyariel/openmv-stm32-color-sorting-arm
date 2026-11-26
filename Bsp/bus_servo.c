#include "bus_servo.h"

#include "usart.h"

#include <stdio.h>
#include <string.h>



#define RX_MAX_BUF 8

//Servo response buffer state
uint8_t Rx_Data[8] = {0};
uint8_t Rx_index = 0;
uint8_t Rx_Flag = 0;
uint8_t RecvFlag = 0;


/* Set a bus servo target position.
 * id: servo ID; 0xFE addresses all servos.
 * value: position from 96 to 4000; center is 2048.
 * time: movement duration; smaller values move faster. 
 * */
void bus_servo_control(uint8_t id, uint16_t value, uint16_t time)
{
    if (value >= 96 && value <= 4000)
    {
        const uint8_t s_id = id & 0xff;
        const uint8_t len = 0x07;
        const uint8_t cmd = 0x03;
        const uint8_t addr = 0x2a;

        const uint8_t pos_H = (value >> 8) & 0xff;
        const uint8_t pos_L = value & 0xff;

        const uint8_t time_H = (time >> 8) & 0xff;
        const uint8_t time_L = time & 0xff;
        
        const uint8_t checknum = (~(s_id + len + cmd + addr + pos_H + pos_L + time_H + time_L)) & 0xff;
        // unsigned char data[] = {0xff, 0xff, s_id, len, cmd, addr, pos_H, pos_L, time_H, time_L, checknum};
        unsigned char data[11] = {0};
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = s_id;
        data[3] = len;
        data[4] = cmd;
        data[5] = addr;
        data[6] = pos_H;
        data[7] = pos_L;
        data[8] = time_H;
        data[9] = time_L;
        data[10] = checknum;
		HAL_UART_Transmit(&huart1,data,11,1000);
		
    }
}

/* Set the servo torque state.
 * id: servo ID; 0xFE addresses all servos.
* value: 1 enables torque;
				 0 releases torque.
 * */

void bus_servo_Unloading(uint8_t id,uint8_t value)
{

        const uint8_t s_id = id & 0xff;
        const uint8_t len = 0x04;//Protocol length field
        const uint8_t cmd = 0x03;//Write command
        const uint8_t addr = 0x28;//Torque control address

        const uint8_t val = value ;
        const uint8_t checknum = (~(s_id + len + cmd + addr + value)) & 0xff;
        unsigned char data[11] = {0};
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = s_id;
        data[3] = len;
        data[4] = cmd;
        data[5] = addr;
        data[6] = val;
        data[7] = checknum;
		HAL_UART_Transmit(&huart1,data,8,1000);

}


/* Assign a servo ID from 1 to 250 */
void bus_servo_set_id(uint8_t id)
{
    if ((id >= 1) && (id <= 250))
    {
        const uint8_t s_id = 0xfe; /* Broadcast ID */
        const uint8_t len = 0x04;
        const uint8_t cmd = 0x03;
        const uint8_t addr = 0x05;
        const uint8_t set_id = id; /* New servo ID */

        const uint8_t checknum = (~(s_id + len + cmd + addr + set_id)) & 0xff;
        // const uint8_t data[] = {0xff, 0xff, s_id, len, cmd, addr, set_id, checknum};
        unsigned char data[8] = {0};
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = s_id;
        data[3] = len;
        data[4] = cmd;
        data[5] = addr;
        data[6] = set_id;
        data[7] = checknum;
		HAL_UART_Transmit(&huart1,data,8,1000);
    }
}

/* Request the current servo position */
void bus_servo_read(uint8_t id)//Read the servo position
{
    if (id > 0 && id <= 250)
    {
        const uint8_t s_id = id & 0xff;
        const uint8_t len = 0x04;
        const uint8_t cmd = 0x02;
        const uint8_t param_H = 0x38;
        const uint8_t param_L = 0x02;
		
        const uint8_t checknum = (~(s_id + len + cmd + param_H + param_L)) & 0xff;
        // const uint8_t data[] = {0xff, 0xff, s_id, len, cmd, param_H, param_L, checknum};

        unsigned char data[8] = {0};
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = s_id;
        data[3] = len;
        data[4] = cmd;
        data[5] = param_H;
        data[6] = param_L;
        data[7] = checknum;
		HAL_UART_Transmit(&huart1,data,8,1000);
    }
}

//Decode the received position value
uint16_t bus_servo_get_value(void)
{
    uint8_t checknum = (~(Rx_Data[2] + Rx_Data[3] + Rx_Data[4] + Rx_Data[5] + Rx_Data[6])) & 0xff;
    if(checknum == Rx_Data[7])
    {
        // uint8_t s_id = Rx_Data[2];
        uint16_t value_H = 0;
        uint16_t value_L = 0;
        uint16_t value = 0;

        value_H = Rx_Data[5];
        value_L = Rx_Data[6];
        value = (value_H << 8) + value_L;
        return value;
    }
    return 0;
}

uint8_t get_Rx_state(void)
{
    return RecvFlag;
}


int now_servo_value[6];//Current servo positions

uint8_t  callback_data[8];
int calue_ = 0;

void Uart3Rx_Handler(char *pdata, uint16_t len)//UART receive callback
{

	if(len == 8)
	{	
		memcpy(callback_data,pdata,8);
		
		uint8_t checknum = (~(callback_data[2] + callback_data[3] + callback_data[4] + callback_data[5] + callback_data[6])) & 0xff;
		
		
		if(checknum == callback_data[7])//Validate the checksum
		{
			//Decode the position
			calue_ = (callback_data[5] << 8) + callback_data[6];
			//Store the position in the servo array
			now_servo_value[callback_data[2]-1] = calue_;
		}
	}
	

}




