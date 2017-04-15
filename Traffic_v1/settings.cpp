#include "settings.hpp"
#define COMBO
#include "property.hpp"
Settings::Settings (int _size, QWidget * parent) : QMainWindow (parent), size (_size) {
	main_line = new QPen (QBrush (QColor ("Black")), 5);
	dot_line = new QPen (QBrush (QColor ("Yellow")), 3);
	edge = new QPen (QColor ("Black"));
	QVector<qreal> dashes;
	dashes << 5 << 3 << 5 << 3;
	dot_line->setDashPattern (dashes);
	car = new QBrush (QColor ("Blue"));
	period = 300;
#ifdef _60_B_
	period = 60;
#endif
#ifdef _90_B_
	period = 90;
#endif
#ifdef _120_B_
	period = 120;
#endif
#ifdef _150_B_
	period = 150;
#endif

	map = new Light[period];
	for (int i = 0; i < (period >> 1); ++i) for (int k = 0; k < TR_NUM; k++) {
		map[i][DIR::A][k] = map[i][DIR::C][k] = Color::Red;
		map[i][DIR::B][k] = map[i][DIR::D][k] = Color::Green;
	}
	for (int i = (period >> 1); i < period; ++i) for (int k = 0; k < TR_NUM; k++) {
		map[i][DIR::A][k] = map[i][DIR::C][k] = Color::Green;
		map[i][DIR::B][k] = map[i][DIR::D][k] = Color::Red;
	}
	slider = new QSlider (Qt::Horizontal, this);
	slider->setGeometry (15.5 * size, 0.7*size, 6 * size, 0.5*size);
	slider->setMinimum (0);
	slider->setMaximum (period - 1);
	connect (slider, SIGNAL (valueChanged (int)), this, SLOT (update ()));
	label = new com_label[DIR_NUM*TR_NUM];
	check = new com_check[DIR_NUM*TR_NUM];
	for (int i = 0; i < DIR_NUM; i++)
		for (int j = 0; j < TR_NUM; j++) {
			label[i*TR_NUM + j] = new QLabel (this);
			label[i*TR_NUM + j]->setFont (QFont ("TimesNewsRoman", 10));
			label[i*TR_NUM + j]->setGeometry (14 * size, 1.5*size + (TR_NUM*i + j)*0.5*size, size, 0.3*size);
			switch (i) {
			case A: label[i*TR_NUM + j]->setText ("W"); break;
			case B: label[i*TR_NUM + j]->setText ("S"); break;
			case C: label[i*TR_NUM + j]->setText ("E"); break;
			case D: label[i*TR_NUM + j]->setText ("N"); break;
			}
			switch (j) {
			case Left:label[i*TR_NUM + j]->setText (label[i*TR_NUM + j]->text () + "L"); break;
			case Right:label[i*TR_NUM + j]->setText (label[i*TR_NUM + j]->text () + "R"); break;
			case Center:label[i*TR_NUM + j]->setText (label[i*TR_NUM + j]->text () + "C"); break;
			}
			check[i*TR_NUM + j] = new QCheckBox (this);
			check[i*TR_NUM + j]->setGeometry (22 * size, 1.5*size + (TR_NUM*i + j)*0.5*size, size, 0.3*size);
		}
	set_green = new QPushButton (this);
	set_yellow = new QPushButton (this);
	set_red = new QPushButton (this);
	set_green->setGeometry (14 * size, 7.8 * size, 2 * size, size);
	set_yellow->setGeometry (14 * size, 9.3 * size, 2 * size, size);
	set_red->setGeometry (14 * size, 10.8 * size, 2 * size, size);
	set_green->setText ("Set Green Behind");
	set_yellow->setText ("Set Yellow Behind");
	set_red->setText ("Set Red Behind");
	connect (set_green, &QPushButton::clicked, [=](void) {
		for (int i = 0; i < DIR_NUM; i++)
			for (int j = 0; j < TR_NUM; j++)
				if (check[i*TR_NUM + j]->isChecked ())
					for (int k = slider->value (); k < period; k++)
						map[k][i][j] = Green;
		this->update ();
		for (int i = 0; i < DIR_NUM*TR_NUM; i++)
			check[i]->setChecked (false);
	});
	connect (set_yellow, &QPushButton::clicked, [=](void) {
		for (int i = 0; i < DIR_NUM; i++)
			for (int j = 0; j < TR_NUM; j++)
				if (check[i*TR_NUM + j]->isChecked ())
					for (int k = slider->value (); k < period; k++)
						map[k][i][j] = Yellow;
		this->update ();
		for (int i = 0; i < DIR_NUM*TR_NUM; i++)
			check[i]->setChecked (false);
	});
	connect (set_red, &QPushButton::clicked, [=](void) {
		for (int i = 0; i < DIR_NUM; i++)
			for (int j = 0; j < TR_NUM; j++)
				if (check[i*TR_NUM + j]->isChecked ())
					for (int k = slider->value (); k < period; k++)
						map[k][i][j] = Red;
		this->update ();
		for (int i = 0; i < DIR_NUM*TR_NUM; i++)
			check[i]->setChecked (false);
	});
	p = new QLabel (this);
	n = new QLabel (this);
	_p = new QLineEdit (this);
	_n = new QLineEdit (this);
	p->setGeometry (17 * size, 7.8*size, size, 0.5*size);
	n->setGeometry (17 * size, 8.5*size, size, 0.5*size);
	_p->setGeometry (18.5 * size, 7.8*size, size, 0.5*size);
	_n->setGeometry (18.5 * size, 8.5*size, size, 0.5*size);
	p->setText ("Period");
	n->setText ("Now");
	p->setFont (QFont ("TimesNewsRoman", 10));
	n->setFont (QFont ("TimesNewsRoman", 10));
	_p->setFont (QFont ("TimesNewsRoman", 10));
	_n->setFont (QFont ("TimesNewsRoman", 10));
	_p->setText (QString::number (period));
	_n->setText (QString::number (slider->value ()));
	connect (slider, &QSlider::valueChanged, [=](const int i) {
		_n->setText (QString::number (i));
	});
	connect (_n, &QLineEdit::editingFinished, [=](void) {
		if (_n->text ().toInt () <= period&&_n->text ().toInt () > 0)
			slider->setValue (_n->text ().toInt ());
		else {
			QMessageBox temp;
			temp.setText ("ILLEGAL INPUT");
			temp.exec ();
			_n->setText (QString::number (slider->value ()));
		}
	});
	_reset = new QPushButton (this);
	_reset->setText ("Reset\r\nchange the period");
	_reset->setGeometry (17 * size, 9.3 * size, 2 * size, size);
	connect (_reset, &QPushButton::clicked, [=](void) {
		if (_p->text ().toInt () > 0) {
			slider->setValue (0);
			_n->setText ("0");
			period = _p->text ().toInt ();
			delete[] map;
			map = new Light[period];
			slider->setMaximum (period - 1);
			for (int i = 0; i < period; i++) for (int j = 0; j < DIR_NUM; j++) for (int k = 0; k < TR_NUM; k++)
				map[i][j][k] = Red;
		}
		else {
			QMessageBox temp;
			temp.setText ("ILLEGAL INPUT");
			temp.exec ();
			_p->setText (QString::number (period));
		}
		this->update ();
	});
	save = new QPushButton (this);
	save->setText ("Save\r\nQuit");
	save->setFont (QFont ("TimesNewsRoman", 10));
	save->setGeometry (17 * size, 10.8 * size, 2 * size, size);
	connect (save, SIGNAL (clicked ()), this, SLOT (hide ()));
}

Settings::~Settings () {}

void Settings::paintEvent (QPaintEvent *) {
	slider->setMinimum (0);
	slider->setMaximum (period - 1);
	double r = 0.2*size;
	QPainter painter (this);
	painter.setPen (*main_line);
	for (int i = 0; i < 3; i++) {
		painter.drawLine (size*far_side[A], size*main_lane[i], size*near_side[A], size*main_lane[i]);
		painter.drawLine (size*main_lane[i], size*far_side[B], size*main_lane[i], size*near_side[B]);
		painter.drawLine (size*far_side[C], size*main_lane[i], size*near_side[C], size*main_lane[i]);
		painter.drawLine (size*main_lane[i], size*far_side[D], size*main_lane[i], size*near_side[D]);
	}
	painter.setPen (*dot_line);
	for (int i = 0; i < 4; i++) {
		painter.drawLine (size*far_side[A], size*aux_lane[i], size*near_side[A], size*aux_lane[i]);
		painter.drawLine (size*aux_lane[i], size*far_side[B], size*aux_lane[i], size*near_side[B]);
		painter.drawLine (size*far_side[C], size*aux_lane[i], size*near_side[C], size*aux_lane[i]);
		painter.drawLine (size*aux_lane[i], size*far_side[D], size*aux_lane[i], size*near_side[D]);
	}
	for (int i = 0; i < period; i++)
		for (int j = 0; j < DIR_NUM; j++)
			for (int k = 0; k < TR_NUM; k++)
				switch (map[i][j][k]) {
				case Red:
					painter.setPen ((QColor ("Red")));
					painter.setBrush ((QColor ("Red")));
					painter.drawRect (15.5 * size + i * 6.0 * size / period, 1.5*size + (TR_NUM*j + k)*0.5*size,
						6.0 * size / period, 0.3*size);
					break;
				case Green:
					painter.setPen ((QColor ("Green")));
					painter.setBrush ((QColor ("Green")));
					painter.drawRect (15.5 * size + i * 6.0 * size / period, 1.5*size + (TR_NUM*j + k)*0.5*size,
						6.0 * size / period, 0.3*size);
					break;
				case Yellow:
					painter.setPen ((QColor ("Yellow")));
					painter.setBrush ((QColor ("Yellow")));
					painter.drawRect (15.5 * size + i * 6.0 * size / period, 1.5*size + (TR_NUM*j + k)*0.5*size,
						6.0 * size / period, 0.3*size);
					break;
				}
	painter.setPen (QPen (QColor ("black"), 3));
	painter.drawLine (15.5 * size + 6.0*(slider->value () + 0.5)*size / period, 1.5*size,
		15.5 * size + 6.0*(slider->value () + 0.5)*size / period, 7.5 * size);
	for (int i = 0; i < TR_NUM; i++) {
		switch (map[slider->value ()][A][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (near_side[C] * size - 1.2 * r, lane_in[A][i] * size, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (near_side[C] * size - 1.2 * r, lane_in[A][i] * size, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (near_side[C] * size - 1.2 * r, lane_in[A][i] * size, r, r);
			break;
		}
		switch (map[slider->value ()][B][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (lane_in[B][i] * size, near_side[D] * size + 1.2 * r, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (lane_in[B][i] * size, near_side[D] * size + 1.2 * r, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (lane_in[B][i] * size, near_side[D] * size + 1.2 * r, r, r);
			break;
		}
		switch (map[slider->value ()][C][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (near_side[A] * size + 1.2 * r, lane_in[C][i] * size, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (near_side[A] * size + 1.2 * r, lane_in[C][i] * size, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (near_side[A] * size + 1.2 * r, lane_in[C][i] * size, r, r);
			break;
		}
		switch (map[slider->value ()][D][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (lane_in[D][i] * size, near_side[B] * size - 1.2 * r, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (lane_in[D][i] * size, near_side[B] * size - 1.2 * r, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (lane_in[D][i] * size, near_side[B] * size - 1.2 * r, r, r);
			break;
		}
	}
	painter.setPen (QPen (QColor ("Blue"), 5));
	// don't change the code below
	if (map[slider->value ()][A][Left] == Green)
		painter.drawArc (3.6875 * size, 3.6875 * size, 2.625*size, 2.625*size, -90 * 16, 90 * 16);
	if (map[slider->value ()][A][Right] == Green)
		painter.drawArc (4.8125 * size, 7.0625 * size, 0.375*size, 0.375*size, 0, 90 * 16);
	if (map[slider->value ()][A][Center] == Green)
		painter.drawLine (5 * size, 6.6875 * size, 7.25*size, 6.6875*size);

	if (map[slider->value ()][B][Left] == Green)
		painter.drawArc (3.6875 * size, 5.9375 * size, 2.625*size, 2.625*size, 0, 90 * 16);
	if (map[slider->value ()][B][Right] == Green)
		painter.drawArc (7.0625 * size, 7.0625 * size, 0.375*size, 0.375*size, 90 * 16, 90 * 16);
	if (map[slider->value ()][B][Center] == Green)
		painter.drawLine (6.6875 * size, 5 * size, 6.6875*size, 7.25*size);

	if (map[slider->value ()][C][Left] == Green)
		painter.drawArc (5.9375 * size, 5.9375 * size, 2.625*size, 2.625*size, 90 * 16, 90 * 16);
	if (map[slider->value ()][C][Right] == Green)
		painter.drawArc (7.0625 * size, 4.8125 * size, 0.375*size, 0.375*size, 180 * 16, 90 * 16);
	if (map[slider->value ()][C][Center] == Green)
		painter.drawLine (5 * size, 5.5625 * size, 7.25*size, 5.5625*size);

	if (map[slider->value ()][D][Left] == Green)
		painter.drawArc (5.9375 * size, 3.6875 * size, 2.625*size, 2.625*size, 180 * 16, 90 * 16);
	if (map[slider->value ()][D][Right] == Green)
		painter.drawArc (4.8125 * size, 4.8175 * size, 0.375*size, 0.375*size, 0, -90 * 16);
	if (map[slider->value ()][D][Center] == Green)
		painter.drawLine (5.5625 * size, 7.25 * size, 5.5625*size, 5 * size);

	QPen* pen = new QPen (*dot_line);
	pen->setColor (QColor ("Blue"));
	painter.setPen (*pen);
	// don't change the code below
	if (map[slider->value ()][A][Left] == Yellow)
		painter.drawArc (3.6875 * size, 3.6875 * size, 2.625*size, 2.625*size, -90 * 16, 90 * 16);
	if (map[slider->value ()][A][Right] == Yellow)
		painter.drawArc (4.8125 * size, 7.0625 * size, 0.375*size, 0.375*size, 0, 90 * 16);
	if (map[slider->value ()][A][Center] == Yellow)
		painter.drawLine (5 * size, 6.6875 * size, 7.25*size, 6.6875*size);

	if (map[slider->value ()][B][Left] == Yellow)
		painter.drawArc (3.6875 * size, 5.9375 * size, 2.625*size, 2.625*size, 0, 90 * 16);
	if (map[slider->value ()][B][Right] == Yellow)
		painter.drawArc (7.0625 * size, 7.0625 * size, 0.375*size, 0.375*size, 90 * 16, 90 * 16);
	if (map[slider->value ()][B][Center] == Yellow)
		painter.drawLine (6.6875 * size, 5 * size, 6.6875*size, 7.25*size);

	if (map[slider->value ()][C][Left] == Yellow)
		painter.drawArc (5.9375 * size, 5.9375 * size, 2.625*size, 2.625*size, 90 * 16, 90 * 16);
	if (map[slider->value ()][C][Right] == Yellow)
		painter.drawArc (7.0625 * size, 4.8125 * size, 0.375*size, 0.375*size, 180 * 16, 90 * 16);
	if (map[slider->value ()][C][Center] == Yellow)
		painter.drawLine (5 * size, 5.5625 * size, 7.25*size, 5.5625*size);

	if (map[slider->value ()][D][Left] == Yellow)
		painter.drawArc (5.9375 * size, 3.6875 * size, 2.625*size, 2.625*size, 180 * 16, 90 * 16);
	if (map[slider->value ()][D][Right] == Yellow)
		painter.drawArc (4.8125 * size, 4.8175 * size, 0.375*size, 0.375*size, 0, -90 * 16);
	if (map[slider->value ()][D][Center] == Yellow)
		painter.drawLine (5.5625 * size, 7.25 * size, 5.5625*size, 5 * size);
}
void Settings::closeEvent (QCloseEvent *event) {
	save->click ();
	event->ignore ();
}