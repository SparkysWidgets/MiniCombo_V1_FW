/* ###################################################################
**     Filename    : main.c
**     Project     : MiniCombo_v1_FW
**     Processor   : MKE04Z8VFK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-07-07, 23:51, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"
#include "PowerEN.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "IFsh1.h"
#include "IntFlashLdd1.h"
#include "ECPDIS.h"
#include "TU1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Util.h"
#include "ADC_Util.h"

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
  	nvmStorage_t *ptrNVMStorageData, nvmStorageData;
  	ptrNVMStorageData = &nvmStorageData;
  	//Now default our data store var
  	setDataToDefault(ptrNVMStorageData);

  	uint16_t *ptrMAGICNUMRinNVM, MAGICNUMRinNVM = NVMC_MAGIC_NUMBER_START_ADDR;
  	ptrMAGICNUMRinNVM = &MAGICNUMRinNVM;

  	uint16_t *ptrI2CADDRinNVM, i2cADDRinNVM = NVMC_I2CADDRESS_START_ADDR;
  	ptrI2CADDRinNVM = &i2cADDRinNVM;

  	uint16_t *ptrStartADDR, startADDR = NVMC_FLASH_START_ADDR;
  	ptrStartADDR = &startADDR;

  	uint8_t debugErr;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
	ptrTimerDeviceData = TU1_Init(NULL);
	//First lets load back the data checking if its erased(NULL)
	uint8_t debugGetDataResponse = NVMC_GetSavedData(ptrNVMStorageData);

	if(debugGetDataResponse == NULL)
	{
		//we need to default we received NULL from loading the data either it couldnt load or was previously erased
		//looks like we failed our magic num test so lets default our data
		setDataToDefault(ptrNVMStorageData);
		//now write it to flash (if you check our write routine we erase before we write
		debugErr = NVMC_SaveParamsData(ptrNVMStorageData,sizeof(nvmStorageData));
	}
	else if(debugGetDataResponse != NULL)
	{
		//Well the our storage area wasnt erased or NULL but does it contain valid data?
		if(shouldDefault(nvmStorageData.nvmMagicNumber))
		{
			//looks like we failed our magic num test so lets default our data
			setDataToDefault(ptrNVMStorageData);
			//now write it to flash (if you check our write routine we erase before we write
			debugErr = NVMC_SaveParamsData(ptrNVMStorageData,sizeof(nvmStorageData));
		}
		//our magic num was correct so lets do nothing!s
	}

 	I2C0_A1 = I2C_A1_AD(nvmStorageData.i2cAddress); //for now only the I2C address
	/* Write your code here */
	for(;;)
	{
		//Well use the internal wait until finished to make sure we have current readings
		AD1_Measure(TRUE);
		//Our measurements are finished nows a good time to read these into our reg buffers
		AD1_GetValue(&registerData.adcChan[0]);

		if(registerData.adcChan[2] > registerData.adcMax )
		{
			registerData.adcMax = registerData.adcChan[2];
		}

		//Should we have our analog power section turned on or off (Charge pump, op-amp etc...)
		if(enAnalogPower)
		{
			PowerEN_ClrVal(PowerEN_DeviceData);
		}
		else
		{
			PowerEN_SetVal(PowerEN_DeviceData);
		}

		if(enECPower)
		{
			ECPDIS_ClrVal(PowerEN_DeviceData);
		}
		else
		{
			ECPDIS_SetVal(PowerEN_DeviceData);
		}

		//This where we handle commands and writes from master we can use this as a delegate function to handle offloading
		//what we want to do after commands are RXd.
		if(i2cWriteMode)
		{
			while(!CI2C1_RecvChar(ptr_tempChar) == ERR_OK)
			{
				//pause until we get a valid char
			}

			handleWriteRequest(registerData, tempChar, &nvmStorageData);
		}
		
	}

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
