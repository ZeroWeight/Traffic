#include "traffic.h"
const double Traffic::expdf (const double lambda) {
	double pV = 1;
	while (pV == 1)
		pV = double (rand ()) / double (RAND_MAX);
	pV = (-1.0 / lambda)*log (1 - pV);
	return pV;
}
const double Traffic::CalMinTime (const double pos, const double vec) {
	double time;
	double pos_cri = (V_max*V_max - vec*vec) / (2 * A_max);
	if (pos <= pos_cri) {
		double vec_ = sqrt (vec*vec + 2 * A_max*pos);//vec at the terminal
		time = (vec_ - vec) / A_max;
	}
	else {
		double time1 = (V_max - vec) / A_max;
		double time2 = (pos - pos_cri) / V_max;
		time = time1 + time2;
	}
	return time;
}

void Traffic::_following () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_out[i].empty ()) {
			free (car_out[i].begin (), i);
			QList<Car>::iterator it;
			QList<Car>::iterator itp;
			for (it = car_out[i].end () - 2, itp = car_out[i].end () - 1;
				itp != car_out[i].begin (); --it, --itp) {
				if (it->pos - itp->pos < (_S << 2)) {
					if (it->pos - itp->pos < _S)
						itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (_S - it->pos + itp->pos);
					else if (it->vec < itp->vec)
						itp->acc = it->acc + (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (_S - it->pos + itp->pos);
					else if (it->vec > itp->vec)
						itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (_S - it->pos + itp->pos);
					if (itp->acc < -5) itp->acc = -5;
					if (itp->acc > 5) itp->acc = 5;
					if (itp->vec < 5)itp->acc = 0;
				}
				free (itp, i);
			}
		}
	}
}
Traffic::~Traffic () {}