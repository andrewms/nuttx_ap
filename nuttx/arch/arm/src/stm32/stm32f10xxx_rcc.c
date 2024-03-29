/****************************************************************************
 * arch/arm/src/stm32/stm32f10xxx_rcc.c
 *
 *   Copyright (C) 2009, 2011-2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

/****************************************************************************
 * Definitions
 ****************************************************************************/

/* Allow up to 100 milliseconds for the high speed clock to become ready.
 * that is a very long delay, but if the clock does not become ready we are
 * hosed anyway.  Normally this is very fast, but I have seen at least one
 * board that required this long, long timeout for the HSE to be ready.
 */

#define HSERDY_TIMEOUT (100 * CONFIG_BOARD_LOOPSPERMSEC)

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rcc_reset
 *
 * Description:
 *   Put all RCC registers in reset state
 *
 ****************************************************************************/

static inline void rcc_reset(void)
{
  uint32_t regval;

  putreg32(0, STM32_RCC_APB2RSTR);          /* Disable APB2 Peripheral Reset */
  putreg32(0, STM32_RCC_APB1RSTR);          /* Disable APB1 Peripheral Reset */
  putreg32(RCC_AHBENR_FLITFEN|RCC_AHBENR_SRAMEN, STM32_RCC_AHBENR); /* FLITF and SRAM Clock ON */
  putreg32(0, STM32_RCC_APB2ENR);           /* Disable APB2 Peripheral Clock */
  putreg32(0, STM32_RCC_APB1ENR);           /* Disable APB1 Peripheral Clock */

  regval  = getreg32(STM32_RCC_CR);         /* Set the HSION bit */
  regval |= RCC_CR_HSION;
  putreg32(regval, STM32_RCC_CR);

  regval  = getreg32(STM32_RCC_CFGR);       /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
  regval &= ~(RCC_CFGR_SW_MASK|RCC_CFGR_HPRE_MASK|RCC_CFGR_PPRE1_MASK|RCC_CFGR_PPRE2_MASK|RCC_CFGR_ADCPRE_MASK|RCC_CFGR_MCO_MASK);
  putreg32(regval, STM32_RCC_CFGR);

  regval  = getreg32(STM32_RCC_CR);         /* Reset HSEON, CSSON and PLLON bits */
  regval &= ~(RCC_CR_HSEON|RCC_CR_CSSON|RCC_CR_PLLON);
  putreg32(regval, STM32_RCC_CR);

  regval  = getreg32(STM32_RCC_CR);         /* Reset HSEBYP bit */
  regval &= ~RCC_CR_HSEBYP;
  putreg32(regval, STM32_RCC_CR);
 
  regval  = getreg32(STM32_RCC_CFGR);       /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE bits */
  regval &= ~(RCC_CFGR_PLLSRC|RCC_CFGR_PLLXTPRE|RCC_CFGR_PLLMUL_MASK|RCC_CFGR_USBPRE);
  putreg32(regval, STM32_RCC_CFGR);

  putreg32(0, STM32_RCC_CIR);               /* Disable all interrupts */
}

/****************************************************************************
 * Name: rcc_enableahb
 *
 * Description:
 *   Enable selected AHB peripherals
 *
 ****************************************************************************/

static inline void rcc_enableahb(void)
{
  uint32_t regval;

  /* Always enable FLITF clock and SRAM clock */

  regval = RCC_AHBENR_FLITFEN|RCC_AHBENR_SRAMEN;

#ifdef CONFIG_STM32_DMA1
  /* DMA 1 clock enable */

  regval |= RCC_AHBENR_DMA1EN;
#endif

#ifdef CONFIG_STM32_DMA2
  /* DMA 2 clock enable */

  regval |= RCC_AHBENR_DMA2EN;
#endif

#ifdef CONFIG_STM32_CRC
  /* CRC clock enable */

  regval |= RCC_AHBENR_CRCEN;
#endif

#ifdef CONFIG_STM32_FSMC
  /* FSMC clock enable */

  regval |=  RCC_AHBENR_FSMCEN;
#endif

#ifdef CONFIG_STM32_SDIO
  /* SDIO clock enable */

  regval |=  RCC_AHBENR_SDIOEN;
#endif

  putreg32(regval, STM32_RCC_AHBENR);   /* Enable peripherals */
}

/****************************************************************************
 * Name: rcc_enableapb1
 *
 * Description:
 *   Enable selected APB1 peripherals
 *
 ****************************************************************************/

static inline void rcc_enableapb1(void)
{
  uint32_t regval;

#ifdef CONFIG_STM32_USB
  /* USB clock divider. This bit must be valid before enabling the USB
   * clock in the RCC_APB1ENR register. This bit can�t be reset if the USB
   * clock is enabled.
   */

  regval  = getreg32(STM32_RCC_CFGR);
  regval &= ~RCC_CFGR_USBPRE;
  regval |= STM32_CFGR_USBPRE;
  putreg32(regval, STM32_RCC_CFGR);
#endif

  /* Set the appropriate bits in the APB1ENR register to enabled the
   * selected APB1 peripherals.
   */

  regval  = getreg32(STM32_RCC_APB1ENR);
#ifdef CONFIG_STM32_TIM2
  /* Timer 2 clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB1ENR_TIM2EN;
#endif
#endif

#ifdef CONFIG_STM32_TIM3
  /* Timer 3 clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB1ENR_TIM3EN;
#endif
#endif

#ifdef CONFIG_STM32_TIM4
  /* Timer 4 clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB1ENR_TIM4EN;
#endif
#endif

#ifdef CONFIG_STM32_TIM5
  /* Timer 5 clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB1ENR_TIM5EN;
#endif
#endif

#ifdef CONFIG_STM32_TIM6
  /* Timer 6 clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB1ENR_TIM6EN;
#endif
#endif

#ifdef CONFIG_STM32_TIM7
  /* Timer 7 clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB1ENR_TIM7EN;
#endif
#endif

#ifdef CONFIG_STM32_WWDG
  /* Window Watchdog clock enable */

  regval |= RCC_APB1ENR_WWDGEN;
#endif

#ifdef CONFIG_STM32_SPI2
  /* SPI 2 clock enable */

  regval |= RCC_APB1ENR_SPI2EN;
#endif
  
#ifdef CONFIG_STM32_SPI3
  /* SPI 3 clock enable */

  regval |= RCC_APB1ENR_SPI3EN;
#endif

#ifdef CONFIG_STM32_USART2
  /* USART 2 clock enable */

  regval |= RCC_APB1ENR_USART2EN;
#endif

#ifdef CONFIG_STM32_USART3
  /* USART 3 clock enable */

  regval |= RCC_APB1ENR_USART3EN;
#endif

#ifdef CONFIG_STM32_UART4
  /* UART 4 clock enable */

  regval |= RCC_APB1ENR_UART4EN;
#endif

#ifdef CONFIG_STM32_UART5
  /* UART 5 clock enable */

  regval |= RCC_APB1ENR_UART5EN;
#endif

#ifdef CONFIG_STM32_I2C1
  /* I2C 1 clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB1ENR_I2C1EN;
#endif
#endif

#ifdef CONFIG_STM32_I2C2
  /* I2C 2 clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB1ENR_I2C2EN;
#endif
#endif

#ifdef CONFIG_STM32_USB
  /* USB clock enable */

  regval |= RCC_APB1ENR_USBEN;
#endif

#ifdef CONFIG_STM32_CAN1
  /* CAN1 clock enable */

  regval |= RCC_APB1ENR_CAN1EN;
#endif

#ifdef CONFIG_STM32_CAN2
  /* CAN2 clock enable */

  regval |= RCC_APB1ENR_CAN2EN;
#endif

#ifdef CONFIG_STM32_BKP
  /* Backup interface clock enable */

  regval |= RCC_APB1ENR_BKPEN;
#endif

#ifdef CONFIG_STM32_PWR
  /*  Power interface clock enable */

  regval |= RCC_APB1ENR_PWREN;
#endif

#if defined(CONFIG_STM32_DAC1) || defined(CONFIG_STM32_DAC2)
  /* DAC interface clock enable */

  regval |= RCC_APB1ENR_DACEN;
#endif
  putreg32(regval, STM32_RCC_APB1ENR);
}

/****************************************************************************
 * Name: rcc_enableapb2
 *
 * Description:
 *   Enable selected APB2 peripherals
 *
 ****************************************************************************/

static inline void rcc_enableapb2(void)
{
  uint32_t regval;

  /* Set the appropriate bits in the APB2ENR register to enabled the
   * selected APB2 peripherals.
   */

  /* Enable GPIOA, GPIOB, ... and AFIO clocks */

  regval = getreg32(STM32_RCC_APB2ENR);
  regval |= (RCC_APB2ENR_AFIOEN
#if STM32_NGPIO > 0
             |RCC_APB2ENR_IOPAEN
#endif
#if STM32_NGPIO > 16
             |RCC_APB2ENR_IOPBEN
#endif
#if STM32_NGPIO > 32
             |RCC_APB2ENR_IOPCEN
#endif
#if STM32_NGPIO > 48
             |RCC_APB2ENR_IOPDEN
#endif
#if STM32_NGPIO > 64
             |RCC_APB2ENR_IOPEEN
#endif
#if STM32_NGPIO > 80
             |RCC_APB2ENR_IOPFEN
#endif
#if STM32_NGPIO > 96
             |RCC_APB2ENR_IOPGEN
#endif
             );

#ifdef CONFIG_STM32_ADC1
  /* ADC 1 interface clock enable */

  regval |= RCC_APB2ENR_ADC1EN;
#endif

#ifdef CONFIG_STM32_ADC2
  /* ADC 2 interface clock enable */

  regval |= RCC_APB2ENR_ADC2EN;
#endif

#ifdef CONFIG_STM32_TIM1
  /* TIM1 Timer clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB2ENR_TIM1EN;
#endif
#endif

#ifdef CONFIG_STM32_SPI1
  /* SPI 1 clock enable */

  regval |= RCC_APB2ENR_SPI1EN;
#endif

#ifdef CONFIG_STM32_TIM8
  /* TIM8 Timer clock enable */
#ifdef CONFIG_STM32_FORCEPOWER
  regval |= RCC_APB2ENR_TIM8EN;
#endif
#endif

#ifdef CONFIG_STM32_USART1
  /* USART1 clock enable */

  regval |= RCC_APB2ENR_USART1EN;
#endif

#ifdef CONFIG_STM32_ADC3
  /*ADC3 interface clock enable */

  regval |= RCC_APB2ENR_ADC3EN;
#endif
  putreg32(regval, STM32_RCC_APB2ENR);
}

/****************************************************************************
 * Name: stm32_stdclockconfig
 *
 * Description:
 *   Called to change to new clock based on settings in board.h
 * 
 *   NOTE:  This logic would need to be extended if you need to select low-
 *   power clocking modes!
 ****************************************************************************/

#ifndef CONFIG_ARCH_BOARD_STM32_CUSTOM_CLOCKCONFIG
static inline void stm32_stdclockconfig(void)
{
  uint32_t regval;
  volatile int32_t timeout;

  /* Enable External High-Speed Clock (HSE) */
 
  regval  = getreg32(STM32_RCC_CR);
  regval &= ~RCC_CR_HSEBYP;         /* Disable HSE clock bypass */
  regval |= RCC_CR_HSEON;           /* Enable HSE */
  putreg32(regval, STM32_RCC_CR);
   
  /* Wait until the HSE is ready (or until a timeout elapsed) */

  for (timeout = HSERDY_TIMEOUT; timeout > 0; timeout--)
  {
    /* Check if the HSERDY flag is the set in the CR */

    if ((getreg32(STM32_RCC_CR) & RCC_CR_HSERDY) != 0)
      {
        /* If so, then break-out with timeout > 0 */

        break;
      }
  }

  /* Check for a timeout.  If this timeout occurs, then we are hosed.  We
   * have no real back-up plan, although the following logic makes it look
   * as though we do.
   */

  if (timeout > 0)
    {
      /* Enable FLASH prefetch buffer and 2 wait states */

      regval  = getreg32(STM32_FLASH_ACR);
      regval &= ~FLASH_ACR_LATENCY_MASK;
      regval |= (FLASH_ACR_LATENCY_2|FLASH_ACR_PRTFBE);
      putreg32(regval, STM32_FLASH_ACR);

      /* Set the HCLK source/divider */
 
      regval = getreg32(STM32_RCC_CFGR);
      regval &= ~RCC_CFGR_HPRE_MASK;
      regval |= STM32_RCC_CFGR_HPRE;
      putreg32(regval, STM32_RCC_CFGR);

      /* Set the PCLK2 divider */

      regval = getreg32(STM32_RCC_CFGR);
      regval &= ~RCC_CFGR_PPRE2_MASK;
      regval |= STM32_RCC_CFGR_PPRE2;
      putreg32(regval, STM32_RCC_CFGR);
  
      /* Set the PCLK1 divider */

      regval = getreg32(STM32_RCC_CFGR);
      regval &= ~RCC_CFGR_PPRE1_MASK;
      regval |= STM32_RCC_CFGR_PPRE1;
      putreg32(regval, STM32_RCC_CFGR);
 
      /* Set the PLL divider and multipler */

      regval = getreg32(STM32_RCC_CFGR);
      regval &= ~(RCC_CFGR_PLLSRC|RCC_CFGR_PLLXTPRE|RCC_CFGR_PLLMUL_MASK);
      regval |= (STM32_CFGR_PLLSRC|STM32_CFGR_PLLXTPRE|STM32_CFGR_PLLMUL);
      putreg32(regval, STM32_RCC_CFGR);
 
      /* Enable the PLL */

      regval = getreg32(STM32_RCC_CR);
      regval |= RCC_CR_PLLON;
      putreg32(regval, STM32_RCC_CR);
 
      /* Wait until the PLL is ready */
  
      while ((getreg32(STM32_RCC_CR) & RCC_CR_PLLRDY) == 0);
 
      /* Select the system clock source (probably the PLL) */
 
      regval  = getreg32(STM32_RCC_CFGR);
      regval &= ~RCC_CFGR_SW_MASK;
      regval |= STM32_SYSCLK_SW;
      putreg32(regval, STM32_RCC_CFGR);

      /* Wait until the selected source is used as the system clock source */
  
      while ((getreg32(STM32_RCC_CFGR) & RCC_CFGR_SWS_MASK) != STM32_SYSCLK_SWS);
  }
}
#endif

/****************************************************************************
 * Name: rcc_enableperiphals
 ****************************************************************************/

static inline void rcc_enableperipherals(void)
{
  rcc_enableahb();
  rcc_enableapb2();
  rcc_enableapb1();
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
