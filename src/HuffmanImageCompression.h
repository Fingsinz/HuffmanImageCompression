#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HuffmanImageCompression.h"

class HuffmanImageCompression : public QMainWindow
{
	Q_OBJECT

public:
	HuffmanImageCompression(QWidget *parent = nullptr);
	~HuffmanImageCompression();

private:
	void setMenu();

public slots:
	void openImg();
	void saveResult();
	void huffmanCompress();

private:
	Ui::HuffmanImageCompressionClass ui;
};
