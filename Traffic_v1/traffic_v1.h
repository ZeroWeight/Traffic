#ifndef TRAFFIC_V1_H
#define TRAFFIC_V1_H

#include <QtWidgets/QMainWindow>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>
#include "settings.hpp"

class Traffic_v1 : public QMainWindow {
	Q_OBJECT
private:
	int now_t;
	int scale_t;
	Settings* s;
	int size;
	double meter;
	QLabel* scale;
	QLabel* now;
	QLineEdit* scaleEdit;
	QPushButton* _reset;
	QPushButton* start;
	QPushButton* end;
	QTimer* timer;
	QPushButton* edit;
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
