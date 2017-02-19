#include "traffic_v1.h"

static const int penalty_time = 20;
static const double A_max_ = 3.0;
static const double V_max = 25.0;
static const double V_min = 5.0;
static const double A_max = 5.0;

void Traffic_v1::st2 () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
	STAT:
		if (!car_in[i].empty ()) {
			if (_head.pos > (car_block[i].empty () ? -0.3 : car_block[i].last ().pos - 4) && ((Get (i) == Color::Red
				|| (!(car_block[i].empty () || (WILL (GetTime, i) == Color::Green
					&& WILL (GetTime - 1, i) == Color::Green
					&&WILL (GetTime - 2, i) == Color::Green
					&& WILL (GetTime - 3, i) == Color::Green
					&& WILL (GetTime - 4, i) == Color::Green)))))) {
				_head.pos = (car_block[i].empty () ? 0 : car_block[i].last ().pos - 4);
				car_block[i] << _head;
				car_in[i].pop_front ();
				++stop_num[i];
				goto STAT;
			}
			if (_head.pos < 0) {
				//solve the head car
				if (_head.pos < -5) {
					int TminH;
					int TmaxH;
					TminH = int (CalMinTime (-_head.pos, _head.vec) + 1.1*car_block[i].count () + ((!car_block[i].empty () && Get (i) == Color::Red) ? 7 : 0));
					TmaxH = int (CalMaxTime (-_head.pos, _head.vec) + 0.9*car_block[i].count () + ((!car_block[i].empty () && Get (i) == Color::Red) ? 7 : 0));
					int rt;
					for (rt = TminH; rt < TmaxH; ++rt) {
						if (WILL (rt + GetTime, i) == Color::Green
							&&WILL (max (rt - 1, 0) + GetTime, i) == Color::Green
							&&WILL (max (rt - 2, 0) + GetTime, i) == Color::Green
							&&WILL (max (rt - 3, 0) + GetTime, i) == Color::Green
							&&WILL (max (rt - 4, 0) + GetTime, i) == Color::Green)
							break;
					}
					if (rt < TmaxH) {
						if (-_head.pos < _head.vec*rt*0.95) _head.acc = -5;
						else if (-_head.pos > _head.vec*rt*1.05) _head.acc = 5;
						else _head.acc = 0;
						_head.time_arr = GetTime + rt;
						if (!car_block[i].empty () && _head.pos > -5 + car_block[i].last ().pos) {
							_head.vec = 4;
							_head.acc = 0;
						}
					}
					else {
						if (_head.pos > -20 + (car_block[i].empty () ? 0 : (-4 + car_block[i].last ().pos))) {
							_head.acc = _head.vec*_head.vec / 2.1 / (car_block[i].empty () ?
								_head.pos : (_head.pos + 4 - car_block[i].last ().pos));
							if (_head.vec < 3) _head.acc = 0;
						}
						else _head.acc = 3;
						_head.time_arr = 0;
					}
				}
				else {
					if (WILL (int (-_head.pos / _head.vec) + GetTime, i) == Color::Green&&
						WILL (int (-_head.pos / _head.vec) + GetTime + 1, i) == Color::Green) {
						_head.acc = 5;
						_head.time_arr = int (-_head.pos / _head.vec) + 1;
						if (!car_block[i].empty () && _head.pos > -5 + car_block[i].last ().pos) {
							_head.vec = 4;
							_head.acc = 0;
						}
					}
					else {
						_head.acc = _head.vec*_head.vec / 2.1 / (car_block[i].empty () ?
							_head.pos : (_head.pos + 4 - car_block[i].last ().pos));
						if (_head.vec < 3) _head.acc = 0;
						_head.time_arr = 0;
					}
				}
			}
			else {
				_head.acc = 5;
				_head.time_arr = 0;
				if (!car_block[i].empty () && _head.pos > -5 + car_block[i].last ().pos) {
					_head.vec = 4;
					_head.acc = 0;
				}
			}
			QList<Car>::iterator it;
			for (it = car_in[i].begin () + 1; it != car_in[i].end (); ++it) {
				//for the other vehicle
				if (it->pos < -5) {
					int MinT = int (CalMinTime (-it->pos, it->vec) + 1.5*car_block[i].count ());
					int MaxT = int (CalMaxTime (-it->pos, it->vec) + 0.95*car_block[i].count ());
					int _rt;
					for (_rt = max (MinT, (it - 1)->time_arr); _rt < MaxT; ++_rt) {
						if (WILL (_rt + GetTime, i) == Color::Green&&
							WILL (max (_rt - 1, 0) + GetTime, i) == Color::Green&&
							WILL (max (_rt - 2, 0) + GetTime, i) == Color::Green&&
							WILL (max (_rt - 3, 0) + GetTime, i) == Color::Green&&
							WILL (max (_rt - 4, 0) + GetTime, i) == Color::Green)
							break;
					}
					if (_rt < MaxT) {
						if (-it->pos < it->vec*_rt*0.95) {
							it->acc = -5;
							if ((it - 1)->acc < it->acc && (it - 1)->pos - it->pos < 15)
								if (it->vec < (it - 1)->vec) it->acc = ((it - 1)->acc + it->acc) / 2;
								else it->acc = (it - 1)->acc - 10;
							else if ((it - 1)->pos - it->pos < 20) it->acc *= 1.1;
						}
						else if (-it->pos > it->vec*_rt*1.05) {
							it->acc = 5;
							if ((it - 1)->acc < 2 && (it - 1)->pos - it->pos < 15 && (it - 1)->pos < -20)
								if (it->vec < (it - 1)->vec) it->acc = ((it - 1)->acc + it->acc) / 2;
								else it->acc = (it - 1)->acc - 5;
						}
						else it->acc = 0;
						it->time_arr = GetTime + _rt;
						if ((it - 1)->pos - it->pos < 7) it->acc = min (-10, (it - 1)->acc - 15);
					}
					else {
						if (it->pos > -20 + (car_block[i].empty () ? 0 :
							(-4 + car_block[i].last ().pos)) + 4 * (car_in[i].begin () - it)) {
							it->acc = it->vec*it->vec / 2.1 / (car_block[i].empty () ?
								it->pos :
								(it->pos + 4 - car_block[i].last ().pos) + 4 * (it - car_in[i].begin ()));
							if (it->vec < 3) it->acc = 0;
						}
						else {
							//just follow the car
							if ((it - 1)->pos - it->pos < 7)it->acc = min ((it - 1)->acc - 10, -10);
							else if (it->vec > (it - 1)->vec && (it - 1)->pos - it->pos < 15) it->acc = (it - 1)->acc - 5;
							else if (it->vec < (it - 1)->vec && (it - 1)->pos - it->pos > 15) it->acc = (it - 1)->acc + 5;
							else if (it->vec > (it - 1)->vec && (it - 1)->pos - it->pos > 15) it->acc = (it - 1)->acc - 1;
							else if (it->vec < (it - 1)->vec && (it - 1)->pos - it->pos < 15) it->acc = (it - 1)->acc + 1;
						}
						it->time_arr = 0;
					}
				}
				else {
					if (WILL (int (-it->pos / it->vec) + GetTime, i) == Color::Green&&
						WILL (int (-it->pos / it->vec) + GetTime + 1, i) == Color::Green) {
						it->acc = 5;
						it->time_arr = int (-it->pos / it->vec) + 1;
					}
					else {
						it->acc = it->vec*it->vec / 2.1 / (car_block[i].empty () ?
							it->pos :
							(it->pos + 4 - car_block[i].last ().pos) + 4 * (it - car_in[i].begin ()));
						if (it->vec < 3) it->acc = 0;
						it->time_arr = 0;
					}
				}
			}
			for (it = car_in[i].begin (); it != car_in[i].end (); ++it)
				if (it->vec + it->acc*0.1 < 0) {
					it->vec = 0;
					it->acc = 1;
				}
			for (it = car_in[i].begin (); it != car_in[i].end (); ++it) {
				if (it->vec + it->acc*0.1 > V_max) {
					it->vec = V_max;
					it->acc = 0;
				}
				if (it->vec < 0) {
					it->vec = 0;
					it->acc = 3;
					++stop_num[i];
					++stop_time[i];
				}
				if (it->acc > A_max) it->acc = A_max;
			}
		}
	}
}