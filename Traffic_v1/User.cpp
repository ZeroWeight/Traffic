#include "traffic_v1.h"
#define GetTime int(now_t/10)
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define HEAD(i) (car_in[(i)].first())
#define Get(i) (s->map[GetTime][dir (i)][tr (i)])
static const int MAX_VEC = 25;
static const int MAX_ACC = 5;
static const int S = 10;
static const int _S = 8;
static std::default_random_engine e;
static std::normal_distribution<double> ND_A (1.5, 0.3);
void Traffic_v1::strategy () {}
void Traffic_v1::following () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_in[i].empty ()) {
			QList<Car>::iterator it;
			QList<Car>::iterator itp;
			for (it = car_in[i].end () - 2, itp = car_in[i].end () - 1;
				itp != car_in[i].begin (); --it, --itp) {
				if (it->pos - itp->pos < (S << 2)) {
					if (it->pos - itp->pos < S)
						itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (S - it->pos + itp->pos);
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
				else {
					itp->acc = ND_A (e);
					while (itp->acc < 0.01 || itp->acc > 2.5)itp->acc = ND_A (e);
				}
			}
		}
	}
}
void Traffic_v1::_following () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_out[i].empty ()) {
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
				else {
					itp->acc = 0;
				}
			}
		}
	}
}