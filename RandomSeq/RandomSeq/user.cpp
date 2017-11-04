#include "traffic.h"

static const int S = 15;
static const double a_max = 5;
static const double c = 0.7;
static const double d = 5;

static std::default_random_engine e;
static std::normal_distribution<double> ND_A_A (3, 0.3);
static std::normal_distribution<double> ND_A (1, 0.3);
static std::normal_distribution<double> ND_A_S (0, 0.1);
static std::normal_distribution<double> ND_A_BS (-0.5, 0.3);
void Traffic::following () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (car_in[i].empty ()) continue;
		head (car_in[i].begin (), i);
		QList<Car>::iterator it;
		QList<Car>::iterator itp;
		for (it = car_in[i].end () - 2, itp = car_in[i].end () - 1;
			itp != car_in[i].begin (); --it, --itp) {
			if (it->pos - itp->pos < ((d + c*itp->vec) * 2)) {
				//slow down
				if (it->pos - itp->pos < (d + c*it->vec))
					itp->acc = -5;
				//car following mode
				else if (it->vec < itp->vec)
					itp->acc = it->acc + 1.1* (itp->vec - it->vec)*(itp->vec - it->vec)
					/ 2.0 / ((d + c*it->vec) - it->pos + itp->pos);
				else if (it->vec > itp->vec)
					itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
					/ 2.0 / ((d + c*it->vec) - it->pos + itp->pos);
				//sat.
				if (itp->acc < -5) itp->acc = -5;
				if (itp->acc > 5) itp->acc = 5;
				//stop
				if (itp->vec + 0.1*itp->acc < 0) {
					itp->acc = 0;
					itp->vec = 0;
				}
			}
			else  free (itp, i);
		}
	}
}

void Traffic::head (QList<Car>::iterator it, int i) {
	switch (Get (i)) {
	case Green:
		free (it, i);
		break;
	case Yellow:case Red:
		if (it->pos > -30) {
			it->acc = -abs (it->vec*it->vec / (2 * (it->pos + 1)));
		}
		else {
			free (it, i);
		}
	}
	if (it->vec + 0.1*it->acc <= 0) {
		it->vec = 0;
		it->acc = 0;
	}
}
void Traffic::free (QList<Car>::iterator it, int i) {
	if (it->vec < 8)  it->acc = ND_A_A (e);
	else if (it->vec < 15) it->acc = ND_A (e);
	else if (it->vec < 17) it->acc = ND_A_S (e);
	else it->acc = ND_A_BS (e);
}