#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx.h"
#include "i2c.h"
#include "gpio.h"
#include "stm32f4xx_i2c.h"
#include "preamp.h"
#include "tm_stm32f4_rotary_encoder.h"
#include "tm_stm32f4_usart.h"
#include "menu.h"
#include "bt.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_fatfs.h"
#include <stdio.h>
#include <string.h>
#include "i2s.h"
#include "pcm.h"

/* Notes */
/* - needs to add USB drivers
 * - types.h  -> select.h commented out !! */

int main(void)
{

	SystemInit();
	Gpio_Init();
	PreampInit();
	TM_USART_Init(UART4, TM_USART_PinsPack_1, 115200);
	MenuInit();
	Cs43Init();

	TM_DELAY_Init();
    //Mount drive
    if (f_mount(&FatFs, "", 1) == FR_OK)
    {
        //Unmount drive, don't forget this!
        f_mount(0, "", 1);
    }

    while(1)
    {
    	BTBtnsProc();
    	MenuProc();
    }
}


