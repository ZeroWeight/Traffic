#pragma once
#include "settings.h"
#define _head car_in[i].first()
#define min(i,j) ((i<j)?(i):(j))
#define max(i,j) ((i>j)?(i):(j))
const static double V_max = 25;
const static double A_max = 5.0;
const static int _S = 8;
class Traffic : public QMainWindow {
	Q_OBJECT
private:
	int size;
	double meter;
	int now_t;
	int index = 0;
	int scale_t;
	int keep_rec = 0;
	int out_num = 0;
	Settings* s;
	QLabel* now;
	QPushButton* start;
	QPushButton* end;
	QTimer* timer;
	QPushButton* edit;
	QList<Car>* car_in;
	QList<Car>* car_out;
	QList<InNode>* Node;
	QRadioButton* fast;
	QRadioButton* medium;
	QRadioButton* slow;
	QRadioButton* very_slow;
	QLabel* speed;
	QLabel* ratio;
	QSlider* ratio_setting;
	QLabel* ratio_shower;
	com_label* st;
	com_label* _st;
	QSlider* A_L;
	QSlider* B_L;
	QSlider* C_L;
	QSlider* D_L;
	QLabel*A_A;
	QLabel*B_A;
	QLabel*C_A;
	QLabel*D_A;
	QLabel*A_B;
	QLabel*B_B;
	QLabel*C_B;
	QLabel*D_B;
	int stop_num[TR_NUM*DIR_NUM];
	int stop_time[TR_NUM*DIR_NUM];
	int car_pass[TR_NUM*DIR_NUM];
	void sim ();
	void generate ();
	void following ();
	void _following ();
	void save_queue ();
	void head (QList<Car>::iterator it, int i);
	void free (QList<Car>::iterator it, int i);
	const double CalMinTime (const double pos, const double vec);
	const double expdf (const double lambda);
	inline const int GetTime () { return int (now_t / 10); }
	inline const int dir (const int i) { return i / 3; }
	inline const int tr (const int i) { return i % 3; }
	inline const Color Get (const int i) { return s->map[GetTime () % s->period][dir (i)][tr (i)]; }
	inline const Color WILL (const int j, int i) { return s->map[(j) % s->period][dir (i)][tr (i)]; }
	inline const int PERIOD () { return s->period; }

public:
	Traffic (QWidget *parent = 0);
	~Traffic ();
	public slots:
	void paintEvent (QPaintEvent *event);
};
