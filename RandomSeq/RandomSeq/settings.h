#pragma once
#include "basic.h"

class Settings : public QMainWindow {
	Q_OBJECT
private:
	int size;
	QSlider* slider;
	com_label* label;
	com_check* check;
	QPushButton* set_green;
	QPushButton* set_yellow;
	QPushButton* set_red;
	QPushButton* _reset;
	QLabel* p;
	QLabel* n;
	QLineEdit* _p;
	QLineEdit* _n;
public:
	QPushButton* save;
	QPen* main_line;
	QPen* dot_line;
	QPen* edge;
	QBrush* car;
	int period;
	Light* map;
	Settings (int _size, QWidget * parent = Q_NULLPTR);
	~Settings ();
	public slots:
	void paintEvent (QPaintEvent *);
protected:
	void closeEvent (QCloseEvent *event);
};