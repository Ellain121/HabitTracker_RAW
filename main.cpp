#include "MainWindow.hpp"

#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QTextStream>

int main(int argc, char* argv[])
{
	QApplication app{argc, argv};
	//	QFile		 file{":/light.qss"};
	//	file.open(QFile::ReadOnly | QFile::Text);
	//	QTextStream stream{&file};
	//	app.setStyleSheet(stream.readAll());
	app.setStyle(QStyleFactory::create("Fusion"));

	MainWindow window;
	window.show();
	return app.exec();
}
