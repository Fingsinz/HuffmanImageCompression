#pragma once

#include "BMP.h"
#include "HuffmanCoding.h"
#include "ui_HuffmanImageCompression.h"
#include <QtWidgets/QMainWindow>

class HuffmanImageCompression : public QMainWindow
{
	Q_OBJECT

public:
	HuffmanImageCompression(QWidget *parent = nullptr);
	~HuffmanImageCompression();

private:
	void setMenu();

public Q_SLOTS:
	void openImg();
	void saveResult();
	void huffmanCompress();

private:
	Ui::HuffmanImageCompressionClass ui;

	BMP inputBmp;
	HuffmanCoding *rHuffCoding, *gHuffCoding, *bHuffCoding;
	std::vector<std::string> rCode, gCode, bCode;
	std::vector<int> rFreq, gFreq, bFreq;
};
