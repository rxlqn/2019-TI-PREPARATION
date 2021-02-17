#include "12864.h"
#include "ascii.h"
#include "spi.h"
void LCD_GPIO_init(void){
	
	GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟	
    GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15; 
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     			
}

void LCD_CS(uint8_t status){
	if(status==1)		
		HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_SET);
	else if(status==0)
	{
		HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_RESET);
	}
}

void LCD_RST(uint8_t status){
	if(status==1) HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_SET);
	else if(status==0) HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_RESET);
}

void LCD_RS(uint8_t status){
		if(status==1) HAL_GPIO_WritePin(LCD_RS_PORT,LCD_RS_PIN,GPIO_PIN_SET);
	else if(status==0) HAL_GPIO_WritePin(LCD_RS_PORT,LCD_RS_PIN,GPIO_PIN_RESET);
}

//void LCD_SCK(uint8_t status){
//	if(status==1) HAL_GPIO_WritePin(LCD_SCK_PORT,LCD_SCK_PIN,GPIO_PIN_SET);
//	else if(status==0) HAL_GPIO_WritePin(LCD_SCK_PORT,LCD_SCK_PIN,GPIO_PIN_RESET);
//}

//void LCD_SDA(uint8_t status){
//		if(status==1) HAL_GPIO_WritePin(LCD_SDA_PORT,LCD_SDA_PIN,GPIO_PIN_SET);
//	else if(status==0) HAL_GPIO_WritePin(LCD_SDA_PORT,LCD_SDA_PIN,GPIO_PIN_RESET);
//}

void LCD_Command(uint8_t data){
	uint8_t i;  
	LCD_CS(0);  
	LCD_RS(0);  											// 写命令
	HAL_SPI_Transmit(&hspi1, &data, 1, 100);
//	for(i=0;i<8;i++)  
//	{
//		LCD_SCK(0);   
//		if(data&0x80)	LCD_SDA(1);   		// 写data
//		else LCD_SDA(0);   
//		LCD_SCK(1);   									// 上升沿送数据
//		data=data<<1;  
//	}
	LCD_CS(1); 	
}

void LCD_Data(uint8_t data){
	u8 i;  
	LCD_CS(0);
	LCD_RS(1);												// 写数据
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);

//	for(i=0;i<8;i++)  
//	{
//		LCD_SCK(0);   
//		if(data&0x80)	LCD_SDA(1);   
//		else LCD_SDA(0);   
//		LCD_SCK(1);   
//		data=data<<1;  
//	}
	LCD_CS(1);
}



void LCD_Address(uint8_t page,uint8_t column){
	LCD_CS(0);  
	column=column-1; 
	page=page-1;  
	LCD_Command(0xb0+page); 								//Setting PAGE address
	LCD_Command(((column>>4)&0x0f)+0x10);		//Setting COLUMN address - Bit7~4
	LCD_Command(column&0x0f);								//Setting COLUMN address - Bit3~0
	LCD_CS(1);
}

void LCD_Initial(void){
	LCD_GPIO_init();
//	LCD_ROM_GPIO_init();
	LCD_CS(0); 
	LCD_RST(0);        	//Low voltage reset
	HAL_Delay(100);  
	LCD_RST(1);      		//Reset completed
	HAL_Delay(20);   
    LCD_Command(0xe2);  //Soft reset  
	HAL_Delay(5);  
	LCD_Command(0x2c);  /*升压步骤 1*/  
	HAL_Delay(5);   
	LCD_Command(0x2e);  /*升压步骤 2*/
	HAL_Delay(5);  
	LCD_Command(0x2f);  /*升压步骤 3*/  
	HAL_Delay(5);  
	LCD_Command(0x23);  /*粗调对比度,可设置范围 0x20~0x27*/ 
	LCD_Command(0x81);  /*微调对比度*/  
	LCD_Command(0x23);  /*0x1a,微调对比度的值,可设置范围 0x00~0x3f*/ 
	LCD_Command(0xa2);  /*1/9 偏压比(bias)*/  
	LCD_Command(0xc8);  /*行扫描顺序:从上到下*/ 
	LCD_Command(0xa0);  /*列扫描顺序:从左到右*/ 
	LCD_Command(0x40);  /*起始行：第一行*/ 
	LCD_Command(0xaf);  /*开显示*/  
	LCD_CS(1); 
	LCD_Clear();	
}

void LCD_Clear(void)					//清屏
{
	uint8_t i,j; 
	LCD_CS(0); 
	for(i=0;i<9;i++) 
	{   
		LCD_Address(1+i,1);  
		for(j=0;j<132;j++)  		LCD_Data(0x00);
	}   
	LCD_CS(1); 
}

void LCD_Display_Graphic_32x32(u8 page,u8 column,u8 *dp) 
{  
	u8 i,j;   
	LCD_CS(0);
	for(j=0;j<4;j++) 
	{  
		LCD_Address(page+j,column);   
		for (i=0;i<31;i++)   
		{    
			LCD_Data(*dp);
			dp++;   
		}  
	}  
	LCD_CS(1);
} 

void LCD_Display_Graphic_16x16(u8 page,u8 column,u8 *dp) 
{  
	u8 i,j;   
	LCD_CS(0);
	for(j=0;j<2;j++) 
	{  
		LCD_Address(page+j,column);   
		for (i=0;i<16;i++)   
		{    
			LCD_Data(*dp);
			dp++;   
		}  
	}  
	LCD_CS(1);
} 

void LCD_Display_Graphic_8x16(u8 page,u8 column,u8 *dp) 
{  
	u8 i,j;   
	LCD_CS(0);	
	for(j=0;j<2;j++) 
	{  
		LCD_Address(page+j,column);   
		for (i=0;i<8;i++)   
		{    
			LCD_Data(*dp);
			dp++;   
		}  
	}  
	LCD_CS(1);
} 
void LCD_Display_String_5x8(u8 page,u8 column,u8 *text) 
{
	u8 i=0,j,k,n;  
	LCD_CS(0);
	while(text[i]>0x00)  
	{
		if((text[i]>=0x20)&&(text[i]<=0x7e))   
		{
			j=text[i]-0x20;        
			LCD_Address(page+n,column);    
			for(k=0;k<5;k++)     
			{      
				LCD_Data(ascii_table_5x8[j][k]);
			}        
			i++;    
			column+=6;       
		}  
		else  	i++;  
	}
	LCD_CS(1);	
}

void LCD_Display_String_16x16(u8 page,u8 column,u8 *text) 
{
	u8 k,n;  
	LCD_CS(0); 
  
			for(n=0;n<2;n++)    
			{        
				LCD_Address(page+n,column);    
				for(k=0;k<16;k++)     
				{      
					LCD_Data(*text);
					text++;
				}    
			}    

	LCD_CS(1); 	
}

void LCD_Display_Dot_1x1(u8 page, u8 column, u8 dot)
{
	LCD_CS(0); 
	LCD_Address((page - 1) / 8 + 1, column);
	LCD_Data(dot); 
	LCD_CS(1); 
}

void LCD_Display_Char_8x16(u8 page,u8 column,u8 text)
{
    u8 j,k,n;
    LCD_CS(0);

    if((text>=0x20)&&(text<=0x7e))
    {
        j=text-0x20;
        for(n=0;n<2;n++)
        {
            LCD_Address(page+n,column);
            for(k=0;k<8;k++)
            {
                LCD_Data(ascii_table_8x16[j][k+8*n]);
            }
        }
        column+=8;
    }
    LCD_CS(1);
}

void LCD_Display_String_8x16(u8 page,u8 column,u8 *text)
{
    u8 i=0;
    while(text[i]>0x00)
    {
        LCD_Display_Char_8x16(page,column,text[i]);
        column+=8;
        i++;
    }
}

void LCD_Display_Number_8x16(u8 page,u8 column,u32 num)
{
    char str[10];
    int index=0;

    do
    {
        str[index++]=num%10+'0';
        num=num/10;
    }
    while(num!=0);

    while(index>0)
    {
        LCD_Display_Char_8x16(page,column,str[--index]);
        column+=8;
    }
}

void LCD_Display_Number_Format_8x16(u8 page,u8 column,int32_t num,u8 digit)
{
    char str[10];
    int index=0,i=0,flag=0,temp=0;

    if(num<0)   flag=1;

    do
    {
        temp=num%10;
        if(temp<0) temp=-temp;
        str[index++]=temp+'0';
        num=num/10;
    }
    while(num!=0);

    if(flag)    str[index++]='-';

    for(;index<=digit;index++)
    {
        str[index]=' ';
    }

    while(index>0)
    {
        LCD_Display_Char_8x16(page,column,str[--index]);
        column+=8;
        i++;
    }

}

