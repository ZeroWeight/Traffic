#include "traffic_v1.h"
#define GetTime (int(now_t/10))
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define HEAD(i) (car_in[(i)].first())
#define PERIOD (s->period)
#define Get(i) (s->map[GetTime%s->period][dir (i)][tr (i)])
#define WILL(j,i) (s->map[(j)%s->period][dir (i)][tr (i)])
static const int S = 15;
static const double a_max = 5;
static std::default_random_engine e;
static std::normal_distribution<double> ND_A_A (3, 0.3);
static std::normal_distribution<double> ND_A (1, 0.3);
static std::normal_distribution<double> ND_A_S (0, 0.1);
static std::normal_distribution<double> ND_A_BS (-0.5, 0.3);
const int penalty_time = 5;
void Traffic_v1::following () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_in[i].empty ()) {
			switch (car_in[i].begin ()->mode) {
			case MODE::BLOCK:
				int j;
				for (j = 1 + GetTime; WILL (j, i) == Green && (j - GetTime) % PERIOD; j++);
				if (Get (i) == Color::Green&&j - GetTime > 10) {
					if (car_in[i].begin ()->block) --car_in[i].begin ()->block;
					else if (car_in[i].begin ()->vec < 3) car_in[i].begin ()->acc = 5;
					else if (car_in[i].begin ()->vec > 3.5) car_in[i].begin ()->acc = -5;
					else car_in[i].begin ()->acc = 0;
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
				head (car_in[i].begin (), i);
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
					for (j = 1 + GetTime; WILL (j, i) == Green && (j - GetTime) % PERIOD; j++);
					if (Get (i) == Color::Green&&j - GetTime > 10) {
						if (itp->block) --itp->block;
						else itp->acc = it->acc + 0.5* (it->vec - itp->vec);
					}
					else {
						itp->acc = itp->vec*itp->vec / 2 / (itp->pos - it->pos + 3);
						if (itp->acc > -1) itp->acc = 0;
						if (itp->vec + 0.1*itp->acc <= 0 || itp->vec < 0.1) {
							itp->acc = it->acc; itp->vec = 0;
							itp->block = penalty_time;
						}
					}
					break;
				case MODE::RUN:
					if (it->vec < 5 && it->acc < 3 && it->pos - itp->pos < 60) {
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
		if (j - GetTime > (-it->pos / it->vec) || it->pos < -100) {//10 sec remains
			if (it->vec < 5)  it->acc = ND_A_A (e);
			else if (it->vec < 16) it->acc = ND_A (e);
			else if (it->vec < 17) it->acc = ND_A_S (e);
			else it->acc = ND_A_BS (e);
			break;
		}
	case Yellow:case Red:
		if (it->pos > -100) {
			it->mode = MODE::BLOCK;
			it->acc = it->vec*it->vec / 2 / (it->pos);
			if (it->vec + 0.1*it->acc < 0) {
				it->vec = it->acc = 0;
				it->mode = MODE::BLOCK;
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