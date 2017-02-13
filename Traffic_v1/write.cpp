#include "traffic_v1.h"

void Traffic_v1::c_write (Car car) {}

void Traffic_v1::main_write () {}

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
	QString command = "mkdir " + folder;
	system (command.toStdString ().data ());
	QString road = folder + "\\road.csv";
	QString car = folder + "\\car.csv";
	_road = new QFile (road);
	_car = new QFile (car);
	qDebug () << _road->open (QIODevice::WriteOnly);
	qDebug () << _car->open (QIODevice::WriteOnly);
}