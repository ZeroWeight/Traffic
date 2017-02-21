#include "traffic_v1.h"
#include <QtWidgets/QApplication>
#include <ctime>
#ifdef _A_A_A_
#error hehehe
#endif
int main (int argc, char *argv[]) {
	srand (time (0));
	QApplication a (argc, argv);
	Traffic_v1 w;
#ifdef BAT
	w.R_0 = QString (argv[1]).toInt ();
	w.R_1 = QString (argv[2]).toInt ();
	w.R_2 = QString (argv[3]).toInt ();
	if (!(w.R_0 + w.R_1 + w.R_2)) {
		w.close (); return 0;
	}
#endif
	w.init_write ();
#ifdef BAT
	w.start->click ();
#endif
#ifndef BAT
	w.show ();
#endif
	return a.exec ();
}
