/*
 * K2 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 *
 * Additions by Kent Haase, VE4KEH
 * Ref Elecraft KIO2 Programmer's Reference (Rev E, 3 Feb, 2004)
 * K2 MCU firmware rev 2.04
 * 
*/
 
#include "K2.h"
#include "support.h"
#include "status.h"

const char K2name_[] = "K2";

const char *K2modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "RTTY-L", "CW-R", "USER-L", "RTTY-U", NULL};

static const char K2_mode_type[] =
	{'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U'};

const char *K2_widths[] = {"FL1", "FL2", "FL3", "FL4", NULL};

RIG_K2::RIG_K2() {
// base class values	
	name_ = K2name_;
	modes_ = K2modes_;
	bandwidths_ = K2_widths;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 3;
	comm_wait = 50;
	comm_timeout = 200;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeB = modeA = 1;
	bwB = bwA = 0;
	freqB = freqA = 14070000;

	has_get_info =
	has_attenuator_control =
	has_preamp_control =
	has_power_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_split =
	has_tune_control = true;

	K2split =
	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_volume_control =
	has_swr_control = false;

	hipower = false;
}

int  RIG_K2::adjust_bandwidth(int m)
{
	return 0;
}

void RIG_K2::initialize()
{
//enable extended command mode
	sendCommand("K22;", 0);
//disable auto reporting of info
	sendCommand("AI0;", 0);
//ensure K2 is in VFO A
    get_power_control();
}

/*
 * IF (Transceiver Information; GET only)
 * 
 * RSP format: IF[f]*****+yyyyrx*00tmvspb01*; where the fields are defined as follows:
 * 
 * 0         1         2         3
 * 0123456789012345678901234567890123456789
 * IFfffffffffff*****+yyyyrx*00tmvspb01*;
 *   0..1   IF 
 *   2..12  [f] operating frequency, excluding any RIT/XIT offset (11 digits; see FA command)
 *   13..17 * represents a space (BLANK, or ASCII 0x20)
 *   18     + either "+" or "-" (sign of RIT/XIT offset)
 *   19..22 yyyy RIT/XIT offset in Hz (range is -9990 to +9990 Hz when computer-controlled)
 *   23     r 1 if RIT is on, 0 if off
 *   24     x 1 if XIT is on, 0 if off
 *   25..27 fill
 *   28     t 1 if the K2 is in transmit mode, 0 if receive
 *   29     m operating mode (see MD command)
 *   30     v receive-mode VFO selection, 0 for VFO A, 1 for VFO B
 *   31     s 1 if scan is in progress, 0 otherwise
 *   32     p 1 if the transeiver is in split mode, 0 otherwise
 *   33     b basic RSP format: always 0
 *            extended RSP format: 
 *                1 if the present IF response is due to a K2 band change and 
 *                0 otherwise
 *   34..35 fill
 *   37     ; terminator
 * The fixed-value fields (space, 0, and 1) are provided for syntactic compatibility with existing software.
*/
// set replystr to teststr to test for various control bytes
//const char *teststr = "IFfffffffffff*****+yyyyrx*000m0s1b01*;";

static void do_selectA(void *)
{
	cb_selectA();
}

static void do_selectB(void *)
{
	cb_selectB();
}

bool RIG_K2::get_info()
{
	rsp = cmd = "IF";
	cmd += ';';
	int ret = waitN(38, 100, "get info", ASC);
	if (ret < 38) return false;
	size_t p = replystr.find(rsp);
	if (PTT && (replystr[p+28]=='0')) Fl::awake(setPTT, (void*)0);
	if (!PTT && (replystr[p+28]=='1')) Fl::awake(setPTT, (void*)1);
	if (useB && (replystr[p+30]=='0')) Fl::awake(do_selectA, (void*)0);
	else if(!useB && (replystr[p+30]=='1')) Fl::awake(do_selectB, (void*)0);
	K2split = replystr[p+32]-'0';
	return true;
}

void RIG_K2::selectA()
{
	cmd = "FR0;FT0";
	sendCommand(cmd);
	K2split = false;
	showresp(WARN, ASC, "select A", cmd, replystr);
}

void RIG_K2::selectB()
{
	cmd = "FR1;FT1";
	sendCommand(cmd);
	K2split = false;
	showresp(WARN, ASC, "select B", cmd, replystr);
}

bool RIG_K2::can_split()
{
	return true;
}

void RIG_K2::set_split(bool val)
{
	if (val) {
		if (useB)
			cmd = "FR1;FT0;";
		else
			cmd = "FR0;FT1;";
		sendCommand(cmd);
		showresp(WARN, ASC, "set split ON", cmd, replystr);
	} else {
		if (useB)
			cmd = "FR1;FT1;";
		else
			cmd = "FR0;FT0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "set split OFF", cmd, replystr);
	}
}

bool RIG_K2::get_split()
{
	return K2split;
}

long RIG_K2::get_vfoA ()
{
	rsp = cmd = "FA";
	cmd += ';';
	int ret = waitN(14, 100, "get vfo A", ASC);
	if (ret < 14) return freqA;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqA;
	long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_K2::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo A", cmd, replystr);
}

long RIG_K2::get_vfoB()
{
	rsp = cmd = "FB";
	cmd += ';';
	int ret = waitN(14, 100, "get vfo B", ASC);
	if (ret < 14) return freqB;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqB;
	long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_K2::set_vfoB(long freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo B", cmd, replystr);
}

void RIG_K2::set_modeA(int val)
{
	modeA = val;
	val++;
	cmd = "MD0;";
	cmd[2] += val;
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);
}

int RIG_K2::get_modeA()
{
	rsp = cmd = "MD";
	cmd += ';';
	int ret = waitN(4, 100, "get mode A", ASC);
	if (ret < 4) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	modeA = replystr[p + 2] - '1';
	return modeA;
}

void RIG_K2::set_modeB(int val)
{
	modeB = val;
	val++;
	cmd = "MD0;";
	cmd[2] += val;
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
}

int  RIG_K2::get_modeB()
{
	rsp = cmd = "MD";
	cmd += ';';
	int ret = waitN(4, 100, "get mode B", ASC);
	if (ret < 4) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	modeB = replystr[p + 2] - '1';
	return modeB;
}

int RIG_K2::get_modetype(int n)
{
	return K2_mode_type[n];
}

void RIG_K2::set_bwA(int val)
{
	bwA = val;
	switch (val) {
		case 0 : cmd = "FW00001;"; break;
		case 1 : cmd = "FW00002;"; break;
		case 2 : cmd = "FW00003;"; break;
		case 3 : cmd = "FW00004;"; break;
		default: cmd = "FW00001;";
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set bwA", cmd, replystr);
}

int RIG_K2::get_bwA()
{
	rsp = cmd = "FW";
	cmd += ';';
	int ret = waitN(9, 100, "get bw A", ASC);
	if (ret < 9) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	bwA = replystr[p + 6] - '1';
	return bwA;
}

void RIG_K2::set_bwB(int val)
{
	bwB = val;
	switch (val) {
		case 0 : cmd = "FW00001;"; break;
		case 1 : cmd = "FW00002;"; break;
		case 2 : cmd = "FW00003;"; break;
		case 3 : cmd = "FW00004;"; break;
		default: cmd = "FW00001;";
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set bwA", cmd, replystr);
}

int  RIG_K2::get_bwB()
{
	rsp = cmd = "FW";
	cmd += ';';
	int ret = waitN(9, 100, "get bw B", ASC);
	if (ret < 9) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	bwB = replystr[p + 6] - '1';
	return bwB;
}

void RIG_K2::tune_rig()
{
	cmd = "SW20;";
	sendCommand(cmd);
	showresp(WARN, ASC, "tune", cmd, replystr);
}

int RIG_K2::get_smeter()
{
	rsp = cmd = "BG";
	cmd += ';';
	int ret = waitN(5, 100, "get smeter", ASC);
	if (ret < 5) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	int mtr = 10 * (replystr[p + 2] - '0') 
			+ replystr[p + 3] - '0';   //assemble two chars into 2 digit int
	if (mtr > 10) {                    //if int greater than 10 (bar mode)
	    mtr -= 12;                     //shift down to 0 thru 10
	}
	mtr *= 10;                         //normalize to 0 thru 100
	return mtr;
}

int RIG_K2::get_power_out()
{
	rsp = cmd = "BG";
	cmd += ';';
	int ret = waitN(5, 100, "get power", ASC);
	if (ret < 5) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	int mtr = 10 * (replystr[p + 2] - '0') 
			+ replystr[p + 3] - '0';   //assemble two chars into 2 digit int
	if (mtr > 10) {                           //if int greater than 10 (bar mode)
	    mtr -= 12;                            //shift down to 0 thru 10
	}
	mtr *= 10;                                //normalize to 0 thru 100
	return mtr;
}

int RIG_K2::get_power_control()
{
	rsp = cmd = "PC;";  // extended mode for get power
	cmd += ';';
	int ret = waitN(7, 100, "get pwr cont", ASC);
	if (ret < 7) return progStatus.power_level;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	hipower = (replystr[p+5] == '1');
	int mtr = fm_decimal(&replystr[p+4], 3);
	if (!hipower) mtr /= 10;
	return mtr;
}

void RIG_K2::set_power_control(double val)
{
    int ival = (int)val;
	cmd = "PC";                    //init the cmd string
	if (val > 15) {
		hipower = true;
		cmd[5] = '1';
		cmd.append(to_decimal(ival,3)).append("1;");
	} else {
		hipower = false;
		cmd.append(to_decimal((int)(val*10),3)).append("0;");
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET pwr", cmd, replystr);
}

void RIG_K2::get_pc_min_max_step(double &min, double &max, double &step)
{
	if (hipower) {
		min = 1.0; max_power = max = 110.0; step = 1.0;
	} else {
		min = 0.1; max_power = max = 20; step = 0.1;
	}
}

void RIG_K2::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET ptt", cmd, replystr);
}

void RIG_K2::set_attenuator(int val)
{
	if (val) cmd = "RA01;";
	else	 cmd = "RA00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET att", cmd, replystr);
}

int RIG_K2::get_attenuator()
{
	rsp = cmd = "RA";
	cmd += ';';
	int ret = waitN(5, 100, "get att", ASC);
	if (ret < 5) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (replystr[p + 3] == '1' ? 1 : 0);
}

void RIG_K2::set_preamp(int val)
{
	if (val) cmd = "PA1;";
	else	 cmd = "PA0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET pre", cmd, replystr);
}

int RIG_K2::get_preamp()
{
	rsp = cmd = "PA";
	cmd += ';';
	int ret = waitN(4, 100, "get pre", ASC);
	if (ret < 4) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (replystr[p + 2] == '1' ? 1 : 0);
}

