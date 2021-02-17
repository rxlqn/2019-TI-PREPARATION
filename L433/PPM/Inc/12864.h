
#include "main.h"

//LCD pin setting

#define LCD_CS_PIN				GPIO_PIN_15
#define LCD_RST_PIN				GPIO_PIN_14
#define LCD_RS_PIN				GPIO_PIN_13
//#define LCD_SDA_PIN				GPIO_PIN_1
//#define LCD_SCK_PIN				GPIO_PIN_0

#define LCD_CS_PORT				GPIOC
#define LCD_RST_PORT			GPIOC
#define LCD_RS_PORT				GPIOC
//#define LCD_SDA_PORT			GPIOB
//#define LCD_SCK_PORT			GPIOB


//底层函数
void LCD_GPIO_init(void);
void LCD_CS(uint8_t status);
void LCD_RST(uint8_t status);
void LCD_RS(uint8_t status);
void LCD_SDA(uint8_t status);
void LCD_SCK(uint8_t status);
void LCD_Command(uint8_t data);
void LCD_Data(uint8_t data);
void LCD_Address(uint8_t page,uint8_t column);

//应用函数
void LCD_Initial(void);				//初始化
void LCD_Clear(void);					//清屏
void LCD_Display_Graphic_32x32(u8 page,u8 column,u8 *dp);
void LCD_Display_Graphic_16x16(u8 page,u8 column,u8 *dp);
void LCD_Display_Graphic_8x16(u8 page,u8 column,u8 *dp);
void LCD_Display_String_8x16(u8 page,u8 column,u8 *text);
void LCD_Display_String_5x8(u8 page,u8 column,u8 *text);
void LCD_Display_String_GB2312(u8 y,u8 x,u8 *text);
void LCD_Display_String_16x16(u8 page,u8 column,u8 *text);
void LCD_Display_Dot_1x1(u8 page, u8 column, u8 dot);
void LCD_Display_hanzi(u8 page, u8 column, u8 zihao);
void LCD_Display_Char_8x16(u8 page,u8 column,u8 text);
void LCD_Display_String_8x16(u8 page,u8 column,u8 *text);
void LCD_Display_Number_8x16(u8 page,u8 column,u32 num);
void LCD_Display_Number_Format_8x16(u8 page,u8 column,int32_t num,u8 digit);

//底层函数
void LCD_GPIO_init(void);
void LCD_CS(uint8_t status);
void LCD_RST(uint8_t status);
void LCD_RS(uint8_t status);
void LCD_SDA(uint8_t status);
void LCD_SCK(uint8_t status);
void LCD_Command(uint8_t data);
void LCD_Data(uint8_t data);
void LCD_Address(uint8_t page,uint8_t column);



