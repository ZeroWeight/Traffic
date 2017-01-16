#ifndef TRAFFIC_V1_H
#define TRAFFIC_V1_H

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
struct Car {
	double acc;
	double vec;
	double pos;
	int index;
};
struct InNode {
	DIR dir;
	TR tr;
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
	QList<Car>* car_in;
	QList<Car>* car_out;
	QList<InNode>* Node;
	QRadioButton* fast;
	QRadioButton* medium;
	QRadioButton* slow;
	QRadioButton* very_slow;
	QLabel* speed;
	com_label* st;
	com_label* _st;
private:
	void Res ();
	void sim ();
	void generate ();
	void strategy ();
	void following ();
	void _following ();
public:
	Traffic_v1 (QWidget *parent = 0);
	~Traffic_v1 ();
	public slots:
	void paintEvent (QPaintEvent *event);
	private slots:
	void Ref_Start ();
	void Ref_End ();
	void Ref_Timer ();
	void Ref_Reset ();
	inline void f (bool b) { if (b) timer->setInterval (1); }
	inline void m (bool b) { if (b) timer->setInterval (10); }
	inline void _s (bool b) { if (b) timer->setInterval (100); }
	inline void ss (bool b) { if (b) timer->setInterval (1000); }
};

#endif // TRAFFIC_V1_H
