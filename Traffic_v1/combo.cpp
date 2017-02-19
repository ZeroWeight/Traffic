#include "traffic_v1.h"

static const double A_max_ = 3.0;
static const double V_max = 25.0;
static const double V_min = 5.0;
static const double A_max = 5.0;
static const int S = 15;
static const double a_max = 5;
static const double c = 0.7;
static const double d = 4;
static std::default_random_engine e;
static std::normal_distribution<double> ND_A_A (3, 0.3);
static std::normal_distribution<double> ND_A (1, 0.3);
static std::normal_distribution<double> ND_A_S (0, 0.1);
static std::normal_distribution<double> ND_A_BS (-0.5, 0.3);

void Traffic_v1::combo () {
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
			else {
				switch (_head.type) {
				case C_0:
					if (_head.pos < 0)head (car_in[i].begin (), i);
					else {
						_head.acc = 5;
						_head.time_arr = 0;
						if (!car_block[i].empty () && _head.pos > -5 + car_block[i].last ().pos) {
							_head.vec = 4;
							_head.acc = 0;
						}
					}
					break;
				case C_1:
					if (_head.pos < 0)st1_head (car_in[i].begin (), i);
					else {
						_head.acc = 5;
						_head.time_arr = 0;
						if (!car_block[i].empty () && _head.pos > -5 + car_block[i].last ().pos) {
							_head.vec = 4;
							_head.acc = 0;
						}
					}
					break;
				case C_2:
					if (_head.pos < 0) {
						//solve the head car
						if (_head.pos < -5) {
							int TminH;
							int TmaxH;
							if (_head.pos < -8 * car_block[i].count ()) {
								TminH = int (CalMinTime (-_head.pos, _head.vec) + 1.5*car_block[i].count ());
								TmaxH = int (CalMaxTime (-_head.pos, _head.vec) + 0.95*car_block[i].count ());
							}
							else {
								TminH = int (CalMinTime (-_head.pos - 3 * car_block[i].count (), _head.vec) + 1.05*car_block[i].count ());
								TmaxH = int (CalMaxTime (-_head.pos - 2 * car_block[i].count (), _head.vec) + 0.95*car_block[i].count ());
							}
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
									if (_head.vec < 3 && !car_block[i].empty ()) _head.acc = 0;
									if (_head.vec < 5) _head.acc = 5;
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
					break;
				default:
					break;
				}
			}
			QList<Car>::iterator it;
			QList<Car>::iterator itp;
			for (it = car_in[i].end () - 2, itp = car_in[i].end () - 1;
				itp != car_in[i].begin (); --it, --itp) {
				switch (itp->type) {
				case C_0:
					if (it->pos - itp->pos < ((d + c*it->vec) * 4)) {
						//slow down
						if (it->pos - itp->pos < (d + c*it->vec))
							itp->acc = -5;
						//car following mode
						else if (it->vec < 1 && itp->vec < 1) itp->acc = 0.5;
						else if (it->vec < itp->vec)
							itp->acc = it->acc + 1.1* (itp->vec - it->vec)*(itp->vec - it->vec)
							/ 2.0 / ((d + c*it->vec) - it->pos + itp->pos);
						else if (it->vec > itp->vec)
							itp->acc = it->acc - 0.5* (itp->vec - it->vec)*(itp->vec - it->vec)
							/ 2.0 / ((d + c*it->vec) - it->pos + itp->pos);
						//sat.
						if (itp->acc < -5) itp->acc = -5;
						if (itp->acc > 5) itp->acc = 5;
						if (it->pos - itp->pos < d)
							itp->acc = -10;
					}
					else  free (itp, i);
					break;
				case C_1:
					if (it->pos - itp->pos < ((d + c*it->vec) * 4)) {
						//slow down
						if (it->pos - itp->pos < (d + c*it->vec))
							itp->acc = -5;
						//car following mode
						else if (it->vec < 1 && itp->vec < 1) itp->acc = 0.5;
						else if (it->vec < itp->vec)
							itp->acc = it->acc + 1.03* (itp->vec - it->vec)*(itp->vec - it->vec)
							/ 2.0 / ((d + c*it->vec) - it->pos + itp->pos);
						else if (it->vec > itp->vec)
							itp->acc = it->acc - 0.97* (itp->vec - it->vec)*(itp->vec - it->vec)
							/ 2.0 / ((d + c*it->vec) - it->pos + itp->pos);
						double a_st1 = 5;
						int Tmin = int (CalMinTime (-itp->pos, itp->vec) + 1.05*car_block[i].count ());
						int Tmax = int (CalMaxTime (-itp->pos, itp->vec) + 0.95*car_block[i].count ());
						int rt;
						for (rt = Tmin + 1; rt < Tmax; ++rt) {
							if (WILL (rt + GetTime, i) == Color::Green&&WILL (rt + GetTime - 1, i) == Color::Green&&WILL (rt + GetTime + 1, i) == Color::Green)
								break;
						}
						if (rt < Tmax) {
							if (itp->vec < -itp->pos / rt*0.9) a_st1 = 5;
							else if (itp->vec > -itp->pos / rt*1.1) a_st1 = -2.1*(itp->pos + itp->vec*rt) / rt / rt;
							else a_st1 = 0;
						}
						itp->acc = itp->acc < a_st1 ? itp->acc : a_st1;
						//sat.
						if (itp->acc < -5) itp->acc = -5;
						if (itp->acc > 5) itp->acc = 5;
						if (it->pos - itp->pos < d)
							itp->acc = -10;
						//stop
					}
					else  st1_free (itp, i);
					break;
				case C_2:
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
								if ((itp - 1)->acc < itp->acc && (itp - 1)->pos - itp->pos < 15)
									if (itp->vec < (itp - 1)->vec) itp->acc = ((itp - 1)->acc + itp->acc) / 2;
									else itp->acc = (itp - 1)->acc - 10;
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
					break;
				default:
					break;
				}
			}
			for (it = car_in[i].begin (); it != car_in[i].end (); ++it) {
				if (it->vec > V_max) {
					it->vec = V_max;
					it->acc = 0;
				}
				if (it->vec < 1) {
					it->vec = 5;
					it->acc = 5;
					++stop_num[i];
				}
				if (it->acc > A_max) it->acc = A_max;
			}
#pragma endregion
		}
	}
}