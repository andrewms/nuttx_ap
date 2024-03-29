Board-Specific Configurations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Table of Contents
^^^^^^^^^^^^^^^^^

  o Board-Specific Configurations
  o Summary of Files
  o Supported Architectures
  o Configuring NuttX

Board-Specific Configurations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The NuttX configuration consists of:

o Processor architecture specific files.  These are the files contained
  in the arch/<arch-name>/ directory.

o Chip/SoC specific files.  Each processor processor architecture
  is embedded in chip or System-on-a-Chip (SoC) architecture.  The
  full chip architecture includes the processor architecture plus
  chip-specific interrupt logic, general purpose I/O (GIO) logic, and
  specialized, internal peripherals (such as UARTs, USB, etc.).

  These chip-specific files are contained within chip-specific
  sub-directories in the arch/<arch-name>/ directory and are selected
  via the CONFIG_ARCH_name selection

o Board specific files.  In order to be usable, the chip must be
  contained in a board environment.  The board configuration defines
  additional properties of the board including such things as
  peripheral LEDs, external peripherals (such as network, USB, etc.).

  These board-specific configuration files can be found in the
  configs/<board-name>/ sub-directories and are discussed in this
  README.  Additional configuration information maybe available in
  board-specific configs/<board-name>/README.txt files.

The configs/ subdirectory contains configuration data for each board.  These
board-specific configurations plus the architecture-specific configurations in
the arch/ subdirectory completely define a customized port of NuttX.

Directory Structure
^^^^^^^^^^^^^^^^^^^

The configs directory contains board specific configurationlogic.  Each
board must provide a subdirectory <board-name> under configs/ with the
following characteristics:


  <board-name>
  |-- README.txt
  |-- include/
  |   `-- (board-specific header files)
  |-- src/
  |   |-- Makefile
  |   `-- (board-specific source files)
  |-- <config1-dir>
  |   |-- Make.defs
  |   |-- defconfig
  |   |-- appconfig*
  |   `-- setenv.sh
  |-- <config2-dir>
  |   |-- Make.defs
  |   |-- defconfig
  |   |-- appconfig*
  |   `-- setenv.sh
  ...

  *optional

Summary of Files
^^^^^^^^^^^^^^^^

README.txt -- This text file provides additional information unique to
  each board configuration sub-directory.

include/ -- This directory contains board specific header files.  This
  directory will be linked as include/arch/board at configuration time and
  can be included via '#include <arch/board/header.h>'.  These header file
  can only be included by files in arch/<arch-name>include/ and
  arch/<arch-name>/src

src/ -- This directory contains board specific drivers.  This
  directory will be linked as arch/<arch-name>/src/board at configuration
  time and will be integrated into the build system.

src/Makefile -- This makefile will be invoked to build the board specific
  drivers.  It must support the following targets:  libext$(LIBEXT), clean,
  and distclean.

A board may have various different configurations using these common source
files.  Each board configuration is described by three files:  Make.defs,
defconfig, and setenv.sh.  Typically, each set of configuration files is
retained in a separate configuration sub-directory (<config1-dir>,
<config2-dir>, .. in the above diagram).

Make.defs -- This makefile fragment provides architecture and
  tool-specific build options.  It will be included by all other
  makefiles in the build (once it is installed).  This make fragment
  should define:

    Tools: CC, LD, AR, NM, OBJCOPY, OBJDUMP
    Tool options: CFLAGS, LDFLAGS
    COMPILE, ASSEMBLE, ARCHIVE, CLEAN, and MKDEP macros

  When this makefile fragment runs, it will be passed TOPDIR which
  is the path to the root directory of the build.  This makefile
  fragment may include ${TOPDIR}/.config to perform configuration
  specific settings.  For example, the CFLAGS will most likely be
  different if CONFIG_DEBUG=y.

defconfig -- This is a configuration file similar to the Linux
  configuration file.  In contains variable/value pairs like:

  CONFIG_VARIABLE=value

  This configuration file will be used at build time:

    (1) as a makefile fragment included in other makefiles, and
    (2) to generate include/nuttx/config.h which is included by
        most C files in the system.

  The following variables are recognized by the build (you may
  also include architecture/board-specific settings).

  Architecture selection:

    CONFIG_ARCH - Identifies the arch/ subdirectory
    CONFIG_ARCH_name - For use in C code
    CONFIG_ARCH_CHIP - Identifies the arch/*/chip subdirectory
    CONFIG_ARCH_CHIP_name - For use in C code
    CONFIG_ARCH_BOARD - Identifies the configs subdirectory and
       hence, the board that supports the particular chip or SoC.
    CONFIG_ARCH_BOARD_name - For use in C code
    CONFIG_ENDIAN_BIG - define if big endian (default is little
       endian)
    CONFIG_ARCH_NOINTC - define if the architecture does not
      support an interrupt controller or otherwise cannot support
      APIs like up_enable_irq() and up_disable_irq().
    CONFIG_ARCH_VECNOTIRQ - Usually the interrupt vector number provided
      to interfaces like irq_attach() and irq_detach are the same as IRQ
      numbers that are provied to IRQ management functions like
      up_enable_irq() and up_disable_irq().  But that is not true for all
      interrupt controller implementations.  For example, the PIC32MX
      interrupt controller manages interrupt sources that have a many-to-one
      relationship to interrupt vectors. In such cases, CONFIG_ARCH_VECNOTIRQ
      must defined so that the OS logic will know not to assume it can use
      a vector number to enable or disable interrupts.
    CONFIG_ARCH_IRQPRIO
      Define if the architecture suports prioritizaton of interrupts
      and the up_prioritize_irq() API.

  Some architectures require a description of the RAM configuration:

    CONFIG_DRAM_SIZE - Describes the installed DRAM.
    CONFIG_DRAM_START - The start address of DRAM (physical)
    CONFIG_DRAM_VSTART - The start address of DRAM (virtual)

  General build options:

    CONFIG_RRLOAD_BINARY - make the rrload binary format used with
      BSPs from www.ridgerun.com using the tools/mkimage.sh script.
    CONFIG_INTELHEX_BINARY - make the Intel HEX binary format
      used with many different loaders using the GNU objcopy program
      Should not be selected if you are not using the GNU toolchain.
    CONFIG_MOTOROLA_SREC - make the Motorola S-Record binary format
      used with many different loaders using the GNU objcopy program
      Should not be selected if you are not using the GNU toolchain.
    CONFIG_RAW_BINARY - make a raw binary format file used with many
      different loaders using the GNU objcopy program.  This option
      should not be selected if you are not using the GNU toolchain.
    CONFIG_HAVE_LIBM - toolchain supports libm.a
    CONFIG_HAVE_CXX - toolchain supports C++ and CXX, CXXFLAGS, and
      COMPILEXX have been defined in the configurations Make.defs
      file.
    CONFIG_HAVE_CXXINITIALIZE - The platform-specific logic includes support
      for initialization of static C++ instances for this architecture
      and for the selected toolchain (via up_cxxinitialize()).

  Building application code:

    CONFIG_APPS_DIR - Identifies the directory that builds the
      application to link with NuttX.  Default: ../apps This symbol must be assigned
      to the path to the application build directory *relative* to
      the NuttX top build direcory. If you had an application
      directory and the NuttX directory each in separate directory
      trees like this:

      build
       |-nuttx
       |  |
       |  `- Makefile
       `-application
       |
       `- Makefile

      Then you would set CONFIG_APPS_DIR=../application.

      The application direction must contain Makefile and this make
      file must support the following targets:

      - libapps$(LIBEXT) (usually libapps.a). libapps.a is a static
      library ( an archive) that contains all of application object
      files.
      - clean. Do whatever is appropriate to clean the application
      directories for a fresh build.
      - distclean. Clean everthing -- auto-generated files, symbolic
      links etc. -- so that the directory contents are the same as
      the contents in your configuration management system.
      This is only done when you change the NuttX configuration.
      - depend. Make or update the application build dependencies.

      When this application is invoked it will receive the setting TOPDIR like:

      $(MAKE) -C $(CONFIG_APPS_DIR) TOPDIR="$(TOPDIR)" <target>

      TOPDIR is the full path to the NuttX directory. It can be used, for
      example, to include makefile fragments (e.g., .config or Make.defs)
      or to set up include file paths.

  Two-pass build options.  If the 2 pass build option is selected, then these
  options configure the make system build a extra link object. This link object
  is assumed to be an incremental (relative) link object, but could be a static
  library (archive) (some modification to this Makefile would be required if
  CONFIG_PASS1_TARGET generates an archive). Pass 1 1ncremental (relative) link
  objects should be put into the processor-specific source directory (where other
  link objects will be created).  If the pass1 obect is an archive, it could
  go anywhere.

    CONFIG_BUILD_2PASS - Enables the two pass build options.

  When the two pass build option is enabled, the following also apply:

    CONFIG_PASS1_TARGET - The name of the first pass build target.  This
      can be specific build target, a special build target (all, default, etc.)
      or may just be left undefined.
    CONFIG_PASS1_BUILDIR - The path, relative to the top NuttX build
      directory to directory that contains the Makefile to build the
      first pass object.  The Makefile must support the following targets:
      - The special target CONFIG_PASS1_TARGET (if defined)
      - and the usual depend, clean, and distclean targets.
    CONFIG_PASS1_OBJECT - May be used to include an extra, pass1 object
      into the final link.  This would probably be the object generated
      from the CONFIG_PASS1_TARGET.  It may be available at link time
      in the arch/<architecture>/src directory.

  General OS setup

    CONFIG_DEBUG - enables built-in debug options
    CONFIG_DEBUG_VERBOSE - enables verbose debug output
    CCONFIG_DEBUG_ENABLE - Support an interface to enable or disable debug output.
    CONFIG_DEBUG_SYMBOLS - build without optimization and with
      debug symbols (needed for use with a debugger).
    CONFIG_DEBUG_SCHED - enable OS debug output (disabled by
      default)
    CONFIG_DEBUG_MM - enable memory management debug output
      (disabled by default)
    CONFIG_DEBUG_NET - enable network debug output (disabled
      by default)
    CONFIG_DEBUG_USB - enable usb debug output (disabled by
      default)
    CONFIG_DEBUG_FS - enable filesystem debug output (disabled
      by default)
    CONFIG_DEBUG_LIB - enable C library debug output (disabled
      by default)
    CONFIG_DEBUG_BINFMT - enable binary loader debug output (disabled
      by default)
    CONFIG_DEBUG_GRAPHICS - enable NX graphics debug output
      (disabled by default)
    CONFIG_ARCH_LOWPUTC - architecture supports low-level, boot
      time console output
    CONFIG_MM_REGIONS - If the architecture includes multiple
      regions of memory to allocate from, this specifies the
      number of memory regions that the memory manager must
      handle and enables the API mm_addregion(start, end);
    CONFIG_MM_SMALL - Each memory allocation has a small allocation
      overhead.  The size of that overhead is normally determined by
      the "width" of the address support by the MCU.  MCUs that support
      16-bit addressability have smaller overhead than devices that
      support 32-bit addressability.  However, there are many MCUs
      that support 32-bit addressability *but* have internal SRAM
      of size less than or equal to 64Kb.  In this case, CONFIG_MM_SMALL
      can be defined so that those MCUs will also benefit from the
      smaller, 16-bit-based allocation overhead.
    CONFIG_MSEC_PER_TICK - The default system timer is 100Hz
      or MSEC_PER_TICK=10.  This setting may be defined to
      inform NuttX that the processor hardware is providing
      system timer interrupts at some interrupt interval other
      than 10 msec.
    CONFIG_RR_INTERVAL - The round robin timeslice will be set
      this number of milliseconds;  Round robin scheduling can
      be disabled by setting this value to zero.
    CONFIG_SCHED_INSTRUMENTATION - enables instrumentation in 
      scheduler to monitor system performance
    CONFIG_TASK_NAME_SIZE - Specifies that maximum size of a
      task name to save in the TCB.  Useful if scheduler
      instrumentation is selected.  Set to zero to disable.
    CONFIG_START_YEAR, CONFIG_START_MONTH, CONFIG_START_DAY -
      Used to initialize the internal time logic.
    CONFIG_GREGORIAN_TIME - Enables Gregorian time conversions.
      You would only need this if you are concerned about accurate
      time conversions in the past or in the distant future.
    CONFIG_JULIAN_TIME - Enables Julian time conversions. You
      would only need this if you are concerned about accurate
      time conversion in the distand past.  You must also define
      CONFIG_GREGORIAN_TIME in order to use Julian time.
    CONFIG_DEV_CONSOLE - Set if architecture-specific logic
      provides /dev/console.  Enables stdout, stderr, stdin.
      This implies the "normal" serial driver provides the
      console unless another console device is specified
      (See CONFIG_DEV_LOWCONSOLE).
    CONFIG_MUTEX_TYPES - Set to enable support for recursive and
      errorcheck mutexes.  Enables pthread_mutexattr_settype().
    CONFIG_PRIORITY_INHERITANCE - Set to enable support for
      priority inheritance on mutexes and semaphores.
    CONFIG_SEM_PREALLOCHOLDERS: This setting is only used if priority
      inheritance is enabled.  It defines the maximum number of
      different threads (minus one) that can take counts on a
      semaphore with priority inheritance support.  This may be 
      set to zero if priority inheritance is disabled OR if you
      are only using semaphores as mutexes (only one holder) OR
      if no more than two threads participate using a counting
      semaphore.
    CONFIG_SEM_NNESTPRIO.  If priority inheritance is enabled,
      then this setting is the maximum number of higher priority
      threads (minus 1) than can be waiting for another thread
      to release a count on a semaphore.  This value may be set
      to zero if no more than one thread is expected to wait for
      a semaphore.
    CONFIG_FDCLONE_DISABLE. Disable cloning of all file descriptors
      by task_create() when a new task is started.  If set, all
        files/drivers will appear to be closed in the new task.
    CONFIG_FDCLONE_STDIO. Disable cloning of all but the first
      three file descriptors (stdin, stdout, stderr) by task_create()
      when a new task is started. If set, all files/drivers will
      appear to be closed in the new task except for stdin, stdout,
      and stderr.
    CONFIG_SDCLONE_DISABLE. Disable cloning of all socket
      desciptors by task_create() when a new task is started. If
      set, all sockets will appear to be closed in the new task.
    CONFIG_NXFLAT. Enable support for the NXFLAT binary format.
      This format will support execution of NuttX binaries located
      in a ROMFS filesystem (see examples/nxflat).
    CONFIG_SCHED_WORKQUEUE.  Create a dedicated "worker" thread to
      handle delayed processing from interrupt handlers.  This feature
      is required for some drivers but, if there are not complaints,
      can be safely disabled.  The worker thread also performs
      garbage collection -- completing any delayed memory deallocations
      from interrupt handlers.  If the worker thread is disabled,
      then that clean will be performed by the IDLE thread instead
      (which runs at the lowest of priority and may not be appropriate
      if memory reclamation is of high priority).  If CONFIG_SCHED_WORKQUEUE
      is enabled, then the following options can also be used:
    CONFIG_SCHED_WORKPRIORITY - The execution priority of the worker
      thread.  Default: 50
    CONFIG_SCHED_WORKPERIOD - How often the worker thread checks for
      work in units of microseconds.  Default: 50*1000 (50 MS).
    CONFIG_SCHED_WORKSTACKSIZE - The stack size allocated for the worker
      thread.  Default: CONFIG_IDLETHREAD_STACKSIZE.
    CONFIG_SIG_SIGWORK - The signal number that will be used to wake-up
      the worker thread.  Default: 4

  System Logging:
    CONFIG_SYSLOG enables general system logging support.

    At present, the only system loggin device is a circular buffer in RAM.
    If CONFIG_SYSLOG is selected, then these options are also available.

    CONFIG_RAMLOG - Enables the RAM logging feature
    CONFIG_RAMLOG_CONSOLE - Use the RAM logging device as a system console.
      If this feature is enabled (along with CONFIG_DEV_CONSOLE), then all
      console output will be re-directed to a circular buffer in RAM.  This
      is useful, for example, if the only console is a Telnet console.  Then
      in that case, console output from non-Telnet threads will go to the
      circular buffer and can be viewed using the NSH 'dmesg' command.
    CONFIG_RAMLOG_SYSLOG - Use the RAM logging device for the syslogging
      interface.  If this feature is enabled (along with CONFIG_SYSLOG),
      then all debug output (only) will be re-directed to the circular
      buffer in RAM.  This RAM log can be view from NSH using the 'dmesg'
      command.
    CONFIG_RAMLOG_NPOLLWAITERS - The number of threads than can be waiting
     for this driver on poll().  Default: 4

    If CONFIG_RAMLOG_CONSOLE or CONFIG_RAMLOG_SYSLOG is selected, then the
    following may also be provided:

    CONFIG_RAMLOG_CONSOLE_BUFSIZE - Size of the console RAM log.  Default: 1024

  Kernel build options:
    CONFIG_NUTTX_KERNEL - Builds NuttX as a separately compiled kernel.
    CONFIG_SYS_RESERVED - Reserved system call values for use
      by architecture-specific logic.

  OS setup related to on-demand paging:

    CONFIG_PAGING - If set =y in your configation file, this setting will
      enable the on-demand paging feature as described in
      http://www.nuttx.org/NuttXDemandPaging.html.

  If CONFIG_PAGING is selected, then you will probabaly need CONFIG_BUILD_2PASS to
  correctly position the code and the following configuration options also apply:

    CONFIG_PAGING_PAGESIZE - The size of one managed page.  This must
      be a value supported by the processor's memory management unit.
    CONFIG_PAGING_NLOCKED - This is the number of locked pages in the
      memory map.  The locked address region will then be from
      CONFIG_DRAM_VSTART through (CONFIG_DRAM_VSTART +
      CONFIG_PAGING_PAGESIZE*CONFIG_PAGING_NLOCKED)
    CONFIG_PAGING_LOCKED_PBASE and CONFIG_PAGING_LOCKED_VBASE - These
      may be defined to determine the base address of the locked page
      regions.  If neither are defined, the logic will be set the bases
      to CONFIG_DRAM_START and CONFIG_DRAM_VSTART (i.e., it assumes
      that the base address of the locked region is at the beginning
      of RAM).
      NOTE:  In some architectures, it may be necessary to take some
      memory from the beginning of this region for vectors or for a
      page table. In such cases, CONFIG_PAGING_LOCKED_P/VBASE should
      take that into consideration to prevent overlapping the locked
      memory region and the system data at the beginning of SRAM.
    CONFIG_PAGING_NPPAGED - This is the number of physical pages
      available to support the paged text region.  This paged region
      begins at (CONFIG_PAGING_LOCKED_PBASE + CONFIG_PAGING_PAGESIZE*CONFIG_PAGING_NPPAGED)
      and continues until (CONFIG_PAGING_LOCKED_PBASE + CONFIG_PAGING_PAGESIZE*(CONFIG_PAGING_NLOCKED +
      CONFIG_PAGING_NPPAGED)
    CONFIG_PAGING_NVPAGED - This actual size of the paged text region
      (in pages).  This is also the number of virtual pages required to
      support the entire paged region. The on-demand paging feature is
      intended to support only the case where the virtual paged text
      area is much larger the available physical pages.  Otherwise, why
      would you enable on-demand paging?
    CONFIG_PAGING_NDATA - This is the number of data pages in the memory
      map.  The data region will extend to the end of RAM unless overridden
      by a setting in the configuration file.
      NOTE:  In some architectures, it may be necessary to take some memory
      from the end of RAM for page tables or other system usage.  The
      configuration settings and linker directives must be cognizant of that:
      CONFIG_PAGING_NDATA should be defined to prevent the data region from
      extending all the way to the end of memory. 
    CONFIG_PAGING_DEFPRIO - The default, minimum priority of the page fill
      worker thread.  The priority of the page fill work thread will be boosted
      boosted dynmically so that it matches the priority of the task on behalf
      of which it peforms the fill.  This defines the minimum priority that
      will be used. Default: 50.
    CONFIG_PAGING_STACKSIZE - Defines the size of the allocated stack
      for the page fill worker thread. Default: 1024.
    CONFIG_PAGING_BLOCKINGFILL - The architecture specific up_fillpage()
      function may be blocking or non-blocking.  If defined, this setting
      indicates that the up_fillpage() implementation will block until the
      transfer is completed. Default:  Undefined (non-blocking).
    CONFIG_PAGING_WORKPERIOD - The page fill worker thread will wake periodically
      even if there is no mapping to do.  This selection controls that wake-up
      period (in microseconds).  This wake-up a failsafe that will handle any 
      cases where a single is lost (that would really be a bug and shouldn't
      happen!) and also supports timeouts for case of non-blocking, asynchronous
      fills (see CONFIG_PAGING_TIMEOUT_TICKS).
    CONFIG_PAGING_TIMEOUT_TICKS - If defined, the implementation will monitor
      the (asynchronous) page fill logic.  If the fill takes longer than this
      number if microseconds, then a fatal error will be declared.
      Default: No timeouts monitored.

    Some architecture-specific settings.  Defaults are architecture specific.
    If you don't know what you are doing, it is best to leave these undefined
    and try the system defaults:
 
    CONFIG_PAGING_VECPPAGE - This the physical address of the page in
      memory to be mapped to the vector address.
    CONFIG_PAGING_VECL2PADDR - This is the physical address of the L2
      page table entry to use for the vector mapping.
    CONFIG_PAGING_VECL2VADDR - This is the virtual address of the L2
      page table entry to use for the vector mapping.
    CONFIG_PAGING_BINPATH - If CONFIG_PAGING_BINPATH is defined, then it
      is the full path to a file on a mounted file system that contains
      a binary image of the NuttX executable.  Pages will be filled by
      reading from offsets into this file that correspond to virtual
      fault addresses.
    CONFIG_PAGING_MOUNTPT - If CONFIG_PAGING_BINPATH is defined, additional
      options may be provided to control the initialization of underlying
      devices. CONFIG_PAGING_MOUNTPT identifies the mountpoint to be used
      if a device is mounted.
    CONFIG_PAGING_MINOR - Some mount operations require a "minor" number
      to identify the specific device instance. Default: 0
    CONFIG_PAGING_SDSLOT - If CONFIG_PAGING_BINPATH is defined, additional
      options may be provided to control the initialization of underlying
      devices. CONFIG_PAGING_SDSLOT identifies the slot number of the SD
      device to initialize. This must be undefined if SD is not being used.
      This should be defined to be zero for the typical device that has
      only a single slot (See CONFIG_MMCSD_NSLOTS). If defined, 
      CONFIG_PAGING_SDSLOT will instruct certain board-specific logic to
      initialize the media in this SD slot.
    CONFIG_PAGING_M25PX - Use the m25px.c FLASH driver.  If this is selected,
      then the MTD interface to the M25Px device will be used to support
      paging.
    CONFIG_PAGING_AT45DB - Use the at45db.c FLASH driver.  If this is selected,
      then the MTD interface to the Atmel AT45DB device will be used to support
      paging.
    CONFIG_PAGING_BINOFFSET - If CONFIG_PAGING_M25PX or is CONFIG_PAGING_AT45DB
      defined then CONFIG_PAGING_BINOFFSET will be used to specify the offset
      in bytes into the FLASH device where the NuttX binary image is located.
      Default: 0
    CONFIG_PAGING_SPIPORT - If CONFIG_PAGING_M25PX CONFIG_PAGING_AT45DB is
       defined and the device has multiple SPI busses (ports), then this
      configuration should be set to indicate which SPI port the device is
      connected. Default: 0

  The following can be used to disable categories of APIs supported
  by the OS.  If the compiler supports weak functions, then it
  should not be necessary to disable functions unless you want to
  restrict usage of those APIs.

  There are certain dependency relationships in these features.

    o mq_notify logic depends on signals to awaken tasks
      waiting for queues to become full or empty.
    o pthread_condtimedwait() depends on signals to wake
      up waiting tasks.

    CONFIG_DISABLE_CLOCK, CONFIG_DISABLE_POSIX_TIMERS, CONFIG_DISABLE_PTHREAD.
    CONFIG_DISABLE_SIGNALS, CONFIG_DISABLE_MQUEUE, CONFIG_DISABLE_MOUNTPOUNT,
    CONFIG_DISABLE_ENVIRON, CONFIG_DISABLE_POLL

  Misc libc settings

    CONFIG_NOPRINTF_FIELDWIDTH - sprintf-related logic is a
       little smaller if we do not support fieldwidthes
    CONFIG_LIBC_FLOATINGPOINT - By default, floating point
      support in printf, sscanf, etc. is disabled.

  Allow for architecture optimized implementations

    The architecture can provide optimized versions of the
    following to improve system performance

      CONFIG_ARCH_MEMCPY, CONFIG_ARCH_MEMCMP, CONFIG_ARCH_MEMMOVE
      CONFIG_ARCH_MEMSET, CONFIG_ARCH_STRCMP, CONFIG_ARCH_STRCPY
      CONFIG_ARCH_STRNCPY, CONFIG_ARCH_STRLEN, CONFIG_ARCH_STRNLEN
      CONFIG_ARCH_BZERO

    The architecture may provide custom versions of certain
    standard header files:

      CONFIG_ARCH_MATH_H, CONFIG_ARCH_STDBOOL_H, CONFIG_ARCH_STDINT_H

    CONFIG_ARCH_ROMGETC - In Harvard architectures, data accesses and
      instruction accesses occur on different busses, perhaps
      concurrently.  All data accesses are performed on the data bus
      unless special machine instructions are used to read data
      from the instruction address space.  Also, in the typical
      MCU, the available SRAM data memory is much smaller that the
      non-volatile FLASH instruction memory.  So if the application
      requires many constant strings, the only practical solution may
      be to store those constant strings in FLASH memory where they
      can only be accessed using architecture-specific machine
      instructions.

      If CONFIG_ARCH_ROMGETC is defined, then the architecture logic
      must export the function up_romgetc().  up_romgetc() will simply
      read one byte of data from the instruction space.
 
      If CONFIG_ARCH_ROMGETC, certain C stdio functions are effected:
      (1) All format strings in printf, fprintf, sprintf, etc. are
      assumed to lie in FLASH (string arguments for %s are still assumed
      to reside in SRAM). And (2), the string argument to puts and fputs
      is assumed to reside in FLASH.  Clearly, these assumptions may have
      to modified for the particular needs of your environment.  There
      is no "one-size-fits-all" solution for this problem.

  Sizes of configurable things (0 disables)

    CONFIG_MAX_TASKS - The maximum number of simultaneously
      active tasks.  This value must be a power of two.
    CONFIG_NPTHREAD_KEYS - The number of items of thread-
      specific data that can be retained
    CONFIG_NFILE_DESCRIPTORS - The maximum number of file
      descriptors (one for each open)
    CONFIG_NFILE_STREAMS - The maximum number of streams that
      can be fopen'ed
    CONFIG_NAME_MAX - The maximum size of a file name.
    CONFIG_STDIO_BUFFER_SIZE - Size of the buffer to allocate
      on fopen. (Only if CONFIG_NFILE_STREAMS > 0)
    CONFIG_STDIO_LINEBUFFER - If standard C buffered I/O is enabled
      (CONFIG_STDIO_BUFFER_SIZE > 0), then this option may be added
      to force automatic, line-oriented flushing the output buffer
      for putc(), fputc(), putchar(), puts(), fputs(), printf(),
      fprintf(), and vfprintf().  When a newline is encountered in
      the output string, the output buffer will be flushed.  This
      (slightly) increases the NuttX footprint but supports the kind
      of behavior that people expect for printf().
    CONFIG_NUNGET_CHARS - Number of characters that can be
      buffered by ungetc() (Only if CONFIG_NFILE_STREAMS > 0)
    CONFIG_PREALLOC_MQ_MSGS - The number of pre-allocated message
      structures.  The system manages a pool of preallocated
      message structures to minimize dynamic allocations
    CONFIG_PREALLOC_IGMPGROUPS - Pre-allocated IGMP groups are used
      only if needed from interrupt level group created (by the IGMP server).
      Default: 4.
    CONFIG_MQ_MAXMSGSIZE - Message structures are allocated with
      a fixed payload size given by this settin (does not include
      other message structure overhead.
    CONFIG_PREALLOC_WDOGS - The number of pre-allocated watchdog
      structures.  The system manages a pool of preallocated
      watchdog structures to minimize dynamic allocations
    CONFIG_DEV_PIPE_SIZE - Size, in bytes, of the buffer to allocated
      for pipe and FIFO support

  Filesystem configuration

    CONFIG_FS_FAT - Enable FAT filesystem support
    CONFIG_FAT_SECTORSIZE - Max supported sector size
    CONFIG_FAT_LCNAMES - Enable use of the NT-style upper/lower case 8.3
      file name support.
    CONFIG_FAT_LFN - Enable FAT long file names.  NOTE:  Microsoft claims
      patents on FAT long file name technology.  Please read the
      disclaimer in the top-level COPYING file and only enable this
      feature if you understand these issues.
    CONFIG_FAT_MAXFNAME - If CONFIG_FAT_LFN is defined, then the
      default, maximum long file name is 255 bytes.  This can eat up
      a lot of memory (especially stack space).  If you are willing
      to live with some non-standard, short long file names, then
      define this value.  A good choice would be the same value as
      selected for CONFIG_NAME_MAX which will limit the visibility
      of longer file names anyway.
    CONFIG_FS_FATTIME: Support FAT date and time. NOTE:  There is not
      much sense in supporting FAT date and time unless you have a
      hardware RTC or other way to get the time and date.
    CONFIG_FS_NXFFS: Enable NuttX FLASH file system (NXFF) support.
    CONFIG_NXFFS_ERASEDSTATE: The erased state of FLASH. 
      This must have one of the values of 0xff or 0x00.
      Default: 0xff.
    CONFIG_NXFFS_PACKTHRESHOLD: When packing flash file data,
      don't both with file chunks smaller than this number of data bytes.
      Default: 32.
    CONFIG_NXFFS_MAXNAMLEN: The maximum size of an NXFFS file name.
      Default: 255.
    CONFIG_NXFFS_PACKTHRESHOLD: When packing flash file data,
      don't both with file chunks smaller than this number of data bytes.
      Default: 32.
    CONFIG_NXFFS_TAILTHRESHOLD: clean-up can either mean
      packing files together toward the end of the file or, if file are
      deleted at the end of the file, clean up can simply mean erasing
      the end of FLASH memory so that it can be re-used again.  However,
      doing this can also harm the life of the FLASH part because it can
      mean that the tail end of the FLASH is re-used too often. This
      threshold determines if/when it is worth erased the tail end of FLASH
      and making it available for re-use (and possible over-wear).
      Default: 8192.
    CONFIG_FS_ROMFS - Enable ROMFS filesystem support
    CONFIG_FS_RAMMAP - For file systems that do not support XIP, this
      option will enable a limited form of memory mapping that is
      implemented by copying whole files into memory.

  RTC

    CONFIG_RTC - Enables general support for a hardware RTC.  Specific
      architectures may require other specific settings.
    CONFIG_RTC_DATETIME - There are two general types of RTC:  (1) A simple
      battery backed counter that keeps the time when power is down, and (2)
       A full date / time RTC the provides the date and time information, often
      in BCD format.  If CONFIG_RTC_DATETIME is selected, it specifies this
      second kind of RTC. In this case, the RTC is used to "seed" the normal
      NuttX timer and the NuttX system timer provides for higher resoution
      time.
    CONFIG_RTC_HIRES - If CONFIG_RTC_DATETIME not selected, then the simple,
      battery backed counter is used.  There are two different implementations
      of such simple counters based on the time resolution of the counter:
      The typical RTC keeps time to resolution of 1 second, usually
      supporting a 32-bit time_t value.  In this case, the RTC is used to
      "seed" the normal NuttX timer and the NuttX timer provides for higher
      resoution time. If CONFIG_RTC_HIRES is enabled in the NuttX configuration,
      then the RTC provides higher resolution time and completely replaces the
      system timer for purpose of date and time.
    CONFIG_RTC_FREQUENCY - If CONFIG_RTC_HIRES is defined, then the frequency
      of the high resolution RTC must be provided.  If CONFIG_RTC_HIRES is
      not defined, CONFIG_RTC_FREQUENCY is assumed to be one.
    CONFIG_RTC_ALARM - Enable if the RTC hardware supports setting of an
      alarm.  A callback function will be executed when the alarm goes off

  CAN driver

    CONFIG_CAN - Enables CAN support (one or both of CONFIG_STM32_CAN1 or
      CONFIG_STM32_CAN2 must also be defined)
    CONFIG_CAN_EXTID - Enables support for the 29-bit extended ID.  Default
      Standard 11-bit IDs.
    CONFIG_CAN_FIFOSIZE - The size of the circular buffer of CAN messages.
      Default: 8
    CONFIG_CAN_NPENDINGRTR - The size of the list of pending RTR requests.
      Default: 4
    CONFIG_CAN_LOOPBACK - A CAN driver may or may not support a loopback
      mode for testing. If the driver does support loopback mode, the setting
      will enable it. (If the driver does not, this setting will have no effect).

  SPI driver

    CONFIG_SPI_OWNBUS - Set if there is only one active device
      on the SPI bus.  No locking or SPI configuration will be performed.
      It is not necessary for clients to lock, re-configure, etc..
    CONFIG_SPI_EXCHANGE - Driver supports a single exchange method
      (vs a recvblock() and sndblock ()methods)

  SPI-based MMC/SD driver

    CONFIG_MMCSD_NSLOTS - Number of MMC/SD slots supported by the
      driver. Default is one.
    CONFIG_MMCSD_READONLY -  Provide read-only access.  Default is
      Read/Write
    CONFIG_MMCSD_SPICLOCK - Maximum SPI clock to drive MMC/SD card.
      Default is 20MHz.

  SDIO/SDHC driver:

    CONFIG_SDIO_DMA - SDIO driver supports DMA
    CONFIG_SDIO_MUXBUS - Set this SDIO interface if the SDIO interface
      or hardware resources are shared with other drivers.
    CONFIG_SDIO_WIDTH_D1_ONLY - Select 1-bit transfer mode.  Default:
      4-bit transfer mode.
    CONFIG_MMCSD_MULTIBLOCK_DISABLE - Use only the single block transfer method.
      This setting is used to work around buggy SDIO drivers that cannot handle
      multiple block transfers.

  SDIO-based MMC/SD driver

    CONFIG_FS_READAHEAD - Enable read-ahead buffering
    CONFIG_FS_WRITEBUFFER - Enable write buffering
    CONFIG_MMCSD_MMCSUPPORT - Enable support for MMC cards
    CONFIG_MMCSD_HAVECARDDETECT - SDIO driver card detection is
      100% accurate

  RiT P14201 OLED driver

    CONFIG_LCD_P14201 - Enable P14201 support
    CONFIG_P14201_SPIMODE - Controls the SPI mode
    CONFIG_P14201_FREQUENCY - Define to use a different bus frequency
    CONFIG_P14201_NINTERFACES - Specifies the number of physical P14201
      devices that will be supported.
    CONFIG_P14201_FRAMEBUFFER - If defined, accesses will be performed
      using an in-memory copy of the OLEDs GDDRAM.  This cost of this
      buffer is 128 * 96 / 2 = 6Kb.  If this is defined, then the driver
      will be fully functional. If not, then it will have the following
      limitations:
      - Reading graphics memory cannot be supported, and
      - All pixel writes must be aligned to byte boundaries.
      The latter limitation effectively reduces the 128x96 disply to 64x96.

  Nokia 6100 Configuration Settings:

    CONFIG_NOKIA6100_SPIMODE - Controls the SPI mode
    CONFIG_NOKIA6100_FREQUENCY - Define to use a different bus frequency
    CONFIG_NOKIA6100_NINTERFACES - Specifies the number of physical Nokia
      6100 devices that will be supported.
    CONFIG_NOKIA6100_BPP - Device supports 8, 12, and 16 bits per pixel.
    CONFIG_NOKIA6100_S1D15G10 - Selects the Epson S1D15G10 display controller
    CONFIG_NOKIA6100_PCF8833 - Selects the Phillips PCF8833 display controller
    CONFIG_NOKIA6100_BLINIT - Initial backlight setting

    The following may need to be tuned for your hardware:
    CONFIG_NOKIA6100_INVERT - Display inversion, 0 or 1, Default: 1
    CONFIG_NOKIA6100_MY - Display row direction, 0 or 1, Default: 0
    CONFIG_NOKIA6100_MX - Display column direction, 0 or 1, Default: 1
    CONFIG_NOKIA6100_V - Display address direction, 0 or 1, Default: 0
    CONFIG_NOKIA6100_ML - Display scan direction, 0 or 1, Default: 0
    CONFIG_NOKIA6100_RGBORD - Display RGB order, 0 or 1, Default: 0

    Required LCD driver settings:
    CONFIG_LCD_NOKIA6100 - Enable Nokia 6100 support
    CONFIG_LCD_MAXCONTRAST - must be 63 with the Epson controller and 127 with
      the Phillips controller.
    CONFIG_LCD_MAXPOWER - Maximum value of backlight setting.  The backlight
      control is managed outside of the 6100 driver so this value has no
      meaning to the driver.  Board-specific logic may place restrictions on
      this value.

  Input Devices

    CONFIG_INPUT
      Enables general support for input devices
    CONFIG_INPUT_TSC2007
      If CONFIG_INPUT is selected, then this setting will enable building
      of the TI TSC2007 touchscreen driver.
    CONFIG_TSC2007_MULTIPLE
      Normally only a single TI TSC2007 touchscreen is used.  But if
      there are multiple TSC2007 touchscreens, this setting will enable
      multiple touchscreens with the same driver.

  Analog Devices

    CONFIG_DAC
      Enables general support for Digital-to-Analog conversion devices.
    CONFIG_ADC
      Enables general support for Analog-to-Digital conversion devices.
    CONFIG_ADC_ADS125X
      Adds support for the TI ADS 125x ADC.

  ENC28J60 Ethernet Driver Configuration Settings:

    CONFIG_NET_ENC28J60 - Enabled ENC28J60 support
    CONFIG_ENC28J60_SPIMODE - Controls the SPI mode
    CONFIG_ENC28J60_FREQUENCY - Define to use a different bus frequency
    CONFIG_ENC28J60_NINTERFACES - Specifies the number of physical ENC28J60
      devices that will be supported.
    CONFIG_ENC28J60_STATS - Collect network statistics
    CONFIG_ENC28J60_HALFDUPPLEX - Default is full duplex

  Networking support via uIP

    CONFIG_NET - Enable or disable all network features
    CONFIG_NET_NOINTS --  CONFIG_NET_NOINT indicates that uIP not called from
      the interrupt level.  If CONFIG_NET_NOINTS is defined, critical sections
      will be managed with semaphores; Otherwise, it assumed that uIP will be
      called from interrupt level handling and critical sections will be
      managed by enabling and disabling interrupts.
    CONFIG_NET_MULTIBUFFER - Traditionally, uIP has used a single buffer
      for all incoming and outgoing traffic.  If this configuration is
      selected, then the driver can manage multiple I/O buffers and can,
      for example, be filling one input buffer while sending another
      output buffer.  Or, as another example, the driver may support
      queuing of concurrent input/ouput and output transfers for better
      performance.
    CONFIG_NET_IPv6 - Build in support for IPv6
    CONFIG_NSOCKET_DESCRIPTORS - Maximum number of socket descriptors
    per task/thread.
    CONFIG_NET_NACTIVESOCKETS - Maximum number of concurrent socket
      operations (recv, send, etc.).  Default: CONFIG_NET_TCP_CONNS+CONFIG_NET_UDP_CONNS
    CONFIG_NET_SOCKOPTS - Enable or disable support for socket options

    CONFIG_NET_BUFSIZE - uIP buffer size
    CONFIG_NET_TCPURGDATA - Determines if support for TCP urgent data
      notification should be compiled in. Urgent data (out-of-band data)
      is a rarely used TCP feature that is very seldom would be required.
    CONFIG_NET_TCP - TCP support on or off
    CONFIG_NET_TCP_CONNS - Maximum number of TCP connections (all tasks)
    CONFIG_NET_MAX_LISTENPORTS - Maximum number of listening TCP ports (all tasks)
    CONFIG_NET_TCP_READAHEAD_BUFSIZE - Size of TCP read-ahead buffers
    CONFIG_NET_NTCP_READAHEAD_BUFFERS - Number of TCP read-ahead buffers
      (may be zero)
    CONFIG_NET_TCPBACKLOG - Incoming connections pend in a backlog until
      accept() is called. The size of the backlog is selected when listen()
      is called.
    CONFIG_NET_UDP - UDP support on or off
    CONFIG_NET_UDP_CHECKSUMS - UDP checksums on or off
    CONFIG_NET_UDP_CONNS - The maximum amount of concurrent UDP
      connections
    CONFIG_NET_ICMP - Enable minimal ICMP support. Includes built-in support
      for sending replies to received ECHO (ping) requests.
    CONFIG_NET_ICMP_PING - Provide interfaces to support application level
      support for sending ECHO (ping) requests and associating ECHO
      replies.
    CONFIG_NET_IGMP - Enable IGMPv2 client support.
    CONFIG_PREALLOC_IGMPGROUPS - Pre-allocated IGMP groups are used
      only if needed from interrupt level group created (by the IGMP server).
      Default: 4.
    CONFIG_NET_PINGADDRCONF - Use "ping" packet for setting IP address
    CONFIG_NET_STATISTICS - uIP statistics on or off
    CONFIG_NET_RECEIVE_WINDOW - The size of the advertised receiver's
      window
    CONFIG_NET_ARPTAB_SIZE - The size of the ARP table
    CONFIG_NET_ARP_IPIN - Harvest IP/MAC address mappings from the ARP table
      from incoming IP packets.
    CONFIG_NET_BROADCAST - Incoming UDP broadcast support
    CONFIG_NET_MULTICAST - Outgoing multi-cast address support
    CONFIG_NET_FWCACHE_SIZE - number of packets to remember when
      looking for duplicates

  SLIP Driver.  SLIP supports point-to-point IP communications over a serial
    port.  The default data link layer for uIP is Ethernet. If CONFIG_NET_SLIP
    is defined in the NuttX configuration file, then SLIP will be supported.
    The basic differences between the SLIP and Ethernet configurations is that
    when SLIP is selected: 

    * The link level header (that comes before the IP header) is omitted. 
    * All MAC address processing is suppressed. 
    * ARP is disabled.

    If CONFIG_NET_SLIP is not selected, then Ethernet will be used (there is
    no need to define anything special in the configuration file to use
    Ethernet -- it is the default). 

    CONFIG_NET_SLIP -- Enables building of the SLIP driver. SLIP requires
      at least one IP protocols selected and the following additional
      network settings: CONFIG_NET_NOINTS and CONFIG_NET_MULTIBUFFER.
      CONFIG_NET_BUFSIZE *must* be set to 296.  Other optional configuration
      settings that affect the SLIP driver: CONFIG_NET_STATISTICS. 
      Default: Ethernet

    If SLIP is selected, then the following SLIP options are available:

    CONFIG_CLIP_NINTERFACES -- Selects the number of physical SLIP
      interfaces to support.  Default: 1
    CONFIG_SLIP_STACKSIZE -- Select the stack size of the SLIP RX and
      TX tasks.  Default: 2048
    CONFIG_SLIP_DEFPRIO - The priority of the SLIP RX and TX tasks.
      Default: 128

  UIP Network Utilities

    CONFIG_NET_DHCP_LIGHT - Reduces size of DHCP
    CONFIG_NET_RESOLV_ENTRIES - Number of resolver entries

  THTTPD

    CONFIG_THTTPD_PORT - THTTPD Server port number
    CONFIG_THTTPD_IPADDR - Server IP address (no host name)
    CONFIG_THTTPD_SERVER_ADDRESS - SERVER_ADDRESS: response
    CONFIG_THTTPD_SERVER_SOFTWARE - SERVER_SOFTWARE: response
    CONFIG_THTTPD_PATH - Server working directory
    CONFIG_THTTPD_CGI_PATH - Path to CGI executables
    CONFIG_THTTPD_CGI_PATTERN - Only CGI programs matching this
      pattern will be executed.  In fact, if this value is not defined
      then no CGI logic will be built.
    CONFIG_THTTPD_CGI_PRIORITY - Provides the priority of CGI child tasks
    CONFIG_THTTPD_CGI_STACKSIZE - Provides the initial stack size of
      CGI child task (will be overridden by the stack size in the NXFLAT
      header)
    CONFIG_THTTPD_CGI_BYTECOUNT - Byte output limit for CGI tasks.
    CONFIG_THTTPD_CGI_TIMELIMIT - How many seconds to allow CGI programs
      to run before killing them.
    CONFIG_THTTPD_CHARSET- The default character set name to use with
      text MIME types.
    CONFIG_THTTPD_IOBUFFERSIZE - 
    CONFIG_THTTPD_INDEX_NAMES - A list of index filenames to check. The
      files are searched for in this order.
    CONFIG_AUTH_FILE - The file to use for authentication. If this is
      defined then thttpd checks for this file in the local directory
      before every fetch. If the file exists then authentication is done,
      otherwise the fetch proceeds as usual. If you leave this undefined
      then thttpd will not implement authentication at all and will not
      check for auth files, which saves a bit of CPU time. A typical
      value is ".htpasswd"
    CONFIG_THTTPD_LISTEN_BACKLOG - The listen() backlog queue length.
    CONFIG_THTTPD_LINGER_MSEC - How many milliseconds to leave a connection
      open while doing a lingering close.
    CONFIG_THTTPD_OCCASIONAL_MSEC - How often to run the occasional
      cleanup job.
    CONFIG_THTTPD_IDLE_READ_LIMIT_SEC - How many seconds to allow for
     reading the initial request on a new connection.
    CONFIG_THTTPD_IDLE_SEND_LIMIT_SEC - How many seconds before an
      idle connection gets closed.
    CONFIG_THTTPD_TILDE_MAP1 and CONFIG_THTTPD_TILDE_MAP2 - Tilde mapping.
      Many URLs use ~username to indicate a user's home directory. thttpd
       provides two options for mapping this construct to an  actual filename.
      1) Map ~username to <prefix>/username. This is the recommended choice.
        Each user gets a subdirectory in the main web tree, and the tilde
        construct points there. The prefix could be something like "users",
        or it could be empty.
      2) Map ~username to <user's homedir>/<postfix>. The postfix would be
        the name of a subdirectory off of the user's actual home dir,
        something like "public_html".
      You can also leave both options undefined, and thttpd will not do
      anything special about tildes. Enabling both options is an error.
      Typical values, if they're defined, are "users" for
      CONFIG_THTTPD_TILDE_MAP1 and "public_html"forCONFIG_THTTPD_TILDE_MAP2. 
    CONFIG_THTTPD_GENERATE_INDICES
    CONFIG_THTTPD_URLPATTERN - If defined, then it will be used to match
      and verify referrers.

  FTP Server

    CONFIG_FTPD_VENDORID - The vendor name to use in FTP communications.
      Default: "NuttX"
    CONFIG_FTPD_SERVERID - The server name to use in FTP communications.
      Default: "NuttX FTP Server"
    CONFIG_FTPD_CMDBUFFERSIZE - The maximum size of one command.  Default:
      128 bytes.
    CONFIG_FTPD_DATABUFFERSIZE - The size of the I/O buffer for data
      transfers.  Default: 512 bytes.
    CONFIG_FTPD_WORKERSTACKSIZE - The stacksize to allocate for each
      FTP daemon worker thread.  Default:  2048 bytes.

    Other required configuration settings:  Of course TCP networking support
    is required.  But here are a couple that are less obvious:

      CONFIG_DISABLE_PTHREAD - pthread support is required
      CONFIG_DISABLE_POLL - poll() support is required

  USB device controller driver

    CONFIG_USBDEV - Enables USB device support
    CONFIG_USBDEV_COMPOSITE
      Enables USB composite device support
    CONFIG_USBDEV_ISOCHRONOUS - Build in extra support for isochronous
      endpoints
    CONFIG_USBDEV_DUALSPEED -Hardware handles high and full speed
      operation (USB 2.0)
    CONFIG_USBDEV_SELFPOWERED - Will cause USB features to indicate
      that the device is self-powered
    CONFIG_USBDEV_MAXPOWER - Maximum power consumption in mA
    CONFIG_USBDEV_TRACE - Enables USB tracing for debug
    CONFIG_USBDEV_TRACE_NRECORDS - Number of trace entries to remember

  USB host controller driver

    CONFIG_USBHOST
      Enables USB host support
    CONFIG_USBHOST_NPREALLOC
      Number of pre-allocated class instances
    CONFIG_USBHOST_BULK_DISABLE
      On some architectures, selecting this setting will reduce driver size
      by disabling bulk endpoint support
    CONFIG_USBHOST_INT_DISABLE
      On some architectures, selecting this setting will reduce driver size
      by disabling interrupt endpoint support
    CONFIG_USBHOST_ISOC_DISABLE
      On some architectures, selecting this setting will reduce driver size
      by disabling isochronous endpoint support

  USB host HID class driver. Requires CONFIG_USBHOST=y,
    CONFIG_USBHOST_INT_DISABLE=n, CONFIG_NFILE_DESCRIPTORS > 0,
    CONFIG_SCHED_WORKQUEUE=y, and CONFIG_DISABLE_SIGNALS=n.
 
    CONFIG_HIDKBD_POLLUSEC
      Device poll rate in microseconds. Default: 100 milliseconds.
    CONFIG_HIDKBD_DEFPRIO
      Priority of the polling thread.  Default: 50.
    CONFIG_HIDKBD_STACKSIZE
      Stack size for polling thread.  Default: 1024
    CONFIG_HIDKBD_BUFSIZE
      Scancode buffer size.  Default: 64.
    CONFIG_HIDKBD_NPOLLWAITERS
      If the poll() method is enabled, this defines the maximum number
      of threads that can be waiting for keyboard events.  Default: 2.
    CONFIG_HIDKBD_RAWSCANCODES
      If set to y no conversion will be made on the raw keyboard scan
      codes.  Default: ASCII conversion.
    CONFIG_HIDKBD_ALLSCANCODES'
      If set to y all 231 possible scancodes will be converted to
      something.  Default:  104 key US keyboard.
    CONFIG_HIDKBD_NODEBOUNCE
      If set to y normal debouncing is disabled.  Default: 
      Debounce enabled (No repeat keys).

  USB host mass storage class driver. Requires CONFIG_USBHOST=y,
    CONFIG_USBHOST_BULK_DISABLE=n, CONFIG_NFILE_DESCRIPTORS > 0,
    and CONFIG_SCHED_WORKQUEUE=y

  USB serial device class driver (Prolific PL2303 Emulation)

    CONFIG_PL2303
      Enable compilation of the USB serial driver
    CONFIG_PL2303_EPINTIN
      The logical 7-bit address of a hardware endpoint that supports
      interrupt IN operation
    CONFIG_PL2303_EPBULKOUT
      The logical 7-bit address of a hardware endpoint that supports
      bulk OUT operation
    CONFIG_PL2303_EPBULKIN
      The logical 7-bit address of a hardware endpoint that supports
      bulk IN operation
    CONFIG_PL2303_NWRREQS and CONFIG_PL2303_NRDREQS
      The number of write/read requests that can be in flight
    CONFIG_PL2303_VENDORID and CONFIG_PL2303_VENDORSTR
      The vendor ID code/string
    CONFIG_PL2303_PRODUCTID and CONFIG_PL2303_PRODUCTSTR
      The product ID code/string
    CONFIG_PL2303_RXBUFSIZE and CONFIG_PL2303_TXBUFSIZE
      Size of the serial receive/transmit buffers

  USB serial device class driver (Standard CDC ACM class)

    CONFIG_CDCACM
      Enable compilation of the USB serial driver
    CONFIG_CDCACM_COMPOSITE
      Configure the CDC serial driver as part of a composite driver
      (only if CONFIG_USBDEV_COMPOSITE is also defined)
    CONFIG_CDCACM_IFNOBASE
       If the CDC driver is part of a composite device, then this may need to 
      be defined to offset the CDC/ACM interface numbers so that they are
      unique and contiguous.  When used with the Mass Storage driver, the
      correct value for this offset is zero.
    CONFIG_CDCACM_STRBASE
      If the CDC driver is part of a composite device, then this may need to 
      be defined to offset the CDC/ACM string numbers so that they are
      unique and contiguous.  When used with the Mass Storage driver, the
      correct value for this offset is four (this value actuallly only needs
      to be defined if names are provided for the Notification interface,
      CONFIG_CDCACM_NOTIFSTR, or the data interface, CONFIG_CDCACM_DATAIFSTR).
    CONFIG_CDCACM_EP0MAXPACKET
      Endpoint 0 max packet size. Default 64.
    CONFIG_CDCACM_EPINTIN
      The logical 7-bit address of a hardware endpoint that supports
      interrupt IN operation.  Default 2.
    CONFIG_CDCACM_EPINTIN_FSSIZE
      Max package size for the interrupt IN endpoint if full speed mode.
      Default 64.
    CONFIG_CDCACM_EPINTIN_HSSIZE
      Max package size for the interrupt IN endpoint if high speed mode.
      Default 64.
    CONFIG_CDCACM_EPBULKOUT
      The logical 7-bit address of a hardware endpoint that supports
      bulk OUT operation
    CONFIG_CDCACM_EPBULKOUT_FSSIZE
      Max package size for the bulk OUT endpoint if full speed mode.
      Default 64.
    CONFIG_CDCACM_EPBULKOUT_HSSIZE
      Max package size for the bulk OUT endpoint if high speed mode.
      Default 512.
    CONFIG_CDCACM_EPBULKIN
      The logical 7-bit address of a hardware endpoint that supports
      bulk IN operation
    CONFIG_CDCACM_EPBULKIN_FSSIZE
      Max package size for the bulk IN endpoint if full speed mode.
      Default 64.
    CONFIG_CDCACM_EPBULKIN_HSSIZE
      Max package size for the bulk IN endpoint if high speed mode.
      Default 512.
    CONFIG_CDCACM_NWRREQS and CONFIG_CDCACM_NRDREQS
      The number of write/read requests that can be in flight.
      CONFIG_CDCACM_NWRREQS includes write requests used for both the
      interrupt and bulk IN endpoints.  Default 4.
    CONFIG_CDCACM_VENDORID and CONFIG_CDCACM_VENDORSTR
      The vendor ID code/string.  Default 0x0525 and "NuttX"
      0x0525 is the Netchip vendor and should not be used in any
      products.  This default VID was selected for compatibility with
      the Linux CDC ACM default VID.
    CONFIG_CDCACM_PRODUCTID and CONFIG_CDCACM_PRODUCTSTR
      The product ID code/string. Default 0xa4a7 and "CDC/ACM Serial"
      0xa4a7 was selected for compatibility with the Linux CDC ACM
      default PID.
    CONFIG_CDCACM_RXBUFSIZE and CONFIG_CDCACM_TXBUFSIZE
      Size of the serial receive/transmit buffers. Default 256.

  USB Storage Device Configuration

    CONFIG_USBMSC
      Enable compilation of the USB storage driver
    CONFIG_USBMSC_COMPOSITE
      Configure the mass storage driver as part of a composite driver
      (only if CONFIG_USBDEV_COMPOSITE is also defined)
    CONFIG_USBMSC_IFNOBASE
      If the CDC driver is part of a composite device, then this may need to 
      be defined to offset the mass storage interface number so that it is
      unique and contiguous.  When used with the CDC/ACM driver, the
      correct value for this offset is two (because of the two CDC/ACM
      interfaces that will precede it).
    CONFIG_USBMSC_STRBASE
      If the CDC driver is part of a composite device, then this may need to 
      be defined to offset the mass storage string numbers so that they are
      unique and contiguous.  When used with the CDC/ACM driver, the
      correct value for this offset is four (or perhaps 5 or 6, depending
      on if CONFIG_CDCACM_NOTIFSTR or CONFIG_CDCACM_DATAIFSTR are defined).
    CONFIG_USBMSC_EP0MAXPACKET
      Max packet size for endpoint 0
    CONFIG_USBMSCEPBULKOUT and CONFIG_USBMSC_EPBULKIN
      The logical 7-bit address of a hardware endpoints that support
      bulk OUT and IN operations
    CONFIG_USBMSC_NWRREQS and CONFIG_USBMSC_NRDREQS
      The number of write/read requests that can be in flight
    CONFIG_USBMSC_BULKINREQLEN and CONFIG_USBMSC_BULKOUTREQLEN
      The size of the buffer in each write/read request.  This
      value needs to be at least as large as the endpoint
      maxpacket and ideally as large as a block device sector.
    CONFIG_USBMSC_VENDORID and CONFIG_USBMSC_VENDORSTR
      The vendor ID code/string
    CONFIG_USBMSC_PRODUCTID and CONFIG_USBMSC_PRODUCTSTR
      The product ID code/string
    CONFIG_USBMSC_REMOVABLE
      Select if the media is removable

  USB Composite Device Configuration

    CONFIG_USBDEV_COMPOSITE
      Enables USB composite device support
    CONFIG_CDCACM_COMPOSITE
      Configure the CDC serial driver as part of a composite driver
      (only if CONFIG_USBDEV_COMPOSITE is also defined)
    CONFIG_USBMSC_COMPOSITE
      Configure the mass storage driver as part of a composite driver
      (only if CONFIG_USBDEV_COMPOSITE is also defined)
    CONFIG_COMPOSITE_IAD
      If one of the members of the composite has multiple interfaces
      (such as CDC/ACM), then an Interface Association Descriptor (IAD)
      will be necessary.  Default:  IAD will be used automatically if
      needed.  It should not be necessary to set this.
    CONFIG_COMPOSITE_EP0MAXPACKET
      Max packet size for endpoint 0
    CONFIG_COMPOSITE_VENDORID and CONFIG_COMPOSITE_VENDORSTR
      The vendor ID code/string
    CONFIG_COMPOSITE_PRODUCTID and CONFIG_COMPOSITE_PRODUCTSTR
      The product ID code/string
    CONFIG_COMPOSITE_SERIALSTR
      Device serial number string
    CONFIG_COMPOSITE_CONFIGSTR
      Configuration string
    CONFIG_COMPOSITE_VERSIONNO
      Interface version number.

  Graphics related configuration settings

    CONFIG_NX
      Enables overall support for graphics library and NX
    CONFIG_NX_MULTIUSER
      Configures NX in multi-user mode
    CONFIG_NX_NPLANES
      Some YUV color formats requires support for multiple planes,
      one for each color component.  Unless you have such special
      hardware, this value should be undefined or set to 1.
    CONFIG_NX_DISABLE_1BPP, CONFIG_NX_DISABLE_2BPP,
    CONFIG_NX_DISABLE_4BPP, CONFIG_NX_DISABLE_8BPP,
    CONFIG_NX_DISABLE_16BPP, CONFIG_NX_DISABLE_24BPP, and
    CONFIG_NX_DISABLE_32BPP
      NX supports a variety of pixel depths.  You can save some
      memory by disabling support for unused color depths.
    CONFIG_NX_PACKEDMSFIRST
      If a pixel depth of less than 8-bits is used, then NX needs
      to know if the pixels pack from the MS to LS or from LS to MS
    CONFIG_NX_LCDDRIVER
      By default, NX builds to use a framebuffer driver (see
      include/nuttx/fb.h). If this option is defined, NX will
      build to use an LCD driver (see include/nuttx/lcd/lcd.h).
    CONFIG_LCD_MAXPOWER - The full-on power setting for an LCD
      device.
    CONFIG_LCD_MAXCONTRAST - The maximum contrast value for an
      LCD device.
    CONFIG_LCD_LANDSCAPE, CONFIG_LCD_PORTRAIT, CONFIG_LCD_RLANDSCAPE,
      and CONFIG_LCD_RPORTRAIT - Some LCD drivers may support
      these options to present the display in landscape, portrait,
      reverse landscape, or reverse portrait orientations.  Check
      the README.txt file in each board configuration directory to
      see if any of these are supported by the board LCD logic.
    CONFIG_NX_MOUSE
      Build in support for mouse input.
    CONFIG_NX_KBD
      Build in support of keypad/keyboard input.
    CONFIG_NXTK_BORDERWIDTH
      Specifies with with of the border (in pixels) used with
      framed windows.   The default is 4.
    CONFIG_NXTK_BORDERCOLOR1 and CONFIG_NXTK_BORDERCOLOR2
      Specify the colors of the border used with framed windows.
      CONFIG_NXTK_BORDERCOLOR2 is the shadow side color and so
      is normally darker.  The default is medium and dark grey,
      respectively
    CONFIG_NXTK_AUTORAISE
      If set, a window will be raised to the top if the mouse position
      is over a visible portion of the window.  Default: A mouse
      button must be clicked over a visible portion of the window.
    CONFIG_NXFONTS_CHARBITS
      The number of bits in the character set.  Current options are
      only 7 and 8.  The default is 7.

    CONFIG_NXFONT_SANS23X27
      This option enables support for a tiny, 23x27 san serif font
      (font ID FONTID_SANS23X27 == 1).
    CONFIG_NXFONT_SANS22X29
      This option enables support for a small, 22x29 san serif font
      (font ID FONTID_SANS22X29 == 2).
    CONFIG_NXFONT_SANS28X37
      This option enables support for a medium, 28x37 san serif font
      (font ID FONTID_SANS28X37 == 3).
    CONFIG_NXFONT_SANS39X48
      This option enables support for a large, 39x48 san serif font
      (font ID FONTID_SANS39X48 == 4).
    CONFIG_NXFONT_SANS22X29B
      This option enables support for a small, 22x29 san serif bold font
      (font ID FONTID_SANS22X29B == 5).
    CONFIG_NXFONT_SANS28X37B
      This option enables support for a medium, 28x37 san serif bold font
      (font ID FONTID_SANS28X37B == 6).
    CONFIG_NXFONT_SANS40X49B
      This option enables support for a large, 40x49 san serif bold font
      (font ID FONTID_SANS40X49B == 7).
    CONFIG_NXFONT_SERIF22X29
      This option enables support for a small, 22x29 font (with serifs)
      (font ID FONTID_SERIF22X29 == 8).
    CONFIG_NXFONT_SERIF29X37
      This option enables support for a medium, 29x37 font (with serifs)
      (font ID FONTID_SERIF29X37 == 9).
    CONFIG_NXFONT_SERIF38X48
      This option enables support for a large, 38x48 font (with serifs)
      (font ID FONTID_SERIF38X48 == 10).
    CONFIG_NXFONT_SERIF22X28B
      This option enables support for a small, 27x38 bold font (with serifs)
      (font ID FONTID_SERIF22X28B == 11).
    CONFIG_NXFONT_SERIF27X38B
      This option enables support for a medium, 27x38 bold font (with serifs)
      (font ID FONTID_SERIF27X38B == 12).
    CONFIG_NXFONT_SERIF38X49B
      This option enables support for a large, 38x49 bold font (with serifs)
      (font ID FONTID_SERIF38X49B == 13).

  NX Multi-user only options:

    CONFIG_NX_BLOCKING
      Open the client message queues in blocking mode.  In this case,
      nx_eventhandler() will never return.
    CONFIG_NX_MXSERVERMSGS and CONFIG_NX_MXCLIENTMSGS
      Specifies the maximum number of messages that can fit in
      the message queues.  No additional resources are allocated, but
      this can be set to prevent flooding of the client or server with
      too many messages (CONFIG_PREALLOC_MQ_MSGS controls how many
      messages are pre-allocated).

  Stack and heap information

    CONFIG_BOOT_RUNFROMFLASH - Some configurations support XIP
      operation from FLASH but must copy initialized .data sections to RAM.
    CONFIG_BOOT_COPYTORAM -  Some configurations boot in FLASH
      but copy themselves entirely into RAM for better performance.
    CONFIG_BOOT_RAMFUNCS - Other configurations may copy just some functions
      into RAM, either for better performance or for errata workarounds.
    CONFIG_STACK_POINTER - The initial stack pointer (may not be supported
      in all architectures).
    CONFIG_STACK_ALIGNMENT - Set if the your application has specific
      stack alignment requirements (may not be supported
      in all architectures).
    CONFIG_IDLETHREAD_STACKSIZE - The size of the initial stack.
      This is the thread that (1) performs the inital boot of the system up
      to the point where user_start() is spawned, and (2) there after is the
      IDLE thread that executes only when there is no other thread ready to
      run.
    CONFIG_USERMAIN_STACKSIZE - The size of the stack to allocate
      for the main user thread that begins at the user_start() entry point.
    CONFIG_PTHREAD_STACK_MIN - Minimum pthread stack size
    CONFIG_PTHREAD_STACK_DEFAULT - Default pthread stack size
    CONFIG_HEAP_BASE - The beginning of the heap
    CONFIG_HEAP_SIZE - The size of the heap

appconfig -- This is another configuration file that is specific to the
  application.  This file is copied into the application build directory
  when NuttX is configured.  See ../apps/README.txt for further details.

setenv.sh -- This is a script that you can include that will be installed at
  the toplevel of the directory structure and can be sourced to set any
  necessary environment variables.  You will most likely have to customize the
  default setenv.sh script in order for it to work correctly in your
  environment.

Supported Boards
^^^^^^^^^^^^^^^^

configs/amber
  This is placeholder for the SoC Robotics Amber Web Server that is based
  on the Atmel AVR ATMega128 MCU.  There is not much there yet and what is
  there is untested due to tool-related issues.

configs/avr32dev1
  This is a port of NuttX to the Atmel AVR32DEV1 board.  That board is
  based on the Atmel AT32UC3B0256 MCU and uses a specially patched
  version of the GNU toolchain:  The patches provide support for the
  AVR32 family.  That patched GNU toolchain is available only from the
  Atmel website.  STATUS: This port is functional but very basic.  There
  are configurations for NSH and the OS test.

configs/c5471evm
  This is a port to the Spectrum Digital C5471 evaluation board.  The
  TMS320C5471 is a dual core processor from TI with an ARM7TDMI general
  purpose processor and a c54 DSP.  It is also known as TMS320DA180 or just DA180. 
  NuttX runs on the ARM core and is built with a GNU arm-elf toolchain*.
  This port is complete and verified.

configs/demo9s12ne64
  Feescale DMO9S12NE64 board based on the MC9S12NE64 hcs12 cpu.  This
  port uses the m9s12x GCC toolchain.  STATUS:  (Still) under development; it
  is code complete but has not yet been verified.

configs/ea3131
  Embedded Artists EA3131 Development bard.  This board is based on the 
  an NXP LPC3131 MCU. This OS is built with the arm-elf toolchain*.
  STATUS:  This port is complete and mature.

configs/ea3152
  Embedded Artists EA3152 Development bard.  This board is based on the 
  an NXP LPC3152 MCU. This OS is built with the arm-elf toolchain*.
  STATUS:  This port is has not be exercised well, but since it is
  a simple derivative of the ea3131, it should be fully functional.

configs/eagle100
  Micromint Eagle-100 Development board.  This board is based on the 
  an ARM Cortex-M3 MCU, the Luminary LM3S6918. This OS is built with the
  arm-elf toolchain*.  STATUS:  This port is complete and mature.

configs/ez80f0910200kitg
  ez80Acclaim! Microcontroller.  This port use the Zilog ez80f0910200kitg
  development kit, eZ80F091 part, and the Zilog ZDS-II Windows command line
  tools.  The development environment is Cygwin under WinXP.

configs/ez80f0910200zco
  ez80Acclaim! Microcontroller.  This port use the Zilog ez80f0910200zco
  development kit, eZ80F091 part, and the Zilog ZDS-II Windows command line
  tools.  The development environment is Cygwin under WinXP.

configs/hymini-stm32v
  A configuration for the HY-Mini STM32v board.  This board is based on the
  STM32F103VCT chip.

configs/kwikstik-k40.
  Kinetis K40 Cortex-M4 MCU.  This port uses the FreeScale KwikStik-K40
  development board.

configs/lm3s6432-s2e
  Stellaris RDK-S2E Reference Design Kit and the MDL-S2E Ethernet to
  Serial module.

configs/lm3s6965-ek
  Stellaris LM3S6965 Evaluation Kit.  This board is based on the 
  an ARM Cortex-M3 MCU, the Luminary/TI LM3S6965. This OS is built with the
  arm-elf toolchain*.  STATUS:  This port is complete and mature.

configs/lm3s8962-ek
  Stellaris LMS38962 Evaluation Kit.

configs/lpcxpresso-lpc1768
  Embedded Artists base board with NXP LPCExpresso LPC1768.  This board
  is based on the NXP LPC1768.  The Code Red toolchain is used by default.
  STATUS:  Under development.

configs/m68322evb
  This is a work in progress for the venerable m68322evb board from
  Motorola. This OS is also built with the arm-elf toolchain*.  STATUS:
  This port was never completed.

configs/mbed
  The configurations in this directory support the mbed board (http://mbed.org)
  that features the NXP LPC1768 microcontroller. This OS is also built
  with the arm-elf toolchain*.  STATUS:  Contributed.

configs/mcu123-lpc214x
  This port is for the NXP LPC2148 as provided on the mcu123.com
  lpc214x development board. This OS is also built with the arm-elf
  toolchain*.  The port supports serial, timer0, spi, and usb.

configs/micropendous3
  This is a port to the Opendous Micropendous 3 board. This board may
  be populated with either an AVR AT90USB646, 647, 1286, or 1287 MCU.
  Support is configured for the AT90USB647.

configs/mx1ads
  This is a port to the Motorola MX1ADS development board.  That board
  is based on the Freescale i.MX1 processor.  The i.MX1 is an ARM920T.
  STATUS:  This port is nearly code complete but was never fully
  integrated due to tool-related issues.

configs/ne64badge
  Future Electronics Group NE64 /PoE Badge board based on the
  MC9S12NE64 hcs12 cpu.  This port uses the m9s12x GCC toolchain.
  STATUS:  Under development.  The port is code-complete but has
  not yet been fully tested.

configs/ntosd-dm320
  This port uses the Neuros OSD v1.0 Dev Board with a GNU arm-elf
  toolchain*: see

    http://wiki.neurostechnology.com/index.php/OSD_1.0_Developer_Home
 
  There are some differences between the Dev Board and the currently
  available commercial v1.0 Boards.  See

    http://wiki.neurostechnology.com/index.php/OSD_Developer_Board_v1

  NuttX operates on the ARM9EJS of this dual core processor.
  STATUS: This port is code complete, verified, and included in the
  NuttX 0.2.1 release.

configs/nucleus2g
  This port uses the Nucleus 2G board (with Babel CAN board).  This board
  features an NXP LPC1768 processor.  See the 2G website (http://www.2g-eng.com/)
  for more information about the Nucleus 2G.

configs/olimex-lpc1766stk
  This port uses the Olimex LPC1766-STK board and a GNU GCC toolchain* under
  Linux or Cygwin.  STATUS: Complete and mature.

configs/olimex-lpc2378
  This port uses the Olimex-lpc2378 board and a GNU arm-elf toolchain* under
  Linux or Cygwin.  STATUS: ostest and NSH configurations available.

configs/olimex-lpc2378
  This port for the NXP LPC2378 was contributed by Rommel Marcelo.

configs/olimex-strp711
  This port uses the Olimex STR-P711 board and a GNU arm-elf toolchain* under
  Linux or Cygwin. See the http://www.olimex.com/dev/str-p711.html" for
  further information.  STATUS: Configurations for the basic OS test and NSH
  are complete and verified.

configs/pcblogic-pic32mx
  This is the port of NuttX to the PIC32MX board from PCB Logic Design Co.
  This board features the MicroChip PIC32MX460F512L.
  The board is a very simple -- little more than a carrier for the PIC32
  MCU plus voltage regulation, debug interface, and an OTG connector.
  STATUS:  Code complete but testing has been stalled due to tool related problems
  (PICkit 2 does not work with the PIC32).

configs/pic32-starterkit

  This README file discusses the port of NuttX to the Microchip PIC32 Ethernet
  Starter Kit (DM320004) with the Multimedia Expansion Board (MEB, DM320005).
  Advanced USB Storage.  See www.microchip.com for further information.

configs/pjrc-8051
  8051 Microcontroller.  This port uses the PJRC 87C52 development system
  and the SDCC toolchain.   This port is not quite ready for prime time.

configs/qemu-i486

  Port of NuttX to QEMU in i486 mode.  This port will also run on real i486
  hardwared (Google the Bifferboard).

configs/rgmp
  RGMP stands for RTOS and GPOS on Multi-Processor.  RGMP is a project for 
  running GPOS and RTOS simultaneously on multi-processor platforms. You can
  port your favorite RTOS to RGMP together with an unmodified Linux to form a
  hybrid operating system. This makes your application able to use both RTOS
  and GPOS features.

  See http://rgmp.sourceforge.net/wiki/index.php/Main_Page for further information
  about RGMP.

configs/sam3u-ek
  The port of NuttX to the Atmel SAM3U-EK development board.

configs/sim
  A user-mode port of NuttX to the x86 Linux platform is available.
  The purpose of this port is primarily to support OS feature development.
  This port does not support interrupts or a real timer (and hence no
  round robin scheduler)  Otherwise, it is complete.

  NOTE: This target will not run on Cygwin probably for many reasons but
  first off because it uses some of the same symbols as does cygwin.dll.

configs/skp16c26
  Renesas M16C processor on the Renesas SKP16C26 StarterKit.  This port
  uses the GNU m32c toolchain.  STATUS:  The port is complete but untested
  due to issues with compiler internal errors.

configs/stm3210e-evel
  STMicro STM3210E-EVAL development board based on the STMicro STM32F103ZET6
  microcontroller (ARM Cortex-M3).  This port uses the GNU Cortex-M3
  toolchain.

configs/stm3220g-eval
  STMicro STM3220G-EVAL development board based.

configs/stm3240g-eval
  STMicro STM3210G-EVAL development board based on the STMicro STM32F103ZET6
  microcontroller (ARM Cortex-M4 with FPU).  This port uses a GNU Cortex-M4
  toolchain (such as CodeSourcery).

configs/stm32f4discovery
  STMicro STM32F4-Discovery board boased on the STMIcro STM32F407VGT6 MCU.

configs/sure-pic32mx
  The "Advanced USB Storage Demo Board," Model DB-DP11215, from Sure
  Electronics (http://www.sureelectronics.net/).  This board features
  the MicroChip PIC32MX440F512H.  See also
  http://www.sureelectronics.net/goods.php?id=1168 for further
  information about the Sure DB-DP11215 board.

configs/teensy
  This is theport of NuttX to the PJRC Teensy++ 2.0 board.  This board is
  developed by http://pjrc.com/teensy/.  The Teensy++ 2.0 is based
  on an Atmel AT90USB1286 MCU.

configs/twr-k60n512
  Kinetis K60 Cortex-M4 MCU.  This port uses the FreeScale TWR-K60N512
  development board.

configs/us7032evb1
  This is a port of the Hitachi SH-1 on the Hitachi SH-1/US7032EVB1 board.
  STATUS:  Work has just began on this port.

configs/vsn
  ISOTEL NetClamps VSN V1.2 ready2go sensor network platform based on the
  STMicro STM32F103RET6.  Contributed by Uros Platise.  See
  http://isotel.eu/NetClamps/

configs/xtrs
  TRS80 Model 3.  This port uses a vintage computer based on the Z80.
  An emulator for this computer is available to run TRS80 programs on a 
  linux platform (http://www.tim-mann.org/xtrs.html).

configs/z16f2800100zcog
  z16f Microcontroller.  This port use the Zilog z16f2800100zcog
  development kit and the Zilog ZDS-II Windows command line tools.  The
  development environment is Cygwin under WinXP.

configs/z80sim
  z80 Microcontroller.  This port uses a Z80 instruction set simulator.
  That simulator can be found in the NuttX SVN at
  http://nuttx.svn.sourceforge.net/viewvc/nuttx/trunk/misc/sims/z80sim.
  This port also uses the SDCC toolchain (http://sdcc.sourceforge.net/")
  (verified with version 2.6.0).

configs/z8encore000zco
  z8Encore! Microcontroller.  This port use the Zilog z8encore000zco
  development kit, Z8F6403 part, and the Zilog ZDS-II Windows command line
  tools.  The development environment is Cygwin under WinXP.

configs/z8f64200100kit
  z8Encore! Microcontroller.  This port use the Zilog z8f64200100kit
  development kit, Z8F6423 part, and the Zilog ZDS-II Windows command line
  tools.  The development environment is Cygwin under WinXP.

Configuring NuttX
^^^^^^^^^^^^^^^^^

Configuring NuttX requires only copying

  configs/<board-name>/<config-dir>/Make.def to ${TOPDIR}/Make.defs
  configs/<board-name>/<config-dir>/setenv.sh to ${TOPDIR}/setenv.sh
  configs/<board-name>/<config-dir>/defconfig to ${TOPDIR}/.config

And if configs/<board-name>/<config-dir>/appconfig exists in the board
configuration directory:

  Copy configs/<board-name>/<config-dir>/appconfig to <app-dir>/.config
  echo "APPS_LOC=\"<app-dir>\"" >> "${TOPDIR}/.config"

tools/configure.sh
  There is a script that automates these steps.  The following steps will
  accomplish the same configuration:

  cd tools
  ./configure.sh <board-name>/<config-dir>

And if configs/<board-name>/<config-dir>/appconfig exists and your
application directory is not in the standard loction (../apps), then
you should also specify the location of the application directory on the
command line like:
  
  cd tools
  ./configure.sh -a <app-dir> <board-name>/<config-dir>
