/*
 * TenTec Omni-VII (TT588) drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

/*
 * Note for anyone wishing to expand on the command set.
 *
 * A rejected command is responded to by a three character sequence "Zy\r".
 * where the y is the letter of the command in error
 * you need to check for that response
 *
*/

#include "TT588.h"
#include "support.h"
#include "math.h"

#include "debug.h"

static const char TT588name_[] = "Omni-VII";

//static const char *TT588modes_[] = { "D-USB", "USB", "LSB", "CW", "AM", "FM", NULL}
//static const char TT588mode_chr[] =  { '1', '1', '2', '3', '0', '4' };
//static const char TT588mode_type[] = { 'U', 'U', 'L', 'L', 'U', 'U' };
static const char *TT588modes_[] = {
		"AM", "USB", "LSB", "CWU", "FM", "CWL", "FSK", NULL};
static const char TT588mode_chr[] =  { '0', '1', '2', '3', '4', '5', '6' };
static const char TT588mode_type[] = { 'U', 'U', 'L', 'U', 'U', 'L', 'L' };

// filter # is 37 - index
static const char *TT588_widths[] = {
"200",   "250",  "300",  "350",  "400",  "450",  "500",  
"600",  "700",  "800", "900",  "1000", 
"1200", "1400", "1600", "1800", "2000", "2200", "2400", 
"2500", "2600", 
"2800", "3000", "3200", "3400", "3600", "3800", "4000", 
"4500", "5000", "5500", "6000", "6500", "7000", "7500", "8000", 
"9000", "12000", NULL};

static const int TT588_numeric_widths[] = {
200,   250,  300,  350,  400,  450,  500,
600,  700,  800,  900,  1000, 
1200, 1400, 1600, 1800, 2000, 2200, 2400, 
2500, 2600, 
2800, 3000, 3200, 3400, 3600, 3800, 4000,
4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000,
9000, 12000, NULL};


static char TT588setFREQA[]		= "*Annnn\r";
static char TT588setFREQB[]		= "*Bnnnn\r";
//static char TT588setAGC[]		= "*Gn\r";
static char TT588setSQLCH[]		= "*Hc\r";
static char TT588setRF[]		= "*Ic\r";
static char TT588setATT[]		= "*Jc\r";
static char TT588setNB[]		= "*Knar\r";
static char TT588setMODE[]		= "*Mnn\r";
static char TT588setSPLIT[]		= "*Nn\n";
static char TT588setPBT[]		= "*Pxx\r";
static char TT588setVOL[]		= "*Un\r";
static char TT588setBW[]		= "*Wx\r";

static char TT588getFREQA[]		= "?A\r";
static char TT588getFREQB[]		= "?B\r";
//static char TT588getAGC[]		= "?G\r";
static char TT588getSQLCH[]		= "?H\r";
static char TT588getRF[]		= "?I\r";
static char TT588getATT[]		= "?J\r";
static char TT588getNB[]		= "?K\r";
static char TT588getMODE[]		= "?M\r";
static char TT588getSPLIT[]		= "?N\r";
static char TT588getPBT[]		= "?P\r";
static char TT588getSMETER[]	= "?S\r";
static char TT588getVOL[]		= "?U\r";
static char TT588getBW[]		= "?W\r";

static char TT588getFWDPWR[]	= "?F\r";

//static char TT588getREFPWR[]	= "?R\r";

static char TT588setXMT[]		= "*Tnn\r";

RIG_TT588::RIG_TT588() {
// base class values
	name_ = TT588name_;
	modes_ = TT588modes_;
	bandwidths_ = TT588_widths;
	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 20;
	comm_timeout = 50;
	comm_echo = false;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;// false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	serloop_timing = 200;

	def_mode = modeB = modeA = A.imode = B.iBW = 1;
	def_bw = bwB = bwA = A.iBW = B.iBW = 25;
	def_freq = freqB = freqA = A.freq = B.freq = 14070000;
	max_power = 100;
	pbt = 0;
	VfoAdj = progStatus.vfo_adj;
	vfo_corr = 0;

	atten_level = 0;
	nb_ = 0;
	an_ = 0;

	has_bpf_center =
	has_power_control =
	has_micgain_control =
	has_preamp_control =
	has_tune_control =
	has_noise_control =
	has_swr_control = 
	has_vfo_adj = false;

	can_change_alt_vfo =

	has_auto_notch =
	has_notch_control = 

	has_split =
	has_smeter =
	has_power_out =
	has_swr_control =
	has_volume_control =
	has_rf_control =
	has_attenuator_control =
	has_ifshift_control =
	has_sql_control =
	has_ptt_control =
	has_bandwidth_control =
	has_auto_notch = 
	has_noise_control =
	has_mode_control = true;

}

void RIG_TT588::initialize()
{
	VfoAdj = progStatus.vfo_adj;
}

void RIG_TT588::shutdown()
{
	set_if_shift(0);
}

long RIG_TT588::get_vfoA ()
{
	cmd = TT588getFREQA;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get vfoA", cmd, replystr);
	if (ret >= 6) {
		size_t p = replystr.rfind("A");
		if (p != string::npos) {
			int f = 0;
			for (size_t n = 1; n < 5; n++)
				f = f*256 + (unsigned char)replystr[p + n];
			freqA = f;
		}
	}
	return (long)(freqA - vfo_corr);
}

void RIG_TT588::set_vfoA (long freq)
{
	freqA = freq;
	vfo_corr = (freq / 1e6) * VfoAdj + 0.5;
	long xfreq = freqA + vfo_corr;
	cmd = TT588setFREQA;
	cmd[5] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[4] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[3] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[2] = xfreq & 0xff;
	sendCommand(cmd, 0);
	LOG_WARN ("%s", str2hex(cmd.c_str(), cmd.length()));
	return ;
}

long RIG_TT588::get_vfoB()
{
	cmd = TT588getFREQB;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get vfoB", cmd, replystr);
	if (ret >= 6) {
		size_t p = replystr.rfind("A");
		if (p != string::npos) {
			int f = 0;
			for (size_t n = 1; n < 5; n++)
				f = f*256 + (unsigned char)replystr[p + n];
			freqA = f;
		}
	}
	return (long)(freqB - vfo_corr);
}

void RIG_TT588::set_vfoB (long freq)
{
	freqB = freq;
	vfo_corr = (freq / 1e6) * VfoAdj + 0.5;
	long xfreq = freqB + vfo_corr;
	cmd = TT588setFREQB;
	cmd[5] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[4] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[3] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[2] = xfreq & 0xff;
	sendCommand(cmd, 0);
	LOG_WARN ("%s", str2hex(cmd.c_str(), cmd.length()));
	return ;
}

void RIG_TT588::setVfoAdj(double v)
{
	VfoAdj = v;
}

void RIG_TT588::set_modeA(int val)
{
	modeA = val;
	cmd = TT588setMODE;
	cmd[2] = cmd[3] = TT588mode_chr[val];
	sendCommand(cmd, 0);
	LOG_WARN ("%s", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_TT588::get_modeA()
{
	cmd = TT588getMODE;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get mode A", cmd, replystr);
	if (ret < 4) return modeA;
	size_t p = replystr.rfind("M");
	if (p == string::npos) return modeA;
	modeA = replystr[p + 1] - '0';
	return modeA;
}

int RIG_TT588::get_modetype(int n)
{
	return TT588mode_type[n];
}

void RIG_TT588::set_bwA(int val)
{
	bwA = val;
	cmd = TT588setBW;
	cmd[2] = 37 - val;
	sendCommand(cmd, 0);
	LOG_WARN ("%s", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_TT588::get_bwA()
{
	cmd = TT588getBW;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get bw A", cmd, replystr);
	if (ret < 3) return bwA;
	size_t p = replystr.rfind("W");
	if (p == string::npos) return bwA;
	bwA = 37 - (unsigned char)replystr[p + 1];
	return bwA;
}

int  RIG_TT588::adjust_bandwidth(int m)
{
	if (m == 0) return 31;
	if (m == 1 || m == 2) return 22;
	if (m == 3 || m == 5) return 7;
	if (m == 4 || m == 6) return 22;
	return 22;
}

void RIG_TT588::set_if_shift(int val)
{
	pbt = val;
	cmd = TT588setPBT;
	short int si = val;
	cmd[2] = (si & 0xff00) >> 8;
	cmd[3] = (si & 0xff);
	sendCommand(cmd, 0);
	LOG_WARN ("%s", str2hex(cmd.c_str(), cmd.length()));
}

bool RIG_TT588::get_if_shift(int &val)
{
	cmd = TT588getPBT;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get att", cmd, replystr);
	val = pbt;
	if (ret >= 4) {
		size_t p = replystr.rfind("P");
		if (p != string::npos) {
			val = ((replystr[p + 1] & 0xFF) << 8) | (replystr[p+2] & 0xFF);
			if (val > 0x7FFF) val -= 0x10000;
		}
	}
	return (val == 0 ? false : true);
}

void RIG_TT588::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -2500;
	max = 2500;
	step = 10;
}

void RIG_TT588::set_attenuator(int val)
{
	cmd = TT588setATT;
	atten_level++;
	if (atten_level == 4) atten_level = 0;
	cmd[2] = '0' + atten_level;
	switch (atten_level) {
		case 0: atten_label("0 dB", false); break;
		case 1: atten_label("6 dB", true); break;
		case 2: atten_label("12 dB", true); break;
		case 3: atten_label("18 dB", true); break;
	}
	sendCommand(cmd, 0);
	LOG_WARN ("%s", str2hex(cmd.c_str(), cmd.length()));
}


int RIG_TT588::get_attenuator()
{
	cmd = TT588getATT;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get att", cmd, replystr);
	int val = atten_level;
	if (ret >= 3) {
		size_t p = replystr.rfind("J");
		if (p != string::npos) val = (replystr[p + 1] - '0');
	}
	if (atten_level != val) atten_level = val;
	switch (atten_level) {
		case 0: atten_label("0 dB", false); break;
		case 1: atten_label("6 dB", true); break;
		case 2: atten_label("12 dB", true); break;
		case 3: atten_label("18 dB", true); break;
	}
	return atten_level;
}

int RIG_TT588::get_smeter()
{
	int sval = 0;
	float fval = 0;
	cmd = TT588getSMETER;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get smeter", cmd, replystr);
	if (ret < 6) return sval;
	size_t p = replystr.rfind("S");
	if (p == string::npos) return sval;

	sscanf(&replystr[p + 1], "%4x", &sval);
	fval = sval/256.0;
	sval = (int)(fval * 100.0 / 18.0);
	if (sval > 100) sval = 100;

	return sval;
}

int RIG_TT588::get_volume_control()
{
	cmd = TT588getVOL;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get vol", cmd, replystr);
	if (ret < 3) return 0;
	size_t p = replystr.rfind("U");
	if (p == string::npos) return 0;

	return (int)((replystr[p + 1] & 0x7F) / 1.27);
}

void RIG_TT588::set_volume_control(int vol)
{
	cmd = TT588setVOL;
	cmd[2] = 0x7F & (int)(vol * 1.27);
	sendCommand(cmd, 0);
}

void RIG_TT588::set_rf_gain(int val)
{
	cmd = TT588setRF;
	cmd[2] = 0x7F & (int)(val * 1.27);
	sendCommand(cmd, 0);
}

int  RIG_TT588::get_rf_gain()
{
	cmd = TT588getRF;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get rfgain", cmd, replystr);
	if (ret < 3) return 100;
	size_t p = replystr.rfind("I");
	if (p == string::npos) return 100;
	return (int)((replystr[p+1] & 0x7F) / 1.27);
}

// Tranceiver PTT on/off

void RIG_TT588::set_PTT_control(int val)
{
	cmd = TT588setXMT;
	if (val) {
		cmd[2] = 0x04;
		cmd[3] = 0;
	} else {
		cmd[2] = 0;
		cmd[3] = 0;
	}
	sendCommand(cmd, 0);
}

int RIG_TT588::get_power_out()
{
	cmd = TT588getFWDPWR;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get pout", cmd, replystr);
	if (ret <  6) return 0;
	size_t p = replystr.rfind("F");
	if (p == string::npos) return 0;
	fwdpwr = replystr[p + 1] & 0x7F;
	refpwr = replystr[p + 2];
	fwdv = sqrtf(fwdpwr);
	refv = sqrtf(refpwr);
	return fwdpwr;
}

void RIG_TT588::set_squelch(int val)
{
	cmd = TT588setSQLCH;
	cmd[2] = (unsigned char)(val * 1.27);
	sendCommand(cmd);
}

int  RIG_TT588::get_squelch()
{
	cmd = TT588getSQLCH;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get sql", cmd, replystr);
	if (ret < 3) return 0;
	size_t p = replystr.rfind("H");
	if (p == string::npos) return 0;
	return (int)((replystr[p+1] & 0x7F) / 1.27);
}

void RIG_TT588::set_noise(bool val)
{
	static char nblabel[] = "NB ";
	nb_++;
	if (nb_ == 8) nb_ = 0;
	nblabel[2] = '0' + nb_;
	nb_label(nblabel, nb_ ? true : false);
	cmd = TT588setNB;
	cmd[2] = (unsigned char)nb_;
	cmd[3] = 0;
	cmd[4] = (unsigned char)an_;
	sendCommand(cmd);
	LOG_WARN ("%s", str2hex(cmd.c_str(), cmd.length()));
}

int  RIG_TT588::get_noise()
{
	cmd = TT588getNB;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get sql", cmd, replystr);
	if (ret < 5) return nb_;
	size_t p = replystr.rfind("K");
	if (p == string::npos) return nb_;
	int val = replystr[p+1];
	if (nb_ != val) nb_ = val;
	static char nblabel[] = "NB ";
	nblabel[2] = '0' + nb_;
	nb_label(nblabel, nb_ ? true : false);
	return nb_;
}

void RIG_TT588::set_auto_notch(int val)
{
	static char anlabel[] = "AN ";
	an_++;
	if (an_ == 10) an_ = 0;
	anlabel[2] = '0' + an_;
	auto_notch_label(anlabel, an_ > 0 ? true : false);
	cmd = TT588setNB;
	cmd[2] = (unsigned char)nb_;
	cmd[3] = 0;
	cmd[4] = (unsigned char)an_;
	sendCommand(cmd);
}

int  RIG_TT588::get_auto_notch()
{
	cmd = TT588getNB;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get AN", cmd, replystr);
	int val = an_;
	if (ret >= 5) {
		size_t p = replystr.rfind("K");
		if (p != string::npos) val = replystr[p+3];
	}
	if (an_ != val) an_ = val;
	static char anlabel[] = "AN ";
	anlabel[2] = '0' + an_;
	auto_notch_label(anlabel, an_ ? true : false);
	return an_;
}

void RIG_TT588::set_split(bool val)
{
	cmd = TT588setSPLIT;
	cmd[2] = val == 0 ? 0 : 1;
	sendCommand(cmd);
	LOG_WARN ("%s", str2hex(cmd.c_str(), cmd.length()));
}

bool RIG_TT588::get_split()
{
	cmd = TT588getSPLIT;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get split", cmd, replystr);
}

