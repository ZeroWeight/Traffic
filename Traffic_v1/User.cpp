#include "traffic_v1.h"

static const int S = 15;
static const double a_max = 5;
static const double c = 0.7;
static const double d = 4;
static std::default_random_engine e;
static std::normal_distribution<double> ND_A_A (3, 0.3);
static std::normal_distribution<double> ND_A (1, 0.3);
static std::normal_distribution<double> ND_A_S (0, 0.1);
static std::normal_distribution<double> ND_A_BS (-0.5, 0.3);
void Traffic_v1::following () {
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

void Traffic_v1::head (QList<Car>::iterator it, int i) {
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
			else if (!car_block[i].empty ()
				&& WILL (GetTime, i) == Color::Green
				&& WILL (GetTime - 1, i) == Color::Green
				&&WILL (GetTime - 2, i) == Color::Green
				&& WILL (GetTime - 3, i) == Color::Green) {
				if (it->vec < 3) it->acc = 2;
				else it->acc = 0;
			}
			else {
				if (it->vec < 0.5) it->acc = 3;
				else it->acc = it->vec*it->vec / 2 / (it->pos - car_block[i].last ().pos + 4);
				if (it->vec + 0.1*it->acc < 0) {
					it->vec = it->acc = 0;
				}
			}
			break;
		}
	case Yellow:case Red:
		if (it->pos > -30 + (car_block[i].empty () ? 0 : car_block[i].last ().pos)) {
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
void Traffic_v1::free (QList<Car>::iterator it, int i) {
	if (it->vec < 8)  it->acc = ND_A_A (e);
	else if (it->vec < 15) it->acc = ND_A (e);
	else if (it->vec < 17) it->acc = ND_A_S (e);
	else it->acc = ND_A_BS (e);
}