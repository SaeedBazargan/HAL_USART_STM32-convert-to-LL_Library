#include "SB_USART_MRLHSL.h"
#include "stm32f4xx.h"
//*****************************************************

static SB_MRL_StatusTypeDef UART_Transmit_IT(SB_MRL_UART_HandleTypeDef *USARTsb);
static SB_MRL_StatusTypeDef UART_EndTransmit_IT(SB_MRL_UART_HandleTypeDef *USARTsb);
static SB_MRL_StatusTypeDef UART_Receive_IT(SB_MRL_UART_HandleTypeDef *USARTsb);
static SB_MRL_StatusTypeDef UART_WaitOnFlagUntilTimeout(SB_MRL_UART_HandleTypeDef *USARTsb, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout);
//*****************************************************

static SB_MRL_StatusTypeDef UART_WaitOnFlagUntilTimeout(SB_MRL_UART_HandleTypeDef *USARTsb, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout)
{
  /* Wait until flag is set */
  while((__SB_MRL_UART_GET_FLAG(USARTsb, Flag) ? SET : RESET) == Status) 
  {
    /* Check for the Timeout */
      if((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout))
      {
        /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
        CLEAR_BIT(USARTsb->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE));
        CLEAR_BIT(USARTsb->Instance->CR3, USART_CR3_EIE);
        
        USARTsb->gState  = SB_MRL_UART_STATE_READY;
        USARTsb->RxState = SB_MRL_UART_STATE_READY;
        
        /* Process Unlocked */
        __SB_MRL_UNLOCK(USARTsb);
        
        return SB_MRL_TIMEOUT;
      }
  }
  
  return SB_MRL_OK;
}


SB_MRL_StatusTypeDef SB_UART_Transmit(SB_MRL_UART_HandleTypeDef *USARTsb, uint8_t Value, uint16_t Size, uint32_t Timeout)
{
	uint32_t Tickstart = 0U;

	if(USARTsb->gState == SB_MRL_UART_STATE_READY)
	{
	  /* Process Locked */
		__SB_MRL_LOCK(USARTsb);
		
		USARTsb->ErrorCode = SB_MRL_UART_ERROR_NONE;
		USARTsb->gState    = SB_MRL_UART_STATE_BUSY_TX;
		
		/* Init tickstart for timeout managment */
		Tickstart = HAL_GetTick();
		
		USARTsb->TxXferSize  = Size;
		USARTsb->TxXferCount = Size;
		
		while(USARTsb->TxXferCount > 0U)
		{
			USARTsb->TxXferCount--;
			if(USARTsb->Init.WordLength == LL_USART_DATAWIDTH_8B)
			{
				USARTsb->Instance->DR = Value;
				if(UART_WaitOnFlagUntilTimeout(USARTsb, UART_FLAG_TXE, RESET, Tickstart, Timeout) != SB_MRL_OK)
				{
					return SB_MRL_TIMEOUT;
				}
			}
		}
		if(UART_WaitOnFlagUntilTimeout(USARTsb, UART_FLAG_TXE, RESET, Tickstart, Timeout) != SB_MRL_OK)
		{
			return SB_MRL_TIMEOUT;
		}
		USARTsb->gState = SB_MRL_UART_STATE_READY;
		
		__SB_MRL_UNLOCK(USARTsb);
		
		return SB_MRL_OK;
	}
	else
	{
		return SB_MRL_BUSY;
	}
}
//*****************************************************
SB_MRL_StatusTypeDef SB_UART_Receive(SB_MRL_UART_HandleTypeDef *USARTsb, uint8_t *ReceiveValue, uint16_t Size, uint32_t Timeout){
	uint32_t Tickstart = 0U;
	
	if(USARTsb->RxState == SB_MRL_UART_STATE_READY)
	{
	  /* Process Locked */
		__SB_MRL_LOCK(USARTsb);
		
		USARTsb->ErrorCode = SB_MRL_UART_ERROR_NONE;
		USARTsb->RxState   = SB_MRL_UART_STATE_BUSY_RX;
		
		/* Init tickstart for timeout managment */
		Tickstart = HAL_GetTick();
		
		USARTsb->RxXferSize  = Size;
		USARTsb->RxXferCount = Size;
		
		for(USARTsb->RxXferCount; (USARTsb->RxXferCount) > 0U; USARTsb->RxXferCount--)
		{
			if(USARTsb->Init.WordLength == LL_USART_DATAWIDTH_8B)
			{
				*ReceiveValue++ = (uint8_t)(READ_BIT(USARTsb->Instance->DR, USART_DR_DR));
				if(UART_WaitOnFlagUntilTimeout(USARTsb, UART_FLAG_RXNE, RESET, Tickstart, Timeout) != SB_MRL_OK)
				{
					return SB_MRL_TIMEOUT;
				}
			}
		}
		USARTsb->RxState = SB_MRL_UART_STATE_READY;
		
		__SB_MRL_UNLOCK(USARTsb);
		
		return SB_MRL_OK;
	}
	else
	{
		return SB_MRL_BUSY;
	}
}
//*****************************************************
SB_MRL_StatusTypeDef SB_UART_Transmit_IT(SB_MRL_UART_HandleTypeDef *USARTsb, uint8_t *Value, uint16_t Size)
{
	if(USARTsb->gState == SB_MRL_UART_STATE_READY)
	{
    /* Process Locked */
		__SB_MRL_LOCK(USARTsb);
		
		USARTsb->pTxBuffPtr  = Value;
		USARTsb->TxXferSize  = Size;
		USARTsb->TxXferCount = Size;
		USARTsb->ErrorCode   = SB_MRL_UART_ERROR_NONE;
		USARTsb->gState      = SB_MRL_UART_STATE_BUSY_TX;
		
		__SB_MRL_UNLOCK(USARTsb);
		
		/* Enable the UART Transmit data register empty Interrupt */
		SET_BIT(USARTsb->Instance->CR1, USART_CR1_TXEIE);
		return SB_MRL_OK;
	}
	else
	{
		return SB_MRL_BUSY;
	}
}
//*****************************************************
SB_MRL_StatusTypeDef SB_UART_Receive_IT(SB_MRL_UART_HandleTypeDef *USARTsb, uint8_t *ReceiveValueIT, uint16_t Size)
{
	/* Check that a Rx process is not already ongoing */
	if(USARTsb->RxState == SB_MRL_UART_STATE_READY)
	{
    /* Process Locked */
    __SB_MRL_LOCK(USARTsb);
		
		USARTsb->pRxBuffPtr  = ReceiveValueIT;
		USARTsb->RxXferSize  = Size;
		USARTsb->RxXferCount = Size;
		
		USARTsb->ErrorCode = SB_MRL_UART_ERROR_NONE;
		USARTsb->RxState   = SB_MRL_UART_STATE_BUSY_RX;
		
    /* Process Unlocked */
		__SB_MRL_UNLOCK(USARTsb);
		
    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(USARTsb->Instance->CR3, USART_CR3_EIE);
		
    /* Enable the UART Parity Error and Data Register not empty Interrupts */
    SET_BIT(USARTsb->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
		
		return SB_MRL_OK;
	}
	else
	{
		return SB_MRL_BUSY;
	}
}
//*****************************************************
static SB_MRL_StatusTypeDef UART_Transmit_IT(SB_MRL_UART_HandleTypeDef *USARTsb)
{
	/* Check that a Tx process is ongoing */
	if(USARTsb->gState == SB_MRL_UART_STATE_BUSY_TX)
	{
		if(USARTsb->Init.WordLength == LL_USART_DATAWIDTH_8B)
		{
			USARTsb->Instance->DR = (uint8_t)(*USARTsb->pTxBuffPtr++ & (uint8_t)0x00FF);
		}
		if(--USARTsb->TxXferCount == 0U)
		{
			/* Disable the UART Transmit Complete Interrupt */
			CLEAR_BIT(USARTsb->Instance->CR1, USART_CR1_TXEIE);
			
			/* Enable the UART Transmit Complete Interrupt */
			SET_BIT(USARTsb->Instance->CR1, USART_CR1_TCIE);
		}
		return SB_MRL_OK;
	}
	else
	{
		return SB_MRL_BUSY;
	}
}
//*****************************************************
static SB_MRL_StatusTypeDef UART_EndTransmit_IT(SB_MRL_UART_HandleTypeDef *USARTsb)
{
	/* Disable the UART Transmit Complete Interrupt */
	CLEAR_BIT(USARTsb->Instance->CR1, USART_CR1_TCIE);
	
	/* Tx process is ended, restore huart->gState to Ready */
	USARTsb->gState = SB_MRL_UART_STATE_READY;
	
	SB_MRL_UART_TxCpltCallback(USARTsb);
	
	return SB_MRL_OK;
}
//*****************************************************
static SB_MRL_StatusTypeDef UART_Receive_IT(SB_MRL_UART_HandleTypeDef *USARTsb)
{
	/* Check that a Rx process is ongoing */
	if(USARTsb->RxState == SB_MRL_UART_STATE_BUSY_RX)
	{
		if(USARTsb->Init.WordLength == LL_USART_DATAWIDTH_8B)
		{
			*USARTsb->pRxBuffPtr++ = (uint8_t)(READ_BIT(USARTsb->Instance->DR, USART_DR_DR));//(uint8_t)(USARTsb->Instance->DR & (uint8_t)0x00FF);
		}
		if(--USARTsb->RxXferCount == 0U)
		{
			/* Disable the UART Parity Error Interrupt and RXNE interrupt*/
			CLEAR_BIT(USARTsb->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));

			/* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
			CLEAR_BIT(USARTsb->Instance->CR3, USART_CR3_EIE);

			/* Rx process is completed, restore huart->RxState to Ready */
			USARTsb->RxState = SB_MRL_UART_STATE_READY;
		 
			SB_MRL_UART_RxCpltCallback(USARTsb);

			return SB_MRL_OK;
		}
		return SB_MRL_OK;
	}
	else
	{
		return SB_MRL_BUSY;
	}
}
//*****************************************************
void SB_MRL_USART_IRQHandler(SB_MRL_UART_HandleTypeDef *USARTsb)
{
   uint32_t isrflags   = READ_REG(USARTsb->Instance->SR);
   uint32_t cr1its     = READ_REG(USARTsb->Instance->CR1);
   uint32_t cr3its     = READ_REG(USARTsb->Instance->CR3);
   uint32_t errorflags = 0x00U;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if(errorflags == RESET)
  {
    /* UART in mode Receiver -------------------------------------------------*/
    if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      UART_Receive_IT(USARTsb);
      return;
    }
  }  

  /* If some errors occur */
  if((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET) || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
  {    
    /* UART frame error interrupt occurred -----------------------------------*/
    if(((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      USARTsb->ErrorCode |= SB_MRL_UART_ERROR_FE;
    }
    
    /* UART Over-Run interrupt occurred --------------------------------------*/
    if(((isrflags & USART_SR_ORE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    { 
      USARTsb->ErrorCode |= SB_MRL_UART_ERROR_ORE;
    }

    /* Call UART Error Call back function if need be --------------------------*/    
    if(USARTsb->ErrorCode != SB_MRL_UART_ERROR_NONE)
    {
      /* UART in mode Receiver -----------------------------------------------*/
      if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
      {
        UART_Receive_IT(USARTsb);
      }
    }
    return;
  } /* End if some error occurs */

  /* UART in mode Transmitter ------------------------------------------------*/
  if(((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    UART_Transmit_IT(USARTsb);
    return;
  }
  
  /* UART in mode Transmitter end --------------------------------------------*/
  if(((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    UART_EndTransmit_IT(USARTsb);
    return;
  }
}
//*****************************************************
__weak void SB_MRL_UART_TxCpltCallback(SB_MRL_UART_HandleTypeDef *USARTsb)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(USARTsb);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */ 	
}
//*****************************************************
__weak void SB_MRL_UART_RxCpltCallback(SB_MRL_UART_HandleTypeDef *USARTsb)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(USARTsb);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}
//*****************************************************
void SB_MRL_USART_Init(SB_MRL_UART_HandleTypeDef *USARTsb, USART_TypeDef *USARTx)
{
	USARTsb->gState   = SB_MRL_UART_STATE_READY;
	USARTsb->RxState  = SB_MRL_UART_STATE_READY;
	USARTsb->Instance = USARTx;
}


