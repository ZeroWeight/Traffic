#include "traffic.h"

void Traffic::save_queue () {
	system (QString ("mkdir result\\" + QString::number (out_num)).toStdString ().data ());
	for (int i = 0; i < DIR_NUM; ++i) {
		QFile f ("result\\" + QString::number (out_num) + "\\" + QString::number (i) + ".csv");
		f.open (QIODevice::WriteOnly | QIODevice::Text);
		QTextStream Output (&f);
		int randindex = rand () % TR_NUM;
		for (Car c : car_in[i * TR_NUM + randindex]) {
			Output << QString::number (c.vec, 'f', 3) << ',' << QString::number (-c.pos, 'f', 3) << endl;
		}
		Output.flush ();
		f.close ();
	}
	out_num++;
}