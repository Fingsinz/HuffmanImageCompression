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
	/**
	 * @brief 设置菜单
	 */
	void setMenu();

	/**
	 * @brief 打开图片
	 * @param fileName 文件名
	 * @return 函数执行情况
	 */
	int openImg(QString const &fileName);

	/**
	 * @brief 打开哈夫曼编码压缩文件
	 * @param fileName 文件名
	 * @return 函数执行情况
	 */
	int openHuf(QString const &fileName);

	/**
	 * @brief 清空哈夫曼树和显示图像
	 */
	void clear();

public Q_SLOTS:
	void openFile();
	void saveImg();
	void saveHuf();
	void huffmanCompress();

private:
	Ui::HuffmanImageCompressionClass ui;

	BMP inputBmp;
	QImage mImg;
	HuffmanCoding *rHuffCoding{ nullptr }, *gHuffCoding{ nullptr }, *bHuffCoding{ nullptr };
	std::vector<std::string> rCode, gCode, bCode;
	std::vector<int> rFreq, gFreq, bFreq;
};
