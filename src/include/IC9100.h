#ifndef _IC9100_H
#define _IC9100_H

#include "IC746.h"

class RIG_IC9100 : public RIG_IC746PRO {
public:
	RIG_IC9100();
	~RIG_IC9100(){}

	void initialize();

	void selectA();
	void selectB();

	void set_modeA(int val);
	int  get_modeA();

	void set_modeB(int val);
	int  get_modeB();

	void set_bwA(int val);
	int  get_bwA();

	void set_bwB(int val);
	int  get_bwB();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);

	void set_attenuator( int val );
	int  get_attenuator();
	void set_mic_gain(int v);

	void set_compression();
	void get_comp_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 10; step = 1; }

	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();
	void get_vox_onoff();
	void get_vox_gain();
	void get_vox_anti();
	void get_vox_hang();
	void get_vox_hang_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 200; step = 10; }

	void get_cw_wpm_min_max(int &min, int &max) {
		min = 6; max = 60; }

	void get_cw_qsk_min_max_step(double &min, double &max, double &step) {
		min = 2.0; max = 13.0; step = 0.1; }

	void get_cw_spot_tone_min_max_step(int &min, int &max, int &step) {
		min = 300; max = 900; step = 5; }

	void set_cw_wpm();
	void set_cw_qsk();
	void set_cw_spot_tone();
	void set_cw_vol();

	void set_split(bool v);
	int  get_split();

	int get_smeter();
	int get_power_out();
	int get_swr();
	int get_alc();

	int  get_auto_notch();
	void set_auto_notch(int v);
	void set_notch(bool on, int val);
	bool get_notch(int &val);
	void get_notch_min_max_step(int &min, int &max, int &step);

};

#endif
