/****************************************************************************
 * sched/sched_setupidlefiles.c
 *
 *   Copyright (C) 2007-2010, 2012 Gregory Nutt. All rights reserved.
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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/fs.h>
#include <nuttx/net/net.h>

#include "os_internal.h"

#if CONFIG_NFILE_DESCRIPTORS > 0 || CONFIG_NSOCKET_DESCRIPTORS > 0

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Function:  sched_setupidlefiles
 *
 * Description:
 *   Configure the idle thread's TCB.
 *
 * Parameters:
 *   tcb - tcb of the idle task.
 *
 * Return Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

int sched_setupidlefiles(FAR _TCB *tcb)
{
#if CONFIG_NFILE_DESCRIPTORS > 0 && defined(CONFIG_DEV_CONSOLE)
  int fd;
#endif

#if CONFIG_NFILE_DESCRIPTORS > 0
  /* Allocate file descriptors for the TCB */

  tcb->filelist = files_alloclist();
  if (!tcb->filelist)
    {
      return -ENOMEM;
    }
#endif /* CONFIG_NFILE_DESCRIPTORS */

#if CONFIG_NSOCKET_DESCRIPTORS > 0
  /* Allocate socket descriptors for the TCB */

  tcb->sockets = net_alloclist();
  if (!tcb->sockets)
    {
      return -ENOMEM;
    }
#endif /* CONFIG_NSOCKET_DESCRIPTORS */

#if CONFIG_NFILE_DESCRIPTORS > 0 && defined(CONFIG_DEV_CONSOLE)
  /* Open stdin, dup to get stdout and stderr. This should always
   * be the first file opened and, hence, should always get file
   * descriptor 0.
   */

  fd = open("/dev/console", O_RDWR);
  if (fd == 0)
    {
      /* Successfully opened /dev/console as stdin (fd == 0) */

      (void)file_dup2(0, 1);
      (void)file_dup2(0, 2);
    }
  else
    {
      /* We failed to open /dev/console OR for some reason, we opened
       * it and got some file descriptor other than 0.
       */
  
      if (fd >- 0)
        {
          slldbg("Open /dev/console fd: %d\n", fd);
          (void)close(fd);
        }
      else
        {
          slldbg("Failed to open /dev/console: %d\n", errno);
        }
      return -ENFILE;
    }

#if CONFIG_NFILE_STREAMS > 0
  /* Allocate file strems for the TCB */

  return sched_setupstreams(tcb);
#else
  return OK;
#endif /* CONFIG_NFILE_STREAMS */
#else
  return OK;
#endif /* CONFIG_NFILE_DESCRIPTORS && CONFIG_DEV_CONSOLE */
}

#endif /* CONFIG_NFILE_DESCRIPTORS || CONFIG_NSOCKET_DESCRIPTORS */
