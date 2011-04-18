/*
 * Icom IC-703
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC703.h"

//=============================================================================
// IC-703
//
const char IC703name_[] = "IC-703";
const char *IC703modes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R",
	"D-LSB", "D-USB", NULL};
const char IC703_mode_type[] = {'L', 'U', 'U', 'L', 'L', 'U', 'U', 'U', 'L', 'U' };

const char *IC703_widths[] = { "NARR", "MED", "WIDE", NULL};

RIG_IC703::RIG_IC703() {
	name_ = IC703name_;
	modes_ = IC703modes_;
 	_mode_type = IC703_mode_type;
	bandwidths_ = IC703_widths;
	comm_baudrate = BR9600;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_ptt_control =
	has_mode_control =
	has_bandwidth_control =
	has_compON =
	has_vox_onoff =
	has_preamp_control =
	has_attenuator_control = true;

	defaultCIV = 0x68;
	adjustCIV(defaultCIV);
};

//=============================================================================

long RIG_IC703::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (!sendICcommand(cmd, 11)) {
		checkresponse();
		return freqA;
	}
	freqA = fm_bcd_be(&replystr[5], 10);
	return freqA;
}

void RIG_IC703::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse();
}

void RIG_IC703::set_modeA(int val)
{
	bool data_mode = val > 7 ? true : false;
	modeA = val;
	cmd = pre_to;
	cmd += '\x06';
	if (val > 7) val -= 8;
	else if (val > 5) val++;
	cmd += val;
	cmd += bwA + 1;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse();
	if (val < 2) {
		cmd = pre_to;
		cmd.append("\x1A\x04");
		cmd += data_mode ? 0x01 : 0x00;
		cmd.append( post );
		sendICcommand( cmd, 6 );
		checkresponse();
	}
}

int RIG_IC703::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		modeA = replystr[5];
		if (modeA > 6) modeA--;
		bwA = replystr[6] - 1;
		if (modeA < 2) {
			cmd = pre_to;
			cmd.append("\x1A\x04");
			cmd.append(post);
			if (sendICcommand(cmd, 8))
				if (replystr[6])
					modeA += 8;
		}
	} else
		checkresponse();
	return modeA;
}

void RIG_IC703::set_bwA(int val)
{
	bwA = val;
	set_modeA(modeA);
}

int RIG_IC703::get_modetype(int n)
{
	return _mode_type[n];
}

int RIG_IC703::get_bwA()
{
	return bwA;
}

void RIG_IC703::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x20' : '\x00';
	cmd.append( post );
	sendCommand (cmd, 6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC703::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand (cmd, 7))
		return replystr[6] == '\x20' ? 1 : 0;
	else
		checkresponse();
	return 0;
}

void RIG_IC703::set_preamp(int val)
{
	if (preamp_level == 0) {
		preamp_level = 1;
		preamp_label("Pre 1", true);
	} else if (preamp_level == 1) {
		preamp_level = 2;
		preamp_label("Pre 2", true);
	} else if (preamp_level == 2) {
		preamp_level = 0;
		preamp_label("Pre", false);
	}
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += (unsigned char) preamp_level;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC703::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	if (sendICcommand (cmd, 8)) {
		if (replystr[6] == 0x01) {
			preamp_label("Pre 1", true);
			preamp_level = 1;
		} else if (replystr[6] == 0x02) {
			preamp_label("Pre 2", true);
			preamp_level = 2;
		} else {
			preamp_label("Pre", false);
			preamp_level = 0;
		}
	} else
		checkresponse();
	return preamp_level;
}

int RIG_IC703::get_smeter()
{
	cmd = pre_to;
	cmd.append("\x15\x02").append(post);
	if (sendICcommand (cmd, 9))
		return fm_bcd(&replystr[6], 3) * 100 / 255;
	checkresponse();
	return 0;
}

int RIG_IC703::get_power_out()
{
	cmd = pre_to;
	cmd.append("\x15\x11").append(post);
	if (sendICcommand (cmd, 9))
		return fm_bcd(&replystr[6], 3) * 100 / 255;
	checkresponse();
	return 0;
}

int RIG_IC703::get_swr()
{
	cmd = pre_to;
	cmd.append("\x15\x12").append(post);
	if (sendICcommand (cmd, 9))
		return fm_bcd(&replystr[6], 3) * 100 / 255;
	checkresponse();
	return 0;
}

void RIG_IC703::set_compression()
{
	if (progStatus.compON) {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse();
	} else {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse();
	}
}

void RIG_IC703::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x01';
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse();
	} else {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x00';
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse();
	}
}

// Tranceiver PTT on/off
void RIG_IC703::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse();
}

