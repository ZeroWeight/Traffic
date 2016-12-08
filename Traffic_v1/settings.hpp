#pragma once
#include <QWidget>
#include <QPainter>
#include <QSlider>
#include "basic.h"
typedef Color Light[DIR_NUM][TR_NUM];
class Settings : public QWidget {
	Q_OBJECT
private:
	int size;
	QSlider* slider;
public:
#pragma region painter
	QPen* main_line;
	QPen* dot_line;
	QPen* edge;
	QBrush* car;
#pragma  endregion
	int period;
	Light* map;
	Settings (int _size, QWidget * parent = Q_NULLPTR);
	~Settings ();
	public slots:
	void paintEvent (QPaintEvent *);
};