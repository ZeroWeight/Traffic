#include "settings.hpp"

Settings::Settings (int _size, QWidget * parent) : QWidget (parent), size (_size) {
	main_line = new QPen (QBrush (QColor ("Black")), 5);
	dot_line = new QPen (QBrush (QColor ("Yellow")), 3);
	edge = new QPen (QColor ("Black"));
	QVector<qreal> dashes;
	dashes << 5 << 3 << 5 << 3;
	dot_line->setDashPattern (dashes);
	car = new QBrush (QColor ("Blue"));
	period = 300;
	map = new Light[period];
	for (int i = 0; i < period; i++) for (int j = 0; j < DIR_NUM; j++) for (int k = 0; k < TR_NUM; k++)
		map[i][j][k] = Red;
	slider = new QSlider (Qt::Horizontal, this);
	slider->setGeometry (15.5 * size, 0.7*size, 6 * size, 0.5*size);
	slider->setMinimum (0);
	slider->setMaximum (period - 1);
	connect (slider, SIGNAL (valueChanged (int)), this, SLOT (update ()));
}

Settings::~Settings () {}

void Settings::paintEvent (QPaintEvent *) {
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
						6.0 * size / period, 0.2*size);
					break;
				case Green:
					painter.setPen ((QColor ("Green")));
					painter.setBrush ((QColor ("Green")));
					painter.drawRect (15.5 * size + i * 6.0 * size / period, 1.5*size + (TR_NUM*j + k)*0.5*size,
						6.0 * size / period, 0.2*size);
					break;
				case Yellow:
					painter.setPen ((QColor ("Yellow")));
					painter.setBrush ((QColor ("Yellow")));
					painter.drawRect (15.5 * size + i * 6.0 * size / period, 1.5*size + (TR_NUM*j + k)*0.5*size,
						6.0 * size / period, 0.2*size);
					break;
				}
	painter.setPen (QPen (QColor ("black"), 3));
	painter.drawLine (15.5 * size + 6.0*(slider->value () + 0.5)*size / period, 1.5*size,
		15.5 * size + 6.0*(slider->value () + 0.5)*size / period, 7.2 * size);
	for (int i = 0; i < TR_NUM; i++) {
		switch (map[slider->value ()][A][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (near_side[C] * size - 1.2 * r, lane_in[A][i][1] * size, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (near_side[C] * size - 1.2 * r, lane_in[A][i][1] * size, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (near_side[C] * size - 1.2 * r, lane_in[A][i][1] * size, r, r);
			break;
		}
		switch (map[slider->value ()][B][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (lane_in[B][i][0] * size, near_side[D] * size + 1.2 * r, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (lane_in[B][i][0] * size, near_side[D] * size + 1.2 * r, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (lane_in[B][i][0] * size, near_side[D] * size + 1.2 * r, r, r);
			break;
		}
		switch (map[slider->value ()][C][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (near_side[A] * size + 1.2 * r, lane_in[C][i][1] * size, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (near_side[A] * size + 1.2 * r, lane_in[C][i][1] * size, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (near_side[A] * size + 1.2 * r, lane_in[C][i][1] * size, r, r);
			break;
		}
		switch (map[slider->value ()][D][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (lane_in[D][i][0] * size, near_side[B] * size - 1.2 * r, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (lane_in[D][i][0] * size, near_side[B] * size - 1.2 * r, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (lane_in[D][i][0] * size, near_side[B] * size - 1.2 * r, r, r);
			break;
		}
	}
}