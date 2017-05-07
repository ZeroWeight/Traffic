#include "traffic_v1.h"
void Traffic_v1::paintEvent (QPaintEvent *event) {
	QPainter painter (this);
	painter.setFont (QFont ("TimesNewRoman", 8));
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

		foreach (Car _car_, car_block[A*TR_NUM + i]) {
			if (_car_.pos >= -50.0) {
				painter.drawRect (near_side[A] * size + _car_.pos*meter - meter, lane_in[A][i] * size + -0.5*meter, 2 * meter, meter);
				painter.drawText (near_side[A] * size + _car_.pos*meter - meter, lane_in[A][i] * size + -0.5*meter, QString::number (_car_.index));
			}
		}
		foreach (Car _car_, car_block[B*TR_NUM + i]) {
			if (_car_.pos >= -50.0) {
				painter.drawRect (lane_in[B][i] * size + -0.5*meter, near_side[B] * size - _car_.pos*meter - meter, meter, 2 * meter);
				painter.drawText (lane_in[B][i] * size + -0.5*meter, near_side[B] * size - _car_.pos*meter - meter, QString::number (_car_.index));
			}
		}
		foreach (Car _car_, car_block[C*TR_NUM + i]) {
			if (_car_.pos >= -50.0) {
				painter.drawRect (near_side[C] * size - _car_.pos*meter - meter, lane_in[C][i] * size + -0.5*meter, 2 * meter, meter);
				painter.drawText (near_side[C] * size - _car_.pos*meter - meter, lane_in[C][i] * size + -0.5*meter, QString::number (_car_.index));
			}
		}
		foreach (Car _car_, car_block[D*TR_NUM + i]) {
			if (_car_.pos >= -50.0) {
				painter.drawRect (lane_in[D][i] * size + -0.5*meter, near_side[D] * size + _car_.pos*meter - meter, meter, 2 * meter);
				painter.drawText (lane_in[D][i] * size + -0.5*meter, near_side[D] * size + _car_.pos*meter - meter, QString::number (_car_.index));
			}
		}
	}
	/*painter.setPen (QPen (QColor ("Blue"), 5));
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
	}*/
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

	foreach (Car _c_, car_out[D*TR_NUM + Left]) {
		if (_c_.pos < 50) {
			painter.drawRect (near_side[C] * size + _c_.pos*meter - meter, lane_in[A][Left] * size + -0.5*meter, 2 * meter, meter);
			painter.drawText (near_side[C] * size + _c_.pos*meter - meter, lane_in[A][Left] * size + -0.5*meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[D*TR_NUM + Right]) {
		if (_c_.pos < 50) {
			painter.drawRect (near_side[A] * size - _c_.pos*meter - meter, lane_in[C][Right] * size + -0.5*meter, 2 * meter, meter);
			painter.drawText (near_side[A] * size - _c_.pos*meter - meter, lane_in[C][Right] * size + -0.5*meter, QString::number (_c_.index));
		}
	}
	foreach (Car _c_, car_out[D*TR_NUM + Center]) {
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
			painter.drawEllipse (near_side[A] * size + 1.2 * r, lane_in[A][i] * size, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (near_side[A] * size + 1.2 * r, lane_in[A][i] * size, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (near_side[A] * size + 1.2 * r, lane_in[A][i] * size, r, r);
			break;
		}
		switch (s->map[(now_t / 10) % s->period][B][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (lane_in[B][i] * size, near_side[B] * size - 1.2 * r, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (lane_in[B][i] * size, near_side[B] * size - 1.2 * r, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (lane_in[B][i] * size, near_side[B] * size - 1.2 * r, r, r);
			break;
		}
		switch (s->map[(now_t / 10) % s->period][C][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (near_side[C] * size - 1.2 * r, lane_in[C][i] * size, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (near_side[C] * size - 1.2 * r, lane_in[C][i] * size, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (near_side[C] * size - 1.2 * r, lane_in[C][i] * size, r, r);
			break;
		}
		switch (s->map[(now_t / 10) % s->period][D][i]) {
		case Red:
			painter.setBrush ((QColor ("Red")));
			painter.setPen ((QColor ("Red")));
			painter.drawEllipse (lane_in[D][i] * size, near_side[D] * size + 1.2 * r, r, r);
			break;
		case Green:
			painter.setBrush ((QColor ("Green")));
			painter.setPen ((QColor ("Green")));
			painter.drawEllipse (lane_in[D][i] * size, near_side[D] * size + 1.2 * r, r, r);
			break;
		case Yellow:
			painter.setBrush ((QColor ("Yellow")));
			painter.setPen ((QColor ("Yellow")));
			painter.drawEllipse (lane_in[D][i] * size, near_side[D] * size + 1.2 * r, r, r);
			break;
		}
	}
}