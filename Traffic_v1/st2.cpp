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
			if ((_head.pos > (car_block[i].empty () ? -0.3 : car_block[i].last ().pos - 5) && (Get (i) == Color::Red)) ||
				(!car_block[i].empty () && _head.pos > car_block[i].last ().pos - 5)) {
				_head.pos = (car_block[i].empty () ? 0 : car_block[i].last ().pos - 5);
				car_block[i] << _head;
				car_in[i].pop_front ();
				if (!(WILL (GetTime, i) == Color::Green &&
					WILL (GetTime - 1, i) == Color::Green&&WILL (GetTime - 2, i) == Color::Green&&
					WILL (GetTime - 3, i) == Color::Green&&WILL (GetTime - 4, i) == Color::Green))
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
							_head.vec = 2;
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
			QList<Car>::iterator itp;
			for (itp = car_in[i].begin () + 1; itp != car_in[i].end (); ++itp) {
				//for the other vehicle
				if (itp->pos < -5) {
					int MinT = int (CalMinTime (-itp->pos, itp->vec) + 1.5*car_block[i].count ());
					int MaxT = int (CalMaxTime (-itp->pos, itp->vec) + 0.95*car_block[i].count ());
					int _rt;
					for (_rt = max (MinT, (itp - 1)->time_arr); _rt < MaxT; ++_rt) {
						if (WILL (_rt + GetTime, i) == Color::Green&&
							WILL (max (_rt - 1, 0) + GetTime, i) == Color::Green&&
							WILL (max (_rt - 2, 0) + GetTime, i) == Color::Green&&
							WILL (max (_rt - 3, 0) + GetTime, i) == Color::Green&&
							WILL (max (_rt - 4, 0) + GetTime, i) == Color::Green)
							break;
					}
					if (_rt < MaxT) {
						if (-itp->pos < itp->vec*_rt*0.95) {
							itp->acc = -5;
							if ((itp - 1)->acc < itp->acc && (itp - 1)->pos - itp->pos < 10)
								if (itp->vec < (itp - 1)->vec) itp->acc = ((itp - 1)->acc + itp->acc) / 2;
								else itp->acc = min ((itp - 1)->acc - 10, -10);
							else if ((itp - 1)->pos - itp->pos < 20) itp->acc *= 1.1;
						}
						else if (-itp->pos > itp->vec*_rt*1.05) {
							itp->acc = 5;
							if ((itp - 1)->acc < 2 && (itp - 1)->pos - itp->pos < 15 && (itp - 1)->pos < -20)
								if (itp->vec < (itp - 1)->vec) itp->acc = ((itp - 1)->acc + itp->acc) / 2;
								else itp->acc = (itp - 1)->acc - 5;
						}
						else itp->acc = 0;
						itp->time_arr = GetTime + _rt;
						if ((itp - 1)->pos - itp->pos < 7) itp->acc = min (-10, (itp - 1)->acc - 15);
					}
					else {
						if (itp->pos > -20 + (car_block[i].empty () ? 0 :
							(-4 + car_block[i].last ().pos)) + 4 * (car_in[i].begin () - itp)) {
							itp->acc = itp->vec*itp->vec / 2.1 / (car_block[i].empty () ?
								itp->pos :
								(itp->pos + 4 - car_block[i].last ().pos) + 4 * (itp - car_in[i].begin ()));
							if (itp->vec < 3) itp->acc = 0;
						}
						else {
							//just follow the car
							if ((itp - 1)->pos - itp->pos < 7)itp->acc = min ((itp - 1)->acc - 10, -10);
							else if (itp->vec > (itp - 1)->vec && (itp - 1)->pos - itp->pos < 15) itp->acc = (itp - 1)->acc - 5;
							else if (itp->vec < (itp - 1)->vec && (itp - 1)->pos - itp->pos > 15) itp->acc = (itp - 1)->acc + 5;
							else if (itp->vec > (itp - 1)->vec && (itp - 1)->pos - itp->pos > 15) itp->acc = (itp - 1)->acc - 1;
							else if (itp->vec < (itp - 1)->vec && (itp - 1)->pos - itp->pos < 15) itp->acc = (itp - 1)->acc + 1;
						}
						itp->time_arr = 0;
					}
				}
				else {
					if (WILL (int (-itp->pos / itp->vec) + GetTime, i) == Color::Green&&
						WILL (int (-itp->pos / itp->vec) + GetTime + 1, i) == Color::Green) {
						itp->acc = 5;
						itp->time_arr = int (-itp->pos / itp->vec) + 1;
					}
					else {
						itp->acc = itp->vec*itp->vec / 2.1 / (car_block[i].empty () ?
							itp->pos :
							(itp->pos + 4 - car_block[i].last ().pos) + 4 * (itp - car_in[i].begin ()));
						if (itp->vec < 3) itp->acc = 0;
						itp->time_arr = 0;
					}
				}
			}
			for (itp = car_in[i].begin (); itp != car_in[i].end (); ++itp) {
				if (itp->vec + itp->acc*0.1 > V_max) {
					itp->vec = V_max;
					itp->acc = 0;
				}
				if (itp->vec < 2) {
					++stop_time[i];
					if (itp->vec < 0) {
						itp->vec = 0;
						itp->acc = 2;
						++stop_num[i];
					}
				}
				if (itp->acc > A_max) itp->acc = A_max;
			}
		}
	}
}