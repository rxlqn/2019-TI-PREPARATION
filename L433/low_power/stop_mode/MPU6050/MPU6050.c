#include "MPU6050.h"
#include "main.h"

uint8_t mode;

//uint8_t MPU_init(uint8_t _mode){
//	Wire.begin();  //iic init
////	TWSR = 0;
////	TWBR = ((16000000L / I2C_SPEED) - 16) / 2;
////	TWCR = 1 << TWEN;
//	mode = _mode;
//	if(testConnection()){
//		initialize();		
//		imuReady = true;
//		if(_mode==MODE_DMP){
//			if(!dmpInitialize()){
//				setDMPEnabled(true); 
//			}else{
//				imuReady = false;
//			}	
//		}else{
//			AHRSinit();
//			gyroPrepare(gyroOffset);
//		}
//	}
//	return imuReady;		
//}

//bool testConnection() {
//    return getDeviceID() == 0x34;
//}

//uint8_t MPU6050::getDeviceID() {
//    I2Cdev::readBits(devAddr, MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, buffer);
//    return buffer[0];
//}