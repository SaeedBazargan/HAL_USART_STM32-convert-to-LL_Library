#ifndef __SB_USART_MRLHSL_H
#define __SB_USART_MRLHSL_H
//*****************************************************
#ifdef __cplusplus
 extern "C" {
#endif
//*****************************************************
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */
//*****************************************************
#include "stdint.h"
#include "stm32f4xx.h"
//*****************************************************
#define  USE_RTOS                     0U 
#define  SB_MRL_UART_ERROR_NONE       0x00000000U     /*!< No error      */
#define  SB_MRL_UART_ERROR_FE         0x00000004U     /*!< Frame error   */
#define  SB_MRL_UART_ERROR_ORE        0x00000008U     /*!< Overrun error */
#define  SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define USART_CR1_TXEIE_Pos           (7U)                                     
#define USART_CR1_TXEIE_Msk           (0x1U << USART_CR1_TXEIE_Pos)            /*!< 0x00000080 */
#define USART_CR1_TXEIE               USART_CR1_TXEIE_Msk                      /*!<PE Interrupt Enable*/

#define USART_CR1_M_Pos               (12U)                                    
#define USART_CR1_M_Msk               (0x1U << USART_CR1_M_Pos)                /*!< 0x00001000 */
#define USART_CR1_M                   USART_CR1_M_Msk                          /*!<Word length */
#define UART_WORDLENGTH_9B            ((uint32_t)USART_CR1_M)

#define USART_SR_TXE_Pos              (7U)                                     
#define USART_SR_TXE_Msk              (0x1U << USART_SR_TXE_Pos)               /*!< 0x00000080 */
#define USART_SR_TXE                  USART_SR_TXE_Msk                         /*!<Transmit Data Register Empty */
#define UART_FLAG_TXE                 ((uint32_t)USART_SR_TXE)

#define USART_SR_RXNE_Pos             (5U)                                     
#define USART_SR_RXNE_Msk             (0x1U << USART_SR_RXNE_Pos)              /*!< 0x00000020 */
#define USART_SR_RXNE                 USART_SR_RXNE_Msk                        /*!<Read Data Register Not Empty */
#define UART_FLAG_RXNE                ((uint32_t)USART_SR_RXNE)

#define UART_PARITY_NONE                    0x00000000U
//*****************************************************
typedef enum 
{
  SB_MRL_UNLOCKED = 0x00U,
  SB_MRL_LOCKED   = 0x01U  
} SB_MRL_LockTypeDef;
//*****************************************************
typedef enum
{
	SB_MRL_OK				= 0x00U,
	SB_MRL_ERROR		= 0X01U,
	SB_MRL_BUSY			= 0X02U,
	SB_MRL_TIMEOUT	= 0X03U			
}SB_MRL_StatusTypeDef;
//*****************************************************
typedef enum
{
  SB_MRL_UART_STATE_RESET             = 0x00U,    /*!< Peripheral is not yet Initialized 
                                                   Value is allowed for gState and RxState */
  SB_MRL_UART_STATE_READY             = 0x20U,    /*!< Peripheral Initialized and ready for use
                                                   Value is allowed for gState and RxState */
  SB_MRL_UART_STATE_BUSY              = 0x24U,    /*!< an internal process is ongoing
                                                   Value is allowed for gState only */
  SB_MRL_UART_STATE_BUSY_TX           = 0x21U,    /*!< Data Transmission process is ongoing 
                                                   Value is allowed for gState only */
  SB_MRL_UART_STATE_BUSY_RX           = 0x22U,    /*!< Data Reception process is ongoing
                                                   Value is allowed for RxState only */
  SB_MRL_UART_STATE_BUSY_TX_RX        = 0x23U,    /*!< Data Transmission and Reception process is ongoing 
                                                   Not to be used for neither gState nor RxState.
                                                   Value is result of combination (Or) between gState and RxState values */
  SB_MRL_UART_STATE_TIMEOUT           = 0xA0U,    /*!< Timeout state    
                                                   Value is allowed for gState only */
  SB_MRL_UART_STATE_ERROR             = 0xE0U     /*!< Error   
                                                   Value is allowed for gState only */
}SB_MRL_UART_StateTypeDef;
//*****************************************************
typedef struct
{
  uint32_t BaudRate;                  /*!< This member configures the UART communication baud rate.
                                           The baud rate is computed using the following formula:
                                           - IntegerDivider = ((PCLKx) / (8 * (OVR8+1) * (huart->Init.BaudRate)))
                                           - FractionalDivider = ((IntegerDivider - ((uint32_t) IntegerDivider)) * 8 * (OVR8+1)) + 0.5 
                                           Where OVR8 is the "oversampling by 8 mode" configuration bit in the CR1 register. */

  uint32_t WordLength;                /*!< Specifies the number of data bits transmitted or received in a frame.
                                           This parameter can be a value of @ref UART_Word_Length */

  uint32_t StopBits;                  /*!< Specifies the number of stop bits transmitted.
                                           This parameter can be a value of @ref UART_Stop_Bits */

  uint32_t Parity;                    /*!< Specifies the parity mode.
                                           This parameter can be a value of @ref UART_Parity
                                           @note When parity is enabled, the computed parity is inserted
                                                 at the MSB position of the transmitted data (9th bit when
                                                 the word length is set to 9 data bits; 8th bit when the
                                                 word length is set to 8 data bits). */
 
  uint32_t Mode;                      /*!< Specifies whether the Receive or Transmit mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Mode */

  uint32_t HwFlowCtl;                 /*!< Specifies whether the hardware flow control mode is enabled
                                           or disabled.
                                           This parameter can be a value of @ref UART_Hardware_Flow_Control */
  
  uint32_t OverSampling;              /*!< Specifies whether the Over sampling 8 is enabled or disabled, to achieve higher speed (up to fPCLK/8).
                                           This parameter can be a value of @ref UART_Over_Sampling */ 
}SB_MRL_UART_InitTypeDef;
//*****************************************************
typedef struct
{
  USART_TypeDef                  *Instance;        /*!< UART registers base address        */
  
  SB_MRL_UART_InitTypeDef         Init;             /*!< UART communication parameters      */
  
  uint8_t                         *pTxBuffPtr;      /*!< Pointer to UART Tx transfer Buffer */
  
  uint16_t                        TxXferSize;       /*!< UART Tx Transfer size              */
  
  __IO uint16_t                   TxXferCount;      /*!< UART Tx Transfer Counter           */
  
  uint8_t                         *pRxBuffPtr;      /*!< Pointer to UART Rx transfer Buffer */
  
  uint16_t                         RxXferSize;       /*!< UART Rx Transfer size              */
  
  __IO uint16_t                    RxXferCount;      /*!< UART Rx Transfer Counter           */  
  
  SB_MRL_LockTypeDef               Lock;             /*!< Locking object                     */

  __IO SB_MRL_UART_StateTypeDef    gState;           /*!< UART state information related to global Handle management 
                                                       and also related to Tx operations.
                                                       This parameter can be a value of @ref HAL_UART_StateTypeDef */
  
  __IO SB_MRL_UART_StateTypeDef    RxState;          /*!< UART state information related to Rx operations.
                                                       This parameter can be a value of @ref HAL_UART_StateTypeDef */
  
  __IO uint32_t             	     ErrorCode;        /*!< UART Error code                    */

}SB_MRL_UART_HandleTypeDef;
//*****************************************************
#if (USE_RTOS == 1U)
  /* Reserved for future use */
  #error "USE_RTOS should be 0 in the current HAL release"
#else
  #define __SB_MRL_LOCK(__HANDLE__)                                           \
                                do{                                        		\
                                    if((__HANDLE__)->Lock == SB_MRL_LOCKED)   \
                                    {                                      		\
                                       return SB_MRL_BUSY;                    \
                                    }                                      		\
                                    else                                   		\
                                    {                                      		\
                                       (__HANDLE__)->Lock = SB_MRL_LOCKED;    \
                                    }                                      		\
                                  }while (0U)

  #define __SB_MRL_UNLOCK(__HANDLE__)                                         \
                                  do{                                       	\
                                      (__HANDLE__)->Lock = SB_MRL_UNLOCKED;   \
                                    }while (0U)
#endif /* USE_RTOS */
//*****************************************************
#define __SB_MRL_UART_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))
//*****************************************************
SB_MRL_StatusTypeDef SB_UART_Transmit(SB_MRL_UART_HandleTypeDef *USARTsb, uint8_t Value, uint16_t Size, uint32_t Timeout);
SB_MRL_StatusTypeDef SB_UART_Receive(SB_MRL_UART_HandleTypeDef *USARTsb, uint8_t *ReceiveValue, uint16_t Size, uint32_t Timeout);
SB_MRL_StatusTypeDef SB_UART_Transmit_IT(SB_MRL_UART_HandleTypeDef *USARTsb, uint8_t *Value, uint16_t Size);
SB_MRL_StatusTypeDef SB_UART_Receive_IT(SB_MRL_UART_HandleTypeDef *USARTsb, uint8_t *ReceiveValueIT, uint16_t Size);

void SB_MRL_USART_Init(SB_MRL_UART_HandleTypeDef *USARTsb, USART_TypeDef *USARTx);
void SB_MRL_USART_IRQHandler(SB_MRL_UART_HandleTypeDef *USARTsb);
void SB_MRL_UART_TxCpltCallback(SB_MRL_UART_HandleTypeDef *USARTsb);
void SB_MRL_UART_RxCpltCallback(SB_MRL_UART_HandleTypeDef *USARTsb);
//*****************************************************
#ifdef __cplusplus
}
#endif
//*****************************************************
#endif /*__SB_USART_MRLHSL_H*/

