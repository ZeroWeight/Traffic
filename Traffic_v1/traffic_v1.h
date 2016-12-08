#ifndef TRAFFIC_V1_H
#define TRAFFIC_V1_H

#include <QtWidgets/QMainWindow>
#include "ui_traffic_v1.h"

class Traffic_v1 : public QMainWindow
{
	Q_OBJECT

public:
	Traffic_v1(QWidget *parent = 0);
	~Traffic_v1();

private:
	Ui::Traffic_v1Class ui;
};

#endif // TRAFFIC_V1_H
