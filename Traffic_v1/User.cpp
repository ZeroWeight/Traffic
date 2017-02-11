#include "traffic_v1.h"
#define GetTime (int(now_t/10))
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define HEAD(i) (car_in[(i)].first())
#define PERIOD (s->period)
#define Get(i) (s->map[GetTime%s->period][dir (i)][tr (i)])
#define WILL(j,i) (s->map[(j)%s->period][dir (i)][tr (i)])
#define _head car_in[i].first()

static const int S = 15;
static const double a_max = 5;
static std::default_random_engine e;
static std::normal_distribution<double> ND_A_A (3, 0.3);
static std::normal_distribution<double> ND_A (1, 0.3);
static std::normal_distribution<double> ND_A_S (0, 0.1);
static std::normal_distribution<double> ND_A_BS (-0.5, 0.3);
const int penalty_time = 20;
void Traffic_v1::following () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
	STAT:
		if (!car_in[i].empty ()) {
			head (car_in[i].begin (), i);
			if (_head.pos > car_block[i].last ().pos - 5 && Get (i) == Color::Red) {
				car_block[i] << _head;
				car_in[i].pop_front ();
				goto STAT;
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
					if (it->pos - itp->pos < (S << 2)) {
						//slow down
						if (it->pos - itp->pos < S) {
							itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
								/ 2.0 * (S - it->pos + itp->pos);
						}
						//car following mode
						else if (it->vec < itp->vec)
							itp->acc = it->acc + (itp->vec - it->vec)*(itp->vec - it->vec)
							/ 2.0 / (S - it->pos + itp->pos);
						//keep on
						else if (it->vec > itp->vec)
							itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
							/ 2.0 / (S - it->pos + itp->pos);
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
					else  free (itp, i);
					break;
				default:
					break;
				}
			}
#pragma endregion
		}
	}
}
void Traffic_v1::head (QList<Car>::iterator it, int i) {
	switch (Get (i)) {
	case Green:
		int j;
		for (j = 1 + GetTime; WILL (j, i) == Green && (j - GetTime) % PERIOD; j++);
		if (j - GetTime > 2 || it->pos < -50) {//10 sec remains
			if (it->vec < 5)  it->acc = ND_A_A (e);
			else if (it->vec < 16) it->acc = ND_A (e);
			else if (it->vec < 17) it->acc = ND_A_S (e);
			else it->acc = ND_A_BS (e);
			break;
		}
	case Yellow:case Red:
		if (it->pos > -50 + car_block[i].empty () ? 0 : car_block[i].last ().pos) {
			it->mode = MODE::BLOCK;
			if (car_block[i].empty ())
				it->acc = it->vec*it->vec / 2 / (it->pos);
			else
				it->acc = it->vec*it->vec / 2 / (it->pos - car_block[i].last ().pos + 5);
			if (it->vec + 0.1*it->acc < 0) {
				it->vec = it->acc = 0;
				it->mode = MODE::BLOCK;
				if (it->pos > -10)
					it->block = penalty_time;
			}
		}
	}
}
void Traffic_v1::free (QList<Car>::iterator it, int i) {
	if (it->vec < 8)  it->acc = ND_A_A (e);
	else if (it->vec < 15) it->acc = ND_A (e);
	else if (it->vec < 17) it->acc = ND_A_S (e);
	else it->acc = ND_A_BS (e);
}