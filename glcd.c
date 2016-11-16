/**
*****************************************************************************
** Kommunik�ci�s m�r�s - glcd.c
** A grafikus LCD f�ggv�nyei
*****************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "glcd.h"
#include "font.h"
#include "gpio.h"
#include "tm_stm32f4_delay.h"
/* Defines -------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* DB0 -> DB1 -> ... -> DB7 */
GPIO_TypeDef *LCDPort[] = {	GLCD_DB0_PORT,GLCD_DB1_PORT,GLCD_DB2_PORT,GLCD_DB3_PORT,
							GLCD_DB4_PORT,GLCD_DB5_PORT,GLCD_DB6_PORT,GLCD_DB7_PORT};
uint16_t LCDPin[] = {	GLCD_DB0_PIN,GLCD_DB1_PIN,GLCD_DB2_PIN,GLCD_DB3_PIN,
						GLCD_DB4_PIN,GLCD_DB5_PIN,GLCD_DB6_PIN,GLCD_DB7_PIN};

//******************************************************************************************
// @le�r�s: Kijelz� �r�s-k�sleltet�si id�.
//******************************************************************************************
void Sys_DelayUs(int us)
{
	sys_delay = us;
	while(sys_delay);
}

void GLCD_Delay(char value)
{
	Sys_DelayUs(value);
}

//******************************************************************************************
// @le�r�s: Inicializ�lja a kijelz�t, majd kirajzolja a kezd�k�pet.
// 			Nem olvassa a "Busy"-flaget.
// 			Az id�z�t�sek szoftveres sz�ml�l� (GLCD_Delay) alapj�n t�rt�nik.
//******************************************************************************************
void GLCD_Init(void){
	GPIO_ResetBits(GLCD_E_PORT,GLCD_E_PIN);	//GLCD_E = 0
	GPIO_SetBits(GLCD_RESET_PORT, GLCD_RESET_PIN);	//GLCD_RESET = 1
	GLCD_Write(3,0,0x3F); 	//GLCD bekapcsol�sa
	GLCD_Write(3,0,0xC0);	//GLCD Start Line
	GLCD_Clear();			//Kijelz� t�rl�se
}

//******************************************************************************************
// @le�r�s: Kijelz� meghajt� kimenet�t enged�lyez� f�ggv�ny
// @param�ter: newState = ENABLE, #GLCDEN = 0
//			   newStare = DISABLE, #GLCDEN = 1
//******************************************************************************************
void GLCDEN(FunctionalState newState){
	if(newState){
		GPIO_ResetBits(GLCD_EN_PIN,GLCD_EN_PIN);
	}else{
		GPIO_SetBits(GLCD_EN_PIN,GLCD_EN_PIN);
	}
}

//******************************************************************************************
// @le�r�s: Be�rja a g_data �rt�ket a kiv�lasztott kijelz�vez�rl�be (cs_s->CS1, CS2)
// 			utas�t�s/adat param�ternek megfelel�en.
// 			�ltal�nosan felhaszn�lhat� 8bit (adat/utas�t�s) be�r�s�ra a kijelz� vez�rl�j�be.
// @param�ter: cs_s, 1 = CS1, 2 = CS2, 3 = CS1&CS2
// @param�ter: d_i, 0 = instruction, 1 = data
//******************************************************************************************
void GLCD_Write(char cs_s,char d_i,char g_data){
	uint32_t i;
	char bit;
	switch(cs_s){
		case 1:
			GPIO_SetBits(GLCD_CS1_PORT, GLCD_CS1_PIN);	//CS1 = 1
		break;
		case 2:
		 	GPIO_SetBits(GLCD_CS2_PORT, GLCD_CS2_PIN);	//CS2 = 1
		break;
		case 3:
			GPIO_SetBits(GLCD_CS1_PORT, GLCD_CS1_PIN);	//CS1 = 1
			GPIO_SetBits(GLCD_CS2_PORT, GLCD_CS2_PIN);	//CS2 = 1
		break;
	}
	switch(d_i){
	case 0:
		GPIO_ResetBits(GLCD_DI_PORT, GLCD_DI_PIN);	//PD6 = 0 -> Instruction
		break;
	case 1:
		GPIO_SetBits(GLCD_DI_PORT, GLCD_DI_PIN);	//PD6 = 1 -> Data
		break;
	}

	for(i=0; i<8; i++)
	{
		bit = g_data & 1;
		if(bit)
		{
			GPIO_WriteBit(LCDPort[i],(LCDPin[i]),Bit_SET);
		}
		else
		{
			GPIO_WriteBit(LCDPort[i],LCDPin[i],Bit_RESET);
		}
		g_data >>= 1;
	}

	GLCD_Delay(1);
	GPIO_SetBits(GLCD_E_PORT,GLCD_E_PIN);	//GLCD_E = 1
	GLCD_Delay(2);
	GPIO_ResetBits(GLCD_E_PORT,GLCD_E_PIN);	//GLCD_E = 0
	GLCD_Delay(4);
	GPIO_ResetBits(GLCD_CS1_PORT, GLCD_CS1_PIN);	//CS1 = 0
	GPIO_ResetBits(GLCD_CS2_PORT, GLCD_CS2_PIN);	//CS2 = 0
}
//******************************************************************************************
// @le�r�s: K�zvetlen�l t�rli a kijelz�t.
// @param�ter: nincs
//******************************************************************************************
void GLCD_Clear(void){
	char x,y;
	for(x=0;x<8;x++){
		GLCD_Write(3,0,0x40);
		GLCD_Write(3,0,(0xB8|x));
		for(y=0;y<64;y++){
			GLCD_Write(3,1,0x00);
	  }//for
	}//for
}
//******************************************************************************************
// @le�r�s: A kijelz� adott sor-oszlop metszetet �ll�tja az m_data �rt�knek megfelel�en.
// @param�ter: m_data: adott metszet rajzolata hex�ba k�dolva (l�sd.: BitFont.excel
//			   cX: sor (0-7)
//			   cY: oszlop (0-127)
//******************************************************************************************
void GLCD_Write_Block(char m_data,char cX,char cY){
	char chip=1;
	if(cY>=64){
	chip=2;
	cY-=64;
	}
	GLCD_Write(chip,0,(0x40|cY));
	GLCD_Write(chip,0,(0xB8|cX));
	GLCD_Write(chip,1,m_data);
}
//******************************************************************************************
// @le�r�s: Az �tadott stringet ki�rja a kijelz�re a font-data alapj�n.
// @param�ter: string: tetsz�leges sz�veg
//			   X: Kijelz� egy sor�t adja meg (0-7)
//			   Y: Kijelz� egy oszlop�t v�lasztja ki (0-127)
//******************************************************************************************
void GLCD_WriteString(const char* string,char Y, char X){
	char temp = 0;
	int i=0;
	while(string[i]!='\0'){
		temp = string[i];
		GLCD_Write_Char(temp-32,X,Y+6*i);
		i++;
	}
}
//******************************************************************************************
// @le�r�s: Egy karaktert �r ki a kijelz�re (fontdata-alapj�n).
// @param�ter: cPlace: (karakter ASCII-�rt�ke)-32
//			   cX: kijelz� egy sor�t adja meg
//			   cY: kijelz� egy oszlop�t adja meg
//******************************************************************************************
void GLCD_Write_Char(char cPlace,char cX,char cY){
	char i=0;
	char chip=1;
	if(cY>=64){
		chip=2;
		cY-=64;
	}//if
	GLCD_Write(chip,0,(0x40|cY));
	GLCD_Write(chip,0,(0xB8|cX));
	for (i=0;i<5;i++){
	  if (cY+i >= 64){
		  chip=2;
		  GLCD_Write(chip,0,(0x40|(cY+i-64)));
		  GLCD_Write(chip,0,(0xB8|cX));
	  }//if
	  GLCD_Write(chip,1,fontdata[cPlace*5+i]);
	  }//for
}

