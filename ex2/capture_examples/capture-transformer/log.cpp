/* ---------------------------------------------------------------------------- 
 * @file log.h
 * @brief abstraction to syslog/printf, timing functions
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: Embedded Machine Vision (Summer 2021)
 *---------------------------------------------------------------------------*/

#include <time.h>
#include "log.h"

// see .h for more details
double get_time_msec(void) {
  
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)((ts.tv_sec)*1000.0 + (ts.tv_nsec)/1000000.0);
}
