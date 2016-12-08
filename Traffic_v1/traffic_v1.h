#ifndef TRAFFIC_V1_H
#define TRAFFIC_V1_H

#include <QtWidgets/QMainWindow>
#include <QDesktopWidget>
#include <QApplication>
#include "settings.hpp"

class Traffic_v1 : public QMainWindow {
	Q_OBJECT
private:
	Settings* s;
	int size;
	double meter;
public:
	Traffic_v1 (QWidget *parent = 0);
	~Traffic_v1 ();
};

#endif // TRAFFIC_V1_H
