#ifndef TRAFFIC_V1_H
#define TRAFFIC_V1_H
#define FIN
#define BAT

#define MAX_LOAD

#define MANUAL
#define ST1
#define ST2
#define COMBO
#include "property.hpp"
#define SUM (R_0+R_1+R_2)
#ifndef BAT
#pragma  region _PRE_DEF_
#define __A__ 0
#define __B__ 0
#define __C__ 0
#define __D__ 0
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
#ifdef COMBO
#undef __D__
#define __D__ 1
#endif
#if __A__+__B__+__C__+__D__- 1
#error Specific stategy need to be chosen
#endif
#if __D__&&!(R_0>=0&&R_1>=0&&R_2>=0&&(R_0+R_1+R_2))
#error Combo stategy ratio need to be corrected
#endif
#pragma  endregion
#endif
#include <QtWidgets/QMainWindow>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <QDate>
#include <QList>
#include <QDebug>
#include <QRadioButton>
#include <iterator>
#include <cmath>
#include <random>
#include "settings.hpp"
#include "basic.h"

struct Car {
	int time_arr;
	double acc;
	double vec;
	double pos;
	int index;
	double vec_init;
	int enter_time_d;
	Type type;
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
public:
#ifdef BAT
	int R_0;
	int R_1;
	int R_2;
#endif
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
public:
	QPushButton* start;
private:
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
	QFile* _road;
	QFile* _car;
	QFile* _stop;
	QFile* _stopT;
	QTextStream* __stopT;
	QTextStream* __road;
	QTextStream* __car;
	QTextStream* __stop;
	int stop_num[TR_NUM*DIR_NUM];
	int stop_time[TR_NUM*DIR_NUM];
private:
	double CalMaxTime (double pos, double vec);
	double CalMinTime (double pos, double vec);
	void sim ();
	void generate ();
	void following ();
	void st1 ();
	void st2 ();
	void _following ();
	void free (QList<Car>::iterator i, int d);
	void head (QList<Car>::iterator i, int d);
	void st1_free (QList<Car>::iterator i, int d);
	void st1_head (QList<Car>::iterator i, int d);
	void c_write (Car car);
	void main_write ();
public:
	void init_write ();
private:
	void combo ();
public:
	Traffic_v1 (QWidget *parent = 0);
	~Traffic_v1 ();
	public slots:
	void paintEvent (QPaintEvent *event);
};

#define GetTime (int(now_t/10))
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define Get(i) (s->map[GetTime%s->period][dir (i)][tr (i)])
#define WILL(j,i) (s->map[(j)%s->period][dir (i)][tr (i)])
#define PERIOD (s->period)
#define _head car_in[i].first()
#define min(i,j) ((i<j)?(i):(j))
#define max(i,j) ((i>j)?(i):(j))

#endif // TRAFFIC_V1_H
