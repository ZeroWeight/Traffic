#include "traffic_v1.h"

Traffic_v1::Traffic_v1 (QWidget *parent)
	: QMainWindow (parent) {
	size = QApplication::desktop ()->height () / 15;
	meter = size / 10.0;
	s = new Settings (size, nullptr);
	this->setFixedHeight (size*12.25);
	this->setFixedWidth (size * 23);
	s->setFixedHeight (size*12.25);
	s->setFixedWidth (size * 23);
	s->hide ();
	scale = new QLabel (this);
	scale->setGeometry (15 * size, size, size, 0.7*size);
	scale->setText ("Time\r\nScale");
	scale->setFont (QFont ("TimesNewRoman", 10));
	scale_t = 1;
	scaleEdit = new QLineEdit (this);
	scaleEdit->setText (QString::number (scale_t));
	scaleEdit->setGeometry (16.5*size, size, size, 0.7*size);
	scaleEdit->setFont (QFont ("TimesNewRoman", 10));
	now = new QLabel (this);
	now->setText ("Time:\t0 s");
	now->setFont (QFont ("TimesNewRoman", 16));
	now->setGeometry (15 * size, 2 * size, 3 * size, size);
	now_t = 0;
	_reset = new QPushButton (this);
	_reset->setGeometry (19 * size, 0.9*size, 2 * size, size);
	_reset->setText ("Reset\r\nSet time scale");
	start = new QPushButton (this);
	start->setText ("Start");
	start->setFont (QFont ("TimesNewRoman", 16));
	start->setGeometry (15 * size, 3.5*size, 2 * size, size);
	end = new QPushButton (this);
	end->setText ("Pause");
	end->setFont (QFont ("TimesNewRoman", 16));
	end->setGeometry (17.5 * size, 3.5*size, 2 * size, size);
	timer = new QTimer (this);
	timer->setInterval (10);
	timer->stop ();
	end->setEnabled (false);
	connect (start, SIGNAL (clicked ()), this, SLOT (Ref_Start ()));
	connect (end, SIGNAL (clicked ()), this, SLOT (Ref_End ()));
	connect (timer, SIGNAL (timeout ()), this, SLOT (Ref_Timer ()));
	connect (_reset, SIGNAL (clicked ()), this, SLOT (Ref_Reset ()));
	edit = new QPushButton (this);
	edit->setText ("Edit the\r\nTraffic light");
	edit->setGeometry (15 * size, 7 * size, 2 * size, size);
	edit->setFont (QFont ("TimesNewRoman", 10));
	connect (edit, SIGNAL (clicked ()), this, SLOT (hide ()));
	connect (edit, SIGNAL (clicked ()), s, SLOT (show ()));
	connect (s->save, SIGNAL (clicked ()), this, SLOT (show ()));
}

Traffic_v1::~Traffic_v1 () {}
void Traffic_v1::paintEvent (QPaintEvent *event) {
	QPainter painter (this);
	painter.setPen (*(s->main_line));
	for (int i = 0; i < 3; i++) {
		painter.drawLine (size*far_side[A], size*main_lane[i], size*near_side[A], size*main_lane[i]);
		painter.drawLine (size*main_lane[i], size*far_side[B], size*main_lane[i], size*near_side[B]);
		painter.drawLine (size*far_side[C], size*main_lane[i], size*near_side[C], size*main_lane[i]);
		painter.drawLine (size*main_lane[i], size*far_side[D], size*main_lane[i], size*near_side[D]);
	}
	painter.setPen (*(s->dot_line));
	for (int i = 0; i < 4; i++) {
		painter.drawLine (size*far_side[A], size*aux_lane[i], size*near_side[A], size*aux_lane[i]);
		painter.drawLine (size*aux_lane[i], size*far_side[B], size*aux_lane[i], size*near_side[B]);
		painter.drawLine (size*far_side[C], size*aux_lane[i], size*near_side[C], size*aux_lane[i]);
		painter.drawLine (size*aux_lane[i], size*far_side[D], size*aux_lane[i], size*near_side[D]);
	}
}
void Traffic_v1::Ref_End () {
	if (timer->isActive ()) {
		start->setText ("Resume");
		end->setText ("End");
	}
	else {
		start->setText ("Start");
		end->setEnabled (false);
		this->now_t = 0;
		now->setText ("Time:\t0 s");
		_reset->setEnabled (true);
		scaleEdit->setEnabled (true);
	}
	timer->stop ();
	start->setEnabled (true);
}
void Traffic_v1::Ref_Start () {
	scaleEdit->setText (QString::number (scale_t));
	scaleEdit->setEnabled (false);
	end->setText ("Pause");
	end->setEnabled (true);
	_reset->setEnabled (false);
	start->setEnabled (false);
	timer->start ();
}
void Traffic_v1::Ref_Timer () {
	now_t += scale_t;
	now->setText ("Time:\t" + QString::number (now_t) + " s");
}
void Traffic_v1::Ref_Reset () {
	now_t = 0;
	now->setText ("Time:\t0 s");
	if (scaleEdit->text ().toInt ())
		scale_t = scaleEdit->text ().toInt ();
	else {
		QMessageBox temp;
		temp.setText ("ILLEGAL INPUT");
		temp.exec ();
		scaleEdit->setText (QString::number (scale_t));
	}
}