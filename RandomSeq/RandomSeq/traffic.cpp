#include "traffic.h"
//designed speed limit:60kmh-1=16.67ms-1
//gen speed 40-60kmh-1
static double lambda[DIR_NUM] = { 1.0,1.0,1.0,1.0 };
static double go[DIR_NUM] = { 0 };
static std::default_random_engine e;
static std::normal_distribution<double> ND_V (13.8, 0.9);
static std::normal_distribution<double> ND (0, 0.5);
static std::normal_distribution<double> ND_A (1.5, 0.3);
Traffic::Traffic (QWidget *parent)
	: QMainWindow (parent) {
	system ("mkdir result");
	for (int i = 0; i < DIR_NUM*TR_NUM; ++i) {
		stop_num[i] = 0; stop_time[i] = 0; car_pass[i] = 0;
	}
	for (int i = 0; i < DIR_NUM; i++) {
		go[i] = expdf (lambda[i]);
	}
	size = QApplication::desktop ()->height () / 15;
	meter = size / 10.0;
	this->setFixedHeight (size*12.25);
	this->setFixedWidth (size * 23);
	s = new Settings (size, nullptr);
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
			}
			Node->clear ();
			index = 0;
		}
		timer->stop ();
		start->setEnabled (true); });
	connect (timer, &QTimer::timeout, [=](void) {
		++now_t;
		if (!(now_t % 360000)) {
			if (timer->isActive ())
				end->click ();
		}
		now->setText ("Time:\t" + QString::number (now_t / 10.0) + " s");
		generate ();
		following ();//st.
		_following ();
		sim ();
		this->update ();
		if (double (rand ()) / double (RAND_MAX) < 0.1 && now_t - keep_rec > 1000) {
			//save_queue ();
			keep_rec = now_t;
		}
	});
	edit = new QPushButton (this);
	edit->setText ("Edit the Traffic light");
	edit->setGeometry (15 * size, 5 * size, 3.3 * size, size);
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
	st[0]->setText ("WL");	st[1]->setText ("WR");	st[2]->setText ("WC");
	st[3]->setText ("SL");	st[4]->setText ("SR");	st[5]->setText ("SC");
	st[6]->setText ("EL");	st[7]->setText ("ER");	st[8]->setText ("EC");
	st[9]->setText ("NL");	st[10]->setText ("NR");	st[11]->setText ("NC");
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
	A_A = new QLabel ("W", this);
	A_A->setFont (QFont ("TimesNewRoman", 10));
	A_A->setGeometry (19 * size, 3.5 * size, 0.5*size, 0.5*size);
	B_A = new QLabel ("S", this);
	B_A->setFont (QFont ("TimesNewRoman", 10));
	B_A->setGeometry (19 * size, 4.2 * size, 0.5*size, 0.5*size);
	C_A = new QLabel ("E", this);
	C_A->setFont (QFont ("TimesNewRoman", 10));
	C_A->setGeometry (19 * size, 4.8 * size, 0.5*size, 0.5*size);
	D_A = new QLabel ("N", this);
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
		A_B->setText (QString::number (double (value) / 3000.0));
		lambda[0] = double (value) / 1000.0;
		go[0] = expdf (lambda[0]);
	});
	connect (B_L, &QSlider::valueChanged,
		[=](const int &value) {
		B_B->setText (QString::number (double (value) / 3000.0));
		lambda[1] = double (value) / 1000.0;
		go[1] = expdf (lambda[1]);
	});
	connect (C_L, &QSlider::valueChanged,
		[=](const int &value) {
		C_B->setText (QString::number (double (value) / 3000.0));
		lambda[2] = double (value) / 1000.0;
		go[2] = expdf (lambda[2]);
	});
	connect (D_L, &QSlider::valueChanged,
		[=](const int &value) {
		D_B->setText (QString::number (double (value) / 3000.0));
		lambda[3] = double (value) / 1000.0;
		go[3] = expdf (lambda[3]);
	});
	ratio = new QLabel (this);
	ratio->setText ("Ratio controlled");
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

void Traffic::sim () {
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
					//#ifndef FIN
					//					qDebug () << "E" << (_car_ - 1)->pos << (_car_ - 1)->vec << (_car_ - 1)->acc << (_car_ - 1)->index;
					//					qDebug () << "P" << (_car_)->pos << (_car_)->vec << (_car_)->acc << (_car_)->index;
					//#endif
				}
			}
		}
		if (!car_out[i].empty ()) {
			for (_car_ = car_out[i].begin (); _car_ != car_out[i].end (); ++_car_) {
				_car_->pos += _car_->vec*0.1 + 0.5*_car_->acc*0.01;
				_car_->vec += _car_->acc*0.1;
			}
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
void Traffic::generate () {
	for (int i = 0; i < DIR_NUM; i++) {
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

				car_in[i*TR_NUM + j] << temp;
			}
			else go[i] -= 0.1;
		}
		else go[i] -= 0.1;
	}
}