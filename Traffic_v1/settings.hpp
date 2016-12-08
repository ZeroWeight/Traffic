#pragma once
#include <QWidget>
#include <QPainter>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include "basic.h"
typedef Color Light[DIR_NUM][TR_NUM];
typedef QLabel* com_label;
typedef QCheckBox* com_check;
class Settings : public QWidget {
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
	QPushButton* save;
	QLabel* p;
	QLabel* n;
	QLineEdit* _p;
	QLineEdit* _n;
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
	private slots:
	void Ref_G ();
	void Ref_Y ();
	void Ref_R ();
	void Ref_S ();
	void Ref_n ();
	void Ref_r ();
};