#ifndef TRAFFIC_V1_H
#define TRAFFIC_V1_H
#define FIN
#undef FIN

#define MAX_LOAD

#define MANUAL
#define ST1
#define ST2

#undef MAX_LOAD

//uncomment the lines below to switch the stategy
#undef MANUAL
//#undef ST1
#undef ST2
#pragma  region _PRE_DEF_
#define __A__ 0
#define __B__ 0
#define __C__ 0
#ifdef MANUAL
#undef __A__
#define __A__ 1
#endif
#ifdef ST1
#undef __B__
#define __B__ 1
#endif
#ifdef ST2
#undef __C__
#define __C__ 1
#endif
#if __A__+__B__+__C__- 1
#error Specific stategy need to be chosen
#endif
#pragma  endregion
#include <QtWidgets/QMainWindow>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>
#include <QList>
#include <QDebug>
#include <QRadioButton>
#include <iterator>
#include <cmath>
#include <random>
#include "settings.hpp"
enum MODE {
	STAT,
	RUN,
	BLOCK,
};
struct Car {
	int arr_time;
	MODE mode;
	int block;
	double acc;
	double vec;
	double pos;
	int index;
};
struct InNode {
	DIR dir;
	TR tr;
	double vec;
	int delay_time;
	int index;
};
class Traffic_v1 : public QMainWindow {
	Q_OBJECT
private:
	int size;
	double meter;
	int now_t;
	int index = 0;
	int scale_t;
	Settings* s;
	QLabel* scale;
	QLabel* now;
	QLineEdit* scaleEdit;
	QPushButton* _reset;
	QPushButton* start;
	QPushButton* end;
	QTimer* timer;
	QPushButton* edit;
	/*---------------------*/
	QList<Car>* car_in;
	QList<Car>* car_out;
	QList<InNode>* Node;
	QList<Car>* car_block;
	/*--------------------*/
	QRadioButton* fast;
	QRadioButton* medium;
	QRadioButton* slow;
	QRadioButton* very_slow;
	QLabel* speed;
	com_label* st;
	com_label* _st;
	QSlider* A_L;
	QSlider* B_L;
	QSlider* C_L;
	QSlider* D_L;
	QLabel*A_A;
	QLabel*B_A;
	QLabel*C_A;
	QLabel*D_A;
	QLabel*A_B;
	QLabel*B_B;
	QLabel*C_B;
	QLabel*D_B;

private:
	double CalMaxTime (double pos, double vec);
	double CalMinTime (double pos, double vec);
	void sim ()const;
	void generate ();
	void following ();
	void st1 ();
	void st2 ();
	void _following ();
	void free (QList<Car>::iterator i, int d);
	void head (QList<Car>::iterator i, int d);
	void st1_free (QList<Car>::iterator i, int d);
	void st1_head (QList<Car>::iterator i, int d);
public:
	Traffic_v1 (QWidget *parent = 0);
	~Traffic_v1 ();
	public slots:
	void paintEvent (QPaintEvent *event);
};

#endif // TRAFFIC_V1_H
