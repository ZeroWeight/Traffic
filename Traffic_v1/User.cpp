#include "traffic_v1.h"
#define GetTime int(now_t/10)
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define HEAD(i) (car_in[(i)].first())
static const int MAX_VEC = 25;
static const int MAX_ACC = 5;
bool Traffic_v1::check () {
	return !(std::rand () & 63);
}
void Traffic_v1::strategy () {
	qDebug () << "Hello";
	foreach (Car _car_, car_in[0]) {
		qDebug () << _car_.vec << endl;
	}
}
void Traffic_v1::emergency () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_in[i].empty ()) {
			if (abs (HEAD (i).vec) < 0.5) HEAD (i).vec = HEAD (i).acc = 0.0;
			if (s->map[GetTime][dir (i)][tr (i)] == Red&& HEAD (i).pos > -20 && HEAD (i).vec > 0.5)
				HEAD (i).acc = -5;
		}
	}
}