#pragma once
#include <cmath>
#include <iterator>
#include <random>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QDesktopWidget>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QTime>
#include <QTimer>
#include <QWidget>

enum DIR {
	A, B, C, D, DIR_NUM
};
enum TR {
	Left, Right, Center, TR_NUM
};
enum Color {
	Green, Yellow, Red
};
//don't change the code below
const double near_side[DIR_NUM] = { 5.0,7.25,7.25,5.0 };
const double far_side[DIR_NUM] = { 0,12.25,12.25,0 };
const double lane_in[DIR_NUM][TR_NUM] = {
	{ 6.3125,7.0625,6.6875 },
	{ 6.3125,7.0625,6.687 },
	{ 5.9375,5.1875,5.5625 },
	{ 5.9375,5.1875,5.5625 }
};
const double main_lane[3] = { 5,6.125,7.25 };
const double aux_lane[4] = { 5.375,5.75,6.5,6.875 };
enum Type {
	C_0, C_1, C_2
};
typedef Color Light[DIR_NUM][TR_NUM];
typedef QLabel* com_label;
typedef QCheckBox* com_check;
struct Car {
	int time_arr;
	double acc;
	double acc_aim;
	double vec;
	double pos;
	int index;
	double vec_init;
	int enter_time_d;
	Type type;
};
struct InNode {
	DIR dir;
	TR tr;
	double vec;
	int delay_time;
	int index;
};