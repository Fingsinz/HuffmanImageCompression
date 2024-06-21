#include "HuffmanImageCompression.h"
#include "qimage.h"
#include <qthread.h>
#include <qfiledialog.h>
#include <qgraphicsview.h>
#include <qmessagebox.h>
#include <qthread.h>
#include <vector>
#include <bitset>

HuffmanImageCompression::HuffmanImageCompression(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setMenu();
}

HuffmanImageCompression::~HuffmanImageCompression()
{}

void HuffmanImageCompression::setMenu()
{
	QFont font(tr("微软雅黑 Light"), 14);
	this->setFont(font);

	QMenu *fileMenu = ui.menuBar->addMenu(tr("文件"));
	fileMenu->setFont(font);
	fileMenu->addAction(tr("打开"), this, SLOT(openFile()))->setShortcut(QKeySequence::Open);
	fileMenu->addAction(tr("保存"), this, SLOT(saveFile()))->setShortcut(QKeySequence::Save);

	QMenu *operateMenu = ui.menuBar->addMenu(tr("操作"));
	operateMenu->setFont(font);
	operateMenu->addAction(tr("哈夫曼压缩"), this, SLOT(huffmanCompress()));
}

void HuffmanImageCompression::openImg(QString const &fileName)
{
	int flag = inputBmp.load(fileName.toStdString());
	switch (flag)
	{
		case -1:
			ui.statusBar->showMessage(tr("打开文件失败"));
			return;
		case -2:
			ui.statusBar->showMessage(tr("文件格式错误"));
			return;
		case 1:
			ui.statusBar->showMessage(tr("打开文件成功"));
			break;
	}

	QImage img(inputBmp.info.height, inputBmp.info.width, QImage::Format_RGB888);

	for (int i = 0; i < inputBmp.info.height; i++)
	{
		for (int j = 0; j < inputBmp.info.width; j++)
		{
			img.setPixelColor(j, inputBmp.info.width - i, QColor(
				inputBmp.imgData[i * inputBmp.info.width + j][2],
				inputBmp.imgData[i * inputBmp.info.width + j][1],
				inputBmp.imgData[i * inputBmp.info.width + j][0]));
		}
	}

	QGraphicsScene *scene = new QGraphicsScene(ui.imgView);
	scene->addPixmap(QPixmap::fromImage(img));
	ui.imgView->setScene(scene);
}

void HuffmanImageCompression::openHuf(QString const &fileName)
{
	std::ifstream in(fileName.toStdString(), std::ios::in | std::ios::binary);

	if (!in.is_open())
	{
		return;
	}

	std::vector<int>(256, 0).swap(rFreq);
	std::vector<int>(256, 0).swap(gFreq);
	std::vector<int>(256, 0).swap(bFreq);

	int height = 0, width = 0;
	in.read(reinterpret_cast<char *>(&height), 4);
	in.read(reinterpret_cast<char *>(&width), 4);

	int freqByte = 0;
	unsigned char ch = 0;
	int freq = 0;

	in.read(reinterpret_cast<char *>(&freqByte), 4);
	for (int i = 0; i < freqByte; i += 5)
	{
		in.read(reinterpret_cast<char *>(&ch), 1);
		in.read(reinterpret_cast<char *>(&freq), 4);
		rFreq[ch] = freq;
	}

	in.read(reinterpret_cast<char *>(&freqByte), 4);
	for (int i = 0; i < freqByte; i += 5)
	{
		in.read(reinterpret_cast<char *>(&ch), 1);
		in.read(reinterpret_cast<char *>(&freq), 4);
		gFreq[ch] = freq;
	}

	in.read(reinterpret_cast<char *>(&freqByte), 4);
	for (int i = 0; i < freqByte; i += 5)
	{
		in.read(reinterpret_cast<char *>(&ch), 1);
		in.read(reinterpret_cast<char *>(&freq), 4);
		bFreq[ch] = freq;
	}

	delete rHuffCoding, delete gHuffCoding, delete bHuffCoding;

	rHuffCoding = new HuffmanCoding();
	gHuffCoding = new HuffmanCoding();
	bHuffCoding = new HuffmanCoding();

	rHuffCoding->coding(rFreq), gHuffCoding->coding(gFreq), bHuffCoding->coding(bFreq);

	// 读入红色像素
	int rBytes = 0, rLens = 0;
	in.read(reinterpret_cast<char *>(&rLens), 4);
	in.read(reinterpret_cast<char *>(&rBytes), 4);
	char *rBuf = new char[rBytes];
	in.read(rBuf, rBytes);
	std::string rCode = "";
	for (int i = 0; i < rBytes; ++ i)
	{
		char data = rBuf[i];
		std::bitset<8> dataBit(data);
		rCode += dataBit.to_string();
	}
	delete[] rBuf;
	rCode = rCode.substr(0, rLens);

	// 读入绿色像素
	int gBytes = 0, gLens = 0;
	in.read(reinterpret_cast<char *>(&gLens), 4);
	in.read(reinterpret_cast<char *>(&gBytes), 4);
	char *gBuf = new char[gBytes];
	in.read(gBuf, gBytes);
	std::string gCode = "";
	for (int i = 0; i < gBytes; ++ i)
	{
		char data = gBuf[i];
		std::bitset<8> dataBit(data);
		gCode += dataBit.to_string();
	}
	delete[] gBuf;
	gCode = gCode.substr(0, gLens);

	// 读入蓝色像素
	int bBytes = 0, bLens = 0;
	in.read(reinterpret_cast<char *>(&bLens), 4);
	in.read(reinterpret_cast<char *>(&bBytes), 4);
	char *bBuf = new char[bBytes];
	in.read(bBuf, bBytes);
	std::string bCode = "";
	for (int i = 0; i < bBytes; ++ i)
	{
		char data = bBuf[i];
		std::bitset<8> dataBit(data);
		bCode += dataBit.to_string();
	}
	delete[] bBuf;
	bCode = bCode.substr(0, bLens);

	std::vector<unsigned char> rColor = rHuffCoding->decode(rCode);
	std::vector<unsigned char> gColor = gHuffCoding->decode(gCode);
	std::vector<unsigned char> bColor = bHuffCoding->decode(bCode);

	QImage img(height, width, QImage::Format_RGB888);

	for (int i = 0; i < height; ++ i)
	{
		for (int j = 0; j < width; ++ j)
		{
			img.setPixelColor(j, i, QColor(rColor[i * width + j], gColor[i * width + j], bColor[i * width + j]));
		}
	}

	QGraphicsScene *scene = new QGraphicsScene(ui.imgView);
	scene->addPixmap(QPixmap::fromImage(img));
	ui.imgView->setScene(scene);

	std::ofstream outHuf("check2.txt", std::ios::out | std::ios::binary);
	for (int i = 0; i < 256; ++ i)
	{
		outHuf << rHuffCoding->getHuffmanCode(i) << "\n";
	}
	outHuf.close();
}

void HuffmanImageCompression::saveImg(QString const &fileName)
{}

void HuffmanImageCompression::saveHuf(QString const &fileName)
{
	std::ofstream out(fileName.toStdString(), std::ios::out | std::ios::binary);

	if (!out.is_open())
	{
		return;
	}

	int height = inputBmp.info.height, width = inputBmp.info.width;

	out.write(reinterpret_cast<char *>(&height), 4);
	out.write(reinterpret_cast<char *>(&width), 4);

	int freqByte = 0;

	// 存储红色像素及其频率
	freqByte = 0;
	for (int x : rFreq)
	{
		if (x != 0)
			freqByte += 1;
	}
	freqByte *= 5;
	out.write(reinterpret_cast<char *>(&freqByte), 4);
	for (int i = 0; i < 256; ++ i)
	{
		if (rFreq[i] != 0)
		{
			unsigned char r = i;
			out.write(reinterpret_cast<char *>(&r), 1);
			out.write(reinterpret_cast<char *>(&rFreq[i]), 4);
		}
	}

	// 存储绿色像素及其频率
	freqByte = 0;
	for (int x : gFreq)
	{
		if (x != 0)
			freqByte += 1;
	}
	freqByte *= 5;
	out.write(reinterpret_cast<char *>(&freqByte), 4);
	for (int i = 0; i < 256; ++ i)
	{
		if (gFreq[i] != 0)
		{
			unsigned char g = i;
			out.write(reinterpret_cast<char *>(&g), 1);
			out.write(reinterpret_cast<char *>(&gFreq[i]), 4);
		}
	}

	// 存储蓝色像素及其频率
	freqByte = 0;
	for (int x : bFreq)
	{
		if (x != 0)
			freqByte += 1;
	}
	freqByte *= 5;
	out.write(reinterpret_cast<char *>(&freqByte), 4);
	for (int i = 0; i < 256; ++ i)
	{
		if (bFreq[i] != 0)
		{
			unsigned char b = i;
			out.write(reinterpret_cast<char *>(&b), 1);
			out.write(reinterpret_cast<char *>(&bFreq[i]), 4);
		}
	}

	// 存储红色像素编码
	int allLen = 0;
	for (std::string const &s : rCode)
		allLen += s.length();
	int needByte = allLen / 8 + (allLen % 8 != 0 ? 1 : 0);
	out.write(reinterpret_cast<char const *>(&allLen), 4);		// 存储编码长度
	out.write(reinterpret_cast<char const *>(&needByte), 4);	// 存储编码需要的字节数
	char *buf = new char[needByte];

	int nowBit = 0, k = 0;
	char data = 0;
	for (int i = 0; i < rCode.size(); ++ i)
	{
		for (int j = 0; j < rCode[i].size(); ++ j)
		{
			char c = rCode[i][j] - '0';
			if (nowBit < 8)
			{
				data += c;
				data <<= 1;
			}

			else
			{
				buf[k ++] = data;
				data = 0;
				nowBit = 0;
			}

			nowBit += 1;
		}
	}
	out.write(buf, needByte);
	delete[] buf;

	// 存储绿色像素编码
	allLen = 0;
	for (std::string const &s : gCode)
		allLen += s.length();
	needByte = allLen / 8 + (allLen % 8 != 0 ? 1 : 0);
	out.write(reinterpret_cast<char const *>(&allLen), 4);		// 存储编码长度
	out.write(reinterpret_cast<char const *>(&needByte), 4);	// 存储编码需要的字节数
	buf = new char[needByte];

	nowBit = 0, k = 0;
	data = 0;
	for (int i = 0; i < gCode.size(); ++ i)
	{
		for (int j = 0; j < gCode[i].size(); ++ j)
		{
			char c = gCode[i][j] - '0';
			if (nowBit < 8)
			{
				data += c;
				data <<= 1;
			}

			else
			{
				buf[k ++] = data;
				data = 0;
				nowBit = 0;
			}

			nowBit += 1;
		}
	}
	out.write(buf, needByte);
	delete[] buf;

	// 存储蓝色像素编码
	allLen = 0;
	for (std::string const &s : bCode)
		allLen += s.length();
	needByte = allLen / 8 + (allLen % 8 != 0 ? 1 : 0);
	out.write(reinterpret_cast<char const *>(&allLen), 4);		// 存储编码长度
	out.write(reinterpret_cast<char const *>(&needByte), 4);	// 存储编码需要的字节数
	buf = new char[needByte];

	nowBit = 0, k = 0;
	data = 0;
	for (int i = 0; i < bCode.size(); ++ i)
	{
		for (int j = 0; j < bCode[i].size(); ++ j)
		{
			char c = bCode[i][j] - '0';
			if (nowBit < 8)
			{
				data += c;
				data <<= 1;
			}

			else
			{
				buf[k ++] = data;
				data = 0;
				nowBit = 0;
			}

			nowBit += 1;
		}
	}
	out.write(buf, needByte);
	delete[] buf;

	out.close();

	std::ofstream outHuf("check1.txt", std::ios::out | std::ios::binary);
	for (int i = 0; i < 256; ++ i)
	{
		outHuf << rHuffCoding->getHuffmanCode(i) << "\n";
	}
	outHuf.close();
}

void HuffmanImageCompression::openFile()
{
	ui.imgView->setScene(nullptr);
	delete rHuffCoding;
	delete gHuffCoding;
	delete bHuffCoding;

	QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "", "*.bmp *.huf");
	if (fileName.isEmpty())
	{
		return;
	}

	std::string suffix = fileName.toStdString().substr(fileName.toStdString().find_last_of("."), fileName.toStdString().length() - fileName.toStdString().find_last_of("."));

	QThread *readTh = nullptr;

	if (suffix == ".bmp")
	{
		readTh = QThread::create([this, fileName] ()
			{
				openImg(fileName);
			});
	}

	else if (suffix == ".huf")
	{
		readTh = QThread::create([this, fileName] ()
			{
				openHuf(fileName);
			});
	}

	else
	{
		return;
	}

	readTh->setParent(this);
	readTh->start();

	QMessageBox::information(this, tr("提示"), tr("读入成功"));
}

void HuffmanImageCompression::saveFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), "", "*.bmp *.huf");
	if (fileName.isEmpty())
	{
		return;
	}

	std::string suffix = fileName.toStdString().substr(fileName.toStdString().find_last_of("."), fileName.toStdString().length() - fileName.toStdString().find_last_of("."));

	QThread *readTh = nullptr;

	if (suffix == ".bmp")
	{
		readTh = QThread::create([this, fileName] ()
			{
				saveImg(fileName);
			});
	}

	else if (suffix == ".huf")
	{
		readTh = QThread::create([this, fileName] ()
			{
				saveHuf(fileName);
			});
	}

	else
	{
		return;
	}

	readTh->setParent(this);
	readTh->start();

	QMessageBox::information(this, tr("提示"), tr("保存成功"));
}

void HuffmanImageCompression::huffmanCompress()
{
	if (inputBmp.imgData.empty())
		return;

	delete rHuffCoding;
	delete gHuffCoding;
	delete bHuffCoding;

	QThread *rChannel = QThread::create([this] ()
		{
			std::vector<unsigned char> r;

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
					r.emplace_back(inputBmp.imgData[(i * inputBmp.info.width) + j][2]);

			std::vector<int> freq(256, 0);
			for (int i = 0; i < r.size(); i ++)
				freq[r[i]] += 1;

			rHuffCoding = new HuffmanCoding();
			rHuffCoding->coding(freq);

			rFreq = freq;

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
				{
					rCode.emplace_back(rHuffCoding->getHuffmanCode(inputBmp.imgData[(i * inputBmp.info.width) + j][2]));
				}
		});

	QThread *gChannel = QThread::create([this] ()
		{
			std::vector<unsigned char> g;

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
					g.emplace_back(inputBmp.imgData[(i * inputBmp.info.width) + j][1]);

			std::vector<int> freq(256, 0);
			for (int i = 0; i < g.size(); i ++)
				freq[g[i]] += 1;

			gHuffCoding = new HuffmanCoding();
			gHuffCoding->coding(freq);

			gFreq = freq;

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
				{
					gCode.emplace_back(gHuffCoding->getHuffmanCode(inputBmp.imgData[(i * inputBmp.info.width) + j][1]));
				}
		});

	QThread *bChannel = QThread::create([this] ()
		{
			std::vector<unsigned char> b;

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
					b.emplace_back(inputBmp.imgData[(i * inputBmp.info.width) + j][0]);

			std::vector<int> freq(256, 0);
			for (int i = 0; i < b.size(); i ++)
				freq[b[i]] += 1;

			bHuffCoding = new HuffmanCoding();
			bHuffCoding->coding(freq);

			bFreq = freq;

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
				{
					bCode.emplace_back(bHuffCoding->getHuffmanCode(inputBmp.imgData[(i * inputBmp.info.width) + j][0]));
				}
		});

	rChannel->setParent(this), gChannel->setParent(this), bChannel->setParent(this);
	rChannel->start(), gChannel->start(), bChannel->start();

	QMessageBox::information(this, tr("提示"), tr("压缩成功"));

	//delete rHuffCoding;

	//rHuffCoding = new HuffmanCoding();

	//rHuffCoding->coding(rFreq);

	//std::ofstream out("check3.txt", std::ios::out | std::ios::binary);

	//for (int i = 0; i < 256; i ++)
	//	out << rHuffCoding->getHuffmanCode(i) << "\n";

	//out.close();
}