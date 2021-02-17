#include "usart.h"
#include "main.h"


void Clear_Buffer(void);//清空缓存	
int BC28_Init(void);
void BC28_PDPACT(void);
void BC28_ConTCP(void);
void BC28_RECData(void);
void BC28_Senddata(uint8_t *len,uint8_t *data);
uint8_t * Send_check_cmd(uint8_t *str);
uint8_t Send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);
uint8_t Send_data(uint8_t *data,uint8_t *ack,uint16_t waittime);
void u1_printf(char* fmt,...)  ;
typedef struct
{
   uint8_t CSQ;    
   uint8_t Socketnum;   //编号
   uint8_t reclen[10];   //获取到数据的长度
   uint8_t res;      
   uint8_t recdatalen[10];
   uint8_t recdata[100];
} BC28;

