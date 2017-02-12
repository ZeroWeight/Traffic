#include "traffic_v1.h"

static const int penalty_time = 20;
static const double A_max_ = 3.0;
static const double V_max = 20.0;
static const double V_min = 10.0;
static const double A_max = 5.0;

void Traffic_v1::st2 () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
	STAT:
		if (!car_in[i].empty ()) {
			if (_head.pos > (car_block[i].empty () ? -0.3 : car_block[i].last ().pos - 4) && Get (i) == Color::Red) {
				_head.pos = (car_block[i].empty () ? 0 : car_block[i].last ().pos - 4);
				car_block[i] << _head;
				car_in[i].pop_front ();
				goto STAT;
			}
			if (_head.pos < 0) {
				//solve the head car
				int TminH = int (CalMinTime (-_head.pos, _head.vec) + 1.05*car_block[i].count ());
				int TmaxH = int (CalMaxTime (-_head.pos, _head.vec) + 0.95*car_block[i].count ());
				int rt;
				for (rt = TminH + 1; rt < TmaxH; ++rt) {
					if (WILL (rt + GetTime, i) == Color::Green&&WILL (rt + GetTime - 1, i) == Color::Green&&WILL (rt + GetTime + 1, i) == Color::Green)
						break;
				}
				if (rt < TmaxH) {
					if (-_head.pos < _head.vec*rt*0.95) _head.acc = 2.1 / rt / rt*(-_head.pos - _head.vec*rt);
					else if (-_head.pos > _head.vec*rt*1.05) _head.acc = 5;
					else _head.acc = 0;
					_head.time_arr = GetTime + rt;
				}
				else {
					if (_head.pos > -50 + (car_block[i].empty () ? 0 : (-5 + car_block[i].last ().pos)))
						_head.acc = _head.vec*_head.vec / 2 / (car_block[i].empty () ?
							_head.pos : (_head.pos + 5 - car_block[i].last ().pos));
					else _head.acc = 3;
					_head.time_arr = -1;
				}
			}
			else {
				_head.acc = 5;
				_head.time_arr = -1;
			}
		}
	}
}