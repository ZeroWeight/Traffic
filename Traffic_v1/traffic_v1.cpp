#include "traffic_v1.h"

Traffic_v1::Traffic_v1 (QWidget *parent)
	: QMainWindow (parent) {
	size = QApplication::desktop ()->height () / 15;
	meter = size / 10.0;
	s = new Settings (size, this);
	this->resize (size * 23, size*12.25);
	s->setGeometry (0, 0, size * 23, size*12.25);
	s->show ();
}

Traffic_v1::~Traffic_v1 () {}