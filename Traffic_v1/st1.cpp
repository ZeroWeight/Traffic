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
static const double c = 0.7;
static const double d = 4;
static std::default_random_engine e;
static std::normal_distribution<double> ND_A_A (3, 0.3);
static std::normal_distribution<double> ND_A (1, 0.3);
static std::normal_distribution<double> ND_A_S (0, 0.1);
static std::normal_distribution<double> ND_A_BS (-0.5, 0.3);
static const int penalty_time = 20;
static const double A_max_ = 3.0;
static const double V_max = 20.0;
static const double V_min = 10.0;
static const double A_max = 5.0;

double Traffic_v1::CalMaxTime (double pos, double vec) {
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
double Traffic_v1::CalMinTime (double pos, double vec) {
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
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
	STAT:
		if (!car_in[i].empty ()) {
			if (_head.pos > (car_block[i].empty () ? -0.3 : car_block[i].last ().pos - 4) && Get (i) == Color::Red) {
				_head.pos = (car_block[i].empty () ? 0 : car_block[i].last ().pos - 4);
				car_block[i] << _head;
				car_in[i].pop_front ();
				goto STAT;
			}
			else head (car_in[i].begin (), i);
#pragma region following
			QList<Car>::iterator it;
			QList<Car>::iterator itp;
			for (it = car_in[i].end () - 2, itp = car_in[i].end () - 1;
				itp != car_in[i].begin (); --it, --itp) {
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
					//sat.
					if (itp->acc < -5) itp->acc = -5;
					if (itp->acc > 5) itp->acc = 5;
					if (it->pos - itp->pos < d)
						itp->acc = -10;
					//stop
					if (itp->vec + 0.1*itp->acc < 0) {
						itp->acc = 0;
						itp->vec = 0;
					}
				}
				else  free (itp, i);
			}
		}
#pragma endregion
	}
}

void Traffic_v1::st1_free (QList<Car>::iterator it, int i) {
	if (it->vec < 8)  it->acc = ND_A_A (e);
	else if (it->vec < 15) it->acc = ND_A (e);
	else if (it->vec < 17) it->acc = ND_A_S (e);
	else it->acc = ND_A_BS (e);
}

void Traffic_v1::st1_head (QList<Car>::iterator it, int i) {
	switch (Get (i)) {
	case Green:
		int j;
		for (j = 1 + GetTime; WILL (j, i) == Green && (j - GetTime) < PERIOD; ++j);
		if (it->pos < -30 + (car_block[i].empty () ? 0 : car_block[i].last ().pos)) {//10 sec remains
			if (it->vec < 5)  it->acc = ND_A_A (e);
			else if (it->vec < 16) it->acc = ND_A (e);
			else if (it->vec < 17) it->acc = ND_A_S (e);
			else it->acc = ND_A_BS (e);
			break;
		}
		else if (j - GetTime > 2 && it->pos > -30 + (car_block[i].empty () ? 0 : car_block[i].last ().pos)) {
			if (car_block[i].empty ()) {
				if (it->vec < 10) it->acc = 5;
				else if (it->vec < 16) it->acc = ND_A (e);
				else if (it->vec < 17) it->acc = ND_A_S (e);
			}
			else if (it->vec < 3) it->acc = 2;
			else it->acc = 0;
			break;
		}
	case Yellow:case Red:
		if (it->pos > -30 + (car_block[i].empty () ? 0 : car_block[i].last ().pos)) {
			it->mode = MODE::BLOCK;
			if (car_block[i].empty ())
				if (it->vec < 0.5) it->acc = 3;
				else it->acc = it->vec*it->vec / 2 / (it->pos);
			else
				if (it->vec < 0.5) it->acc = 3;
				else it->acc = it->vec*it->vec / 2 / (it->pos - car_block[i].last ().pos + 4);
				if (it->vec + 0.1*it->acc < 0) {
					it->vec = it->acc = 0;
				}
		}
		else {
			if (it->vec < 5)  it->acc = ND_A_A (e);
			else if (it->vec < 16) it->acc = ND_A (e);
			else if (it->vec < 17) it->acc = ND_A_S (e);
			else it->acc = ND_A_BS (e);
			break;
		}
	}
}