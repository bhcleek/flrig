/*
 * Icom IC-735, 735PRO, 756, 756PRO ... drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "IC735.h"

//=============================================================================
// IC-735
//
const char IC735name_[] = "IC-735";
const char *IC735modes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", NULL};
const char *IC735_widths[] = { "NARR", "WIDE", NULL};

RIG_IC735::RIG_IC735() {
	name_ = IC735name_;
	modes_ = IC735modes_;
	bandwidths_ = IC735_widths;
	comm_baudrate = BR1200;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 10;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_mode_control = true;
	has_bandwidth_control = true;

	defaultCIV = 0x04;
	adjustCIV(defaultCIV);

};

//=============================================================================

long RIG_IC735::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (!sendCommand(cmd, 10)) {
		checkresponse();
		return freqA;
	}
	freqA = fm_bcd_be(&replystr[5], 8);
	return freqA;
}

void RIG_IC735::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	sendCommand(cmd, 6);
	checkresponse();
}

void RIG_IC735::set_modeA(int val)
{
	modeA = val;
	cmd = pre_to;
	cmd += "\x06";
	cmd += modeA;		   // set the mode byte
	cmd.append( post );
	sendCommand(cmd, 6);
	checkresponse();
}

void RIG_IC735::set_bwA(int val)
{
	bwA = val;
	cmd = pre_to;
	cmd += "\0x06";
	cmd += modeA;
	cmd += bwA;
	cmd.append(post);
	sendCommand(cmd, 6);
	checkresponse();
}


