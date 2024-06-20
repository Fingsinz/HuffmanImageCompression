#include "HuffmanImageCompression.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HuffmanImageCompression w;
	w.show();
	return a.exec();
}