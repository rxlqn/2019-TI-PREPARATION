
#include <string.h>
#include <stdlib.h> 			/* NULL macro */

#include "LPC13xx.h"

#include "ringfifo_uart.h"

#include "protocol.h"

#include "AppProcess.h"





tsPacketOfDataLink sPacketOfDataLinkDown = 
																			 {
																					0x00,													// u8RecAddr
																					0x00,													// u8SendAddr
																					(void *)&sPacketOfAppInfoRec,	// *pPacketOfAppInfo
																					0x00													// u8Checksum
																			 };


tsPacketOfAppInfo sPacketOfAppInfoRec = 	 
																			 {
																					0x00,										// u8PacketNO
																					0x00,										// u8CMD
																					0,											// u16LEN
																					(void *)u8AppRecData		//*pData
																			 };

																			 
tsPacketOfDataLink sPacketOfDataLinkUp = 
																			 {
																					0x00,													// u8RecAddr
																					0x01,													// u8SendAddr
																					(void *)&sPacketOfAppInfoSend,// *pPacketOfAppInfo
																					0x00													// u8Checksum
																			 };


tsPacketOfAppInfo sPacketOfAppInfoSend = 	 
																			 {
																					0x00,										// u8PacketNO
																					0x01,										// u8CMD
																					8,											// u16LEN
																					(void *)u8AppSendData		//*pData
																			 };
																			 
uint8_t u8AppRecData[APP_DATA_LEN];
uint8_t u8AppSendData[APP_DATA_LEN] = { 0xaa };

uint8_t u8DataPool[APP_DATA_LEN];

uint8_t * memstr(uint8_t *haystack, uint32_t size, uint8_t *needle, uint32_t needlesize)
{
	uint8_t *p, u8Single[1];

	if( needle == NULL )
	{
			u8Single[0] = needlesize;
			needle = u8Single;
			needlesize = 1;
	}
	for (p = haystack; p <= (haystack-needlesize+size); p++)
	{
		if( memcmp(p, needle, needlesize) == 0 )
		{
				return p; /* found */
		}
	}
	return NULL;
}


uint32_t UpLinkTransform( uint8_t u8InputData[], uint8_t u8OutputData[], uint32_t u32Len )
{
		 uint32_t u32I = 0, u32J = 0;
		 uint32_t u32DataIndex = 0, u32Tmp  = 0;
	
		 for( ; u32DataIndex < u32Len; u32DataIndex++ )
		 {
										
					if( u32I++ > 6 )
					{
							u32I = 0;
							u8OutputData[u32J << 3] = u32Tmp;
							u32J++;
							u32Tmp = 0;
						
							u32DataIndex--;
					}
					else
					{
							u32Tmp = (u32Tmp >> 1);
							if( u8InputData[u32DataIndex] & 0x80 )
							{
									u32Tmp = (u32Tmp ) | 0x40;
							}
							else
							{
							}
							u8OutputData[(u32J << 3) + u32I ] = u8InputData[u32DataIndex] & 0x7f;
					}
					
		 }// End for

					
		 if( u32I != 0 )
		 {
				 u8OutputData[u32J << 3] = u32Tmp;
		 }
		 
		 return (u32J << 3) + u32I + 1;
}

void DownLinkTransform( uint8_t u8InputData[], uint8_t u8OutputData[], uint32_t u32Len )
{
		uint32_t u32I, u32J, u32Block, u32Remainder;
		uint32_t u32DataIndex = 0;
	
	  u32Block = u32Len >> 3;						// u32Block = u32Len / 8
		u32Remainder = u32Len & (8-1);		// u32Remainder = u32Len % 8
	
		for( u32I = 0; u32I < u32Block; u32I++ )
		{
				 for( u32J = 0; u32J < 8; u32J++ )
				 {
						  if( u32J >= 7 )
							{
									continue;
							}
							if( (u8InputData[(u32I<<3) + 0]) & (0x01 << u32J) )
							{
									u8OutputData[u32DataIndex] = u8InputData[(u32I<<3) + u32J + 1] | 0x80;
							}
							else
							{
									u8OutputData[u32DataIndex] = u8InputData[(u32I<<3) + u32J + 1];
							}
							u32DataIndex++;
				 }
		}
		
		if( u32Remainder == 0 )
		{
				return ;
		}
		
		for( u32J = 0; u32J < (u32Remainder - 1); u32J++ )
		{
				 if( (u8InputData[(u32Block<<3) + 0]) & (0x01 << u32J) )
				 {
						 u8OutputData[u32DataIndex] = u8InputData[(u32Block<<3) + u32J + 1] | 0x80;
				 }
				 else
				 {
						 u8OutputData[u32DataIndex] = u8InputData[(u32Block<<3) + u32J + 1];
				 }			
				 u32DataIndex++;
		}// End for
		
		return;
}


uint8_t u8RecBuffer[REC_BUG_LENGTH];
uint8_t u8SendBuffer[REC_BUG_LENGTH];

void UnloadDataToDataLink(uint8_t u8InputData[], tsPacketOfDataLink *psPacketOfDataLinkRec, uint32_t u32DataLen)
{
		uint32_t u32DataIndex = 0, u32Len;
		tsPacketOfAppInfo *psTmpPacketOfAppInfo;
	
		DownLinkTransform( u8InputData, u8DataPool, u32DataLen );
	
		u32Len = sizeof(psPacketOfDataLinkRec->u8RecAddr) + sizeof(psPacketOfDataLinkRec->u8SendAddr);
		memcpy( (uint8_t *)psPacketOfDataLinkRec, 
						&u8DataPool[u32DataIndex], 
						u32Len
					);
		
		u32DataIndex += u32Len;
	
		u32Len = sizeof(tsPacketOfAppInfo) - sizeof(void*);
		memcpy( (uint8_t *)psPacketOfDataLinkRec->pPacketOfAppInfo, 
						&u8DataPool[u32DataIndex], 
						u32Len
					);
		
		u32DataIndex += u32Len;
	
		psTmpPacketOfAppInfo = psPacketOfDataLinkRec->pPacketOfAppInfo;
		u32Len = psTmpPacketOfAppInfo->u16LEN;
		memcpy( (uint8_t *)psTmpPacketOfAppInfo->pData, 
						&u8DataPool[u32DataIndex],
						u32Len
					);
				
		u32DataIndex += u32Len;
		
		psPacketOfDataLinkRec->u8Checksum = u8DataPool[u32DataIndex++];
		
}

uint32_t LoadDataToDataLink(tsPacketOfDataLink *psPacketOfDataLinkUp, uint8_t u8OutDataBufToSend[])
{
	uint32_t u32I, u32DataIndex = 0, u32Len;
	tsPacketOfAppInfo *psTmpPacketOfAppInfo;
	
	
	u32Len = sizeof(psPacketOfDataLinkUp->u8RecAddr) + sizeof(psPacketOfDataLinkUp->u8SendAddr);
	memcpy( &u8DataPool[u32DataIndex], 
					(uint8_t *)psPacketOfDataLinkUp, 
					u32Len
				);
	
	u32DataIndex += u32Len;
	
	u32Len = sizeof(tsPacketOfAppInfo) - sizeof(void*);
	memcpy( &u8DataPool[u32DataIndex], 
					(uint8_t *)psPacketOfDataLinkUp->pPacketOfAppInfo, 
					u32Len
				);
	
	u32DataIndex += u32Len;
	
	psTmpPacketOfAppInfo = psPacketOfDataLinkUp->pPacketOfAppInfo;
	u32Len = psTmpPacketOfAppInfo->u16LEN;
	memcpy( &u8DataPool[u32DataIndex], 
					(uint8_t *)psTmpPacketOfAppInfo->pData,
					u32Len
				);
				
	u32DataIndex += u32Len;
	
	u8DataPool[u32DataIndex++] = psPacketOfDataLinkUp->u8Checksum;
	// 计算checksum, 重新赋值
	
	
	// 转换至到链路数据层
	u32DataIndex = UpLinkTransform( u8DataPool, u8OutDataBufToSend + 1, u32DataIndex );


	u8OutDataBufToSend[0] = STX;

	u8OutDataBufToSend[1 + u32DataIndex] = ETX;
	
	return u32DataIndex + 1 + 1;
}

void ParseProtocol(uint8_t u8RecData[], uint32_t u32MaxLen)
{
		static uint32_t su32ParseStatus, su32Pos1, su32Pos2;
	
		static uint32_t	su32Len = 0, su32Index = 0, su32LenToBeRec = 0;
		static uint8_t *pStr;	
	
		uint8_t *pu8Tmp;
		uint32_t u32Tmp;
	

		su32Len = su32Len % u32MaxLen; 
	
		su32Len += UARTReceive( LPC_UART, u8RecData + su32Len, u32MaxLen - su32Len );
	
	  if( su32Len == 0 )
		{
				return;
		}
		
	
// 		u8RecData[su32Len++] = 0xbb;
// 		u32Tmp = LoadDataToDataLink(&sPacketOfDataLinkUp, u8SendBuffer);
// 		memcpy( u8RecData + su32Len, u8SendBuffer, u32Tmp);
// 		su32Len += u32Tmp;
// 		u8RecData[su32Len++] = 0xcc;
		
		switch(su32ParseStatus)
		{
				case 0:
						// 在u8RecData中搜索STX字符，搜索长度为目前接收到的字节总数su32Len
						pu8Tmp = memstr( u8RecData, su32Len, NULL, STX );
				
						if( pu8Tmp != NULL )
						{
								su32Pos1 = pu8Tmp - u8RecData; 
								su32ParseStatus = 1;
							
								// 在搜索到STX的后续位置(su32Pos1 + 1)继续搜索ETX，
								// 搜索长度为目前接收到的字节总数su32Len - (su32Pos1 + 1)
								// 此处的1一般一般情况是sizeof(needle string)
								pu8Tmp = memstr( u8RecData + (su32Pos1 + 1), 
																 su32Len - (su32Pos1 + 1), NULL, ETX );  
								if( pu8Tmp != NULL )
								{
										// 匹配到ETX, su32Pos2相对于u8RecData首地址的位置
										//su32Pos2 = pu8Tmp - (u8RecData + (su32Pos1 + 1)) + (su32Pos1 + 1)); 
										su32Pos2 = pu8Tmp - u8RecData; 
										su32ParseStatus = 2;
										
										UnloadDataToDataLink( u8RecBuffer + (su32Pos1 + 1),
																					&sPacketOfDataLinkDown,
																					su32Pos2 - su32Pos1 - 1
																				);
									
										// 计算ETX后接收字节
										u32Tmp = su32Len - (su32Pos2 + 1);
										memmove( u8RecBuffer + su32Pos1, 
														 u8RecBuffer + su32Pos2 + 1,
														 u32Tmp
													 );
										su32Len -= (su32Pos2 - su32Pos1 + 1);	
									
										// 处理用户层
										AppProcess( sPacketOfDataLinkDown.pPacketOfAppInfo );
										
										su32ParseStatus = 0;
								}// End if
								else
								{
										// su32ParseStatus仍然为1
								}
						}
						else
						{
						}
				break;
				case 1:
						// 在搜索到STX的后续位置(su32Pos1 + 1)继续搜索ETX，
						// 搜索长度为目前接收到的字节总数su32Len - (su32Pos1 + 1)
						// 此处的1一般一般情况是sizeof(needle string)
						pu8Tmp = memstr( u8RecData + (su32Pos1 + 1), 
														 su32Len - (su32Pos1 + 1), NULL, ETX );  
						if( pu8Tmp != NULL )
						{
								// 匹配到ETX
								//su32Pos2 = pu8Tmp - (u8RecData + (su32Pos1 + 1)) + (su32Pos1 + 1)); 
								su32Pos2 = pu8Tmp - u8RecData; 
								su32ParseStatus = 2;
							
								UnloadDataToDataLink( u8RecBuffer + (su32Pos1 + 1),
																			&sPacketOfDataLinkDown,
																			su32Pos2 - su32Pos1 - 1
																		);
								
								// 计算ETX后接收字节
								u32Tmp = su32Len - (su32Pos2 + 1);
								memmove( u8RecBuffer + su32Pos1, 
												 u8RecBuffer + su32Pos2 + 1,
												 u32Tmp
											 );
								su32Len -= (su32Pos2 - su32Pos1 + 1);	
									
								// 处理用户层
								AppProcess( sPacketOfDataLinkDown.pPacketOfAppInfo );
								
								su32ParseStatus = 0;
						}
						else
						{
								// su32ParseStatus仍然为1
						}
				break;
				case 2:
				break;
				default:
				break;
		}
}
