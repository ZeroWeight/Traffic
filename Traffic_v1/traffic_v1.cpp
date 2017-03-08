#include "traffic_v1.h"

#define GetTime (int(now_t/10))
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define Get(i) (s->map[GetTime%s->period][dir (i)][tr (i)])
#define WILL(j,i) (s->map[(j)%s->period][dir (i)][tr (i)])
#define COM2 //for debug
static const int _S = 8;
double expdf (double lambda) {
	double pV = 1;
	while (pV == 1)
		pV = double (rand ()) / double (RAND_MAX);
	pV = (-1.0 / lambda)*log (1 - pV);
	return pV;
}
//designed speed limit:60kmh-1=16.67ms-1
//gen speed 40-60kmh-1
static double lambda[DIR_NUM] = { 1.0,1.0,1.0,1.0 };
static double go[DIR_NUM] = { 0 };
static std::default_random_engine e;
static std::normal_distribution<double> ND_V (13.8, 0.9);
static std::normal_distribution<double> ND (0, 0.5);
static std::normal_distribution<double> ND_A (1.5, 0.3);
Traffic_v1::Traffic_v1 (QWidget *parent)
	: QMainWindow (parent) {
	init_write ();
	for (int i = 0; i < DIR_NUM*TR_NUM; ++i) {
		stop_num[i] = 0; stop_time[i] = 0; car_pass[i] = 0;
	}
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
	scale_t = 1;
	now = new QLabel (this);
	now->setText ("Time:\t0 s");
	now->setFont (QFont ("TimesNewRoman", 16));
	now->setGeometry (15 * size, 2 * size, 5 * size, size);
	now_t = 0;
	start = new QPushButton (this);
	start->setText ("Start");
	start->setFont (QFont ("TimesNewRoman", 12));
	start->setGeometry (15 * size, 3.5*size, 1.5 * size, size);
	end = new QPushButton (this);
	end->setText ("Pause");
	end->setFont (QFont ("TimesNewRoman", 12));
	end->setGeometry (16.8 * size, 3.5*size, 1.5 * size, size);
	timer = new QTimer (this);
	timer->setInterval (10);
	timer->stop ();
	end->setEnabled (false);
	connect (start, &QPushButton::clicked, [=](void) {
		end->setText ("Pause");
		end->setEnabled (true);
		start->setEnabled (false);
		timer->start ();
	});
	connect (end, &QPushButton::clicked, [=](void) {
		if (timer->isActive ()) {
			start->setText ("Resume");
			end->setText ("End");
		}
		else {
			for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
				_st[i]->setText (QString::number (0));
			}
			start->setText ("Start");
			end->setEnabled (false);
			this->now_t = 0;
			now->setText ("Time:\t0 s");
			for (int i = 0; i < TR_NUM*DIR_NUM; i++) {
				car_in[i].clear ();
				car_out[i].clear ();
				car_block[i].clear ();
			}
			Node->clear ();
			index = 0;
		}
		timer->stop ();
		start->setEnabled (true); });
	connect (timer, &QTimer::timeout, [=](void) {
		++now_t;
		if (!(now_t % 36000)) {
			if (timer->isActive ())
				end->click ();
		}
		now->setText ("Time:\t" + QString::number (now_t / 10.0) + " s");
		generate ();
#ifdef MANUAL
		following ();
#endif
#ifdef ST1
		st1 ();
#endif
#ifdef ST2
		st2 ();
#endif
#ifdef COMBO
		combo ();
#endif
		_following ();
		sim ();
		this->update ();
		main_write ();
	});
	edit = new QPushButton (this);
	edit->setText ("Edit the Traffic light");
	edit->setGeometry (15 * size, 5 * size, 3.3 * size, size);
	edit->setFont (QFont ("TimesNewRoman", 10));
	connect (edit, SIGNAL (clicked ()), this, SLOT (hide ()));
	connect (edit, SIGNAL (clicked ()), s, SLOT (show ()));
	connect (s->save, SIGNAL (clicked ()), this, SLOT (show ()));
	car_in = new QList<Car>[DIR_NUM*TR_NUM];
	car_block = new QList<Car>[DIR_NUM*TR_NUM];
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
	connect (fast, &QRadioButton::toggled, [=](const bool b) {if (b) timer->setInterval (1); });
	connect (medium, &QRadioButton::toggled, [=](const bool b) {if (b) timer->setInterval (10); });
	connect (slow, &QRadioButton::toggled, [=](const bool b) {if (b) timer->setInterval (100); });
	connect (very_slow, &QRadioButton::toggled, [=](const bool b) {if (b) timer->setInterval (1000); });
	st = new com_label[TR_NUM*DIR_NUM];
	_st = new com_label[TR_NUM*DIR_NUM];
	for (int i = 0; i < TR_NUM*DIR_NUM; i++) {
		st[i] = new QLabel (this);
		_st[i] = new QLabel ("0", this);
		st[i]->setFont (QFont ("TimesNewRoman", 10));
		_st[i]->setFont (QFont ("TimesNewRoman", 10));
		_st[i]->setGeometry (16 * size + (i / 3) * 2 * size, 8 * size + (i % 3)*size, size, size);
		st[i]->setGeometry (15 * size + (i / 3) * 2 * size, 8 * size + (i % 3)*size, size, size);
	}
	st[0]->setText ("AL");	st[1]->setText ("AR");	st[2]->setText ("AC");
	st[3]->setText ("BL");	st[4]->setText ("BR");	st[5]->setText ("BC");
	st[6]->setText ("CL");	st[7]->setText ("CR");	st[8]->setText ("CC");
	st[9]->setText ("DL");	st[10]->setText ("DR");	st[11]->setText ("DC");
	A_L = new QSlider (Qt::Horizontal, this);
	A_L->setMaximum (1500);
	A_L->setValue (1000);
	A_L->setMinimum (1);
	A_L->setGeometry (19.5 * size, 3.5 * size, 1.5*size, 0.5*size);
	B_L = new QSlider (Qt::Horizontal, this);
	B_L->setMaximum (1500);
	B_L->setValue (1000);
	B_L->setMinimum (1);
	B_L->setGeometry (19.5 * size, 4.2 * size, 1.5*size, 0.5*size);
	C_L = new QSlider (Qt::Horizontal, this);
	C_L->setMaximum (1500);
	C_L->setValue (1000);
	C_L->setMinimum (1);
	C_L->setGeometry (19.5 * size, 4.8 * size, 1.5*size, 0.5*size);
	D_L = new QSlider (Qt::Horizontal, this);
	D_L->setMaximum (1500);
	D_L->setValue (1000);
	D_L->setMinimum (1);
	D_L->setGeometry (19.5 * size, 5.5 * size, 1.5*size, 0.5*size);
	A_A = new QLabel ("A", this);
	A_A->setFont (QFont ("TimesNewRoman", 10));
	A_A->setGeometry (19 * size, 3.5 * size, 0.5*size, 0.5*size);
	B_A = new QLabel ("B", this);
	B_A->setFont (QFont ("TimesNewRoman", 10));
	B_A->setGeometry (19 * size, 4.2 * size, 0.5*size, 0.5*size);
	C_A = new QLabel ("C", this);
	C_A->setFont (QFont ("TimesNewRoman", 10));
	C_A->setGeometry (19 * size, 4.8 * size, 0.5*size, 0.5*size);
	D_A = new QLabel ("D", this);
	D_A->setFont (QFont ("TimesNewRoman", 10));
	D_A->setGeometry (19 * size, 5.5 * size, 0.5*size, 0.5*size);
	A_B = new QLabel (QString::number (1000.0 / 3000.0), this);
	A_B->setFont (QFont ("TimesNewRoman", 10));
	A_B->setGeometry (21.5 * size, 3.5 * size, size, 0.5*size);
	B_B = new QLabel (QString::number (1000.0 / 3000.0), this);
	B_B->setFont (QFont ("TimesNewRoman", 10));
	B_B->setGeometry (21.5 * size, 4.2 * size, size, 0.5*size);
	C_B = new QLabel (QString::number (1000.0 / 3000.0), this);
	C_B->setFont (QFont ("TimesNewRoman", 10));
	C_B->setGeometry (21.5 * size, 4.8 * size, size, 0.5*size);
	D_B = new QLabel (QString::number (1000.0 / 3000.0), this);
	D_B->setFont (QFont ("TimesNewRoman", 10));
	D_B->setGeometry (21.5 * size, 5.5 * size, size, 0.5*size);
	connect (A_L, &QSlider::valueChanged,
		[=](const int &value) {
		A_B->setText (QString::number (double (value) / 3000));
		lambda[0] = double (value) / 1000;
		go[0] = expdf (lambda[0]);
	});
	connect (B_L, &QSlider::valueChanged,
		[=](const int &value) {
		B_B->setText (QString::number (double (value) / 3000));
		lambda[1] = double (value) / 1000;
		go[1] = expdf (lambda[1]);
	});
	connect (C_L, &QSlider::valueChanged,
		[=](const int &value) {
		C_B->setText (QString::number (double (value) / 3000));
		lambda[2] = double (value) / 1000;
		go[2] = expdf (lambda[2]);
	});
	connect (D_L, &QSlider::valueChanged,
		[=](const int &value) {
		D_B->setText (QString::number (double (value) / 3000));
		lambda[3] = double (value) / 1000;
		go[3] = expdf (lambda[3]);
	});
	ratio = new QLabel (this);
	ratio->setText ("Ratio controled");
	ratio->setFont (QFont ("TimesNewRoman", 16));
	ratio->setGeometry (15 * size, size, size * 3, size);
	ratio_setting = new QSlider (Qt::Horizontal, this);
	ratio_setting->setMaximum (1000);
	ratio_setting->setMinimum (0);
	ratio_setting->setGeometry (19 * size, 1.25*size, size * 3, 0.5*size);
	ratio_shower = new QLabel (this);
	ratio_shower->setFont (QFont ("TimesNewRoman", 16));
	ratio_shower->setGeometry (19 * size, 2 * size, 3 * size, size);
	ratio_shower->setText ("Ratio: 0.000");
	connect (ratio_setting, &QSlider::valueChanged, [=](const int &value) {
		ratio_shower->setText (QString ("Ratio: ") + QString::number (double (ratio_setting->value ()) / 1000));
	});
}
Traffic_v1::~Traffic_v1 () {
	_car->flush ();
	_stop->flush ();
	_road->flush ();
	_stopT->flush ();
	_car->close ();
	_stop->close ();
	_road->close ();
	_stopT->close ();
}
void Traffic_v1::sim () {
	for (int i = 0; i < TR_NUM*DIR_NUM*(now_t != 0); i++) {
		QList<Car>::iterator _car_;
		if (!car_in[i].empty ()) {
			for (_car_ = car_in[i].begin (); _car_ != car_in[i].end (); ++_car_) {
				_car_->pos += _car_->vec*0.1 + 0.5*_car_->acc*0.01;
				_car_->vec += _car_->acc*0.1;
				if (_car_ != car_in[i].begin () && (_car_->pos - (_car_ - 1)->pos) > -1.0) {
#ifdef FIN
					(_car_)->vec = (_car_ - 1)->vec;
#endif
#ifndef FIN
					qDebug () << "E" << (_car_ - 1)->pos << (_car_ - 1)->vec << (_car_ - 1)->acc << (_car_ - 1)->index;
					qDebug () << "P" << (_car_)->pos << (_car_)->vec << (_car_)->acc << (_car_)->index;
#endif
				}
			}
		}
		if (!car_out[i].empty ()) {
			for (_car_ = car_out[i].begin (); _car_ != car_out[i].end (); ++_car_) {
				_car_->pos += _car_->vec*0.1 + 0.5*_car_->acc*0.01;
				_car_->vec += _car_->acc*0.1;
			}
		}
		//space: 4m
		//max load: 1
		//therefore
		if (!car_block[i].empty ()
			&& WILL (GetTime, i) == Color::Green
			&& WILL (GetTime - 1, i) == Color::Green
			&&WILL (GetTime - 2, i) == Color::Green
			&& WILL (GetTime - 3, i) == Color::Green
			&& WILL (GetTime - 4, i) == Color::Green) {
			for (_car_ = car_block[i].begin (); _car_ != car_block[i].end (); ++_car_)
				if (car_pass[i] < 10)
					_car_->pos += 0.18 + 0.004*car_pass[i];
				else _car_->pos += 0.22;
		}
		else if (!car_block[i].empty () && car_block[i].first ().pos < 0) {
			for (_car_ = car_block[i].begin (); _car_ != car_block[i].end (); ++_car_)
				_car_->pos += 0.1;
		}
		if (Get (i) != Color::Green&&WILL (GetTime - 1, i) == Color::Green&&now_t == 10 * GetTime) stop_num[i] += car_block[i].count ();
		if (Get (i) != Color::Green) {
			car_pass[i] = 0;
			stop_time[i] += car_block[i].count ();
		}
		while (!car_in[i].empty () && car_in[i].first ().pos >= 0.5) {
			InNode temp;
			_st[i]->setText (QString::number (_st[i]->text ().toInt () + 1));
			temp.delay_time = (i % 3) ? ((i % 3 == 1) ? 10 : 20) : 30;
			temp.dir = (DIR)(i / 3);
			temp.tr = (TR)(i % 3);
			temp.vec = car_in[i].first ().vec;
			temp.index = car_in[i].first ().index;
			Node->append (temp);
			c_write (car_in[i].first ());
			car_in[i].removeFirst ();
		}
		while (!car_block[i].empty () && car_block[i].first ().pos >= 0.5) {
			InNode temp;
			_st[i]->setText (QString::number (_st[i]->text ().toInt () + 1));
			temp.delay_time = (i % 3) ? ((i % 3 == 1) ? 10 : 20) : 30;
			temp.dir = (DIR)(i / 3);
			temp.tr = (TR)(i % 3);
			temp.vec = 10;
			temp.index = car_block[i].first ().index;
			Node->append (temp);
			c_write (car_block[i].first ());
			car_block[i].removeFirst ();
			++car_pass[i];
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
				while (temp.acc < 0.01 || temp.acc > 2.5)temp.acc = ND_A (e);
				temp.vec = _n_->vec;
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
#ifndef MAX_LOAD
		if (go[i] <= 0) {
			bool OK[TR_NUM];
			for (int j = 0; j < TR_NUM; j++) OK[j] = car_in[i*TR_NUM + j].empty ()
				|| car_in[i*TR_NUM + j].last ().pos > -180.00;
			int cont = 0;
			for (int j = 0; j < TR_NUM; j++)if (OK[j]) cont++;
			if (cont) {
				go[i] = expdf (lambda[i]);
				int k = rand () % cont;
				int j;
				for (j = 0; j < TR_NUM; j++) {
					if (OK[j] && !k) break;
					else if (OK[j]) --k;
				}
				Car temp;
				temp.enter_time_d = now_t;
				temp.pos = -200.0;//control length 200m;
				if (car_in[i*TR_NUM + j].empty ())
					temp.vec = ND_V (e);
				else
					temp.vec = car_in[i*TR_NUM + j].last ().vec + ND (e);
				temp.index = ++this->index;
				if (!car_in[i*TR_NUM + j].empty ())
					if (temp.vec < (car_in[i*TR_NUM + j].end () - 1)->vec - 3
						|| temp.vec> (car_in[i*TR_NUM + j].end () - 1)->vec + 3
						|| temp.vec > 25 || temp.vec < 5)
						goto RES;
					else
						while (temp.vec < 10 || temp.vec>16) {
						RES:
							temp.vec = ND_V (e);
						}
				temp.acc = ND_A (e);
				temp.time_arr = 0;
				temp.vec_init = temp.vec;
				while (temp.acc < 0.01 || temp.acc > 2.5)temp.acc = ND_A (e);
#ifdef COMBO
				double var = double (rand ()) / double (RAND_MAX);
				double value = double (ratio_setting->value ()) / 1000.0;
				if (var < value)

#ifdef COM1
					temp.type = C_1;
#endif
#ifdef COM2
				temp.type = C_2;
#endif
				else temp.type = C_0;
#endif
				car_in[i*TR_NUM + j] << temp;
			}
			else go[i] -= 0.1;
		}
		else go[i] -= 0.1;
#endif
#ifdef MAX_LOAD
		for (int j = 0; j < TR_NUM; ++j) {
			if (car_in[i*TR_NUM + j].empty () || car_in[i*TR_NUM + j].last ().pos > -185) {
				Car temp;
				temp.acc = ND_A (e);
				temp.block = false;
				temp.mode = MODE::RUN;
				temp.index = ++this->index;
				temp.pos = -200.0;
				if (car_in[i*TR_NUM + j].empty ()) temp.vec = ND_V (e);
				else temp.vec = car_in[i*TR_NUM + j].last ().vec;
				car_in[i*TR_NUM + j] << temp;
			}
		}
#endif
	}
}
void Traffic_v1::_following () {
	for (int i = 0; i < TR_NUM*DIR_NUM; ++i) {
		if (!car_out[i].empty ()) {
			free (car_out[i].begin (), i);
			QList<Car>::iterator it;
			QList<Car>::iterator itp;
			for (it = car_out[i].end () - 2, itp = car_out[i].end () - 1;
				itp != car_out[i].begin (); --it, --itp) {
				if (it->pos - itp->pos < (_S << 2)) {
					if (it->pos - itp->pos < _S)
						itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (_S - it->pos + itp->pos);
					else if (it->vec < itp->vec)
						itp->acc = it->acc + (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (_S - it->pos + itp->pos);
					else if (it->vec > itp->vec)
						itp->acc = it->acc - (itp->vec - it->vec)*(itp->vec - it->vec)
						/ 2.0 / (_S - it->pos + itp->pos);
					if (itp->acc < -5) itp->acc = -5;
					if (itp->acc > 5) itp->acc = 5;
					if (itp->vec < 5)itp->acc = 0;
				}
				free (itp, i);
			}
		}
	}
}