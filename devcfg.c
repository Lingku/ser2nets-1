/*
 *  ser2net - A program for allowing telnet connection to serial ports
 *  Copyright (C) 2001  Corey Minyard <minyard@acm.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* This code handles generating the configuration for the serial port. */

#include <termios.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "devcfg.h"

/* Initialize a serial port control structure for the first time.
   This should only be called when the port is created.  It sets the
   port to the default 9600N81. */
void
devinit(struct termios *termctl)
{
    cfmakeraw(termctl);
    cfsetospeed(termctl, B9600);
    cfsetispeed(termctl, B9600);
    termctl->c_cflag &= ~(CSTOPB);
    termctl->c_cflag &= ~(CSIZE);
    termctl->c_cflag |= CS8;
    termctl->c_cflag &= ~(PARENB);
}

/* Configure a serial port control structure based upon input strings
   in instr.  These strings are described in the man page for this
   program. */
int
devconfig(char *instr, struct termios *termctl)
{
    char *str;
    char *pos;
    char *strtok_data;
    int  rv = 0;

    str = malloc(strlen(instr) + 1);
    if (str == NULL) {
	return -1;
    }

    strcpy(str, instr);

    pos = strtok_r(str, " \t", &strtok_data);
    while (pos != NULL) {
	if (strcmp(pos, "300") == 0) {
	    cfsetospeed(termctl, B300);
	    cfsetispeed(termctl, B300);
	} else if (strcmp(pos, "1200") == 0) {
	    cfsetospeed(termctl, B1200);
	    cfsetispeed(termctl, B1200);
	} else if (strcmp(pos, "2400") == 0) {
	    cfsetospeed(termctl, B2400);
	    cfsetispeed(termctl, B2400);
	} else if (strcmp(pos, "4800") == 0) {
	    cfsetospeed(termctl, B4800);
	    cfsetispeed(termctl, B4800);
	} else if (strcmp(pos, "9600") == 0) {
	    cfsetospeed(termctl, B9600);
	    cfsetispeed(termctl, B9600);
	} else if (strcmp(pos, "19200") == 0) {
	    cfsetospeed(termctl, B19200);
	    cfsetispeed(termctl, B19200);
	} else if (strcmp(pos, "38400") == 0) {
	    cfsetospeed(termctl, B38400);
	    cfsetispeed(termctl, B38400);
	} else if (strcmp(pos, "115200") == 0) {
	    cfsetospeed(termctl, B115200);
	    cfsetispeed(termctl, B115200);
	} else if (strcmp(pos, "1STOPBIT") == 0) {
	    termctl->c_cflag &= ~(CSTOPB);
	} else if (strcmp(pos, "2STOPBITS") == 0) {
	    termctl->c_cflag |= CSTOPB;
	} else if (strcmp(pos, "7DATABITS") == 0) {
	    termctl->c_cflag &= ~(CSIZE);
	    termctl->c_cflag |= CS7;
	} else if (strcmp(pos, "8DATABITS") == 0) {
	    termctl->c_cflag &= ~(CSIZE);
	    termctl->c_cflag |= CS8;
	} else if (strcmp(pos, "NONE") == 0) {
	    termctl->c_cflag &= ~(PARENB);
	} else if (strcmp(pos, "EVEN") == 0) {
	    termctl->c_cflag |= PARENB;
	    termctl->c_cflag &= ~(PARODD);
	} else if (strcmp(pos, "ODD") == 0) {
	    termctl->c_cflag |= PARENB | PARODD;
	} else {
	    rv = -1;
	    goto out;
	}

	pos = strtok_r(NULL, " \t", &strtok_data);
    }

out:
    free(str);
    return rv;
}

/* Send the serial port device configuration to the control port. */
void
show_devcfg(struct controller_info *cntlr, struct termios *termctl)
{
    speed_t speed = cfgetospeed(termctl);
    int     stopbits = termctl->c_cflag & CSTOPB;
    int     databits = termctl->c_cflag & CSIZE;
    int     parity_enabled = termctl->c_cflag & PARENB;
    int     parity = termctl->c_cflag & PARODD;
    char    *str;

    switch (speed) {
    case B300: str = "300"; break;
    case B1200: str = "1200"; break;
    case B2400: str = "2400"; break;
    case B4800: str = "4800"; break;
    case B9600: str = "9600"; break;
    case B19200: str = "19200"; break;
    case B38400: str = "38400"; break;
    case B115200: str = "115200"; break;
    default: str = "unknown speed";
    }
    controller_output(cntlr, str, strlen(str));
    controller_output(cntlr, " ", 1);

    if (stopbits) {
	str = "2STOPBITS";
    } else {
	str = "1STOPBIT";
    }
    controller_output(cntlr, str, strlen(str));
    controller_output(cntlr, " ", 1);

    switch (databits) {
    case CS7: str = "7DATABITS"; break;
    case CS8: str = "8DATABITS"; break;
    default: str = "unknown databits";
    }
    controller_output(cntlr, str, strlen(str));
    controller_output(cntlr, " ", 1);

    if (parity_enabled) {
	if (parity) {
	    str = "ODD";
	} else {
	    str = "EVEN";
	}
    } else {
	str = "NONE";
    }
    controller_output(cntlr, str, strlen(str));
}