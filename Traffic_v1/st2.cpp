#include "traffic_v1.h"
#define head car_in[i].first()
#define GetTime (int(now_t/10))
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define HEAD(i) (car_in[(i)].first())
#define PERIOD (s->period)
#define Get(i) (s->map[GetTime%s->period][dir (i)][tr (i)])
#define WILL(j,i) (s->map[(j)%s->period][dir (i)][tr (i)])
static const int penalty_time = 20;
static const double A_max_ = 3.0;
static const double V_max = 20.0;
static const double V_min = 10.0;
static const double A_max = 5.0;

void Traffic_v1::st2 () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_in[i].empty ()) {
			/*-------------------- Calc Head -------------------*/
			double MinT = CalMinTime (-head.pos, head.vec);
			double MaxT = CalMaxTime (-head.pos, head.vec);
		}
	}
}