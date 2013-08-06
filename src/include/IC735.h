#ifndef _IC735_H
#define _IC735_H

#include "ICbase.h"

class RIG_IC735 : public RIG_ICOM {
public:
	RIG_IC735();
	~RIG_IC735(){}

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	void set_bwA(int val);
};

#endif
