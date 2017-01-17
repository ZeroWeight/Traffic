#include "traffic_v1.h"
#define GetTime int(now_t/10)
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define HEAD(i) (car_in[(i)].first())
#define PERIOD (s->period)
#define Get(i) (s->map[GetTime%s->period][dir (i)][tr (i)])
#define WILL(j,i) (s->map[j%s->period][dir (i)][tr (i)])
static const int S = 10;
static const int _S = 8;
static std::default_random_engine e;
static std::normal_distribution<double> ND_A_A (3, 0.3);
static std::normal_distribution<double> ND_A (1, 0.3);
static std::normal_distribution<double> ND_A_S (0, 0.1);
static std::normal_distribution<double> ND_A_BS (-0.5, 0.3);
void Traffic_v1::strategy () {}
void Traffic_v1::following () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_in[i].empty ()) {
			head (car_in[i].begin (), i);
			QList<Car>::iterator it;
			QList<Car>::iterator itp;
			for (it = car_in[i].end () - 2, itp = car_in[i].end () - 1;
				itp != car_in[i].begin (); --it, --itp) {
				if (it->vec < 10) {
					itp->acc = itp->vec*itp->vec / 2 / (itp->pos - it->pos + 3);
					if (itp->acc > -1) itp->acc = 0;
					if (itp->vec + 0.1*itp->acc <= 0 || itp->vec < 0.1) {
						itp->acc = it->acc; itp->vec = 0;
						if (itp->pos - it->pos + 5 < 0)itp->acc = 1 + it->acc;
					}
					if (itp->vec < 5 && itp->pos - it->pos + S < 0 && it->acc>0 && Get (i) == Green)itp->acc = ND_A (e);
					continue;
				}
				if (it->pos - itp->pos < (S << 2)) {
					if (it->pos - itp->pos < S) {
						itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
							/ 2.0 / (S - it->pos + itp->pos);
					}
					else if (it->vec < itp->vec)
						itp->acc = it->acc + (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (S - it->pos + itp->pos);
					else if (it->vec > itp->vec)
						itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (S - it->pos + itp->pos);
					if (itp->acc < -5) itp->acc = -5;
					if (itp->acc > 5) itp->acc = 5;
					if (itp->vec + 0.1*itp->acc < 0) {
						itp->acc = 0; itp->vec = 0;
					}
				}
				else  free (itp, i);
			}
		}
	}
}
void Traffic_v1::_following () {
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
						/ 2.0 / (S - it->pos + itp->pos);
					else if (it->vec > itp->vec)
						itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (S - it->pos + itp->pos);
					if (itp->acc < -5) itp->acc = -5;
					if (itp->acc > 5) itp->acc = 5;
					if (itp->vec < 5)itp->acc = 0;
				}
				free (itp, i);
			}
		}
	}
}
void Traffic_v1::head (QList<Car>::iterator it, int i) {
	switch (Get (i)) {
	case Green:
		int j;
		for (j = 1 + GetTime; WILL (j, i) == Green && (j - GetTime) % PERIOD; j++);
		if (j - GetTime > 10 || it->pos < -100) {//10 sec remains
			if (it->vec < 5)  it->acc = ND_A_A (e);
			else if (it->vec < 16) it->acc = ND_A (e);
			else if (it->vec < 17) it->acc = ND_A_S (e);
			else it->acc = ND_A_BS (e);
			break;
		}
	case Yellow:case Red:
		if (it->pos > -100) {
			it->acc = it->vec*it->vec / 2 / (it->pos + 0.5);
			if (it->vec + 0.1*it->acc < 0) {
				it->vec = it->acc = 0;
			}
		}
	}
}
void Traffic_v1::free (QList<Car>::iterator it, int i) {
	if (it->vec < 8)  it->acc = ND_A_A (e);
	else if (it->vec < 16) it->acc = ND_A (e);
	else if (it->vec < 17) it->acc = ND_A_S (e);
	else it->acc = ND_A_BS (e);
}