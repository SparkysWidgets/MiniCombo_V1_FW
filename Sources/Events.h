/* ###################################################################
**     Filename    : Events.h
**     Project     : MiniCombo_v1_FW
**     Processor   : MKE04Z8VFK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-07-07, 23:51, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"
#include "PowerEN.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "IFsh1.h"
#include "IntFlashLdd1.h"
#include "ECPDIS.h"
#include "TU1.h"

#ifdef __cplusplus
extern "C" {
#endif 



/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKE04Z8FK4]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void);


/*
** ===================================================================
**     Event       :  CI2C1_OnWriteReq (module Events)
**
**     Component   :  CI2C1 [InternalI2C]
**     Description :
**         This event is invoked when its own specific address is
**         matched with the calling address and the value of the R/W
**         command bit of the calling address is set to 0, ie., the
**         Master requests writing to the slave (the slave will be
**         receiving). After the return from the event call the first
**         data byte receiving starts. This event is not available for
**         the MASTER mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void CI2C1_OnWriteReq(void);

/*
** ===================================================================
**     Event       :  CI2C1_OnRxChar (module Events)
**
**     Component   :  CI2C1 [InternalI2C]
**     Description :
**         Called when a correct character is received. This event is
**         not available for the MASTER mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void CI2C1_OnRxChar(void);

/*
** ===================================================================
**     Event       :  CI2C1_OnTxChar (module Events)
**
**     Component   :  CI2C1 [InternalI2C]
**     Description :
**         Called when a correct character is sent (placed to the
**         transmitter). This event is not available for the MASTER
**         mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void CI2C1_OnTxChar(void);


/*
** ===================================================================
**     Event       :  CI2C1_OnReadReq (module Events)
**
**     Component   :  CI2C1 [InternalI2C]
**     Description :
**         This event is invoked when its own specific address is
**         matched with the calling address and the value of the R/W
**         command bit of the calling address equal to 1, ie., the
**         Master requests reading from slave (the slave will be
**         sending). After the return from the event call the first
**         data byte sending will start. This event is not available
**         for the MASTER mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void CI2C1_OnReadReq(void);

void AD1_OnEnd(void);
/*
** ===================================================================
**     Event       :  AD1_OnEnd (module Events)
**
**     Component   :  AD1 [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

/*!
**     @brief
**         Called at the end of the whole write / erase operation. if
**         the event is enabled. See SetEventMask() and GetEventMask()
**         methods.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
*/
/* ===================================================================*/

/*
** ===================================================================
**     Event       :  CI2C1_OnFreeTxBuf (module Events)
**
**     Component   :  CI2C1 [InternalI2C]
**     Description :
**         The event is called when the last byte from the output
**         buffer is sent. This event is not available for the MASTER
**         mode.
**         If OnTxChar event is also generated, this event is called
**         after OnTxChar.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void CI2C1_OnFreeTxBuf(void);

/*
** ===================================================================
**     Event       :  TU1_OnCounterRestart (module Events)
**
**     Component   :  TU1 [TimerUnit_LDD]
*/
/*!
**     @brief
**         Called if counter overflow/underflow or counter is
**         reinitialized by modulo or compare register matching.
**         OnCounterRestart event and Timer unit must be enabled. See
**         [SetEventMask] and [GetEventMask] methods. This event is
**         available only if a [Interrupt] is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. The pointer passed as
**                           the parameter of Init method.
*/
/* ===================================================================*/
void TU1_OnCounterRestart(LDD_TUserData *UserDataPtr);

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __Events_H*/
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
