#ifndef _IC728_H
#define _IC728_H

#include "ICbase.h"

class RIG_IC728 : public RIG_ICOM {
public:
	RIG_IC728();
	~RIG_IC728(){}

	long get_vfoA();
	void set_vfoA(long);

	int  adjust_bandwidth(int m) { return 1; }

};

#endif
