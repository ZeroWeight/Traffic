#include "traffic_v1.h"

static const int penalty_time = 20;
static const double A_max_ = 3.0;
static const double V_max = 20.0;
static const double V_min = 10.0;
static const double A_max = 5.0;

void Traffic_v1::st2 () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_in[i].empty ()) {
			//solve the head car
			int TminH = int (CalMinTime (-_head.pos, _head.vec) + 1.05*car_block[i].count ());
			int TmaxH = int (CalMaxTime (-_head.pos, _head.vec) + 0.95*car_block[i].count ());
			int rt;
			for (rt = TminH + 1; rt < TmaxH; ++rt) {
				if (WILL (rt + GetTime, i) == Color::Green&&WILL (rt + GetTime - 1, i) == Color::Green&&WILL (rt + GetTime + 1, i) == Color::Green)
					break;
			}
		}
	}
}