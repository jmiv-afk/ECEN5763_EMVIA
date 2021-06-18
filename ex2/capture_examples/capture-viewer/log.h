/* ---------------------------------------------------------------------------- 
 * @file log.h
 * @brief abstraction to syslog/printf, timing functions
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: Embedded Machine Vision (Summer 2021)
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stddef.h>
#include <syslog.h>

#ifndef LOG_H_
#define LOG_H_

// Enable logging with following switches:
#define LOGSYS_ENABLED
#define LOGP_ENABLED

#ifdef LOGP_ENABLED
  #define LOGP printf
#else 
  // do nothing, get rid of output 
  #define LOGP(...) 
#endif

#ifdef LOGSYS_ENABLED
  #define LOGSYS(args...) syslog(LOG_KERN | LOG_CRIT, ##args)
#else
  // do nothing, get rid of output
  #define LOGSYS(...)
#endif

#define USEC_TO_NSEC (1000)
#define MSEC_TO_NSEC (1000000)
#define SEC_TO_NSEC  (1000000000)

/* @brief  Gets the time in msec via CLOCK_MONOTONIC
 *
 * @param  None 
 * @return double time, the time returned in millisec (msec)
 */
double get_time_msec(void);

#endif // LOG_H_
