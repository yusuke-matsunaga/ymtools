﻿/*
 * Revision Control Information
 *
 * $Source: /users/pchong/CVS/sis/utility/prtime.c,v $
 * $Author: pchong $
 * $Revision: 1.1.1.1 $
 * $Date: 2004/02/07 10:14:13 $
 *
 */
/* LINTLIBRARY */
#include "copyright.h"
#include "port.h"
#include "utility.h"

/*
 *  util_print_time -- massage a long which represents a time interval in
 *  milliseconds, into a string suitable for output
 *
 *  Hack for IBM/PC -- avoids using floating point
 */

char *
util_print_time(long t)
{
    static char s[40];

    (void) sprintf(s, "%ld.%02ld sec", t/1000, (t%1000)/10);
    return s;
}
