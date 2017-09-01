// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "FT1000.h"
#include "rig.h"
#include "stdlib.h"


static const char FT1000name_[] = "FT-1000/D";

static const char *FT1000modes_[] = {
	"LSB", "USB", "CW2.4", "CW500", "AM6.0",
	"AM2.4", "FM1","FM2", "RTTY(L)", "RTTY(U)",
	"PKT(L)", "PKT(FM)", NULL};

static const int FT1000_mode_val[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static const char *FT1000widths_[] = {
	"2400", "2000", "500", "250", "6000", NULL};

static int FT1000_bw_vals[] = {0,1,2,3,4,WVALS_LIMIT};


RIG_FT1000::RIG_FT1000() {
	name_ = FT1000name_;
	modes_ = FT1000modes_;
	bandwidths_ = FT1000widths_;
	bw_vals_ = FT1000_bw_vals;

	can_change_alt_vfo = true;

	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 100;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	afreq = bfreq = A.freq = B.freq = 7100000;
	//amode = bmode = A.imode = B.imode = 1;
	aBW = bBW = A.iBW = B.iBW = 0;

	precision = 10;
	ndigits = 7;
	max_power = 200;

	has_split_AB = true;
	has_get_info = true;
	has_smeter = true;
	has_power_out = true;
	has_mode_control = true;
	has_bandwidth_control = true;
	has_ptt_control = true;
	has_tune_control = true;
}


int  RIG_FT1000::adjust_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 2 || m == 5 || m == 8 || m == 9 || m == 10)
		return 0; // 0 1 5 2
	if (m == 3)
		return 2; // 2 3 1
	if (m == 4 || m == 6 || m == 11)
		return 4; // 4 6 7 3
	return 2;
}


void RIG_FT1000::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
	replystr.clear();

}


void RIG_FT1000::initialize()
{
	//return
}


bool RIG_FT1000::get_info()
{

	unsigned char *d = 0;
	int alt = 0;
	int md = 0;
	int i = 0;

//	bool memmode = false;
	init_cmd();
	cmd[3] = 0x00;
	cmd[4] = 0xFA;
	int ret = waitN(5, 100, "Read flags");

	if (ret >= 5) {
		size_t p = ret - 5;
//		memmode = ((replystr[p+1] & 0x10) == 0x10); // Memory Tune Activated
		split = ((replystr[p] & 0x01) == 0x01); // SPLIT is ON or OFF receive operation
	}

	init_cmd();
	cmd[3] = 0x03;  // read 32 bytes of data for status of both vfos
	cmd[4] = 0x10;
	ret = waitN(32, 100, "get info", ASC);
	d = (unsigned char *)replybuff;
	if (ret >= 32) {
		if (ret > 32) d += (ret - 32);
		// vfo A data string
		A.freq = ((((d[1]<<8) + d[2])<<8) + d[3]) *10;
		md = d[7] & 0x07;
		alt = d[8] & 0x80;

			switch (md) {
			case 0 : A.imode = 0;
			break; // LSB
			case 1 : A.imode = 1;
			break; // USB
			case 2 : A.imode = 2;
			break; // CW
			case 3 : A.imode = 4;
			break; // AM
			case 4 : A.imode = 6;
			break; // FM
			case 5 : A.imode = 8;
			break; // RTTY
			case 6 : A.imode = 10;
			break; // PKT
			}

		A.imode = A.imode + ((alt == 128) ? 1 : 0);
		A.iBW = 5*((d[8] & 0x70) >> 4) + (d[8] & 0x07);

		if (A.iBW > 4) A.iBW = 4;
		if ((A.imode == 2) && (A.iBW == 2)) { A.imode = 3; }

		if ((A.imode == 4) && (A.iBW == 4)) {A.imode = 5; A.iBW  = 0; }
		if ((A.imode == 4) && (A.iBW == 0)) {A.imode = 4; A.iBW = 4; }

	    if ((A.imode == 6) && (A.iBW == 0)) {A.iBW  = 4; }
	    if ((A.imode == 11) && (A.iBW == 0)) {A.iBW  = 4; }

		d += 16; // vfo B data string
		B.freq = ((((d[1]<<8) + d[2])<<8) + d[3]) *10;
		md = d[7] & 0x07;
		alt = d[8] & 0x80;

		switch (md) {
			case 0 : B.imode = 0; break; // LSB
			case 1 : B.imode = 1; break; // USB
			case 2 : B.imode = 2; break; // CW
			case 3 : B.imode = 4; break; // AM
			case 4 : B.imode = 6; break; // FM
			case 5 : B.imode = 8; break; // RTTY
			case 6 : B.imode = 10; break; // PKT
			default : B.imode = 1; break;
			}

		B.imode = i + ((alt == 128) ? 1 : 0);
		B.iBW = 5*((d[8] & 0x70) >> 4) + (d[8] & 0x07);

		if (B.iBW > 4) B.iBW = 4;
		if (B.iBW > 4) B.iBW = 4;
		if ((B.imode == 2) && (B.iBW == 2)) { B.imode = 3; }
		if ((B.imode == 4) && (B.iBW == 4)) {B.imode = 5; B.iBW  = 0; }
		if ((B.imode == 4) && (B.iBW == 0)) {B.imode = 4; B.iBW = 4; }
		if ((B.imode == 6) && (B.iBW == 0)) { B.iBW  = 4; }
		if ((B.imode == 11) && (B.iBW == 0)) { B.iBW  = 4; }

		return true;
	}
	return false;
}


long RIG_FT1000::get_vfoA ()
{
	return A.freq;
}


int RIG_FT1000::get_modeA()
{
	return A.imode;
}


int RIG_FT1000::get_bwA()
{
	return A.iBW;
}


void RIG_FT1000::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; //
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set freq A", cmd, replystr);
}


void RIG_FT1000::set_modeA(int val)
{

	A.imode = val;
	init_cmd();
	cmd[3] = FT1000_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, replystr);
	MilliSleep(50);



}

void RIG_FT1000::set_bwA (int val)
{
	if (A.imode == 5) {val = 4;}
	A.iBW = val;
	init_cmd();
	cmd[3] = FT1000_bw_vals[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set BW A", cmd, replystr);
	MilliSleep(50);
}


long RIG_FT1000::get_vfoB()
{
	return B.freq;
}


int  RIG_FT1000::get_modeB()
{
	return B.imode;
}


int  RIG_FT1000::get_bwB()
{
	return B.iBW;
}


void RIG_FT1000::set_vfoB(long freq)
{

	B.freq = freq;
	freq /=10;
	cmd = to_bcd_be(freq, 8);
	cmd += 0x8A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set freq B", cmd, replystr);
}

void RIG_FT1000::set_modeB(int val)
{

	B.imode = val;
	init_cmd();
	cmd[3] = FT1000_mode_val[val] + 0x80;
	cmd[4] = 0x0C;
	sendCommand(cmd);
	MilliSleep(50);
	showresp(WARN, HEX, "set mode B", cmd, replystr);
	
}


void RIG_FT1000::set_bwB(int val)
{
	if (B.imode == 5) {val = 4;}
	B.iBW = val;
	init_cmd();
	cmd[3] = FT1000_bw_vals[val] + 0x80;
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set bw B", cmd, replystr);
	MilliSleep(50);
}

void RIG_FT1000::selectA()
{
  init_cmd();
 	cmd[3] = 0x00;
 	cmd[4] = 0x05;
 	sendCommand(cmd);
 	showresp(WARN, HEX, "select A", cmd, replystr);
}

void RIG_FT1000::selectB()
{
 	init_cmd();
 	cmd[3] = 0x01;
 	cmd[4] = 0x05;
 	sendCommand(cmd);
 	showresp(WARN, HEX, "select B", cmd, replystr);
}

void RIG_FT1000::swapvfos() 
{
	init_cmd();
	cmd[4] = 0x85;
	sendCommand(cmd);
	showresp(WARN, HEX, "copy active vfo to background vfo", cmd, replystr);
	MilliSleep(50);
}


void RIG_FT1000::set_split(bool val)

{
	split = val;
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	sendCommand(cmd);
	if (val)
		showresp(WARN, HEX, "set split ON", cmd, replystr);
	else
		showresp(WARN, HEX, "set split OFF", cmd, replystr);
}

int RIG_FT1000::get_split()
{
	return split;
}


void RIG_FT1000::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	cmd[4] = 0x0F;
	sendCommand(cmd);
	if (val)
		showresp(WARN, HEX, "set PTT ON", cmd, replystr);
	else
		showresp(WARN, HEX, "set PTT OFF", cmd, replystr);
}


void RIG_FT1000::tune_rig()
{
	init_cmd();
	cmd[4] = 0x82; // initiate tuner cycle
	sendCommand(cmd,0);
	LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}


int RIG_FT1000::get_smeter()
{
	float val = 0;
	init_cmd();
	//cmd[0] = 0x00;
	cmd[4] = 0xF7;
	int ret = waitN(5, 100, "get smeter", HEX);
	if (ret < 5) return 0;

	val = (unsigned char)(replybuff[ret-5]);
	if (val <= 15) val = 5;
	else if (val <=154) val = 5 + 80 * (val - 15) / (154 - 15);
	else val = 50 + 50 * (val - 154.0) / (255.0 - 154.0);

	LOG_INFO("%s => %d",str2hex(replybuff,1), (int)val);

	return (int)val;
}


int RIG_FT1000::get_power_out()
{

float pwr;

	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(5, 100, "Power out");
	if (ret < 5) return 0;
	pwr = (unsigned char)(replybuff[ret - 5]);
	if (pwr <=53) {pwr /= 53; pwr = 20 * pwr * pwr; }
	else if (pwr <= 77) {pwr /= 77; pwr = 40 * pwr * pwr; }
	else if (pwr <= 98) {pwr /= 98; pwr = 60 * pwr * pwr; }
	else if (pwr <= 114) {pwr /= 114; pwr = 80 * pwr * pwr; }
	else if (pwr <= 130) {pwr /= 130; pwr = 105 * pwr * pwr; }
	else {pwr /= 177; pwr = 160 * pwr * pwr; }
	LOG_INFO("%s => %d",str2hex(replybuff,1), (int)pwr);
	return (int)pwr;
}

