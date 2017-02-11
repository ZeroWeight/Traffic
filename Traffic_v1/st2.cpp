#include "traffic_v1.h"
#define head car_in[i].first()
#define GetTime (int(now_t/10))
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define HEAD(i) (car_in[(i)].first())
#define PERIOD (s->period)
#define Get(i) (s->map[GetTime%s->period][dir (i)][tr (i)])
#define WILL(j,i) (s->map[(j)%s->period][dir (i)][tr (i)])
#define min(i,j) ((i<j)?(i):(j))
#define max(i,j) ((i>j)?(i):(j))
static const int penalty_time = 20;
static const double A_max_ = 3.0;
static const double V_max = 20.0;
static const double V_min = 10.0;
static const double A_max = 5.0;

void Traffic_v1::st2 () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_in[i].empty ()) {
			/*-------------------- Calc Head -------------------*/
			int MinT = int (CalMinTime (-head.pos, head.vec) + 1);
			int MaxT = int (CalMaxTime (-head.pos, head.vec) + 1);
			int HAble;
			bool OK = false;
			for (HAble = MinT; HAble < MaxT; ++HAble)
				if (WILL (GetTime + HAble, i) == Color::Green&&
					WILL (GetTime + min (HAble - 1, 0), i) == Color::Green&&
					WILL (GetTime + min (HAble - 2, 0), i) == Color::Green) OK = true;
			if (OK) {
				//able to use the advanced tech
				head.arr_time = HAble + GetTime;
				if (head.vec < -head.pos / HAble*0.97)
					head.acc = A_max;
				else if (head.vec > -head.pos / HAble*1.03)
					head.acc = 0.5*(head.vec*head.vec - head.pos*head.pos / HAble / HAble) / head.pos*1.03;
				else head.acc = 0;
			}
			else {
				head.arr_time = MaxT;
				//unable to use the advanced tech
				if (head.pos < -30) head.acc = 1;
				else head.acc = head.vec*head.vec / head.pos*0.5;
			}
			/*-------------------- Calc other -------------------*/
			QList<Car>::iterator it;
			for (it = car_in[i].begin () + 1; it != car_in[i].end (); ++it) {
				int Min = int (CalMinTime (-it->pos, it->vec) + 1);
				int Max = int (CalMaxTime (-it->pos, it->vec) + 1);
				int _HAble;
				bool _OK;
				for (_HAble = max (Min, (it - 1)->arr_time - GetTime); _HAble < Max; ++_HAble)
					if (WILL (GetTime + _HAble, i) == Color::Green&&
						WILL (GetTime + min (_HAble - 1, 0), i) == Color::Green&&
						WILL (GetTime + min (_HAble - 2, 0), i) == Color::Green) OK = true;
				if (OK) {
					//able to use the advanced tech
					it->arr_time = _HAble + GetTime;
				}
				else {
					//unable to use the advanced tech
				}
			}
		}
	}
}