#include "traffic_v1.h"
#include <QtWidgets/QApplication>
#include <ctime>
int main (int argc, char *argv[]) {
	srand (time (0));
	QApplication a (argc, argv);
	Traffic_v1 w;
	w.show ();
	return a.exec ();
}