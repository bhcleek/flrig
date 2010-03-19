/*
 * TenTec Jupiter (TT538) drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

/*
 *	Note for anyone wishing to expand on the command set.
 *
 *	The Jupiter always sends a response and ends the response with a "G\r" to
 *	indicate that the command was accepted.  A rejected command is responded to by a
 *	two character sequence "Z\r".  You should always expect a maximum response equal
 *	to the number of data bytes plus two.
 *
 *	For example:
 *		A request for the present receiver filter bandwidth is the the string:
 *			"?W\r" which is 3 bytes in length
 *		The response from the Argonaut V will be:
 *			"Wn\rG\r" which is 5 bytes in length, where n is an unsigned char (byte)
 *		If the transceiver failed to receive the command correctly it will respond:
 *			"Z\r" ----> you need to check for that condition
 *
*/

#include "TT538.h"
#include "support.h"
#include "math.h"

static const char TT538name_[] = "TT-538";

//static const char *TT538modes_[] = { "D-USB", "USB", "LSB", "CW", "AM", "FM", NULL}
//static const char TT538mode_chr[] =  { '1', '1', '2', '3', '0', '4' };
//static const char TT538mode_type[] = { 'U', 'U', 'L', 'L', 'U', 'U' };
static const char *TT538modes_[] = {
		"AM", "USB", "LSB", "CW", "FM", NULL};
static const char TT538mode_chr[] =  { '0', '1', '2', '3', '4' };
static const char TT538mode_type[] = { 'U', 'U', 'L', 'U', 'U' };

// filter # is 38 - index
static const char *TT538_widths[] = {
"150",   "165",  "180",  "225",  "260",  "300",  "330",  "375",  "450",  "525",
"600",   "675",  "750",  "900", "1050", "1200", "1350", "1500", "1650", "1800",
"1950", "2100", "2250", "2400", "2550", "2700", "2850", "3000", "3300", "3600",
"3900", "4200", "4500", "4800", "5100", "5400", "5700", "6000", "8000", NULL};

static const int TT538_numeric_widths[] = {
150,   165,  180,  225,  260,  300,  330,  375,  450,  525,
600,   675,  750,  900, 1050, 1200, 1350, 1500, 1650, 1800,
1950, 2100, 2250, 2400, 2550, 2700, 2850, 3000, 3300, 3600,
3900, 4200, 4500, 4800, 5100, 5400, 5700, 6000, 8000};

static char TT538setFREQA[]		= "*Annnn\r";
//static char TT538setFREQB[]		= "*Bnnnn\r";
//static char TT538setAGC[]		= "*Gn\r";
//static char TT538setSQLCH[]		= "*Hc\r";
static char TT538setRF[]		= "*Ic\r";
static char TT538setATT[]		= "*Jc\r";
//static char TT538setNB[]		= "*Knar\r";
static char TT538setMODE[]		= "*Mnn\r";
static char TT538setPBT[]		= "*Pxx\r";
static char TT538setVOL[]		= "*Un\r";
static char TT538setBW[]		= "*Wx\r";

static char TT538getFREQA[]		= "?A\r";
//static char TT538getFWDPWR[]	= "?F\r";
//static char TT538getAGC[]		= "?G\r";
//static char TT538getSQLCH[]		= "?H\r";
static char TT538getRF[]		= "?I\r";
static char TT538getATT[]		= "?J\r";
//static char TT538getNB[]		= "?K\r";
static char TT538getMODE[]		= "?M\r";
static char TT538getPBT[]		= "?P\r";
static char TT538getSMETER[]	= "?S\r";
static char TT538getVOL[]		= "?U\r";
static char TT538getBW[]		= "?W\r";

//static char TT538getREFPWR[]	= "?R\r";
//static char TT538setXMT[]		= "#1\r";
//static char TT538setRCV[]		= "#0\r";

static char TT538setXMT[]		= "Q1\r";
static char TT538setRCV[]		= "Q0\r";

RIG_TT538::RIG_TT538() {
// base class values
	name_ = TT538name_;
	modes_ = TT538modes_;
	bandwidths_ = TT538_widths;
	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 20;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;// false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	serloop_timing = 200;

	mode_ = 1;
	bw_ = 25;
	def_mode = 3;
	defbw_ = 25;
	deffreq_ = 14070000;
	max_power = 100;
	pbt = 0;
	VfoAdj = progStatus.vfo_adj;
	vfo_corr = 0;

	has_power_control =
	has_micgain_control =
	has_notch_control =
	has_preamp_control =
	has_tune_control =
	has_noise_control =
	has_swr_control = false;

	has_bpf_center = 
	has_volume_control =
	has_rf_control =
	has_attenuator_control =
	has_ifshift_control =
	has_ptt_control =
	has_bandwidth_control =
	has_mode_control = 
	has_vfo_adj = true;

}

void RIG_TT538::checkresponse(string s)
{
	if (RigSerial.IsOpen() == false)
		return;
	LOG_ERROR("%s:\nsent  %s\nreply %s\n", s.c_str(),
		str2hex(cmd.c_str(), cmd.length()),
		str2hex((char *)replybuff, strlen((char *)replybuff)));
}

void RIG_TT538::showresponse(string s)
{
	printf("%s : %s\n", s.c_str(), str2hex(replystr.c_str(), replystr.length()));
}

void RIG_TT538::initialize()
{
	VfoAdj = progStatus.vfo_adj;
}

void RIG_TT538::shutdown()
{
	set_if_shift(0);
}

long RIG_TT538::get_vfoA ()
{
	cmd = TT538getFREQA;
	bool ret = sendCommand(cmd, 6, true);
	if (ret == true && replybuff[0] == 'A') {
		int f = 0;
		for (size_t n = 1; n < 5; n++)
			f = f*256 + (unsigned char)replybuff[n];
		freq_ = f;
	}
	return (long)(freq_ - vfo_corr);
}

void RIG_TT538::set_vfoA (long freq)
{
	freq_ = freq;
	vfo_corr = (freq / 1e6) * VfoAdj + 0.5;
	long xfreq = freq_ + vfo_corr;
	cmd = TT538setFREQA;
	cmd[5] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[4] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[3] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[2] = xfreq & 0xff;
	sendCommand(cmd, 0, true);
	set_if_shift(pbt);
	return ;
}

void RIG_TT538::setVfoAdj(double v)
{
	VfoAdj = v;
}

void RIG_TT538::set_mode(int val)
{
	mode_ = val;
	cmd = TT538setMODE;
	cmd[2] = cmd[3] = TT538mode_chr[val];
	sendCommand(cmd, 0, true);
}

int RIG_TT538::get_mode()
{
	cmd = TT538getMODE;
	sendCommand(cmd, 4, true);
	if (replybuff[0] == 'M') {
		mode_ = replybuff[1] - '0';
	}
	return mode_;
}

int RIG_TT538::get_modetype(int n)
{
	return TT538mode_type[n];
}

void RIG_TT538::set_bandwidth(int val)
{
	bw_ = val;
	cmd = TT538setBW;
	cmd[2] = 38 - val;
	sendCommand(cmd, 0, true);
	set_if_shift(pbt);
}

int RIG_TT538::get_bandwidth()
{
	cmd = TT538getBW;
	sendCommand(cmd, 3, true);
	if (replybuff[0] == 'W')
		bw_ = 38 - (unsigned char)replybuff[1];
	return bw_;
}

void RIG_TT538::set_if_shift(int val)
{
	pbt = val;
	cmd = TT538setPBT;
	int bpval = progStatus.bpf_center - 200 - TT538_numeric_widths[bw_]/2;
	short int si = val;
	if ((mode_ == 1 || mode_ == 2) && progStatus.use_bpf_center)
		si += (bpval > 0 ? bpval : 0);
	cmd[2] = (si & 0xff00) >> 8;
	cmd[3] = (si & 0xff);
	sendCommand(cmd, 0, true);
	sendCommand(TT538getPBT, 4, true);
	if (replybuff[1] != cmd[2] || replybuff[2] != cmd[3]) {
		sendCommand(cmd, 0, true);
	}
}

bool RIG_TT538::get_if_shift(int &val)
{
	val = 0;
	cmd = TT538getPBT;
	sendCommand(cmd, 4, true);
	return false;
}

void RIG_TT538::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -2000;
	max = 2000;
	step = 10;
}

void RIG_TT538::set_attenuator(int val)
{
	cmd = TT538setATT;
	if (val) cmd[2] = '1';
	else     cmd[2] = '0';
	sendCommand(cmd, 0, true);
}


int RIG_TT538::get_attenuator()
{
	cmd = TT538getATT;
	sendCommand(cmd, 3, true);
	if (replybuff[0] == 'J' && replybuff[1] == '1')
		return 1;
	return 0;
}

int RIG_TT538::get_smeter()
{
	double sig = 0.0;
	cmd = TT538getSMETER;
	sendCommand(cmd, 6, true);
	if (replybuff[0] == 'S') {
		int sval;
		replybuff[5] = 0;
		sscanf(&replybuff[1], "%4x", &sval);
		sig = sval / 256.0;
	}
	return (int)(sig * 50.0 / 9.0);
}

static int gaintable[] = {
0, 2, 3, 4, 6, 7, 8, 9, 11, 12,
13, 14, 16, 17, 18, 19, 21, 22, 23, 24,
26, 27, 28, 30, 31, 32, 33, 35, 36, 37,
38, 40, 41, 42, 43, 45, 46, 47, 48, 50,
51, 52, 53, 55, 56, 57, 58, 60, 61, 62,
64, 65, 66, 68, 69, 70, 71, 72, 74, 75,
76, 77, 79, 80, 81, 82, 84, 85, 86, 88,
89, 90, 91, 92, 94, 95, 96, 98, 99, 100,
101, 103, 104, 105, 107, 108, 109, 110, 111, 113,
114, 115, 117, 118, 119, 120, 121, 123, 127, 125, 127 };

static int pot2val(int n)
{
	int i = 0;
	for (i = 0; i < 100; i++)
		if (gaintable[i] >= n) break;
	return gaintable[i];
}

static int val2pot(int n)
{
	return gaintable[n];
}

int RIG_TT538::get_volume_control()
{
	cmd = TT538getVOL;
	sendCommand(cmd, 3, true);
	if (replybuff[0] == 'U') {
		return  pot2val(replybuff[1] & 0x7F);
	}
	return 0;
}

void RIG_TT538::set_volume_control(int vol)
{
	cmd = TT538setVOL;
	cmd[2] = val2pot(vol);
	sendCommand(cmd, 0, true);
}

void RIG_TT538::set_rf_gain(int val)
{
	cmd = TT538setRF;
	cmd[2] = val2pot(val);
	sendCommand(cmd, 0, true);
}

int  RIG_TT538::get_rf_gain()
{
//	cmd = TT538getRF;
//	sendCommand(cmd, 3, true);
//showresponse(cmd);
	return 100; 
// Jupiter does not reply with values as specified in the programmers manual
// Panel RF gain 0..50% replies with 80H..00H
// Panel RF gain 50..100% replies with 00H
}

// Tranceiver PTT on/off
void RIG_TT538::set_PTT_control(int val)
{
	if (val) sendCommand(TT538setXMT, 0, true);
	else     sendCommand(TT538setRCV, 0, true);
}
