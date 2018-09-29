/*
 * Util.c
 *
 *  Created on: Sep 24, 2015
 *      Author: Ryan
 */

#include "Util.h"
#include "IFsh1.h"
#include "qfplib.h"

volatile bool i2cWriteMode = FALSE;
volatile bool i2cTxDone = FALSE;
//This is a small hack to fix a bug that showed up shifting to 3.3v and adding in all the I2C regs the processor would "reset"
//causing a failed APEN response and not enabling the pH analog power rail, for now rail is defaulted on and can now be switched with resposne
volatile bool enAnalogPower = TRUE;
volatile bool enECPower = FALSE;
volatile char tempChar = 0;
volatile char *ptr_tempChar = &tempChar;
I2CRegister registerData;
LDD_TDeviceData* ptrTimerDeviceData;
float vRef = 3.28;

bool isErased(uint8_t *ptr, int nofBytes) {
  while (nofBytes>0) {
    if (*ptr!=0xFF) {
      return FALSE; /* byte not erased */
    }
    ptr++;
    nofBytes--;
  }
  return TRUE;
}

bool shouldDefault(uint16_t numtocompare)
{

    if (numtocompare != NVMC_MAGIC_NUMBER)
    {
      return TRUE; /* byte not erased */
    }

    return FALSE;
}

uint8_t NVMC_SaveParamsData(nvmStorage_t *data, uint16_t dataSize)
{
	uint8_t debugErrOut = 0;
	//quick size upper bounds check
	if (dataSize>NVMC_TOTAL_DATA_SIZE)
	{
		debugErrOut = ERR_OVERFLOW;
		return debugErrOut;
	}
	//we didnt choose destructive write using Processor Expert, we need to make sure we erase first!
	EnterCritical();
	debugErrOut = IFsh1_EraseSector(NVMC_FLASH_START_ADDR);
	if(debugErrOut != ERR_OK)
	{
		return debugErrOut;
	}

	debugErrOut = IFsh1_SetBlockFlash((IFsh1_TDataAddress)data, (IFsh1_TAddress)(NVMC_FLASH_START_ADDR), dataSize);
	ExitCritical();
	return debugErrOut;
}

uint8_t NVMC_GetSavedData(nvmStorage_t *ptrNVMStorageData)
{
  if (isErased((uint8_t*)NVMC_FLASH_START_ADDR, NVMC_TOTAL_DATA_SIZE))
  {
    return NULL;
  }
  IFsh1_GetBlockFlash(NVMC_FLASH_START_ADDR, (IFsh1_TDataAddress)ptrNVMStorageData, NVMC_TOTAL_DATA_SIZE);
  return 1;
}

uint8_t setDataToDefault(nvmStorage_t *ptrDataToStore)
{
	ptrDataToStore->nvmMagicNumber = 0xCADE;
	ptrDataToStore->i2cAddress = 0x4F;
	ptrDataToStore->altI2CAddress = 0x4A;
	strcpy(ptrDataToStore->deviceIDInfo, "Sparky's Widgets MiniCombo V1!");
	ptrDataToStore->deviceVerison = 0x0001;

	ptrDataToStore->phCalType = 'L';
	ptrDataToStore->phLowCalPoint = 0x0100;
	ptrDataToStore->phMidCalPoint = 0x0100;
	ptrDataToStore->phHighCalPoint = 0x0100;
	ptrDataToStore->phCalSlope = 0x01000100;

	ptrDataToStore->eCProbeType[0] = 'x';
	ptrDataToStore->eCProbeType[1] = '1';
	ptrDataToStore->eCHighCalPoint = 0x0100;
	ptrDataToStore->eCHighSolutionVal = 0x0100;

	ptrDataToStore->backPadding[0] = 0xE5;
	ptrDataToStore->backPadding[1] = 0xE6;
	ptrDataToStore->backPadding[2] = 0xE5;
	ptrDataToStore->backPadding[3] = 0xE6;
	ptrDataToStore->backPadding[4] = 0xE5;
	ptrDataToStore->backPadding[5] = 0xE6;
	ptrDataToStore->backPadding[6] = 0xE5;
	ptrDataToStore->backPadding[7] = 0xE6;
	ptrDataToStore->backPadding[8] = 0xE5;
}

void handleWriteRequest(I2CRegister RegData, char recvChars, nvmStorage_t * ptrstoredData)
{

  word sentData = 0;
  word recvData = 0;

  switch(recvChars)
  {

  	  case I2C_CMD_READ_REG1 :
  	  {
  		  uint16_t temp = 0;
		  temp = RegData.adcChan[0];
		  temp = reverseUint16(temp);
		  CI2C1_SendBlock(&temp,2,&sentData);
  	  }
	  break;

  	  case I2C_CMD_READ_REG2 :
  	  {
  		  uint16_t temp = 0;
		  temp = RegData.adcChan[1];
		  temp = reverseUint16(temp);
		  CI2C1_SendBlock(&temp,2,&sentData);
  	  }
	  break;

  	  case I2C_CMD_READ_REG3 :
	  {
		  uint16_t temp = 0;
		  temp = RegData.adcChan[2];
		  temp = reverseUint16(temp);
		  CI2C1_SendBlock(&temp,2,&sentData);
	  }
	  break;

  	  case I2C_CMD_EN_AnalogPower :
  	  {
  		  char tempResponse[4];
		  tempResponse[0] = 'A';
		  tempResponse[1] = 'P';
		  tempResponse[2] = 'E';
		  tempResponse[3] = 'N';
		  CI2C1_SendBlock(&tempResponse,4,&sentData);
		  //This is a small hack to fix a bug that showed up shifting to 3.3v and adding in all the I2C regs the processor would "reset"
		  //causing a failed APEN response and not enabling the pH analog power rail, for now rail is defaulted on and can now be switched with resposne
		  //We want to send the OK response to the old address, since our I2C driver is interrupt driven
		  //need to pause a bit and let the response go out before changing the address
		  i2cTxDone = FALSE;
			while(!i2cTxDone)
			{
				//pause until buffer is empty
			}
		  enAnalogPower = TRUE;
  	  }
	  break;

  	  case I2C_CMD_DIS_AnalogPower :
  	  {
  		  char tempResponse[4];
		  enAnalogPower = FALSE;
		  tempResponse[0] = 'A';
		  tempResponse[1] = 'P';
		  tempResponse[2] = 'F';
		  tempResponse[3] = 'F';
		  CI2C1_SendBlock(&tempResponse,4,&sentData);
  	  }
	  break;

  	  case I2C_CMD_EN_ECPower :
	  {
		  char tempResponse[4];
		  enECPower = TRUE;
		  tempResponse[0] = 'E';
		  tempResponse[1] = 'C';
		  tempResponse[2] = 'E';
		  tempResponse[3] = 'N';
		  CI2C1_SendBlock(&tempResponse,4,&sentData);
	  }
	  break;

	  case I2C_CMD_DIS_ECPower :
	  {
		  char tempResponse[4];
		  enECPower = FALSE;
		  tempResponse[0] = 'E';
		  tempResponse[1] = 'C';
		  tempResponse[2] = 'F';
		  tempResponse[3] = 'F';
		  CI2C1_SendBlock(&tempResponse,4,&sentData);
	  }
	  break;

  	  case I2C_CMD_CHG_ADDRS :
  	  {

  		//Change the I2C Address CMD
		//read the new address in

		while(!CI2C1_RecvChar(ptr_tempChar) == ERR_OK)
		{
		  //pause until we get a valid char
		}

		i2cTxDone = FALSE;

		char tempResponse[4];
		tempResponse[0] = 'O';
		tempResponse[1] = 'K';
		tempResponse[2] = '-';
		tempResponse[3] = tempChar;
		//Send response before changing address

		CI2C1_SendBlock(&tempResponse,4,&sentData);
		//update our params structure
		ptrstoredData->i2cAddress = (uint8_t)tempChar;

		NVMC_SaveParamsData(ptrstoredData,NVMC_TOTAL_DATA_SIZE);

		//We want to send the OK response to the old address, since our I2C driver is interrupt driven
		//need to pause a bit and let the response go out before changing the address
		while(!i2cTxDone)
		{
			//pause until buffer is empty
		}
		//now change the slave address of the I2C driver
		I2C0_A1 = I2C_A1_AD((uint8_t)tempChar);
  	  }
  	  break;

  	  case I2C_CMD_GET_VERSION :
	  {
		  uint16_t temp = ptrstoredData->deviceVerison;
		  temp = reverseUint16(temp);
		  CI2C1_SendBlock(&temp,2,&sentData);
	  }
	  break;

  	  case I2C_CMD_GET_INFO :
	  {
		  //Info is 32 bytes long so we need to just pass it along
		  //the best way to do this is in a for loop so we don't swamp out buffer out
		  //Lets send 4 chars at a time for now
		  while(!CI2C1_RecvChar(ptr_tempChar) == ERR_OK)
		  {
			  //pause until we get a valid char
		  }
		  uint8_t tmp = (uint8_t)tempChar;
		  char tempResponse[8];

		  for(int i = 0; i < 8; i++)
		  {
			  tempResponse[i] = ptrstoredData->deviceIDInfo[i+(tmp*8)];
		  }
		  CI2C1_SendBlock(&tempResponse,8,&sentData);
	  }
	  break;

  	  case I2C_CMD_SET_PH_CAL_TYPE :
	  {
		while(!CI2C1_RecvChar(ptr_tempChar) == ERR_OK)
		{
		  //pause until we get a valid char
		}

		i2cTxDone = FALSE;

		char tempResponse[4];
		tempResponse[0] = 'O';
		tempResponse[1] = 'K';
		tempResponse[2] = '-';
		tempResponse[3] = tempChar;
		//Send response before changing address

		CI2C1_SendBlock(&tempResponse,4,&sentData);
		//update our params structure
		ptrstoredData->phCalType = (char)tempChar;

		NVMC_SaveParamsData(ptrstoredData,NVMC_TOTAL_DATA_SIZE);
	  }
	  break;

  	  case I2C_CMD_SET_PH_CAL_LOW :
	  {
			//set cal point based on reg reading
		  char response[2];
		  response[0] = 'O';
		  response[1] = 'K';
		  CI2C1_SendBlock(&response,2,&sentData);

		ptrstoredData->phLowCalPoint = RegData.adcChan[0];
		calcPHSlope(ptrstoredData);
		//Calc slope does a NVMC write
		//NVMC_SaveParamsData(ptrstoredData,NVMC_TOTAL_DATA_SIZE);
	  }
	  break;

  	  case I2C_CMD_SET_PH_CAL_MID :
	  {
		//set cal point based on reg reading
		char response[2];
		response[0] = 'O';
		response[1] = 'K';
		CI2C1_SendBlock(&response,2,&sentData);

		ptrstoredData->phMidCalPoint = RegData.adcChan[0];
		calcPHSlope(ptrstoredData);
		//Calc slope does a NVMC write
		//NVMC_SaveParamsData(ptrstoredData,NVMC_TOTAL_DATA_SIZE);
	  }
	  break;

  	  case I2C_CMD_SET_PH_CAL_HIGH :
	  {
		//set cal point based on reg reading
		char response[2];
		response[0] = 'O';
		response[1] = 'K';
		CI2C1_SendBlock(&response,2,&sentData);

		ptrstoredData->phHighCalPoint = RegData.adcChan[0];
		ptrstoredData->phCalSlope = calcPHSlope(ptrstoredData);
		//Calc slope does a NVMC write
		//NVMC_SaveParamsData(ptrstoredData,NVMC_TOTAL_DATA_SIZE);
	  }
	  break;

  	  case I2C_CMD_GET_PH_CAL_INFO_CT :
	  {
		char temp = ptrstoredData->phCalType;
		CI2C1_SendBlock(&temp,1,&sentData);
	  }
	  break;

	  case I2C_CMD_GET_PH_CAL_INFO_LV :
	  {
		uint16_t temp = ptrstoredData->phLowCalPoint;
		temp = reverseUint16(temp);
		CI2C1_SendBlock(&temp,2,&sentData);
	  }
	  break;

	  case I2C_CMD_GET_PH_CAL_INFO_MV :
	  {
		  uint16_t temp = ptrstoredData->phMidCalPoint;
		  temp = reverseUint16(temp);
		  CI2C1_SendBlock(&temp,2,&sentData);
	  }
	  break;

	  case I2C_CMD_GET_PH_CAL_INFO_HV :
	  {
		  uint16_t temp = ptrstoredData->phHighCalPoint;
		  temp = reverseUint16(temp);
		  CI2C1_SendBlock(&temp,2,&sentData);
	  }
	  break;

	  case I2C_CMD_GET_PH_CAL_INFO_SLP :
	  {
		float pHCalSlope = ptrstoredData->phCalSlope;
		CI2C1_SendBlock(&pHCalSlope,4,&sentData);
	  }
	  break;

  	  case I2C_CMD_GET_PH :
	  {
		  float pH = calcPH(RegData, ptrstoredData);
		  CI2C1_SendBlock(&pH,4,&sentData);
	  }
	  break;

  	  case I2C_CMD_SET_EC_CAL_PROBEMX :
	  {
		char probeType[2];

		while(!CI2C1_RecvChar(ptr_tempChar) == ERR_OK)
		{
		  //pause until we get a valid char
		}

		i2cTxDone = FALSE;
		probeType[0] = tempChar;

		while(!CI2C1_RecvChar(ptr_tempChar) == ERR_OK)
		{
		  //pause until we get a valid char
		}

		i2cTxDone = FALSE;
		probeType[1] = tempChar;

		char tempResponse[5];

		tempResponse[0] = 'O';
		tempResponse[1] = 'K';
		tempResponse[2] = '-';
		tempResponse[3] = probeType[0];
		tempResponse[4] = probeType[1];
		//Send response before changing address

		CI2C1_SendBlock(&tempResponse,5,&sentData);
		//update our params structure
		ptrstoredData->eCProbeType[0] = probeType[0];
		ptrstoredData->eCProbeType[1] = probeType[1];

		NVMC_SaveParamsData(ptrstoredData,NVMC_TOTAL_DATA_SIZE);
	  }
	  break;

	  case I2C_CMD_SET_EC_CAL_HIGH :
	  {
		  //set cal point based on reg reading
		uint16_t  highSolutionValue;
		byte a;
		byte b;

		while(!CI2C1_RecvChar(ptr_tempChar) == ERR_OK)
		{
		  //pause until we get a valid char
		}

		a = tempChar;

		while(!CI2C1_RecvChar(ptr_tempChar) == ERR_OK)
		{
		  //pause until we get a valid char
		}

		b = tempChar;
		highSolutionValue = a + (b << 8);
		char response[2];
		response[0] = 'O';
		response[1] = 'K';
		//Send response before changing address
		CI2C1_SendBlock(&response,2,&sentData);

		ptrstoredData->eCHighSolutionVal = highSolutionValue;
		ptrstoredData->eCHighCalPoint = registerData.adcMax;
		NVMC_SaveParamsData(ptrstoredData,NVMC_TOTAL_DATA_SIZE);
	  }
	  break;

	  case I2C_CMD_GET_EC_CAL_INFO_PT :
	  {
		//temp = reverseUint16(temp);
		char temp[2];
		temp[0] = ptrstoredData->eCProbeType[0];
		temp[1] = ptrstoredData->eCProbeType[1];
		CI2C1_SendBlock(&temp,2,&sentData);
	  }
	  break;

	  case I2C_CMD_GET_EC_CAL_INFO_CHV :
	  {
		uint16_t temp = ptrstoredData->eCHighCalPoint;
		temp = reverseUint16(temp);
		CI2C1_SendBlock(&temp,2,&sentData);
	  }
	  break;

	  case I2C_CMD_GET_EC_CAL_INFO_SHV :
	  {
		uint16_t temp = ptrstoredData->eCHighSolutionVal;
		temp = reverseUint16(temp);
		CI2C1_SendBlock(&temp,2,&sentData);
	  }
	  break;

	  case I2C_CMD_GET_EC :
	  {
		//set cal point based on reg reading
		float eC = calcEC(RegData, ptrstoredData);
		CI2C1_SendBlock(&eC,4,&sentData);
	  }
	  break;
  }
  i2cWriteMode = FALSE;
}

float calcPHSlope(nvmStorage_t * ptrstoredData)
{
	float phSlope = 0.0;
	uint16_t calPointDiff;
	if(ptrstoredData->phCalType == 'H')
	{
		calPointDiff = ptrstoredData->phHighCalPoint - ptrstoredData->phMidCalPoint;
	}
	else
	{
		calPointDiff = ptrstoredData->phMidCalPoint - ptrstoredData->phLowCalPoint;
	}
	phSlope = qfp_fmul(vRef, calPointDiff);
	phSlope = qfp_fdiv(phSlope, 4095);
	phSlope = qfp_fmul(phSlope, 1000);
	phSlope = qfp_fdiv(phSlope, 5.25);
	phSlope = qfp_fdiv(phSlope, 3);

	ptrstoredData->phCalSlope = phSlope;
	NVMC_SaveParamsData(ptrstoredData,NVMC_TOTAL_DATA_SIZE);
	return phSlope;
}

float calcPH(I2CRegister RegData, nvmStorage_t * ptrstoredData)
{
	float pH = 0.0;
	float miliVolts = qfp_fdiv(RegData.adcChan[0], 4095);
	miliVolts = qfp_fmul(miliVolts, vRef);
	miliVolts = qfp_fmul(miliVolts, 1000);

	float temp = qfp_fmul(ptrstoredData->phMidCalPoint, vRef);
	temp = qfp_fdiv(temp, 4095);
	temp = qfp_fmul(temp, 1000);
	temp = qfp_fsub(temp, miliVolts);
	temp = qfp_fdiv(temp, 5.25);

	pH = qfp_fdiv(temp, ptrstoredData->phCalSlope);
	pH = qfp_fsub(7, pH);
	return pH;
}

float calcEC(I2CRegister RegData, nvmStorage_t * ptrstoredData)
{
	float EC = 0.0;
	//int inputRange = ptrstoredData->eCHighCalPoint-ptrstoredData->eCLowCalPoint;
	int inputRange = ptrstoredData->eCHighCalPoint-0;
	int outputRange = ptrstoredData->eCHighSolutionVal - 0;
	//using map as a reference val = map(val, 0, 1023, 0, 255);
	//long map(long x, long in_min, long in_max, long out_min, long out_max)
	//{
	//  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	//}
	EC = qfp_fdiv((RegData.adcMax)*outputRange, ((inputRange) + 0));

	return EC;
}

//function to provide swap Endianess for the Master end of the I2C bus
uint16_t reverseUint16(uint16_t v)
{
  //v = ((v >> 1) & 0x5555) | ((v & 0x5555) << 1); /* swap odd/even bits */
  //v = ((v >> 2) & 0x3333) | ((v & 0x3333) << 2); /* swap bit pairs */
  //v = ((v >> 4) & 0x0F0F) | ((v & 0x0F0F) << 4); /* swap nibbles */
  v = ((v >> 8) & 0x00FF) | ((v & 0x00FF) << 8); /* swap bytes */
  return v;
}
