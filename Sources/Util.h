#ifndef __Util_h
#define __Util_h

/**
 *
 * This provides an interface to store and retrieve data from the on-chip memory.
 */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "AD1.h"
//#include "AdcLdd1.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"

#ifdef __cplusplus
extern "C" {
#endif

//NVMC Section
//This is our section for our NVMC Defines, Globals and declarations
//Magic, if the value in NVM doesn't equal this value we should reset to default!
#define NVMC_FLASH_START_ADDR    0x200//0x1E00 /* NVRM_Config, start address of configuration data in flash */
#define NVMC_MAGIC_NUMBER		 0xCADE //Very simple magic number if its anything other than this we need to reset to default

#define NVMC_END_PADDING		 0xE5E6 //Sigma Mu padding symbols ascii extended 229/0xE5, 230/0XE6

#define NVMC_FLASH_SIZE			 0x200 /* NVRM_Config, start address of configuration data in flash */
#define NVMC_FLASH_ERASED_UINT8  0xFF
#define NVMC_FLASH_ERASED_UINT16 0xFFFF

#define NVMC_MAGIC_NUMBER_START_ADDR         	(NVMC_FLASH_START_ADDR)
#define NVMC_MAGIC_NUMBER_SIZE          		(2) /* 1 byte of data */
#define NVMC_MAGIC_NUMBER_END_ADDR				(NVMC_MAGIC_NUMBER_START_ADDR+NVMC_MAGIC_NUMBER_SIZE)

#define NVMC_I2CADDRESS_START_ADDR         		(NVMC_MAGIC_NUMBER_END_ADDR)
#define NVMC_I2CADDRESS_SIZE               		(1) /* 1 byte of data */
#define NVMC_I2CADDRESS_END_ADDR                (NVMC_I2CADDRESS_START_ADDR+NVMC_I2CADDRESS_SIZE)

#define NVMC_ALTI2CADDRESS_START_ADDR         	(NVMC_I2CADDRESS_END_ADDR)
#define NVMC_ALTI2CADDRESS_SIZE               	(1) /* 1 byte of data */
#define NVMC_ALTI2CADDRESS_END_ADDR             (NVMC_ALTI2CADDRESS_START_ADDR+NVMC_ALTI2CADDRESS_SIZE)

//32bytes for ID info IE Sparky's Widgets MinipH NXP
#define NVMC_ID_DATA_START_ADDR  				(NVMC_ALTI2CADDRESS_END_ADDR)
#define NVMC_ID_DATA_SIZE        				(32) //32 Bytes for a basic ID string etc...
#define NVMC_ID_END_ADDR         				(NVMC_ID_DATA_START_ADDR+NVMC_ID_DATA_SIZE)

//2 bytes for version info
#define NVMC_VERSION_START_ADDR  				(NVMC_ID_END_ADDR)
#define NVMC_VERSION_SIZE        				(2) //2 Bytes for Versioning
#define NVMC_VERSION_END_ADDR         			(NVMC_VERSION_START_ADDR+NVMC_VERSION_SIZE)

//10 bytes for pH Probe Calibration Data
#define NVMC_PH_CALIBRATION_DATA_START_ADDR  	(NVMC_VERSION_END_ADDR)
#define NVMC_PH_CALIBRATION_DATA_SIZE        	(11) //11 Bytes: 1 for pH Cal Type(L[4] or H[10]), 2 for ph 4, 2 for ph7, 2 for ph 10 and 4 for pH slope
#define NVMC_PH_CALIBRATION_END_ADDR         	(NVMC_PH_CALIBRATION_DATA_START_ADDR+NVMC_PH_CALIBRATION_DATA_SIZE)

//10 bytes for eC Probe Calibration Data
#define NVMC_EC_CALIBRATION_DATA_START_ADDR  	(NVMC_PH_CALIBRATION_END_ADDR)
#define NVMC_EC_CALIBRATION_DATA_SIZE        	(6) //6 Bytes: 2 bytes for probe type, 2 for eC High - 2 for High uS
#define NVMC_EC_CALIBRATION_END_ADDR         	(NVMC_EC_CALIBRATION_DATA_START_ADDR+NVMC_EC_CALIBRATION_DATA_SIZE)

#define NVMC_END_PADDING_START_ADDR         	(NVMC_EC_CALIBRATION_END_ADDR)
#define NVMC_END_PADDING_SIZE          			(9) //9 Bytes for padding brings us to 64bytes
#define NVMC_PADDING_END_ADDR					(NVMC_END_PADDING_START_ADDR+NVMC_END_PADDING_SIZE)

#define NVMC_TOTAL_DATA_SIZE			  		(NVMC_MAGIC_NUMBER_SIZE+NVMC_I2CADDRESS_SIZE+NVMC_ALTI2CADDRESS_SIZE+NVMC_ID_DATA_SIZE+NVMC_VERSION_SIZE+NVMC_PH_CALIBRATION_DATA_SIZE+NVMC_EC_CALIBRATION_DATA_SIZE+NVMC_END_PADDING_SIZE)

#define ADC_MAX_DEFAULT 0
#define ADC_MIN_DEFAULT 4095

uint16_t reverseUint16(uint16_t);

bool isErased(uint8_t *ptr, int nofBytes);
bool shouldDefault(uint16_t);

//END NVMC Section
//Struct representing our NVM
//I2C Registers
typedef struct
{
	//Config info from Mem Map(NVM_Config)
	uint16_t nvmMagicNumber;
	//i2c addresses
	uint8_t i2cAddress;
	uint8_t altI2CAddress;
	//Device info string
	char deviceIDInfo[32];
	//Device Hardware/FW Version flag
	uint16_t deviceVerison;

	//MiniPH Calibration Registers
	char phCalType;
	uint16_t phLowCalPoint;
	uint16_t phMidCalPoint;
	uint16_t phHighCalPoint;
	float phCalSlope;

	//eC Calibration Registers
	char eCProbeType[2];
	uint16_t eCHighCalPoint;
	uint16_t eCHighSolutionVal;
	//End of struct marker
	byte backPadding[9];

} nvmStorage_t;

uint8_t setDataToDefault(nvmStorage_t *);

uint8_t NVMC_SaveParamsData(nvmStorage_t *, uint16_t);
uint8_t NVMC_GetSavedData(nvmStorage_t *);

//MiniPH Output registers
typedef struct
{
	uint16_t adcChan[3];
	uint16_t adcMax;

} I2CRegister;

//OurState variables for I2C and ADC interrupt flagging
extern volatile bool i2cWriteMode;
extern volatile bool i2cTxDone;
extern volatile bool enAnalogPower;
extern volatile bool enECPower;
extern volatile char tempChar;
extern volatile char *ptr_tempChar;
extern I2CRegister registerData;
extern LDD_TDeviceData* ptrTimerDeviceData;

//I2C Interrupt helper Defines, Globals and Methods
enum
{
	I2C_CMD_READ_REG1 			= 1,
	I2C_CMD_READ_REG2 			= 2,
	I2C_CMD_READ_REG3   		= 3,
	I2C_CMD_EN_AnalogPower 		= 4,
	I2C_CMD_DIS_AnalogPower		= 5,
	I2C_CMD_CHG_ADDRS 			= 6,
	I2C_CMD_USE_MAIN_ADDRS		= 7,
	I2C_CMD_GET_VERSION			= 8,
	I2C_CMD_GET_INFO    		= 9,
	I2C_CMD_SET_PH_CAL_TYPE		= 10,
	I2C_CMD_SET_PH_CAL_LOW    	= 11,
	I2C_CMD_SET_PH_CAL_MID    	= 12,
	I2C_CMD_SET_PH_CAL_HIGH    	= 13,
	I2C_CMD_GET_PH_CAL_INFO_CT	= 14,
	I2C_CMD_GET_PH_CAL_INFO_LV	= 15,
	I2C_CMD_GET_PH_CAL_INFO_MV	= 16,
	I2C_CMD_GET_PH_CAL_INFO_HV	= 17,
	I2C_CMD_GET_PH_CAL_INFO_SLP	= 18,
	I2C_CMD_GET_PH				= 19,
	I2C_CMD_EN_ECPower 			= 20,
	I2C_CMD_DIS_ECPower			= 21,
	I2C_CMD_SET_EC_CAL_PROBEMX 	= 22,
	I2C_CMD_SET_EC_CAL_LOW    	= 23,
	I2C_CMD_SET_EC_CAL_HIGH    	= 24,
	I2C_CMD_GET_EC_CAL_INFO_PT	= 25,
	I2C_CMD_GET_EC_CAL_INFO_CLV	= 26,
	I2C_CMD_GET_EC_CAL_INFO_SLV	= 28,
	I2C_CMD_GET_EC_CAL_INFO_CHV	= 29,
	I2C_CMD_GET_EC_CAL_INFO_SHV	= 30,
	I2C_CMD_GET_EC_CAL_INFO_SLP	= 31,
	I2C_CMD_GET_EC				= 32,
	I2C_CMD_CHG_ALT_ADDRS		= 33,
	I2C_CMD_USE_ALT_ADDRS		= 34,
	//Not sure if we will ever implement this but we got it just in case
	CMD_NVM_WRITE     			= 35

};
//Basically our Slave responses to Master calls
//Keep these very short just take data from REG and return math is handles elsewhere!!!!!!
void handleWriteRequest(I2CRegister RegData, char, nvmStorage_t *);

float calcPHSlope(nvmStorage_t *);
float calcPH(I2CRegister, nvmStorage_t *);

float calcEC(I2CRegister, nvmStorage_t *);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* ifndef __CI2C1 */

