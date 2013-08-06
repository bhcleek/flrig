/*
 * Yaesu FT-2000 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */


#include "FT2000.h"
#include "debug.h"
#include "support.h"

static const char FT2000name_[] = "FT-2000";

static const char *FT2000modes_[] = {
"LSB", "USB", "CW", "FM", "AM", "RTTY-L", "CW-R", "PKT-L", "RTTY-U", "PKT-FM", "FM-N", "PKT-U", NULL};

static const char FT2000_mode_chr[] =  { 
'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C' };
static const char FT2000_mode_type[] = { 
'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U', 'U' };

static const char *FT2000_SSBwidths[] = {
"NORM", "200", "400", "600", "850", "1100", "1350", "1500", "1650", "1800", "1950", "2100", "2250", NULL};
static const char *FT2000_CWwidths[] = {
"NORM", "50", "100", "200", "300", "400", "500", "800", "1200", "1400", "1700", "2000", NULL};
static const char *FT2000_PKT_RTTYwidths[] = {
"NORM", "25", "50", "100", "200", "300", "400", NULL};
static const char *FT2000_AMFMwidths[] = {
"NORM", "NARR", NULL};

static const char *FT2000_US_60m[] = {NULL, "126", "127", "128", "130", NULL};
// US has 5 60M presets. Using dummy numbers for all.
// First NULL means skip 60m sets in set_band_selection().
// Maybe someone can do a cat command MC; on all 5 presets and add returned numbers above.
// To send cat commands in flrig goto menu Config->Xcvr select->Send Cmd.

static const char **Channels_60m = FT2000_US_60m;

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)btnNotch,    214, 125,  50 },
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN,  54, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,   266, 145, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

void RIG_FT2000::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = btnIFsh;
	rig_widgets[3].W = sldrIFSHIFT;
	rig_widgets[4].W = btnNotch;
	rig_widgets[5].W = sldrNOTCH;
	rig_widgets[6].W = sldrMICGAIN;
	rig_widgets[7].W = sldrPOWER;
}

RIG_FT2000::RIG_FT2000() {
// base class values	
	name_ = FT2000name_;
	modes_ = FT2000modes_;
	bandwidths_ = FT2000_SSBwidths;

	widgets = rig_widgets;

	comm_baudrate = BR4800;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 2;
	def_mode = 1;
	def_bw = 2;
	def_freq = 14070000;

	has_band_selection =
	has_extras =
	has_smeter =
	has_power_out =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_noise_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control =
	has_swr_control = true;
	
// derived specific
	notch_on = false;

	atten_level = 0;
	preamp_level = 0;
	m_60m_indx = 0;

	precision = 10;
	ndigits = 7;

}

void RIG_FT2000::set_band_selection(int v)
{
	int inc_60m = false;
	cmd = "IF;";
	waitN(27, 100, "get vfo mode in set_band_selection", ASC);
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return;
	if (replystr[p+21] != '0') {	// vfo 60M memory mode
		inc_60m = true;
	}

	if (v == 12) {	// 5MHz 60m presets
		if (Channels_60m[0] == NULL) return;	// no 60m Channels so skip
		if (inc_60m) {
			if (Channels_60m[++m_60m_indx] == NULL)
				m_60m_indx = 0;
		}
		cmd.assign("MC").append(Channels_60m[m_60m_indx]).append(";");
	} else {		// v == 1..11 band selection OR return to vfo mode == 0
		if (inc_60m)
			cmd = "VM;";
		else {
			if (v < 3)
				v = v - 1;
			cmd.assign("BS").append(to_decimal(v, 2)).append(";");
		}
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "Select Band Stacks", cmd, replystr);
}

long RIG_FT2000::get_vfoA ()
{
	cmd = "FA;";
	int ret = sendCommand(cmd);
	if (ret < 11) return freqA;
	size_t p = replystr.rfind("FA");
	if (p == string::npos) return freqA;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;

	return freqA;
}

void RIG_FT2000::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0);
}

int RIG_FT2000::get_smeter()
{
	cmd = "SM0;";
	int ret = sendCommand(cmd);
	if (ret < 7) return 0;
	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;
	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 3]);
	mtr = mtr * 100.0 / 256.0;
	return mtr;
}

int RIG_FT2000::get_swr()
{
	cmd = "RM6;";
	int ret = sendCommand(cmd);
	if (ret < 7) return 0;
	size_t p = replystr.rfind("RM");
	if (p == string::npos) return 0;
	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 3]);
	return mtr / 2.56;
}

int RIG_FT2000::get_power_out()
{
	cmd = "RM5;";
	int ret = sendCommand(cmd);
	if (ret < 7) return 0;
	size_t p = replystr.rfind("RM");
	if (p == string::npos) return 0;
	replystr[p + 6] = 0;
	double mtr = (double)(atoi(&replystr[p + 3]));
	mtr = -6.6263535 + .11813178 * mtr + .0013607405 * mtr * mtr;
	return (int)mtr;
}

int RIG_FT2000::get_power_control()
{
	cmd = "PC;";
	int ret = sendCommand(cmd);
	if (ret < 6) return 0;
	size_t p = replystr.rfind("PC");
	if (p == string::npos) return 0;
	replystr[p + 5] = 0;
	int mtr = atoi(&replystr[p + 2]);
	return mtr;
}

// Volume control return 0 ... 100
int RIG_FT2000::get_volume_control()
{
	cmd = "AG0;";
	int ret = sendCommand(cmd);
	if (ret < 7) return 0;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;
	replystr[p + 6] = 0;
	int val = atoi(&replystr[p + 3]);
	return (int)(val / 2.55);
}

void RIG_FT2000::set_volume_control(int val) 
{
	int ivol = (int)(val * 2.55);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd, 0);
}

// Transceiver power level
void RIG_FT2000::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd, 0);
}

// Tranceiver PTT on/off
void RIG_FT2000::set_PTT_control(int val)
{
	if (val) sendCommand("TX1;", 0);
	else	 sendCommand("TX0;", 0);
}

void RIG_FT2000::tune_rig()
{
	sendCommand("AC002;",0);
}

void RIG_FT2000::set_attenuator(int val)
{
	switch (atten_level) {
		case 0 :
			atten_level = 1;
			atten_label("6 dB", true);
			cmd = "RA01;";
			break;
		case 1 :
			atten_level = 2;
			atten_label("12 dB", true);
			cmd = "RA02;";
			break;
		case 2 :
			atten_level = 3;
			atten_label("18 dB", true);
			cmd = "RA03;";
			break;
		case 3 :
		default :
			atten_level = 0;
			atten_label("Att", false);
			cmd = "RA00;";
	}
	sendCommand(cmd, 0);
}

int RIG_FT2000::get_attenuator()
{
	cmd = "RA0;";
	int ret = sendCommand(cmd);
	if (ret < 5) {
		atten_label("Att", false);
		return 0;
	}
	size_t p = replystr.rfind("RA");
	if (p == string::npos) {
		atten_label("Att", false);
		return 0;
	}
	int reply = replystr[p + 3];
	switch (reply) {
		case '1' :
			atten_label("6 dB", true);
			atten_level = 1;
			break;
		case '2' :
			atten_label("12 dB", true);
			atten_level = 2;
			break;
		case '3' :
			atten_label("18 dB", true);
			atten_level = 3;
			break;
		default :
			atten_label("Att", false);
			atten_level = 0;
			break;
	}
	return atten_level;
}

void RIG_FT2000::set_preamp(int val)
{
	switch (preamp_level) {
		case 0 :
			preamp_level = 1;
			preamp_label("Pre 1", true);
			cmd = "PA01;";
			break;
		case 1 :
			preamp_level = 2;
			preamp_label("Pre 2", true);
			cmd = "PA02;";
			break;
		case 2 :
		default :
			preamp_level = 0;
			preamp_label("Pre", false);
			cmd = "PA00;";
	}
	sendCommand(cmd, 0);
}

int RIG_FT2000::get_preamp()
{
	cmd = "PA0;";
	int ret = sendCommand(cmd);
	if (ret < 5) {
		preamp_label("Pre", false);
		return 0;
	}
	size_t p = replystr.rfind("PA");
	if (p == string::npos) {
		preamp_label("Pre", false);
		return 0;
	}
	int reply = replystr[p + 3];
	switch (reply) {
		case '1' :
			preamp_level = 1;
			preamp_label("Pre 1", true);
			break;
		case '2' :
			preamp_level = 2;
			preamp_label("Pre 2", true);
			break;
		case '0' :
		default :
			preamp_level = 0;
			preamp_label("Pre", false);
	}
	return preamp_level;
}


void RIG_FT2000::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0";
	cmd += FT2000_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
}

int RIG_FT2000::get_modeA()
{
	cmd = "MD0;";
	int ret = sendCommand(cmd);
	if (ret < 5) return modeA;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return modeA;
	modeA = replystr[p + 3] - '1';
	return modeA;
}

int RIG_FT2000::adjust_bandwidth(int m)
{
	switch (m) {
		case 0 : case 1 :
			bandwidths_ = FT2000_SSBwidths;
			bwA = 0;
			break;
		case 2 : case 6 :
			bandwidths_ = FT2000_CWwidths;
			bwA = 0;
			break;
		case 5 : case 7 : case 8 : case 11 :
			bandwidths_ = FT2000_PKT_RTTYwidths;
			bwA = 0;
			break;
		case 3 : case 4 : case 9 : case 10 :
			bandwidths_ = FT2000_AMFMwidths;
			bwA = 0;
			break;
	}
	return bwA;
}

int RIG_FT2000::def_bandwidth(int m)
{
	return 0;
}

const char **RIG_FT2000::bwtable(int m)
{
	switch (m) {
		case 0 : case 1 :
			return FT2000_SSBwidths;
			break;
		case 2 : case 6 :
			return FT2000_CWwidths;
			break;
		case 5 : case 7 : case 8 : case 11 :
			return FT2000_PKT_RTTYwidths;
			break;
		case 3 : case 4 : case 9 : case 10 :
			return FT2000_AMFMwidths;
			break;
	}
	return FT2000_SSBwidths;
}

void RIG_FT2000::set_bwA(int val)
{
	bwA = val;

	if (val == 0) {
		sendCommand("NA00;", 0);
		return;
	}

	val--;
	switch (modeA) {
//SSB
		case 0 : case 1 :
			cmd = "EX106";
			if (val > 9) cmd += '1';
			else         cmd += '0';
			val %= 10;
			cmd += val + '0';
			cmd += ';';
			sendCommand(cmd, 0);
			break;
//CW
		case 2 : case 6 :
			cmd = "EX097";
			if (val > 9) cmd += '1';
			else         cmd += '0';
			val %= 10;
			cmd += val + '0';
			cmd += ';';
			sendCommand(cmd, 0);
			break;
//PKT-RTTY
		case 5 : case 7 : case 8 : case 11 :
			cmd = "EX1030";
			cmd += val + '0';
			cmd += ';';
			sendCommand(cmd, 0);
			break;
//AM-FM
		case 3 : case 4 : case 9 : case 10 :
			break;
	}
	sendCommand("NA10;", 0);
}

int RIG_FT2000::get_bwA()
{
	return bwA;
}

int RIG_FT2000::get_modetype(int n)
{
	return FT2000_mode_type[n];
}

void RIG_FT2000::set_if_shift(int val)
{
	cmd = "IS0+0000;";
	if (val < 0) cmd[3] = '-';
	val = abs(val);
	for (int i = 4; i > 0; i--) {
		cmd[3+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd, 0);
}

bool RIG_FT2000::get_if_shift(int &val)
{
	static int oldval = 0;
	int ret = sendCommand("IS0;");
	if (ret < 9) return false;
	size_t p = replystr.rfind("IS");
	if (p == string::npos) return false;

	replystr[p + 8] = 0;
	val = atoi(&replystr[p + 3]);
	if (val != 0 || oldval != val) {
		oldval = val;
		return true;
	}
	oldval = val;
	return false;
}

void RIG_FT2000::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

void RIG_FT2000::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (on == false) {
		sendCommand(cmd, 0);
		notch_on = false;
		return;
	}
	if (!notch_on) {
		cmd[6] = '1'; // notch ON
		sendCommand(cmd, 0);
		cmd[6] = '0';
		notch_on = true;
	}
	cmd[3] = '1'; // manual NOTCH position
// set notch value offset by 200, ie: 001 -> 400
	val = (-val / 9) + 200;
	if (val < 1) val = 1;
	if (val > 400) val = 400;
	for (int i = 3; i > 0; i--) {
		cmd[3 + i] += val % 10;
		val /=10;
	}
	sendCommand(cmd, 0);
}

bool  RIG_FT2000::get_notch(int &val)
{
	bool ison = false;
	cmd = "BP00;";
	int ret = sendCommand(cmd);
	if (ret < 8) return ison;
	size_t p = replystr.rfind("BP");
	if (p == string::npos) return ison;

	if (replystr[p + 6] == '1') {
		ison = true;
		cmd = "BP01;";
		ret = sendCommand(cmd);
		if (ret < 8) return ison;
		p = replystr.rfind("BP");
		if (p == string::npos) return ison;
		replystr[p + 7] = 0;
		val = atoi(&replystr[p + 4]);
		val -= 200;
		val *= -9;
	}
	return ison;
}

void RIG_FT2000::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1143;
	max = +1143;
	step = 9;
}

void RIG_FT2000::set_noise(bool b)
{
	if (b)
		cmd = "NB01;";
	else
		cmd = "NB00;";
	sendCommand(cmd, 0);
}

// val 0 .. 100
void RIG_FT2000::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 2.50);
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
}

int RIG_FT2000::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	waitN(6, 100, "get mic", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.mic_gain;
	int val = atoi(&replystr[p+2]);
	val = (int)(val / 2.50);
	if (val > 100) val = 100;
	return ceil(val);
}

void RIG_FT2000::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}
