#include "openmv.h"
#include <string.h>

uint8_t receive_buff3[18];  // Receive buffer
extern DMA_HandleTypeDef hdma_usart3_rx;  // USART3 receive DMA handle
extern UART_HandleTypeDef huart3;  // USART3 handle
extern UART_HandleTypeDef huart4;  // UART4 handle

volatile OpenmvTypeDef Openmv={0,0,0,0};  // Stores data received from OpenMV

/**
 * @brief Initialize the OpenMV UART receiver
 * 
 * Enable USART3 idle interrupts and start DMA reception.
 */
void OpenmvInit(void)
{	
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);  // Enable the USART3 idle interrupt
	HAL_UART_Receive_DMA(&huart3, receive_buff3, sizeof(receive_buff3));  // Start USART3 DMA reception
}

/**
 * @brief Handle the USART3 idle interrupt
 * 
 * @param huart UART handle
 */
void USER_UART3_IRQHandler(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3 &&
        __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        HAL_UART3_RxCpltCallback(huart);
    }
}

/**
 * @brief Process a received OpenMV frame
 * 
 * @param huart UART handle
 */
void HAL_UART3_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint16_t data_length =
        sizeof(receive_buff3) - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
    OpenmvTypeDef frame;

    HAL_UART_DMAStop(huart);
    if (data_length == sizeof(receive_buff3) &&
        receive_buff3[0] == 0xA5 &&
        receive_buff3[17] == 0x5A)
    {
        memcpy(&frame, &receive_buff3[1], sizeof(frame));
        Openmv.x = frame.x;
        Openmv.y = frame.y;
        Openmv.h = frame.h;
        Openmv.m = frame.m;
    }

    memset(receive_buff3, 0, sizeof(receive_buff3));
    HAL_UART_Receive_DMA(huart, receive_buff3, sizeof(receive_buff3));
}

/**
 * @brief Convert a signed byte to an integer
 * 
 * @param a Signed byte
 * @return int Converted integer value
 */
int ByteToInt(char a)
{
	int data;
	if ((a & 0x7F) == a) return a;  // Return nonnegative values unchanged
	else
	{
		data = a - 0x01;  // Prepare the negative value for conversion
		data = data ^ 0xFF;  // Recover the negative value's magnitude
		return data * -1;  // Restore the negative sign
	}
}
