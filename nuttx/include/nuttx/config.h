/* config.h -- Autogenerated! Do not edit. */

#ifndef __INCLUDE_NUTTX_CONFIG_H
#define __INCLUDE_NUTTX_CONFIG_H

/* Architecture-specific options *************************/

#define CONFIG_ARCH arm
#define CONFIG_ARCH_ARM 1
#define CONFIG_ARCH_CORTEXM4 1
#define CONFIG_ARCH_CHIP stm32
#define CONFIG_ARCH_CHIP_STM32F407VG 1
#define CONFIG_ARCH_BOARD stm32f4discovery
#define CONFIG_ARCH_BOARD_STM32F4_DISCOVERY 1
#define CONFIG_BOARD_LOOPSPERMSEC 16717
#define CONFIG_DRAM_SIZE 0x00030000
#define CONFIG_DRAM_START 0x20000000
#define CONFIG_DRAM_END (CONFIG_DRAM_START+CONFIG_DRAM_SIZE)
#define CONFIG_ARCH_IRQPRIO 1
#undef CONFIG_ARCH_FPU
#undef CONFIG_ARCH_INTERRUPTSTACK
#define CONFIG_ARCH_STACKDUMP 1
#undef CONFIG_ARCH_BOOTLOADER
#define CONFIG_ARCH_LEDS 1
#define CONFIG_ARCH_BUTTONS 1
#undef CONFIG_ARCH_CALIBRATION
#undef CONFIG_ARCH_DMA
#undef CONFIG_STM32_CODESOURCERYW
#define CONFIG_STM32_CODESOURCERYL 1
#undef CONFIG_STM32_ATOLLIC_LITE
#undef CONFIG_STM32_ATOLLIC_PRO
#undef CONFIG_STM32_DEVKITARM
#undef CONFIG_STM32_RAISONANCE
#undef CONFIG_STM32_BUILDROOT
#undef CONFIG_STM32_DFU
#undef CONFIG_STM32_JTAG_FULL_ENABLE
#undef CONFIG_STM32_JTAG_NOJNTRST_ENABLE
#define CONFIG_STM32_JTAG_SW_ENABLE 1
#undef CONFIG_STM32_CRC
#undef CONFIG_STM32_BKPSRAM
#undef CONFIG_STM32_CCMDATARAM
#undef CONFIG_STM32_DMA1
#undef CONFIG_STM32_DMA2
#undef CONFIG_STM32_ETHMAC
#undef CONFIG_STM32_OTGHS
#undef CONFIG_STM32_DCMI
#undef CONFIG_STM32_CRYP
#undef CONFIG_STM32_HASH
#undef CONFIG_STM32_RNG
#define CONFIG_STM32_OTGFS 1
#undef CONFIG_STM32_FSMC
#undef CONFIG_STM32_TIM2
#undef CONFIG_STM32_TIM3
#undef CONFIG_STM32_TIM4
#undef CONFIG_STM32_TIM5
#undef CONFIG_STM32_TIM6
#undef CONFIG_STM32_TIM7
#undef CONFIG_STM32_TIM12
#undef CONFIG_STM32_TIM13
#undef CONFIG_STM32_TIM14
#undef CONFIG_STM32_WWDG
#undef CONFIG_STM32_SPI2
#undef CONFIG_STM32_SPI3
#define CONFIG_STM32_USART2 1
#undef CONFIG_STM32_USART3
#undef CONFIG_STM32_UART4
#undef CONFIG_STM32_UART5
#undef CONFIG_STM32_I2C1
#undef CONFIG_STM32_I2C2
#undef CONFIG_STM32_I2C3
#undef CONFIG_STM32_CAN1
#undef CONFIG_STM32_CAN2
#undef CONFIG_STM32_DAC
#define CONFIG_STM32_PWR 1
#undef CONFIG_STM32_TIM1
#undef CONFIG_STM32_TIM8
#undef CONFIG_STM32_USART1
#undef CONFIG_STM32_USART6
#undef CONFIG_STM32_ADC1
#undef CONFIG_STM32_ADC2
#undef CONFIG_STM32_ADC3
#undef CONFIG_STM32_SDIO
#define CONFIG_STM32_SPI1 1
#define CONFIG_STM32_SYSCFG 1
#undef CONFIG_STM32_TIM9
#undef CONFIG_STM32_TIM10
#undef CONFIG_STM32_TIM11
#undef CONFIG_USART1_SERIAL_CONSOLE
#define CONFIG_USART2_SERIAL_CONSOLE 1
#undef CONFIG_USART3_SERIAL_CONSOLE
#undef CONFIG_USART4_SERIAL_CONSOLE
#undef CONFIG_USART5_SERIAL_CONSOLE
#define CONFIG_USART1_TXBUFSIZE 128
#define CONFIG_USART2_TXBUFSIZE 128
#define CONFIG_USART3_TXBUFSIZE 128
#define CONFIG_USART4_TXBUFSIZE 128
#define CONFIG_USART5_TXBUFSIZE 128
#define CONFIG_USART1_RXBUFSIZE 128
#define CONFIG_USART2_RXBUFSIZE 128
#define CONFIG_USART3_RXBUFSIZE 128
#define CONFIG_USART4_RXBUFSIZE 128
#define CONFIG_USART5_RXBUFSIZE 128
#define CONFIG_USART1_BAUD 115200
#define CONFIG_USART2_BAUD 115200
#define CONFIG_USART3_BAUD 115200
#define CONFIG_USART4_BAUD 115200
#define CONFIG_USART5_BAUD 115200
#define CONFIG_USART1_BITS 8
#define CONFIG_USART2_BITS 8
#define CONFIG_USART3_BITS 8
#define CONFIG_USART4_BITS 8
#define CONFIG_USART5_BITS 8
#define CONFIG_USART1_PARITY 0
#define CONFIG_USART2_PARITY 0
#define CONFIG_USART3_PARITY 0
#define CONFIG_USART4_PARITY 0
#define CONFIG_USART5_PARITY 0
#define CONFIG_USART1_2STOP 0
#define CONFIG_USART2_2STOP 0
#define CONFIG_USART3_2STOP 0
#define CONFIG_USART4_2STOP 0
#define CONFIG_USART5_2STOP 0
#define CONFIG_SPI_EXCHANGE 1
#undef CONFIG_CAN
#undef CONFIG_CAN_EXTID
#undef CONFIG_CAN_LOOPBACK
#define CONFIG_CAN1_BAUD 700000
#define CONFIG_CAN2_BAUD 700000
#define CONFIG_STM32_PHYADDR 0x01
#define CONFIG_STM32_MII 1
#define CONFIG_STM32_MII_MCO1 1
#undef CONFIG_STM32_MII_MCO2
#undef CONFIG_STM32_RMII
#define CONFIG_STM32_AUTONEG 1
#define CONFIG_STM32_PHYSR 16
#define CONFIG_STM32_PHYSR_SPEED 0x0002
#define CONFIG_STM32_PHYSR_100MBPS 0x0000
#define CONFIG_STM32_PHYSR_MODE 0x0004
#define CONFIG_STM32_PHYSR_FULLDUPLEX 0x0004
#undef CONFIG_STM32_ETH_PTP
#undef CONFIG_STM32_ETHMAC_REGDEBUG
#undef CONFIG_PWM
#define CONFIG_STM32_TIM4_PWM 1
#define CONFIG_STM32_TIM4_CHANNEL 2
#undef CONFIG_QENCODER
#undef CONFIG_STM32_TIM2_QE
#define CONFIG_STM32_TIM2_QECLKOUT 28000000
#define CONFIG_STM32_TIM8_QE 1
#define CONFIG_STM32_TIM8_QECLKOUT 28000000
#undef CONFIG_RRLOAD_BINARY
#define CONFIG_INTELHEX_BINARY 1
#undef CONFIG_MOTOROLA_SREC
#define CONFIG_RAW_BINARY 1
#undef CONFIG_HAVE_LIBM
#undef CONFIG_DEBUG
#undef CONFIG_DEBUG_VERBOSE
#undef CONFIG_DEBUG_SYMBOLS
#undef CONFIG_DEBUG_FS
#undef CONFIG_DEBUG_GRAPHICS
#undef CONFIG_DEBUG_LCD
#undef CONFIG_DEBUG_USB
#undef CONFIG_DEBUG_NET
#undef CONFIG_DEBUG_RTC
#undef CONFIG_DEBUG_ANALOG
#undef CONFIG_DEBUG_PWM
#undef CONFIG_DEBUG_CAN
#undef CONFIG_DEBUG_QENCODER
#define CONFIG_HAVE_CXX 1
#define CONFIG_HAVE_CXXINITIALIZE 1
#define CONFIG_MM_REGIONS 2
#define CONFIG_ARCH_LOWPUTC 1
#define CONFIG_RR_INTERVAL 200
#undef CONFIG_SCHED_INSTRUMENTATION
#define CONFIG_TASK_NAME_SIZE 0
#define CONFIG_START_YEAR 2011
#define CONFIG_START_MONTH 12
#define CONFIG_START_DAY 6
#undef CONFIG_GREGORIAN_TIME
#undef CONFIG_JULIAN_TIME
#define CONFIG_DEV_CONSOLE 1
#undef CONFIG_DEV_LOWCONSOLE
#undef CONFIG_MUTEX_TYPES
#undef CONFIG_PRIORITY_INHERITANCE
#define CONFIG_SEM_PREALLOCHOLDERS 0
#define CONFIG_SEM_NNESTPRIO 0
#undef CONFIG_FDCLONE_DISABLE
#undef CONFIG_FDCLONE_STDIO
#define CONFIG_SDCLONE_DISABLE 1
#undef CONFIG_SCHED_WORKQUEUE
#define CONFIG_SCHED_WORKPRIORITY 50
#define CONFIG_SCHED_WORKPERIOD (50*1000)
#define CONFIG_SCHED_WORKSTACKSIZE 1024
#define CONFIG_SIG_SIGWORK 4
#define CONFIG_SCHED_WAITPID 1
#undef CONFIG_SCHED_ATEXIT
#undef CONFIG_NXFLAT
#undef CONFIG_NXFLAT_DUMPBUFFER
#define CONFIG_SYMTAB_ORDEREDBYNAME 1
#undef CONFIG_DISABLE_CLOCK
#undef CONFIG_DISABLE_POSIX_TIMERS
#undef CONFIG_DISABLE_PTHREAD
#undef CONFIG_DISABLE_SIGNALS
#undef CONFIG_DISABLE_MQUEUE
#undef CONFIG_DISABLE_MOUNTPOINT
#undef CONFIG_DISABLE_ENVIRON
#define CONFIG_DISABLE_POLL 1
#undef CONFIG_NOPRINTF_FIELDWIDTH
#undef CONFIG_ARCH_MEMCPY
#undef CONFIG_ARCH_MEMCMP
#undef CONFIG_ARCH_MEMMOVE
#undef CONFIG_ARCH_MEMSET
#undef CONFIG_ARCH_STRCMP
#undef CONFIG_ARCH_STRCPY
#undef CONFIG_ARCH_STRNCPY
#undef CONFIG_ARCH_STRLEN
#undef CONFIG_ARCH_STRNLEN
#undef CONFIG_ARCH_BZERO
#define CONFIG_MAX_TASKS 16
#define CONFIG_MAX_TASK_ARGS 4
#define CONFIG_NPTHREAD_KEYS 4
#define CONFIG_NFILE_DESCRIPTORS 8
#define CONFIG_NFILE_STREAMS 8
#define CONFIG_NAME_MAX 32
#define CONFIG_STDIO_BUFFER_SIZE 256
#define CONFIG_STDIO_LINEBUFFER 1
#define CONFIG_NUNGET_CHARS 2
#define CONFIG_PREALLOC_MQ_MSGS 4
#define CONFIG_MQ_MAXMSGSIZE 32
#define CONFIG_MAX_WDOGPARMS 2
#define CONFIG_PREALLOC_WDOGS 4
#define CONFIG_PREALLOC_TIMERS 4
#undef CONFIG_FB_CMAP
#undef CONFIG_FB_HWCURSOR
#undef CONFIG_FB_HWCURSORIMAGE
#undef CONFIG_FS_FAT
#define CONFIG_FAT_LCNAMES 1
#define CONFIG_FAT_LFN 1
#define CONFIG_FAT_MAXFNAME 32
#undef CONFIG_FS_NXFFS
#undef CONFIG_FS_ROMFS
#define CONFIG_MMCSD_NSLOTS 0
#undef CONFIG_MMCSD_READONLY
#define CONFIG_MMCSD_SPICLOCK 12500000
#undef CONFIG_FS_READAHEAD
#undef CONFIG_FS_WRITEBUFFER
#undef CONFIG_SDIO_DMA
#define CONFIG_MMCSD_MULTIBLOCK_DISABLE 1
#undef CONFIG_MMCSD_MMCSUPPORT
#undef CONFIG_MMCSD_HAVECARDDETECT
#undef CONFIG_NET
#undef CONFIG_NET_NOINTS
#define CONFIG_NET_MULTIBUFFER 1
#undef CONFIG_NET_IPv6
#define CONFIG_NSOCKET_DESCRIPTORS 10
#define CONFIG_NET_SOCKOPTS 1
#define CONFIG_NET_BUFSIZE 562
#define CONFIG_NET_TCP 1
#define CONFIG_NET_TCP_CONNS 40
#define CONFIG_NET_NTCP_READAHEAD_BUFFERS 16
#define CONFIG_NET_TCPBACKLOG 1
#define CONFIG_NET_MAX_LISTENPORTS 40
#define CONFIG_NET_UDP 1
#define CONFIG_NET_UDP_CHECKSUMS 1
#define CONFIG_NET_ICMP 1
#define CONFIG_NET_ICMP_PING 1
#define CONFIG_NET_STATISTICS 1
#undef CONFIG_NET_BROADCAST
#undef CONFIG_NET_ARP_IPIN
#undef CONFIG_NET_MULTICAST
#undef CONFIG_NET_DHCP_LIGHT
#define CONFIG_NET_RESOLV_ENTRIES 4
#undef CONFIG_RTC
#define CONFIG_RTC_DATETIME 1
#undef CONFIG_RTC_HIRES
#undef CONFIG_RTC_FREQUENCY
#undef CONFIG_RTC_ALARM
#undef CONFIG_USBDEV
#undef CONFIG_USBDEV_ISOCHRONOUS
#undef CONFIG_USBDEV_DUALSPEED
#define CONFIG_USBDEV_SELFPOWERED 1
#undef CONFIG_USBDEV_REMOTEWAKEUP
#define CONFIG_USBDEV_MAXPOWER 100
#undef CONFIG_USBDEV_TRACE
#define CONFIG_USBDEV_TRACE_NRECORDS 128
#undef CONFIG_PL2303
#define CONFIG_PL2303_EPINTIN 1
#define CONFIG_PL2303_EPBULKOUT 2
#define CONFIG_PL2303_EPBULKIN 3
#define CONFIG_PL2303_NWRREQS 4
#define CONFIG_PL2303_NRDREQS 4
#define CONFIG_PL2303_VENDORID 0x067b
#define CONFIG_PL2303_PRODUCTID 0x2303
#define CONFIG_PL2303_VENDORSTR "Nuttx"
#define CONFIG_PL2303_PRODUCTSTR "USBdev Serial"
#define CONFIG_PL2303_RXBUFSIZE 512
#define CONFIG_PL2303_TXBUFSIZE 512
#undef CONFIG_USBMSC
#define CONFIG_USBMSC_EP0MAXPACKET 64
#define CONFIG_USBMSC_EPBULKOUT 2
#define CONFIG_USBMSC_EPBULKIN 5
#define CONFIG_USBMSC_NRDREQS 2
#define CONFIG_USBMSC_NWRREQS 2
#define CONFIG_USBMSC_BULKINREQLEN 256
#define CONFIG_USBMSC_BULKOUTREQLEN 256
#define CONFIG_USBMSC_VENDORID 0x584e
#define CONFIG_USBMSC_VENDORSTR "NuttX"
#define CONFIG_USBMSC_PRODUCTID 0x5342
#define CONFIG_USBMSC_PRODUCTSTR "USBdev Storage"
#define CONFIG_USBMSC_VERSIONNO 0x0399
#define CONFIG_USBMSC_REMOVABLE 1
#undef CONFIG_NX
#undef CONFIG_NX_MULTIUSER
#define CONFIG_NX_NPLANES 1
#define CONFIG_NX_DISABLE_1BPP 1
#define CONFIG_NX_DISABLE_2BPP 1
#define CONFIG_NX_DISABLE_4BPP 1
#define CONFIG_NX_DISABLE_8BPP 1
#undef CONFIG_NX_DISABLE_16BPP
#define CONFIG_NX_DISABLE_24BPP 1
#define CONFIG_NX_DISABLE_32BPP 1
#undef CONFIG_NX_PACKEDMSFIRST
#define CONFIG_NX_LCDDRIVER 1
#define CONFIG_LCD_MAXPOWER 1
#define CONFIG_LCD_MAXCONTRAST 1
#define CONFIG_NX_MOUSE 1
#define CONFIG_NX_KBD 1
#define CONFIG_NXTK_BORDERCOLOR1 0xd69a
#define CONFIG_NXTK_BORDERCOLOR2 0xad55
#undef CONFIG_NXTK_AUTORAISE
#define CONFIG_NXFONT_SANS17X22 1
#undef CONFIG_NXFONT_SANS20X26
#undef CONFIG_NXFONT_SANS22X29
#undef CONFIG_NXFONT_SANS23X27
#undef CONFIG_NXFONT_SANS28X37
#undef CONFIG_NXFONT_SANS17X23B
#define CONFIG_NXFONT_SANS20X27B 1
#define CONFIG_NXFONT_SANS22X29B 1
#undef CONFIG_NXFONT_SANS28X37B
#undef CONFIG_NXFONT_SANS40X49B
#undef CONFIG_NXFONT_SERIF22X29
#undef CONFIG_NXFONT_SERIF29X37
#undef CONFIG_NXFONT_SERIF38X48
#undef CONFIG_NXFONT_SERIF22X28B
#undef CONFIG_NXFONT_SERIF27X38B
#undef CONFIG_NXFONT_SERIF38X49B
#define CONFIG_NXFONTS_CHARBITS 7
#define CONFIG_NX_BLOCKING 1
#define CONFIG_NX_MXSERVERMSGS 32
#define CONFIG_NX_MXCLIENTMSGS 16
#define CONFIG_EXAMPLE_UIP_IPADDR (10<<24|0<<16|0<<8|2)
#define CONFIG_EXAMPLE_UIP_DRIPADDR (10<<24|0<<16|0<<8|1)
#define CONFIG_EXAMPLE_UIP_NETMASK (255<<24|255<<16|255<<8|0)
#undef CONFIG_EXAMPLE_UIP_DHCPC
#undef CONFIG_EXAMPLE_NETTEST_SERVER
#undef CONFIG_EXAMPLE_NETTEST_PERFORMANCE
#define CONFIG_EXAMPLE_NETTEST_NOMAC 1
#define CONFIG_EXAMPLE_NETTEST_IPADDR (10<<24|0<<16|0<<8|2)
#define CONFIG_EXAMPLE_NETTEST_DRIPADDR (10<<24|0<<16|0<<8|1)
#define CONFIG_EXAMPLE_NETTEST_NETMASK (255<<24|255<<16|255<<8|0)
#define CONFIG_EXAMPLE_NETTEST_CLIENTIP (10<<24|0<<16|0<<8|1)
#define CONFIG_EXAMPLES_OSTEST_LOOPS 1
#define CONFIG_EXAMPLES_OSTEST_STACKSIZE 2048
#define CONFIG_EXAMPLES_OSTEST_NBARRIER_THREADS 3
#define CONFIG_BUILTIN_APP_START "autopilotone"
#define CONFIG_NSH_BUILTIN_APPS 1
#define CONFIG_NSH_FILEIOSIZE 512
#undef CONFIG_NSH_STRERROR
#define CONFIG_NSH_LINELEN 64
#define CONFIG_NSH_NESTDEPTH 3
#undef CONFIG_NSH_DISABLESCRIPT
#undef CONFIG_NSH_DISABLEBG
#undef CONFIG_NSH_ROMFSETC
#define CONFIG_NSH_CONSOLE 1
#undef CONFIG_NSH_TELNET
#undef CONFIG_NSH_ARCHINIT
#define CONFIG_NSH_IOBUFFER_SIZE 512
#undef CONFIG_NSH_DHCPC
#define CONFIG_NSH_NOMAC 1
#define CONFIG_NSH_IPADDR (10<<24|0<<16|0<<8|2)
#define CONFIG_NSH_DRIPADDR (10<<24|0<<16|0<<8|1)
#define CONFIG_NSH_NETMASK (255<<24|255<<16|255<<8|0)
#define CONFIG_NSH_ROMFSMOUNTPT "/etc"
#define CONFIG_NSH_INITSCRIPT "init.d/rcS"
#define CONFIG_NSH_ROMFSDEVNO 0
#define CONFIG_NSH_ROMFSSECTSIZE 64
#define CONFIG_NSH_FATDEVNO 1
#define CONFIG_NSH_FATSECTSIZE 512
#define CONFIG_NSH_FATNSECTORS 1024
#define CONFIG_NSH_FATMOUNTPT /tmp
#define CONFIG_NSH_MMCSDSPIPORTNO 0
#define CONFIG_NSH_MMCSDSLOTNO 0
#define CONFIG_NSH_MMCSDMINOR 0
#undef CONFIG_EXAMPLES_USBSERIAL_INONLY
#undef CONFIG_EXAMPLES_USBSERIAL_OUTONLY
#undef CONFIG_EXAMPLES_USBSERIAL_ONLYSMALL
#undef CONFIG_EXAMPLES_USBSERIAL_ONLYBIG
#undef CONFIG_EXAMPLES_USBSERIAL_TRACEINIT
#undef CONFIG_EXAMPLES_USBSERIAL_TRACECLASS
#undef CONFIG_EXAMPLES_USBSERIAL_TRACETRANSFERS
#undef CONFIG_EXAMPLES_USBSERIAL_TRACECONTROLLER
#undef CONFIG_EXAMPLES_USBSERIAL_TRACEINTERRUPTS
#undef CONFIG_BOOT_RUNFROMFLASH
#undef CONFIG_BOOT_COPYTORAM
#undef CONFIG_CUSTOM_STACK
#undef CONFIG_STACK_POINTER
#define CONFIG_IDLETHREAD_STACKSIZE 1024
#define CONFIG_USERMAIN_STACKSIZE 2048
#define CONFIG_PTHREAD_STACK_MIN 256
#define CONFIG_PTHREAD_STACK_DEFAULT 2048
#undef CONFIG_HEAP_BASE
#undef CONFIG_HEAP_SIZE
#define CONFIG_APPS_DIR "../apps"

/* Sanity Checks *****************************************/

/* If this is an NXFLAT, external build, then make sure that
 * NXFLAT support is enabled in the base code.
 */

#if defined(__NXFLAT__) && !defined(CONFIG_NXFLAT)
# error "NXFLAT support not enabled in this configuration"
#endif

/* NXFLAT requires PIC support in the TCBs. */

#if defined(CONFIG_NXFLAT)
# undef CONFIG_PIC
# define CONFIG_PIC 1
#endif

/* Binary format support is disabled if no binary formats are
 * configured (at present, NXFLAT is the only supported binary.
 * format).
 */

#if !defined(CONFIG_NXFLAT)
# undef CONFIG_BINFMT_DISABLE
# define CONFIG_BINFMT_DISABLE 1
#endif

/* The correct way to disable RR scheduling is to set the
 * timeslice to zero.
 */

#ifndef CONFIG_RR_INTERVAL
# define CONFIG_RR_INTERVAL 0
#endif

/* The correct way to disable filesystem supuport is to set the
 * number of file descriptors to zero.
 */

#ifndef CONFIG_NFILE_DESCRIPTORS
# define CONFIG_NFILE_DESCRIPTORS 0
#endif

/* If a console is selected, then make sure that there are
 * resources for 3 file descriptors and, if any streams are
 * selected, also for 3 file streams.
 */

#ifdef CONFIG_DEV_CONSOLE
# if CONFIG_NFILE_DESCRIPTORS < 3
#   undef CONFIG_NFILE_DESCRIPTORS
#   define CONFIG_NFILE_DESCRIPTORS 3
# endif

# if CONFIG_NFILE_STREAMS > 0 && CONFIG_NFILE_STREAMS < 3
#  undef CONFIG_NFILE_STREAMS
#  define CONFIG_NFILE_STREAMS 3
# endif

/* If no console is selected, then disable all console devices */

#else
#  undef CONFIG_DEV_LOWCONSOLE
#  undef CONFIG_RAMLOG_CONSOLE
#  undef CONFIG_CDCACM_CONSOLE
#  undef CONFIG_PL2303_CONSOLE
#endif

/* If priority inheritance is disabled, then do not allocate any
 * associated resources.
 */

#if !defined(CONFIG_PRIORITY_INHERITANCE) || !defined(CONFIG_SEM_PREALLOCHOLDERS)
# undef CONFIG_SEM_PREALLOCHOLDERS
# define CONFIG_SEM_PREALLOCHOLDERS 0
#endif

#if !defined(CONFIG_PRIORITY_INHERITANCE) || !defined(CONFIG_SEM_NNESTPRIO)
# undef CONFIG_SEM_NNESTPRIO
# define CONFIG_SEM_NNESTPRIO 0
#endif

/* If no file descriptors are configured, then make certain no
 * streams are configured either.
 */

#if CONFIG_NFILE_DESCRIPTORS == 0
# undef CONFIG_NFILE_STREAMS
# define CONFIG_NFILE_STREAMS 0
#endif

/* There must be at least one memory region. */

#ifndef CONFIG_MM_REGIONS
# define CONFIG_MM_REGIONS 1
#endif

/* If no file streams are configured, then make certain that buffered I/O
 * support is disabled
 */

#if CONFIG_NFILE_STREAMS == 0
# undef CONFIG_STDIO_BUFFER_SIZE
# define CONFIG_STDIO_BUFFER_SIZE 0
#endif

/* We are building a kernel version of the C library, then some user-space features
 * need to be disabled
 */

#if defined(CONFIG_NUTTX_KERNEL) && defined(__KERNEL__)
# undef CONFIG_STDIO_BUFFER_SIZE
# define CONFIG_STDIO_BUFFER_SIZE 0
# undef CONFIG_NUNGET_CHARS
# define CONFIG_NUNGET_CHARS 0
#endif

/* If no standard C buffered I/O is not supported, then line-oriented buffering
 * cannot be supported.
 */

#if CONFIG_STDIO_BUFFER_SIZE == 0
# undef CONFIG_STDIO_LINEBUFFER
#endif

/* If the maximum message size is zero, then we assume that message queues
 * support should be disabled
 */

#if CONFIG_MQ_MAXMSGSIZE <= 0 && !defined(CONFIG_DISABLE_MQUEUE)
# define CONFIG_DISABLE_MQUEUE 1
#endif

/* If mountpoint support in not included, then no filesystem can be supported */

#ifdef CONFIG_DISABLE_MOUNTPOINT
# undef CONFIG_FS_FAT
# undef CONFIG_FS_ROMFS
#endif

/* Check if any readable and writable filesystem (OR USB storage) is supported */

#undef CONFIG_FS_READABLE
#undef CONFIG_FS_WRITABLE
#if defined(CONFIG_FS_FAT) || defined(CONFIG_FS_ROMFS) || defined(CONFIG_USBMSC) || \
    defined(CONFIG_FS_NXFFS) || defined(CONFIG_APPS_BINDIR)
# define CONFIG_FS_READABLE 1
#endif

#if defined(CONFIG_FS_FAT) || defined(CONFIG_USBMSC) || defined(CONFIG_FS_NXFFS)
# define CONFIG_FS_WRITABLE 1
#endif

/* There can be no network support with no socket descriptors */

#if CONFIG_NSOCKET_DESCRIPTORS <= 0
# undef CONFIG_NET
#endif

/* Conversely, if there is no network support, there is no need for
 * socket descriptors
 */

#ifndef CONFIG_NET
# undef CONFIG_NSOCKET_DESCRIPTORS
# define CONFIG_NSOCKET_DESCRIPTORS 0
#endif

/* Protocol support can only be provided on top of basic network support */

#ifndef CONFIG_NET
# undef CONFIG_NET_TCP
# undef CONFIG_NET_UDP
# undef CONFIG_NET_ICMP
#endif

/* Verbose debug and sub-system debug only make sense if debug is enabled */

#ifndef CONFIG_DEBUG
# undef CONFIG_DEBUG_VERBOSE
# undef CONFIG_DEBUG_SCHED
# undef CONFIG_DEBUG_MM
# undef CONFIG_DEBUG_PAGING
# undef CONFIG_DEBUG_DMA
# undef CONFIG_DEBUG_FS
# undef CONFIG_DEBUG_LIB
# undef CONFIG_DEBUG_BINFMT
# undef CONFIG_DEBUG_NET
# undef CONFIG_DEBUG_USB
# undef CONFIG_DEBUG_GRAPHICS
# undef CONFIG_DEBUG_GPIO
# undef CONFIG_DEBUG_STACK
#endif

#endif /* __INCLUDE_NUTTX_CONFIG_H */