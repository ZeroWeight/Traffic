#include "traffic_v1.h"

void Traffic_v1::c_write (Car car) {
	double time_r = double (now_t - car.enter_time_d) / 10.0;
	(*__car) << car.vec_init << ',' << CalMinTime (200, car.vec_init)
		<< ',' << time_r << ',' << time_r - CalMinTime (200, car.vec_init) << '\n';
}

void Traffic_v1::main_write () {
	double sum = _st[0]->text ().toInt ();
	(*__road) << double (now_t) / 10.0 << ',' << _st[0]->text ().toInt ();
	for (int i = 1; i < TR_NUM*DIR_NUM; ++i) {
		(*__road) << ',' << _st[i]->text ().toInt ();
		sum += _st[i]->text ().toInt ();
	}
	(*__road) << ',' << sum / 12.0 << ',' << index << '\n';
	sum = stop_num[0];
	(*__stop) << double (now_t) / 10.0 << ',' << stop_num[0];
	for (int i = 1; i < TR_NUM*DIR_NUM; ++i) {
		(*__stop) << ',' << stop_num[i];
		sum += stop_num[i];
	}
	(*__stop) << ',' << sum / 12.0 << ',' << index << '\n';
}

void Traffic_v1::init_write () {
	int hour = QTime::currentTime ().hour ();
	int minute = QTime::currentTime ().minute ();
	int second = QTime::currentTime ().second ();
	int month = QDate::currentDate ().month ();
	int day = QDate::currentDate ().day ();
	QString folder = "..\\result\\" + (month < 10 ? '0' + QString::number (month) : QString::number (month)) +
		(day < 10 ? '0' + QString::number (day) : QString::number (day)) + '-' +
		(hour < 10 ? '0' + QString::number (hour) : QString::number (hour)) +
		(minute < 10 ? '0' + QString::number (minute) : QString::number (minute)) +
		(second < 10 ? '0' + QString::number (second) : QString::number (second)) + '+';
#ifdef MAN
	folder += "0";
#endif
#ifdef ST1
	folder += "1";
#endif
#ifdef ST2
	folder += "2";
#endif
#ifdef COMBO
	folder += QString::number (R_0) + ',' + QString::number (R_1) + ',' + QString::number (R_2);
#endif
	system (QString ("mkdir " + folder).toStdString ().data ());
	_road = new QFile (folder + "\\road.csv");
	_car = new QFile (folder + "\\car.csv");
	_stop = new QFile (folder + "\\stop.csv");
	_road->open (QIODevice::WriteOnly);
	_car->open (QIODevice::WriteOnly);
	_stop->open (QIODevice::WriteOnly);
	__road = new QTextStream (_road);
	__stop = new QTextStream (_stop);
	__car = new QTextStream (_car);
	(*__car) << QString ("init_velocity,thoritical_time,act_time,delta\n");
	(*__road) << QString ("time,00,01,02,10,11,12,20,21,22,30,31,32,AVE,ALL\n");
	(*__stop) << QString ("time,00,01,02,10,11,12,20,21,22,30,31,32,AVE,ALL\n");
}