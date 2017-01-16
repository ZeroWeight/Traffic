#include "traffic_v1.h"
double expdf (double lambda) {
	double pV = 0.0;
	while (1) {
		pV = (double)rand () / (double)RAND_MAX;
		if (pV != 1)
			break;
	}
	pV = (-1.0 / lambda)*log (1 - pV);
	return pV;
}
static const double lambda[DIR_NUM] = { 0.5,0.5,0.5,0.5 };
static double go[DIR_NUM] = { 0 };
static std::default_random_engine e;
static std::normal_distribution<double> ND_V (15.0, 2);
static std::normal_distribution<double> ND_A (2.5, 0.8);
Traffic_v1::Traffic_v1 (QWidget *parent)
	: QMainWindow (parent) {
	for (int i = 0; i < DIR_NUM; i++) {
		go[i] = expdf (lambda[i]);
	}
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
	now->setGeometry (15 * size, 2 * size, 5 * size, size);
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
	edit->setText ("Edit the Traffic light");
	edit->setGeometry (15 * size, 5 * size, 4.5 * size, size);
	edit->setFont (QFont ("TimesNewRoman", 10));
	connect (edit, SIGNAL (clicked ()), this, SLOT (hide ()));
	connect (edit, SIGNAL (clicked ()), s, SLOT (show ()));
	connect (s->save, SIGNAL (clicked ()), this, SLOT (show ()));
	car_in = new QList<Car>[DIR_NUM*TR_NUM];
	car_out = new QList<Car>[DIR_NUM*TR_NUM];
	Node = new QList<InNode> ();
	fast = new QRadioButton ("Fast", this);
	medium = new QRadioButton ("Medium", this);
	slow = new QRadioButton ("Slow", this);
	very_slow = new QRadioButton ("Very\r\nSlow", this);
	fast->setFont (QFont ("TimesNewRoman", 10));
	medium->setFont (QFont ("TimesNewRoman", 10));
	slow->setFont (QFont ("TimesNewRoman", 10));
	very_slow->setFont (QFont ("TimesNewRoman", 10));
	fast->setGeometry (16.5 * size, 6.5*size, size*1.5, size);
	medium->setGeometry (18 * size, 6.5*size, size*1.5, size);
	slow->setGeometry (19.5 * size, 6.5*size, size*1.5, size);
	very_slow->setGeometry (21 * size, 6.5*size, size*1.5, size);
	medium->setChecked (true);
	speed = new QLabel ("Sim Speed", this);
	speed->setFont (QFont ("TimesNewRoman", 10));
	speed->setGeometry (15 * size, 6.5*size, size*1.5, size);
	connect (fast, SIGNAL (toggled (bool)), this, SLOT (f (bool)));
	connect (medium, SIGNAL (toggled (bool)), this, SLOT (m (bool)));
	connect (slow, SIGNAL (toggled (bool)), this, SLOT (_s (bool)));
	connect (very_slow, SIGNAL (toggled (bool)), this, SLOT (ss (bool)));
}
Traffic_v1::~Traffic_v1 () {}
void Traffic_v1::paintEvent (QPaintEvent *event) {
	QPainter painter (this);
	painter.setFont (QFont ("TimesNewRoman", 12));
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
#pragma  region paint_car
	painter.setPen (QPen (QColor ("Blue"))); painter.setBrush (*s->car);
	for (int i = 0; i < TR_NUM; i++) {
		foreach (Car _car_, car_in[A*TR_NUM + i]) {
			if (_car_.pos >= -50.0) {
				painter.drawRect (near_side[A] * size + _car_.pos*meter - meter, lane_in[A][i] * size + -0.5*meter, 2 * meter, meter);
				painter.drawText (near_side[A] * size + _car_.pos*meter - meter, lane_in[A][i] * size + -0.5*meter, QString::number (_car_.index));
			}
		}
		foreach (Car _car_, car_in[B*TR_NUM + i]) {
			if (_car_.pos >= -50.0) {
				painter.drawRect (lane_in[B][i] * size + -0.5*meter, near_side[B] * size - _car_.pos*meter - meter, meter, 2 * meter);
				painter.drawText (lane_in[B][i] * size + -0.5*meter, near_side[B] * size - _car_.pos*meter - meter, QString::number (_car_.index));
			}
		}
		foreach (Car _car_, car_in[C*TR_NUM + i]) {
			if (_car_.pos >= -50.0) {
				painter.drawRect (near_side[C] * size - _car_.pos*meter - meter, lane_in[C][i] * size + -0.5*meter, 2 * meter, meter);
				painter.drawText (near_side[C] * size - _car_.pos*meter - meter, lane_in[C][i] * size + -0.5*meter, QString::number (_car_.index));
			}
		}
		foreach (Car _car_, car_in[D*TR_NUM + i]) {
			if (_car_.pos >= -50.0) {
				painter.drawRect (lane_in[D][i] * size + -0.5*meter, near_side[D] * size + _car_.pos*meter - meter, meter, 2 * meter);
				painter.drawText (lane_in[D][i] * size + -0.5*meter, near_side[D] * size + _car_.pos*meter - meter, QString::number (_car_.index));
			}
		}
	}
	painter.setPen (QPen (QColor ("Blue"), 5));
	foreach (InNode _n_, *Node) {
		if (_n_.delay_time >= 0) {
			switch (_n_.dir) {
			case A:
				switch (_n_.tr) {
				case Left:
					painter.drawArc (3.6875 * size, 3.6875 * size, 2.625*size, 2.625*size, -90 * 16, 90 * 16);
					break;
				case Right:
					painter.drawArc (4.8125 * size, 7.0625 * size, 0.375*size, 0.375*size, 0, 90 * 16);
					break;
				case Center:
					painter.drawLine (5 * size, 6.6875 * size, 7.25*size, 6.6875*size);
					break;
				}
				break;
			case B:
				switch (_n_.tr) {
				case Left:
					painter.drawArc (3.6875 * size, 5.9375 * size, 2.625*size, 2.625*size, 0, 90 * 16);
					break;
				case Right:
					painter.drawArc (7.0625 * size, 7.0625 * size, 0.375*size, 0.375*size, 90 * 16, 90 * 16);
					break;
				case Center:
					painter.drawLine (6.6875 * size, 5 * size, 6.6875*size, 7.25*size);
					break;
				}
				break;
			case C:
				switch (_n_.tr) {
				case Left:
					painter.drawArc (5.9375 * size, 5.9375 * size, 2.625*size, 2.625*size, 90 * 16, 90 * 16);
					break;
				case Right:
					painter.drawArc (7.0625 * size, 4.8125 * size, 0.375*size, 0.375*size, 180 * 16, 90 * 16);
					break;
				case Center:
					painter.drawLine (5 * size, 5.5625 * size, 7.25*size, 5.5625*size);
					break;
				}
				break;
			case D:
				switch (_n_.tr) {
				case Left:
					painter.drawArc (5.9375 * size, 3.6875 * size, 2.625*size, 2.625*size, 180 * 16, 90 * 16);
					break;
				case Right:
					break;
					painter.drawArc (4.8125 * size, 4.8175 * size, 0.375*size, 0.375*size, 0, -90 * 16);
				case Center:
					painter.drawLine (5.5625 * size, 7.25 * size, 5.5625*size, 5 * size);
					break;
				}
				break;
			}
		}
	}
	painter.setPen (QPen (QColor ("Blue"))); painter.setBrush (*s->car);
	foreach (Car _c_, car_out[A*TR_NUM + Left]) {
		if (_c_.pos < 50) {
			painter.drawRect (lane_in[B][Left] * size + -0.5*meter, near_side[D] * size - _c_.pos*meter - meter, meter, 2 * meter);
			painter.drawText (lane_in[B][Left] * size + -0.5*meter, near_side[D] * size - _c_.pos*meter - meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[A*TR_NUM + Right]) {
		if (_c_.pos < 50) {
			painter.drawRect (lane_in[D][Right] * size + -0.5*meter, near_side[B] * size + _c_.pos*meter - meter, meter, 2 * meter);
			painter.drawText (lane_in[D][Right] * size + -0.5*meter, near_side[B] * size + _c_.pos*meter - meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[A*TR_NUM + Center]) {
		if (_c_.pos < 50) {
			painter.drawRect (near_side[C] * size + _c_.pos*meter - meter, lane_in[A][Center] * size + -0.5*meter, 2 * meter, meter);
			painter.drawText (near_side[C] * size + _c_.pos*meter - meter, lane_in[A][Center] * size + -0.5*meter, QString::number (_c_.index));
		}
	}

	foreach (Car _c_, car_out[B*TR_NUM + Left]) {
		if (_c_.pos < 50) {
			painter.drawRect (near_side[A] * size - _c_.pos*meter - meter, lane_in[C][Left] * size + -0.5*meter, 2 * meter, meter);
			painter.drawText (near_side[A] * size - _c_.pos*meter - meter, lane_in[C][Left] * size + -0.5*meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[B*TR_NUM + Right]) {
		if (_c_.pos < 50) {
			painter.drawRect (near_side[C] * size + _c_.pos*meter - meter, lane_in[A][Right] * size + -0.5*meter, 2 * meter, meter);
			painter.drawText (near_side[C] * size + _c_.pos*meter - meter, lane_in[A][Right] * size + -0.5*meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[B*TR_NUM + Center]) {
		if (_c_.pos < 50) {
			painter.drawRect (lane_in[B][Center] * size + -0.5*meter, near_side[D] * size - _c_.pos*meter - meter, meter, 2 * meter);
			painter.drawText (lane_in[B][Center] * size + -0.5*meter, near_side[D] * size - _c_.pos*meter - meter, QString::number (_c_.index));
		}
	}

	foreach (Car _c_, car_out[C*TR_NUM + Left]) {
		if (_c_.pos < 50) {
			painter.drawRect (lane_in[D][Left] * size + -0.5*meter, near_side[B] * size + _c_.pos*meter - meter, meter, 2 * meter);
			painter.drawText (lane_in[D][Left] * size + -0.5*meter, near_side[B] * size + _c_.pos*meter - meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[C*TR_NUM + Right]) {
		if (_c_.pos < 50) {
			painter.drawRect (lane_in[B][Right] * size + -0.5*meter, near_side[D] * size - _c_.pos*meter - meter, meter, 2 * meter);
			painter.drawText (lane_in[B][Right] * size + -0.5*meter, near_side[D] * size - _c_.pos*meter - meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[C*TR_NUM + Center]) {
		if (_c_.pos < 50) {
			painter.drawRect (near_side[A] * size - _c_.pos*meter - meter, lane_in[C][Center] * size + -0.5*meter, 2 * meter, meter);
			painter.drawText (near_side[A] * size - _c_.pos*meter - meter, lane_in[C][Center] * size + -0.5*meter, QString::number (_c_.index));
		}
	}

	foreach (Car _c_, car_out[B*TR_NUM + Left]) {
		if (_c_.pos < 50) {
			painter.drawRect (near_side[C] * size + _c_.pos*meter - meter, lane_in[A][Left] * size + -0.5*meter, 2 * meter, meter);
			painter.drawText (near_side[C] * size + _c_.pos*meter - meter, lane_in[A][Left] * size + -0.5*meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[B*TR_NUM + Right]) {
		if (_c_.pos < 50) {
			painter.drawRect (near_side[A] * size - _c_.pos*meter - meter, lane_in[C][Right] * size + -0.5*meter, 2 * meter, meter);
			painter.drawText (near_side[A] * size - _c_.pos*meter - meter, lane_in[C][Right] * size + -0.5*meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[B*TR_NUM + Center]) {
		if (_c_.pos < 50) {
			painter.drawRect (lane_in[D][Center] * size + -0.5*meter, near_side[B] * size + _c_.pos*meter - meter, meter, 2 * meter);
			painter.drawText (lane_in[D][Center] * size + -0.5*meter, near_side[B] * size + _c_.pos*meter - meter, QString::number (_c_.index));
		}
	}
#pragma  endregion
	double r = 0.2*size;
	for (int i = 0; i < TR_NUM; i++) {
		switch (s->map[(now_t / 10) % s->period][A][i]) {
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
		switch (s->map[(now_t / 10) % s->period][B][i]) {
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
		switch (s->map[(now_t / 10) % s->period][C][i]) {
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
		switch (s->map[(now_t / 10) % s->period][D][i]) {
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
		Res ();
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
	++now_t;
	now->setText ("Time:\t" + QString::number (now_t / 10.0) + " s");
	generate ();
	if (!(now_t % (10 * scale_t))) strategy ();
	else following ();
	_following ();
	sim ();
	this->update ();
}
void Traffic_v1::Ref_Reset () {
	now_t = 0;
	now->setText ("Time:\t0 s");
	if (scaleEdit->text ().toInt () > 0)
		scale_t = scaleEdit->text ().toInt ();
	else {
		QMessageBox temp;
		temp.setText ("ILLEGAL INPUT");
		temp.exec ();
		scaleEdit->setText (QString::number (scale_t));
	}
	Res ();
}
void Traffic_v1::sim () {
	for (int i = 0; i < TR_NUM*DIR_NUM*(now_t != 0); i++) {
		QList<Car>::iterator _car_;
		if (!car_in[i].empty ()) {
			for (_car_ = car_in[i].begin (); _car_ != car_in[i].end (); ++_car_) {
				_car_->pos += _car_->vec*0.1 + 0.5*_car_->acc*0.01;
				_car_->vec += _car_->acc*0.1;
				if (_car_ != car_in[i].begin () && (_car_->pos - (_car_ - 1)->pos) > -1.0) {
					qDebug () << "E";
				}
			}
		}
		if (!car_out[i].empty ()) {
			for (_car_ = car_out[i].begin (); _car_ != car_out[i].end (); ++_car_) {
				_car_->pos += _car_->vec*0.1 + 0.5*_car_->acc*0.01;
				_car_->vec += _car_->acc*0.1;
			}
		}
		while (!car_in[i].empty () && car_in[i].first ().pos >= 0) {
			InNode temp;
			temp.delay_time = (i % 3) ? ((i % 3 == 1) ? 10 : 20) : 30;
			temp.dir = (DIR)(i / 3);
			temp.tr = (TR)(i % 3);
			temp.index = car_in[i].first ().index;
			Node->append (temp);
			car_in[i].removeFirst ();
		}
		while (!car_out[i].empty () && car_out[i].first ().pos >= 150) {
			car_out[i].removeFirst ();
		}
	}
	QList<InNode>::iterator _n_;
	if (!Node->empty ()) {
		for (_n_ = Node->begin (); _n_ != Node->end (); ++_n_) {
			--(_n_->delay_time);
			if (_n_->delay_time == -1) {
				Car temp;
				temp.acc = ND_A (e);
				temp.vec = 10;
				temp.pos = 0;
				temp.index = _n_->index;
				car_out[_n_->dir*TR_NUM + _n_->tr] << temp;
			}
		}
	}
	while (!Node->empty () && Node->first ().delay_time < 0) Node->removeFirst ();
}
void Traffic_v1::generate () {
	for (int i = 0; i < DIR_NUM; i++) {
		if (go[i] <= 0) {
			bool OK[TR_NUM];
			for (int j = 0; j < TR_NUM; j++) OK[j] = car_in[i*TR_NUM + j].empty ()
				|| car_in[i*TR_NUM + j].last ().pos > 185;
			int cont = 0;
			for (int j = 0; j < TR_NUM; j++)if (OK[j]) cont++;
			if (cont) {
				int k = rand () % cont;
				int j;
				for (j = 0; j < TR_NUM; j++) {
					if (OK[j] && !k) break;
					else if (OK[j]) --k;
				}
				Car temp;
				temp.pos = -200.0;//control length 200m;
				temp.vec = ND_V (e);
				temp.index = ++this->index;
				while (temp.vec < 10 || temp.vec>20)temp.vec = ND_V (e);
				temp.acc = ND_A (e);
				while (temp.acc < 0.1)temp.vec = ND_A (e);
				car_in[i*TR_NUM + j] << temp;
			}
			else continue;
		}
		else go[i] -= 0.1;
	}
}
void Traffic_v1::Res () {
	for (int i = 0; i < TR_NUM*DIR_NUM; i++) {
		car_in[i].clear ();
		car_out[i].clear ();
	}
	Node->clear ();
	index = 0;
}