#include "traffic_v1.h"
bool Traffic_v1::check () {
	return !(std::rand () & 127);
}
void Traffic_v1::strategy () {
	/*foreach (Car _car_, car_in[0]) {
		qDebug () << _car_.vec << endl;
	}*/
}