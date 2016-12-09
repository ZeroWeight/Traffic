#ifndef TRAFFIC_V1_H
#define TRAFFIC_V1_H

#include <QtWidgets/QMainWindow>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>
#include <QList>
#include <iterator>
#include "settings.hpp"
struct Car {
	double acc;
	double vec;
	double pos;
};
struct InNode {
	DIR dir;
	TR tr;
	int delay_time;
};
class Traffic_v1 : public QMainWindow {
	Q_OBJECT
private:
	int size;
	double meter;
	int now_t;
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
private:
	void sim ();
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
};

#endif // TRAFFIC_V1_H
