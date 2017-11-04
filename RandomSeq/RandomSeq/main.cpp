#include "traffic.h"

int main (int argc, char *argv[])
{
	QApplication a (argc, argv);
	Traffic w;
	w.show ();
	return a.exec ();
}