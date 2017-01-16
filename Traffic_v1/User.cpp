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
static std::normal_distribution<double> ND_A (2.5, 0.8);
bool Traffic_v1::check () {
	return !(std::rand () & 63);
}
void Traffic_v1::strategy () {}
void Traffic_v1::following () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_in[i].empty ()) {
			QList<Car>::iterator it;
			QList<Car>::iterator itp;
			for (it = car_in[i].end () - 2, itp = car_in[i].end () - 1;
				itp != car_in[i].begin (); --it, --itp) {
				if (it->pos - itp->pos < (S << 2))
					itp->acc = it->acc + (itp->vec - it->vec)*(itp->vec - it->vec)
					/ 2.0 / (S - it->pos + itp->pos);
				else itp->acc = ND_A (e);
				while (itp->acc < 0.1)itp->acc = ND_A (e);
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
				if (it->pos - itp->pos < (_S << 2))
					itp->acc = it->acc + (itp->vec - it->vec)*(itp->vec - it->vec)
					/ 2.0 / (_S - it->pos + itp->pos);
				else itp->acc = ND_A (e);
				while (itp->acc < 0.1)itp->acc = ND_A (e);
			}
		}
	}
}