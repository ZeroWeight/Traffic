﻿#include "traffic_v1.h"
#define GetTime (int(now_t/10))
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define HEAD(i) (car_in[(i)].first())
#define PERIOD (s->period)
#define Get(i) (s->map[GetTime%s->period][dir (i)][tr (i)])
#define WILL(j,i) (s->map[(j)%s->period][dir (i)][tr (i)])

static std::default_random_engine e;
static std::normal_distribution<double> ND_A_A (3, 0.3);
static std::normal_distribution<double> ND_A (1, 0.3);
static std::normal_distribution<double> ND_A_S (0, 0.1);
static std::normal_distribution<double> ND_A_BS (-0.5, 0.3);
static const int penalty_time = 5;
static const double A_max_ = 3.0;
static const double V_max = 20.0;
static const double V_min = 10.0;
static const double A_max = 5.0;

static inline double CalMaxTime (double pos, double vec) {
	//dec. to the min. vec.
	//keep the max vec till aim
	//
	//pos>0 vec>0
	double time;
	double pos_cri = (vec*vec - V_min*V_min) / (2 * A_max_);
	if (pos <= pos_cri) {
		double vec_ = sqrt (vec*vec - 2 * A_max_*pos);//vec at the terminal
		time = (vec - vec_) / A_max_;
	}
	else {
		double time1 = (vec - V_min) / A_max_;
		double time2 = (pos - pos_cri) / V_min;
		time = time1 + time2;
	}
	return time;
}
static inline double CalMinTime (double pos, double vec) {
	double time;
	double pos_cri = (V_max*V_max - vec*vec) / (2 * A_max);
	if (pos <= pos_cri) {
		double vec_ = sqrt (vec*vec + 2 * A_max*pos);//vec at the terminal
		time = (vec_ - vec) / A_max;
	}
	else {
		double time1 = (V_max - vec) / A_max;
		double time2 = (pos - pos_cri) / V_max;
		time = time1 + time2;
	}
	return time;
}

// the stategy of flowing
void Traffic_v1::st1 () {
	//check the head car
		//check the shortest time arrive
		//check the longest time arrive
		//choose the shortest time possible to go to
			//or choose the normal stategy
		//cal. the time,acc etc
	//check the car following
		//check the shortest time and the longest time to ..
		//choose the shortest time to arrive:block red and the safety driving
			//or choose the normal stategy
		//cal. the time ,acc etc
	//for the basic stategy:
		//same as the stategy given
	//for cal. the saftydriving
	//suppose the a
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_in[i].empty ()) {
			switch (car_in[i].begin ()->mode) {
			case MODE::BLOCK:
				int j;
				for (j = 1 + GetTime; WILL (j, i) == Green && (j - GetTime) % PERIOD; j++);
				if (Get (i) == Color::Green&&j - GetTime > 10) {
					if (car_in[i].first ().pos > -20) {
						if (car_in[i].begin ()->block) --car_in[i].begin ()->block;
						else if (car_in[i].begin ()->vec < 3) car_in[i].begin ()->acc = 5;
						else if (car_in[i].begin ()->vec > 3.5) car_in[i].begin ()->acc = -5;
						else car_in[i].begin ()->acc = 0;
					}
					else car_in[i].first ().mode = MODE::RUN;
				}
				else {
					car_in[i].begin ()->acc = car_in[i].begin ()->vec*car_in[i].begin ()->vec / 2 / (car_in[i].begin ()->pos);
					if (car_in[i].begin ()->acc > -1) car_in[i].begin ()->acc = 0;
					if (car_in[i].begin ()->vec + 0.1*car_in[i].begin ()->acc <= 0 || car_in[i].begin ()->vec < 0.1) {
						car_in[i].begin ()->acc = car_in[i].begin ()->acc; car_in[i].begin ()->vec = 0;
						car_in[i].begin ()->block = penalty_time;
					}
				}
				break;
			case MODE::RUN:
#pragma region HEAD_STAT
				double MaxT = CalMaxTime (-car_in[i].first ().pos, car_in[i].first ().vec);
				double MinT = CalMinTime (-car_in[i].first ().pos, car_in[i].first ().vec);
				int tt = int (MinT) + 1;
				//set the correct tt
#pragma endregion
				break;
			default:
				break;
			}

#pragma region following
			QList<Car>::iterator it;
			QList<Car>::iterator itp;
			for (it = car_in[i].end () - 2, itp = car_in[i].end () - 1;
				itp != car_in[i].begin (); --it, --itp) {
				switch (itp->mode) {
				case MODE::BLOCK:
					int j;
					if (it->mode == MODE::RUN&&it->pos - itp->pos > 10) itp->mode = MODE::RUN;
					for (j = 1 + GetTime; WILL (j, i) == Green && (j - GetTime) % PERIOD; j++);
					if (Get (i) == Color::Green&&j - GetTime > 10) {
						if (itp->block) --itp->block;
						else if (it->pos - itp->pos < 2)
							if (it->vec > itp->vec) itp->acc = -5;
							else itp->acc = it->acc;

						else itp->acc = it->acc + 0.5* (it->vec - itp->vec);
						if (itp->vec + 0.1*itp->acc <= 0 || itp->vec < 0.1) {
							itp->acc = it->acc; itp->vec = 0;
						}
						else if (it->pos - itp->pos > 20) itp->mode = MODE::RUN;
					}
					else {
						if (it->pos - itp->pos > 5 && (itp->vec*itp->vec / 2 / (itp->pos - it->pos + 3)) > -3) itp->acc = 3;
						else {
							itp->acc = itp->vec*itp->vec / 2 / (itp->pos - it->pos + 3);
							if (itp->acc > -1) itp->acc = 0;
							if (itp->vec + 0.1*itp->acc <= 0 || itp->vec < 0.1) {
								itp->acc = it->acc; itp->vec = 0;
								itp->block = penalty_time;
							}
						}
					}
					break;
				case MODE::RUN:
					if (it->vec < 5 && it->acc < 3) {
						itp->acc = itp->vec*itp->vec / 2 / (itp->pos - it->pos + 3);
						if (it->pos - itp->pos < 10 && it->mode == MODE::BLOCK)
							itp->mode = MODE::BLOCK;
						if (itp->acc > -1) itp->acc = 0;
						if (itp->vec + 0.1*itp->acc <= 0 || itp->vec < 0.1) {
							itp->acc = it->acc; itp->vec = 0;
							itp->block = penalty_time;
						}
						continue;
					}
					if (it->pos - itp->pos < 30) {
						//slow down
						if (it->pos - itp->pos < 7) {
							itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
								/ 2.0 * (7 - it->pos + itp->pos);
						}
						//car following mode
						else if (it->vec < itp->vec)
							itp->acc = it->acc + (itp->vec - it->vec)*(itp->vec - it->vec)
							/ 2.0 / (7 - it->pos + itp->pos);
						//keep on
						else if (it->vec > itp->vec)
							itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
							/ 2.0 / (7 - it->pos + itp->pos);
						//sat.
						if (itp->acc < -5) itp->acc = -5;
						if (itp->acc > 5) itp->acc = 5;
						//stop
						if (itp->vec + 0.1*itp->acc < 0) {
							itp->acc = 0;
							itp->vec = 0;
							itp->block = penalty_time;
						}
					}
					else {
#pragma region NORM_

#pragma endregion
					}
					break;
				default:
					break;
				}
			}
#pragma endregion
		}
	}
}