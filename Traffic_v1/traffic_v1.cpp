#include "traffic_v1.h"

#define GetTime (int(now_t/10))
#define dir(i) ((i)/3)
#define tr(i) ((i)%3)
#define Get(i) (s->map[GetTime%s->period][dir (i)][tr (i)])
#define WILL(j,i) (s->map[(j)%s->period][dir (i)][tr (i)])

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
static double lambda[DIR_NUM] = { 3,3,3,3 };
static double go[DIR_NUM] = { 0 };
static std::default_random_engine e;
static std::normal_distribution<double> ND_V (13.8, 0.9);
static std::normal_distribution<double> ND (0, 1);
static std::normal_distribution<double> ND_A (1.5, 0.3);
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
		scaleEdit->setText (QString::number (scale_t));
		scaleEdit->setEnabled (false);
		end->setText ("Pause");
		end->setEnabled (true);
		_reset->setEnabled (false);
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
			_reset->setEnabled (true);
			scaleEdit->setEnabled (true);
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
		_following ();
		sim ();
		this->update ();
	});
	connect (_reset, &QPushButton::clicked, [=](void) {
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
		for (int i = 0; i < TR_NUM*DIR_NUM; i++) {
			car_in[i].clear ();
			car_out[i].clear ();
		}
		Node->clear ();
		index = 0;
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
	A_L->setMaximum (3000);
	A_L->setValue (3000);
	A_L->setMinimum (1);
	A_L->setGeometry (19.5 * size, 3.5 * size, 1.5*size, 0.5*size);
	B_L = new QSlider (Qt::Horizontal, this);
	B_L->setMaximum (3000);
	B_L->setValue (3000);
	B_L->setMinimum (1);
	B_L->setGeometry (19.5 * size, 4.2 * size, 1.5*size, 0.5*size);
	C_L = new QSlider (Qt::Horizontal, this);
	C_L->setMaximum (3000);
	C_L->setValue (3000);
	C_L->setMinimum (1);
	C_L->setGeometry (19.5 * size, 4.8 * size, 1.5*size, 0.5*size);
	D_L = new QSlider (Qt::Horizontal, this);
	D_L->setMaximum (3000);
	D_L->setValue (3000);
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
	A_B = new QLabel ("1", this);
	A_B->setFont (QFont ("TimesNewRoman", 10));
	A_B->setGeometry (21.5 * size, 3.5 * size, size, 0.5*size);
	B_B = new QLabel ("1", this);
	B_B->setFont (QFont ("TimesNewRoman", 10));
	B_B->setGeometry (21.5 * size, 4.2 * size, size, 0.5*size);
	C_B = new QLabel ("1", this);
	C_B->setFont (QFont ("TimesNewRoman", 10));
	C_B->setGeometry (21.5 * size, 4.8 * size, size, 0.5*size);
	D_B = new QLabel ("1", this);
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
}
Traffic_v1::~Traffic_v1 () {}
void Traffic_v1::sim ()const {
	for (int i = 0; i < TR_NUM*DIR_NUM*(now_t != 0); i++) {
		QList<Car>::iterator _car_;
		if (!car_in[i].empty ()) {
			for (_car_ = car_in[i].begin (); _car_ != car_in[i].end (); ++_car_) {
				_car_->pos += _car_->vec*0.1 + 0.5*_car_->acc*0.01;
				_car_->vec += _car_->acc*0.1;
				if (_car_ != car_in[i].begin () && (_car_->pos - (_car_ - 1)->pos) > -1.0) {
#ifdef FIN
					double temp;
					temp = (_car_ - 1)->vec;
					(_car_ - 1)->vec = _car_->vec;
					_car_->vec = temp;
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
		//space: 5m
		//max load: 1
		//therefore
		if (!car_block[i].empty () && WILL (GetTime - 1, i) == Color::Green&&WILL (GetTime, i) == Color::Green&&WILL (GetTime - 2, i) == Color::Green) {
			for (_car_ = car_block[i].begin (); _car_ != car_block[i].end (); ++_car_)
				_car_->pos += 0.5;
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
		while (!car_block[i].empty () && car_block[i].first ().pos >= 0.5) {
			InNode temp;
			_st[i]->setText (QString::number (_st[i]->text ().toInt () + 1));
			temp.delay_time = (i % 3) ? ((i % 3 == 1) ? 10 : 20) : 30;
			temp.dir = (DIR)(i / 3);
			temp.tr = (TR)(i % 3);
			temp.vec = 10;
			temp.index = car_in[i].first ().index;
			Node->append (temp);
			car_block[i].removeFirst ();
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
				temp.mode = MODE::RUN;
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
			go[i] = expdf (lambda[i]);
			bool OK[TR_NUM];
			for (int j = 0; j < TR_NUM; j++) OK[j] = car_in[i*TR_NUM + j].empty ()
				|| car_in[i*TR_NUM + j].last ().pos > -180.00;
			int cont = 0;
			for (int j = 0; j < TR_NUM; j++)if (OK[j]) cont++;
			if (cont) {
				int k = rand () % cont;
				int j;
				for (j = 0; j < TR_NUM; j++) {
					if (OK[j] && !k) break;
					else if (OK[j]) --k;
				}
				if (j == TR_NUM) {
					//return;
					int max = 0;
					for (int t = 0; t < TR_NUM; ++k)
						if (car_in[i*TR_NUM + t].last ().pos > car_in[i*TR_NUM + max].last ().pos) max = t;
					Car temp;
					temp.pos = -200.0;
					temp.vec = (car_in[i*TR_NUM + max].last ().vec) - abs (ND (e));
					temp.mode = MODE::RUN;
					temp.block = false;
					temp.index = ++this->index;
					temp.acc = ND_A (e);
					while (temp.acc<0.01 || temp.acc>2.5) temp.acc = ND_A (e);
					while (temp.vec < car_in[i*TR_NUM + max].last ().vec - 5 || temp.vec<0 || temp.vec>car_in[i*TR_NUM + max].last ().vec)
						temp.vec = car_in[i*TR_NUM + max].last ().vec - abs (ND (e));
				}
				else {
					Car temp;
					temp.pos = -200.0;//control length 200m;
					if (car_in[i*TR_NUM + j].empty ())
						temp.vec = ND_V (e);
					else
						temp.vec = (car_in[i*TR_NUM + j].end () - 1)->vec + ND (e);
					temp.mode = MODE::RUN;
					temp.block = false;
					temp.index = ++this->index;
					if (!car_in[i*TR_NUM + j].empty ())
						while (temp.vec < (car_in[i*TR_NUM + j].end () - 1)->vec - 3 || temp.vec> (car_in[i*TR_NUM + j].end () - 1)->vec + 3)
							temp.vec = (car_in[i*TR_NUM + j].end () - 1)->vec + ND (e);
					else
						while (temp.vec < 10 || temp.vec>16) temp.vec = ND_V (e);
					temp.acc = ND_A (e);
					while (temp.acc < 0.01 || temp.acc > 2.5)temp.acc = ND_A (e);
					car_in[i*TR_NUM + j] << temp;
				}
			}
			else {
				i++; continue;
			}
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