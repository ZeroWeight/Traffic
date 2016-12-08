#pragma once
#include <QWidget>
#include "basic.h"

class Settings : public QWidget {
	Q_OBJECT
private:
	int size;
public:
	Settings (int _size, QWidget * parent = Q_NULLPTR);
	~Settings ();
};