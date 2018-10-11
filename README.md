# HAL_USART_STM32-convert-to-LL_Library
I decided not to use HAL for USART, Because of the extra operation, there is a DMA that I do not use and slows down the processing speed. So I wrote a huddle that lowered the extra operation and is a combination of HAL and LL libraries.
