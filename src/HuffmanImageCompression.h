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
	 */
	void openImg(QString const &fileName);

	/**
	 * @brief 打开哈夫曼编码压缩文件
	 * @param fileName 文件名
	 */
	void openHuf(QString const &fileName);

	/**
	 * @brief 保存图片
	 * @param fileName 文件名
	 */
	void saveImg(QString const &fileName);

	/**
	 * @brief 保存哈夫曼编码压缩文件
	 * @param fileName 文件名
	 */
	void saveHuf(QString const &fileName);

public Q_SLOTS:
	void openFile();
	void saveFile();
	void huffmanCompress();

private:
	Ui::HuffmanImageCompressionClass ui;

	BMP inputBmp;
	HuffmanCoding *rHuffCoding, *gHuffCoding, *bHuffCoding;
	std::vector<std::string> rCode, gCode, bCode;
	std::vector<int> rFreq, gFreq, bFreq;
};
