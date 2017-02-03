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
enum MODE {
	STAT,
	RUN,
	BLOCK,
};
struct Car {
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
	void sim ()const;
	void generate ();
	void following ();
	void st1 ();
	void st2 ();
	void _following ();
	void free (QList<Car>::iterator i, int d);
	void head (QList<Car>::iterator i, int d);
public:
	Traffic_v1 (QWidget *parent = 0);
	~Traffic_v1 ();
	public slots:
	void paintEvent (QPaintEvent *event);
};

#endif // TRAFFIC_V1_H
