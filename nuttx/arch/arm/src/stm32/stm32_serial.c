/****************************************************************************
 * arch/arm/src/stm32/stm32_serial.c
 *
 *   Copyright (C) 2009-2012 Gregory Nutt. All rights reserved.
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

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/serial.h>

#include <arch/serial.h>
#include <arch/board/board.h>

#include "chip.h"
#include "stm32_uart.h"
#include "up_arch.h"
#include "up_internal.h"
#include "os_internal.h"

/****************************************************************************
 * Definitions
 ****************************************************************************/
/* Some sanity checks *******************************************************/
/* Is there a USART enabled? */

#if defined(CONFIG_STM32_USART1) || defined(CONFIG_STM32_USART2) || \
    defined(CONFIG_STM32_USART3) || defined(CONFIG_STM32_UART4)  || \
    defined(CONFIG_STM32_UART5)  || defined(CONFIG_STM32_USART6)
#  define HAVE_UART 1
#endif

/* Is there a serial console? */

#if defined(CONFIG_USART1_SERIAL_CONSOLE) && defined(CONFIG_STM32_USART1)
#  define CONSOLE_UART 1
#elif defined(CONFIG_USART2_SERIAL_CONSOLE) && defined(CONFIG_STM32_USART2)
#  define CONSOLE_UART 2
#elif defined(CONFIG_USART3_SERIAL_CONSOLE) && defined(CONFIG_STM32_USART3)
#  define CONSOLE_UART 3
#elif defined(CONFIG_USART4_SERIAL_CONSOLE) && defined(CONFIG_STM32_UART4)
#  define CONSOLE_UART 4
#elif defined(CONFIG_USART5_SERIAL_CONSOLE) && defined(CONFIG_STM32_UART5)
#  define CONSOLE_UART 5
#elif defined(CONFIG_USART6_SERIAL_CONSOLE) && defined(CONFIG_STM32_USART6)
#  define CONSOLE_UART 6
#else
#  define CONSOLE_UART 0
#endif

/* If we are not using the serial driver for the console, then we still must
 * provide some minimal implementation of up_putc.
 */

#ifdef USE_SERIALDRIVER
#ifdef HAVE_UART

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct up_dev_s
{
  struct uart_dev_s dev;       /* Generic UART device */
  uint16_t          ie;        /* Saved interrupt mask bits value */
  uint16_t          sr;        /* Saved status bits */

  const uint8_t     irq;       /* IRQ associated with this USART */
  const uint8_t     parity;    /* 0=none, 1=odd, 2=even */
  const uint8_t     bits;      /* Number of bits (7 or 8) */
  const bool        stopbits2; /* True: Configure with 2 stop bits instead of 1 */
  const uint32_t    baud;      /* Configured baud */
  const uint32_t    apbclock;  /* PCLK 1 or 2 frequency */
  const uint32_t    usartbase; /* Base address of USART registers */
  const uint32_t    tx_gpio;   /* U[S]ART TX GPIO pin configuration */
  const uint32_t    rx_gpio;   /* U[S]ART RX GPIO pin configuration */
  const uint32_t    rts_gpio;  /* U[S]ART RTS GPIO pin configuration */
  const uint32_t    cts_gpio;  /* U[S]ART CTS GPIO pin configuration */

  int (* const vector)(int irq, void *context); /* Interrupt handler */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int  up_setup(struct uart_dev_s *dev);
static void up_shutdown(struct uart_dev_s *dev);
static int  up_attach(struct uart_dev_s *dev);
static void up_detach(struct uart_dev_s *dev);
static int  up_interrupt_common(struct up_dev_s *dev);
static int  up_ioctl(struct file *filep, int cmd, unsigned long arg);
static int  up_receive(struct uart_dev_s *dev, uint32_t *status);
static void up_rxint(struct uart_dev_s *dev, bool enable);
static bool up_rxavailable(struct uart_dev_s *dev);
static void up_send(struct uart_dev_s *dev, int ch);
static void up_txint(struct uart_dev_s *dev, bool enable);
static bool up_txready(struct uart_dev_s *dev);

#ifdef CONFIG_STM32_USART1
static int up_interrupt_usart1(int irq, void *context);
#endif
#ifdef CONFIG_STM32_USART2
static int up_interrupt_usart2(int irq, void *context);
#endif
#ifdef CONFIG_STM32_USART3
static int up_interrupt_usart3(int irq, void *context);
#endif
#ifdef CONFIG_STM32_UART4
static int up_interrupt_uart4(int irq, void *context);
#endif
#ifdef CONFIG_STM32_UART5
static int up_interrupt_uart5(int irq, void *context);
#endif
#ifdef CONFIG_STM32_USART6
static int up_interrupt_usart6(int irq, void *context);
#endif

/****************************************************************************
 * Private Variables
 ****************************************************************************/

struct uart_ops_s g_uart_ops =
{
  .setup          = up_setup,
  .shutdown       = up_shutdown,
  .attach         = up_attach,
  .detach         = up_detach,
  .ioctl          = up_ioctl,
  .receive        = up_receive,
  .rxint          = up_rxint,
  .rxavailable    = up_rxavailable,
  .send           = up_send,
  .txint          = up_txint,
  .txready        = up_txready,
  .txempty        = up_txready,
};

/* I/O buffers */

#ifdef CONFIG_STM32_USART1
static char g_usart1rxbuffer[CONFIG_USART1_RXBUFSIZE];
static char g_usart1txbuffer[CONFIG_USART1_TXBUFSIZE];
#endif
#ifdef CONFIG_STM32_USART2
static char g_usart2rxbuffer[CONFIG_USART2_RXBUFSIZE];
static char g_usart2txbuffer[CONFIG_USART2_TXBUFSIZE];
#endif
#ifdef CONFIG_STM32_USART3
static char g_usart3rxbuffer[CONFIG_USART3_RXBUFSIZE];
static char g_usart3txbuffer[CONFIG_USART3_TXBUFSIZE];
#endif
#ifdef CONFIG_STM32_UART4
static char g_uart4rxbuffer[CONFIG_USART4_RXBUFSIZE];
static char g_uart4txbuffer[CONFIG_USART4_TXBUFSIZE];
#endif
#ifdef CONFIG_STM32_UART5
static char g_uart5rxbuffer[CONFIG_USART5_RXBUFSIZE];
static char g_uart5txbuffer[CONFIG_USART5_TXBUFSIZE];
#endif
#ifdef CONFIG_STM32_USART6
static char g_usart6rxbuffer[CONFIG_USART6_RXBUFSIZE];
static char g_usart6txbuffer[CONFIG_USART6_TXBUFSIZE];
#endif

/* This describes the state of the STM32 USART1 ports. */

#ifdef CONFIG_STM32_USART1
static struct up_dev_s g_usart1priv =
{
  .dev =
    {
#if CONSOLE_UART == 1
      .isconsole = true,
#endif
      .recv      =
      {
        .size    = CONFIG_USART1_RXBUFSIZE,
        .buffer  = g_usart1rxbuffer,
      },
      .xmit      =
      {
        .size    = CONFIG_USART1_TXBUFSIZE,
        .buffer  = g_usart1txbuffer,
      },
      .ops       = &g_uart_ops,
      .priv      = &g_usart1priv,
    },

  .irq           = STM32_IRQ_USART1,
  .parity        = CONFIG_USART1_PARITY,
  .bits          = CONFIG_USART1_BITS,
  .stopbits2     = CONFIG_USART1_2STOP,
  .baud          = CONFIG_USART1_BAUD,
  .apbclock      = STM32_PCLK2_FREQUENCY,
  .usartbase     = STM32_USART1_BASE,
  .tx_gpio       = GPIO_USART1_TX,
  .rx_gpio       = GPIO_USART1_RX,
#ifdef GPIO_USART1_CTS
  .cts_gpio      = GPIO_USART1_CTS,
#endif
#ifdef GPIO_USART1_RTS
  .rts_gpio      = GPIO_USART1_RTS,
#endif
  .vector        = up_interrupt_usart1,
};
#endif

/* This describes the state of the STM32 USART2 port. */

#ifdef CONFIG_STM32_USART2
static struct up_dev_s g_usart2priv =
{
  .dev =
    {
#if CONSOLE_UART == 2
      .isconsole = true,
#endif
      .recv      =
      {
        .size    = CONFIG_USART2_RXBUFSIZE,
        .buffer  = g_usart2rxbuffer,
      },
      .xmit      =
      {
        .size    = CONFIG_USART2_TXBUFSIZE,
        .buffer  = g_usart2txbuffer,
      },
      .ops       = &g_uart_ops,
      .priv      = &g_usart2priv,
    },

  .irq           = STM32_IRQ_USART2,
  .parity        = CONFIG_USART2_PARITY,
  .bits          = CONFIG_USART2_BITS,
  .stopbits2     = CONFIG_USART2_2STOP,
  .baud          = CONFIG_USART2_BAUD,
  .apbclock      = STM32_PCLK1_FREQUENCY,
  .usartbase     = STM32_USART2_BASE,
  .tx_gpio       = GPIO_USART2_TX,
  .rx_gpio       = GPIO_USART2_RX,
#ifdef GPIO_USART2_CTS
  .cts_gpio      = GPIO_USART2_CTS,
#endif
#ifdef GPIO_USART2_RTS
  .rts_gpio      = GPIO_USART2_RTS,
#endif
  .vector        = up_interrupt_usart2,
};
#endif

/* This describes the state of the STM32 USART3 port. */

#ifdef CONFIG_STM32_USART3
static struct up_dev_s g_usart3priv =
{
  .dev =
    {
#if CONSOLE_UART == 3
      .isconsole = true,
#endif
      .recv      =
      {
        .size    = CONFIG_USART3_RXBUFSIZE,
        .buffer  = g_usart3rxbuffer,
      },
      .xmit      =
      {
        .size    = CONFIG_USART3_TXBUFSIZE,
        .buffer  = g_usart3txbuffer,
      },
      .ops       = &g_uart_ops,
      .priv      = &g_usart3priv,
    },

  .irq           = STM32_IRQ_USART3,
  .parity        = CONFIG_USART3_PARITY,
  .bits          = CONFIG_USART3_BITS,
  .stopbits2     = CONFIG_USART3_2STOP,
  .baud          = CONFIG_USART3_BAUD,
  .apbclock      = STM32_PCLK1_FREQUENCY,
  .usartbase     = STM32_USART3_BASE,
  .tx_gpio       = GPIO_USART3_TX,
  .rx_gpio       = GPIO_USART3_RX,
#ifdef GPIO_USART3_CTS
  .cts_gpio      = GPIO_USART3_CTS,
#endif
#ifdef GPIO_USART3_RTS
  .rts_gpio      = GPIO_USART3_RTS,
#endif
  .vector        = up_interrupt_usart3,
};
#endif

/* This describes the state of the STM32 UART4 port. */

#ifdef CONFIG_STM32_UART4
static struct up_dev_s g_uart4priv =
{
  .dev =
    {
#if CONSOLE_UART == 4
      .isconsole = true,
#endif
      .recv      =
      {
        .size    = CONFIG_USART4_RXBUFSIZE,
        .buffer  = g_uart4rxbuffer,
      },
      .xmit      =
      {
        .size    = CONFIG_USART4_TXBUFSIZE,
        .buffer  = g_uart4txbuffer,
      },
      .ops       = &g_uart_ops,
      .priv      = &g_uart4priv,
    },

  .irq           = STM32_IRQ_UART4,
  .parity        = CONFIG_USART4_PARITY,
  .bits          = CONFIG_USART4_BITS,
  .stopbits2     = CONFIG_USART4_2STOP,
  .baud          = CONFIG_USART4_BAUD,
  .apbclock      = STM32_PCLK1_FREQUENCY,
  .usartbase     = STM32_UART4_BASE,
  .tx_gpio       = GPIO_UART4_TX,
  .rx_gpio       = GPIO_UART4_RX,
#ifdef GPIO_USART4_CTS
  .cts_gpio      = GPIO_UART4_CTS,
#endif
#ifdef GPIO_USART4_RTS
  .rts_gpio      = GPIO_UART4_RTS,
#endif
  .vector        = up_interrupt_uart4,
};
#endif

/* This describes the state of the STM32 UART5 port. */

#ifdef CONFIG_STM32_UART5
static struct up_dev_s g_uart5priv =
{
  .dev =
    {
#if CONSOLE_UART == 5
      .isconsole = true,
#endif
      .recv     =
      {
        .size   = CONFIG_USART5_RXBUFSIZE,
        .buffer = g_uart5rxbuffer,
      },
      .xmit     =
      {
        .size   = CONFIG_USART5_TXBUFSIZE,
        .buffer = g_uart5txbuffer,
      },
      .ops      = &g_uart_ops,
      .priv     = &g_uart5priv,
    },

  .irq            = STM32_IRQ_UART5,
  .parity         = CONFIG_USART5_PARITY,
  .bits           = CONFIG_USART5_BITS,
  .stopbits2      = CONFIG_USART5_2STOP,
  .baud           = CONFIG_USART5_BAUD,
  .apbclock       = STM32_PCLK1_FREQUENCY,
  .usartbase      = STM32_UART5_BASE,
  .tx_gpio        = GPIO_UART5_TX,
  .rx_gpio        = GPIO_UART5_RX,
#ifdef GPIO_USART5_CTS
  .cts_gpio       = GPIO_UART5_CTS,
#endif
#ifdef GPIO_USART5_RTS
  .rts_gpio       = GPIO_UART5_RTS,
#endif
  .vector         = up_interrupt_uart5,
};
#endif

/* This describes the state of the STM32 USART6 port. */

#ifdef CONFIG_STM32_USART6
static struct up_dev_s g_usart6priv =
{
  .dev =
    {
#if CONSOLE_UART == 6
      .isconsole = true,
#endif
      .recv     =
      {
        .size   = CONFIG_USART6_RXBUFSIZE,
        .buffer = g_usart6rxbuffer,
      },
      .xmit     =
      {
        .size   = CONFIG_USART6_TXBUFSIZE,
        .buffer = g_usart6txbuffer,
      },
      .ops      = &g_uart_ops,
      .priv     = &g_usart6priv,
    },

  .irq            = STM32_IRQ_USART6,
  .parity         = CONFIG_USART6_PARITY,
  .bits           = CONFIG_USART6_BITS,
  .stopbits2      = CONFIG_USART6_2STOP,
  .baud           = CONFIG_USART6_BAUD,
  .apbclock       = STM32_PCLK2_FREQUENCY,
  .usartbase      = STM32_USART6_BASE,
  .tx_gpio        = GPIO_USART6_TX,
  .rx_gpio        = GPIO_USART6_RX,
#ifdef GPIO_USART6_CTS
  .cts_gpio       = GPIO_USART6_CTS,
#endif
#ifdef GPIO_USART6_RTS
  .rts_gpio       = GPIO_USART6_RTS,
#endif
  .vector         = up_interrupt_usart6,
};
#endif

/* This table lets us iterate over the configured USARTs */

static struct up_dev_s *uart_devs[STM32_NUSART] =
{
#ifdef CONFIG_STM32_USART1
  [0] = &g_usart1priv,
#endif
#ifdef CONFIG_STM32_USART2
  [1] = &g_usart2priv,
#endif
#ifdef CONFIG_STM32_USART3
  [2] = &g_usart3priv,
#endif
#ifdef CONFIG_STM32_UART4
  [3] = &g_uart4priv,
#endif
#ifdef CONFIG_STM32_UART5
  [4] = &g_uart5priv,
#endif
#ifdef CONFIG_STM32_USART6
  [5] = &g_usart6priv,
#endif
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_serialin
 ****************************************************************************/

static inline uint32_t up_serialin(struct up_dev_s *priv, int offset)
{
  return getreg32(priv->usartbase + offset);
}

/****************************************************************************
 * Name: up_serialout
 ****************************************************************************/

static inline void up_serialout(struct up_dev_s *priv, int offset, uint32_t value)
{
  putreg32(value, priv->usartbase + offset);
}

/****************************************************************************
 * Name: up_restoreusartint
 ****************************************************************************/

static void up_restoreusartint(struct up_dev_s *priv, uint16_t ie)
{
  uint32_t cr;

  /* Save the interrupt mask */

  priv->ie = ie;

  /* And restore the interrupt state (see the interrupt enable/usage table above) */

  cr = up_serialin(priv, STM32_USART_CR1_OFFSET);
  cr &= ~(USART_CR1_RXNEIE|USART_CR1_TXEIE|USART_CR1_PEIE);
  cr |= (ie & (USART_CR1_RXNEIE|USART_CR1_TXEIE|USART_CR1_PEIE));
  up_serialout(priv, STM32_USART_CR1_OFFSET, cr);

  cr = up_serialin(priv, STM32_USART_CR3_OFFSET);
  cr &= ~USART_CR3_EIE;
  cr |= (ie & USART_CR3_EIE);
  up_serialout(priv, STM32_USART_CR3_OFFSET, cr);
}

/****************************************************************************
 * Name: up_disableusartint
 ****************************************************************************/

static inline void up_disableusartint(struct up_dev_s *priv, uint16_t *ie)
{
  if (ie)
    {
      uint32_t cr1;
      uint32_t cr3;

      /* USART interrupts:
       *
       * Enable             Bit Status          Meaning                        Usage
       * ------------------ --- --------------- ------------------------------ ----------
       * USART_CR1_IDLEIE    4  USART_SR_IDLE   Idle Line Detected             (not used)
       * USART_CR1_RXNEIE    5  USART_SR_RXNE   Received Data Ready to be Read
       * "              "       USART_SR_ORE    Overrun Error Detected
       * USART_CR1_TCIE      6  USART_SR_TC     Transmission Complete          (not used)
       * USART_CR1_TXEIE     7  USART_SR_TXE    Transmit Data Register Empty
       * USART_CR1_PEIE      8  USART_SR_PE     Parity Error
       *
       * USART_CR2_LBDIE     6  USART_SR_LBD    Break Flag                     (not used)
       * USART_CR3_EIE       0  USART_SR_FE     Framing Error
       * "           "          USART_SR_NE     Noise Error
       * "           "          USART_SR_ORE    Overrun Error Detected
       * USART_CR3_CTSIE    10  USART_SR_CTS    CTS flag                       (not used)
       */

      cr1 = up_serialin(priv, STM32_USART_CR1_OFFSET);
      cr3 = up_serialin(priv, STM32_USART_CR3_OFFSET);

      /* Return the current interrupt mask value for the used interrupts.  Notice
       * that this depends on the fact that none of the used interrupt enable bits
       * overlap.  This logic would fail if we needed the break interrupt!
       */

      *ie = (cr1 & (USART_CR1_RXNEIE|USART_CR1_TXEIE|USART_CR1_PEIE)) | (cr3 & USART_CR3_EIE);
    }

  /* Disable all interrupts */

  up_restoreusartint(priv, 0);
}

/****************************************************************************
 * Name: up_setup
 *
 * Description:
 *   Configure the USART baud, bits, parity, etc. This method is called the
 *   first time that the serial port is opened.
 *
 ****************************************************************************/

static int up_setup(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
#ifndef CONFIG_SUPPRESS_UART_CONFIG
  uint32_t usartdiv32;
  uint32_t mantissa;
  uint32_t fraction;
  uint32_t brr;
  uint32_t regval;

  /* Note: The logic here depends on the fact that that the USART module
   * was enabled in stm32_lowsetup().
   */

  /* Configure pins for USART use */

  stm32_configgpio(priv->tx_gpio);
  stm32_configgpio(priv->rx_gpio);

  if (priv->cts_gpio != 0)
    {
      stm32_configgpio(priv->cts_gpio);
    }

  if (priv->rts_gpio != 0)
    {
      stm32_configgpio(priv->rts_gpio);
    }

  /* Configure CR2 */
  /* Clear STOP, CLKEN, CPOL, CPHA, LBCL, and interrupt enable bits */

  regval = up_serialin(priv, STM32_USART_CR2_OFFSET);
  regval &= ~(USART_CR2_STOP_MASK|USART_CR2_CLKEN|USART_CR2_CPOL|
              USART_CR2_CPHA|USART_CR2_LBCL|USART_CR2_LBDIE);

  /* Configure STOP bits */

  if (priv->stopbits2)
    {
      regval |= USART_CR2_STOP2;
    }
  up_serialout(priv, STM32_USART_CR2_OFFSET, regval);

  /* Configure CR1 */
  /* Clear M, PCE, PS, TE, REm and all interrupt enable bits */

  regval  = up_serialin(priv, STM32_USART_CR1_OFFSET);
  regval &= ~(USART_CR1_M|USART_CR1_PCE|USART_CR1_PS|USART_CR1_TE|
              USART_CR1_RE|USART_CR1_ALLINTS);

  /* Configure word length and parity mode */

  if (priv->bits == 9)				/* Default: 1 start, 8 data, n stop */
    {
      regval |= USART_CR1_M;			/* 1 start, 9 data, n stop */
    }

  if (priv->parity == 1)			/* Odd parity */
    {
      regval |= (USART_CR1_PCE|USART_CR1_PS);
    }
  else if (priv->parity == 2)			/* Even parity */
    {
      regval |= USART_CR1_PCE;
    }
  up_serialout(priv, STM32_USART_CR1_OFFSET, regval);

  /* Configure CR3 */
  /* Clear CTSE, RTSE, and all interrupt enable bits */

  regval  = up_serialin(priv, STM32_USART_CR3_OFFSET);
  regval &= ~(USART_CR3_CTSIE|USART_CR3_CTSE|USART_CR3_RTSE|USART_CR3_EIE);

  /* Configure hardware flow control -- Not yet supported */

  up_serialout(priv, STM32_USART_CR3_OFFSET, regval);

  /* Configure the USART Baud Rate.  The baud rate for the receiver and
   * transmitter (Rx and Tx) are both set to the same value as programmed
   * in the Mantissa and Fraction values of USARTDIV.
   *
   *   baud     = fCK / (16 * usartdiv)
   *   usartdiv = fCK / (16 * baud)
   *
   * Where fCK is the input clock to the peripheral (PCLK1 for USART2, 3, 4, 5
   * or PCLK2 for USART1)
   *
   * First calculate (NOTE: all stand baud values are even so dividing by two
   * does not lose precision):
   *
   *   usartdiv32 = 32 * usartdiv = fCK / (baud/2)
   */

   usartdiv32 = priv->apbclock / (priv->baud >> 1);

   /* The mantissa part is then */

   mantissa   = usartdiv32 >> 5;
   brr        = mantissa << USART_BRR_MANT_SHIFT;

   /* The fractional remainder (with rounding) */

   fraction   = (usartdiv32 - (mantissa << 5) + 1) >> 1;
   brr       |= fraction << USART_BRR_FRAC_SHIFT;
   up_serialout(priv, STM32_USART_BRR_OFFSET, brr);

  /* Enable Rx, Tx, and the USART */

  regval      = up_serialin(priv, STM32_USART_CR1_OFFSET);
  regval     |= (USART_CR1_UE|USART_CR1_TE|USART_CR1_RE);
  up_serialout(priv, STM32_USART_CR1_OFFSET, regval);
#endif

  /* Set up the cached interrupt enables value */

  priv->ie    = 0;
  return OK;
}

/****************************************************************************
 * Name: up_shutdown
 *
 * Description:
 *   Disable the USART.  This method is called when the serial
 *   port is closed
 *
 ****************************************************************************/

static void up_shutdown(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
  uint32_t regval;

  /* Disable all interrupts */

  up_disableusartint(priv, NULL);

  /* Disable Rx, Tx, and the UART */

  regval      = up_serialin(priv, STM32_USART_CR1_OFFSET);
  regval     &= ~(USART_CR1_UE|USART_CR1_TE|USART_CR1_RE);
  up_serialout(priv, STM32_USART_CR1_OFFSET, regval);
}

/****************************************************************************
 * Name: up_attach
 *
 * Description:
 *   Configure the USART to operation in interrupt driven mode.  This method is
 *   called when the serial port is opened.  Normally, this is just after the
 *   the setup() method is called, however, the serial console may operate in
 *   a non-interrupt driven mode during the boot phase.
 *
 *   RX and TX interrupts are not enabled when by the attach method (unless the
 *   hardware supports multiple levels of interrupt enabling).  The RX and TX
 *   interrupts are not enabled until the txint() and rxint() methods are called.
 *
 ****************************************************************************/

static int up_attach(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
  int ret;

  /* Attach and enable the IRQ */

  ret = irq_attach(priv->irq, priv->vector);
  if (ret == OK)
    {
       /* Enable the interrupt (RX and TX interrupts are still disabled
        * in the USART
        */

       up_enable_irq(priv->irq);
    }
  return ret;
}

/****************************************************************************
 * Name: up_detach
 *
 * Description:
 *   Detach USART interrupts.  This method is called when the serial port is
 *   closed normally just before the shutdown method is called.  The exception
 *   is the serial console which is never shutdown.
 *
 ****************************************************************************/

static void up_detach(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
  up_disable_irq(priv->irq);
  irq_detach(priv->irq);
}

/****************************************************************************
 * Name: up_interrupt_common
 *
 * Description:
 *   This is the USART interrupt handler.  It will be invoked when an
 *   interrupt received on the 'irq'  It should call uart_transmitchars or
 *   uart_receivechar to perform the appropriate data transfers.  The
 *   interrupt handling logic must be able to map the 'irq' number into the
 *   approprite uart_dev_s structure in order to call these functions.
 *
 ****************************************************************************/

static int up_interrupt_common(struct up_dev_s *priv)
{
  int                passes;
  bool               handled;

  /* Loop until there are no characters to be transferred or,
   * until we have been looping for a long time.
   */

  handled = true;
  for (passes = 0; passes < 256 && handled; passes++)
    {
      handled = false;

      /* Get the masked USART status and clear the pending interrupts. */

      priv->sr = up_serialin(priv, STM32_USART_SR_OFFSET);

      /* USART interrupts:
       *
       * Enable             Bit Status          Meaning                         Usage
       * ------------------ --- --------------- ------------------------------- ----------
       * USART_CR1_IDLEIE    4  USART_SR_IDLE   Idle Line Detected              (not used)
       * USART_CR1_RXNEIE    5  USART_SR_RXNE   Received Data Ready to be Read
       * "              "       USART_SR_ORE    Overrun Error Detected
       * USART_CR1_TCIE      6  USART_SR_TC     Transmission Complete           (not used)
       * USART_CR1_TXEIE     7  USART_SR_TXE    Transmit Data Register Empty
       * USART_CR1_PEIE      8  USART_SR_PE     Parity Error
       *
       * USART_CR2_LBDIE     6  USART_SR_LBD    Break Flag                      (not used)
       * USART_CR3_EIE       0  USART_SR_FE     Framing Error
       * "           "          USART_SR_NE     Noise Error
       * "           "          USART_SR_ORE    Overrun Error Detected
       * USART_CR3_CTSIE    10  USART_SR_CTS    CTS flag                        (not used)
       *
       * NOTE: Some of these status bits must be cleared by explicity writing zero
       * to the SR register: USART_SR_CTS, USART_SR_LBD. Note of those are currently
       * being used.
       */

      /* Handle incoming, receive bytes (with or without timeout) */

      if ((priv->sr & USART_SR_RXNE) != 0 && (priv->ie & USART_CR1_RXNEIE) != 0)
        {
           /* Received data ready... process incoming bytes */

           uart_recvchars(&priv->dev);
           handled = true;
        }

      /* Handle outgoing, transmit bytes */

      if ((priv->sr & USART_SR_TXE) != 0 && (priv->ie & USART_CR1_TXEIE) != 0)
        {
           /* Transmit data regiser empty ... process outgoing bytes */

           uart_xmitchars(&priv->dev);
           handled = true;
        }
    }
    return OK;
}

/****************************************************************************
 * Name: up_ioctl
 *
 * Description:
 *   All ioctl calls will be routed through this method
 *
 ****************************************************************************/

static int up_ioctl(struct file *filep, int cmd, unsigned long arg)
{
  struct inode      *inode = filep->f_inode;
  struct uart_dev_s *dev   = inode->i_private;
#ifdef CONFIG_USART_BREAKS
  struct up_dev_s   *priv  = (struct up_dev_s*)dev->priv;
#endif
  int                ret    = OK;

  switch (cmd)
    {
    case TIOCSERGSTRUCT:
      {
         struct up_dev_s *user = (struct up_dev_s*)arg;
         if (!user)
           {
             ret = -EINVAL;
           }
         else
           {
             memcpy(user, dev, sizeof(struct up_dev_s));
           }
       }
       break;

#ifdef CONFIG_USART_BREAKS
    case TIOCSBRK:  /* BSD compatibility: Turn break on, unconditionally */
      {
        irqstate_t flags = irqsave();
        uint32_t cr2 = up_serialin(priv, STM32_USART_CR2_OFFSET);
        up_serialout(priv, STM32_USART_CR2_OFFSET, cr2 | USART_CR2_LINEN);
        irqrestore(flags);
      }
      break;

    case TIOCCBRK:  /* BSD compatibility: Turn break off, unconditionally */
      {
        irqstate_t flags;
        flags = irqsave();
        uint32_t cr1 = up_serialin(priv, STM32_USART_CR2_OFFSET);
        up_serialout(priv, STM32_USART_CR2_OFFSET, cr2 & ~USART_CR2_LINEN);
        irqrestore(flags);
      }
      break;
#endif

    default:
      ret = -ENOTTY;
      break;
    }

  return ret;
}

/****************************************************************************
 * Name: up_receive
 *
 * Description:
 *   Called (usually) from the interrupt level to receive one
 *   character from the USART.  Error bits associated with the
 *   receipt are provided in the return 'status'.
 *
 ****************************************************************************/

static int up_receive(struct uart_dev_s *dev, uint32_t *status)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
  uint32_t dr;

  /* Get the Rx byte */

  dr       = up_serialin(priv, STM32_USART_DR_OFFSET);

  /* Get the Rx byte plux error information.  Return those in status */

  *status  = priv->sr << 16 | dr;
  priv->sr = 0;

  /* Then return the actual received byte */

  return dr & 0xff;
}

/****************************************************************************
 * Name: up_rxint
 *
 * Description:
 *   Call to enable or disable RX interrupts
 *
 ****************************************************************************/

static void up_rxint(struct uart_dev_s *dev, bool enable)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
  uint16_t ie;

      /* USART receive interrupts:
       *
       * Enable             Bit Status          Meaning                         Usage
       * ------------------ --- --------------- ------------------------------- ----------
       * USART_CR1_IDLEIE    4  USART_SR_IDLE   Idle Line Detected              (not used)
       * USART_CR1_RXNEIE    5  USART_SR_RXNE   Received Data Ready to be Read
       * "              "       USART_SR_ORE    Overrun Error Detected
       * USART_CR1_PEIE      8  USART_SR_PE     Parity Error
       *
       * USART_CR2_LBDIE     6  USART_SR_LBD    Break Flag                      (not used)
       * USART_CR3_EIE       0  USART_SR_FE     Framing Error
       * "           "          USART_SR_NE     Noise Error
       * "           "          USART_SR_ORE    Overrun Error Detected
       */

  ie = priv->ie;
  if (enable)
    {
      /* Receive an interrupt when their is anything in the Rx data register (or an Rx
       * timeout occurs).
       */

#ifndef CONFIG_SUPPRESS_SERIAL_INTS
#ifdef CONFIG_USART_ERRINTS
      ie |= (USART_CR1_RXNEIE|USART_CR1_PEIE|USART_CR3_EIE);
#else
      ie |= USART_CR1_RXNEIE;
#endif
#endif
    }
  else
    {
      ie &= ~(USART_CR1_RXNEIE|USART_CR1_PEIE|USART_CR3_EIE);
    }

  /* Then set the new interrupt state */

  up_restoreusartint(priv, ie);
}

/****************************************************************************
 * Name: up_rxavailable
 *
 * Description:
 *   Return true if the receive register is not empty
 *
 ****************************************************************************/

static bool up_rxavailable(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
  return ((up_serialin(priv, STM32_USART_SR_OFFSET) & USART_SR_RXNE) != 0);
}

/****************************************************************************
 * Name: up_send
 *
 * Description:
 *   This method will send one byte on the USART
 *
 ****************************************************************************/

static void up_send(struct uart_dev_s *dev, int ch)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
  up_serialout(priv, STM32_USART_DR_OFFSET, (uint32_t)ch);
}

/****************************************************************************
 * Name: up_txint
 *
 * Description:
 *   Call to enable or disable TX interrupts
 *
 ****************************************************************************/

static void up_txint(struct uart_dev_s *dev, bool enable)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
  irqstate_t flags;

  /* USART transmit interrupts:
   *
   * Enable             Bit Status          Meaning                      Usage
   * ------------------ --- --------------- ---------------------------- ----------
   * USART_CR1_TCIE      6  USART_SR_TC     Transmission Complete        (not used)
   * USART_CR1_TXEIE     7  USART_SR_TXE    Transmit Data Register Empty
   * USART_CR3_CTSIE    10  USART_SR_CTS    CTS flag                     (not used)
   */
 
  flags = irqsave();
  if (enable)
    {
      /* Set to receive an interrupt when the TX data register is empty */

#ifndef CONFIG_SUPPRESS_SERIAL_INTS
      up_restoreusartint(priv, priv->ie | USART_CR1_TXEIE);

      /* Fake a TX interrupt here by just calling uart_xmitchars() with
       * interrupts disabled (note this may recurse).
       */

      uart_xmitchars(dev);
#endif
    }
  else
    {
      /* Disable the TX interrupt */

      up_restoreusartint(priv, priv->ie & ~USART_CR1_TXEIE);
    }
  irqrestore(flags);
}

/****************************************************************************
 * Name: up_txready
 *
 * Description:
 *   Return true if the tranmsit data register is empty
 *
 ****************************************************************************/

static bool up_txready(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s*)dev->priv;
  return ((up_serialin(priv, STM32_USART_SR_OFFSET) & USART_SR_TXE) != 0);
}

/****************************************************************************
 * Name: up_interrupt_u[s]art[n]
 *
 * Description:
 *   Interrupt handlers for U[S]ART[n] where n=1,..,6.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32_USART1
static int up_interrupt_usart1(int irq, void *context)
{
  return up_interrupt_common(&g_usart1priv);
}
#endif

#ifdef CONFIG_STM32_USART2
static int up_interrupt_usart2(int irq, void *context)
{
  return up_interrupt_common(&g_usart2priv);
}
#endif

#ifdef CONFIG_STM32_USART3
static int up_interrupt_usart3(int irq, void *context)
{
  return up_interrupt_common(&g_usart3priv);
}
#endif

#ifdef CONFIG_STM32_UART4
static int up_interrupt_uart4(int irq, void *context)
{
  return up_interrupt_common(&g_uart4priv);
}
#endif

#ifdef CONFIG_STM32_UART5
static int up_interrupt_uart5(int irq, void *context)
{
  return up_interrupt_common(&g_uart5priv);
}
#endif

#ifdef CONFIG_STM32_USART6
static int up_interrupt_usart6(int irq, void *context)
{
  return up_interrupt_common(&g_usart6priv);
}
#endif
#endif /* HAVE UART */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_earlyserialinit
 *
 * Description:
 *   Performs the low level USART initialization early in debug so that the
 *   serial console will be available during bootup.  This must be called
 *   before up_serialinit.
 *
 ****************************************************************************/

void up_earlyserialinit(void)
{
#ifdef HAVE_UART
  unsigned i;

  /* Disable all USART interrupts */

  for (i = 0; i < STM32_NUSART; i++) 
    {
      if (uart_devs[i])
        {
          up_disableusartint(uart_devs[i], NULL);
        }
    }

  /* Configure whichever one is the console */

#if CONSOLE_UART > 0
  up_setup(&uart_devs[CONSOLE_UART - 1]->dev);
#endif
#endif /* HAVE UART */
}

/****************************************************************************
 * Name: up_serialinit
 *
 * Description:
 *   Register serial console and serial ports.  This assumes
 *   that up_earlyserialinit was called previously.
 *
 ****************************************************************************/

void up_serialinit(void)
{
#ifdef HAVE_UART
  char devname[16];
  unsigned i, j;

  /* Register the console */

#if CONSOLE_UART > 0
  (void)uart_register("/dev/console", &uart_devs[CONSOLE_UART - 1]->dev);
  (void)uart_register("/dev/ttyS0",   &uart_devs[CONSOLE_UART - 1]->dev);
#endif

  /* Register all remaining USARTs */

  strcpy(devname, "/dev/ttySx");

  for (i = 0, j = 1; i < STM32_NUSART; i++) 
    {

      /* don't create a device for the console - we did that above */

      if ((uart_devs[i] == 0) || (uart_devs[i]->dev.isconsole))
        {
          continue;
        }

      /* register USARTs as devices in increasing order */

      devname[9] = '0' + j++;
      (void)uart_register(devname, &uart_devs[i]->dev);
    }
#endif /* HAVE UART */
}

/****************************************************************************
 * Name: up_putc
 *
 * Description:
 *   Provide priority, low-level access to support OS debug  writes
 *
 ****************************************************************************/

int up_putc(int ch)
{
#if CONSOLE_UART > 0
  struct up_dev_s *priv = uart_devs[CONSOLE_UART - 1];
  uint16_t ie;

  up_disableusartint(priv, &ie);

  /* Check for LF */

  if (ch == '\n')
    {
      /* Add CR */

      up_lowputc('\r');
    }

  up_lowputc(ch);
  up_restoreusartint(priv, ie);
#endif
  return ch;
}

#else /* USE_SERIALDRIVER */

/****************************************************************************
 * Name: up_putc
 *
 * Description:
 *   Provide priority, low-level access to support OS debug writes
 *
 ****************************************************************************/

int up_putc(int ch)
{
#if CONSOLE_UART > 0
  /* Check for LF */

  if (ch == '\n')
    {
      /* Add CR */

      up_lowputc('\r');
    }

  up_lowputc(ch);
#endif
  return ch;
}

#endif /* USE_SERIALDRIVER */
