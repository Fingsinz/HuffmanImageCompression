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
	QFont font(tr("微软雅黑 Light"), 12);
	this->setFont(font);

	QMenu *fileMenu = ui.menuBar->addMenu(tr("文件"));
	fileMenu->setFont(font);
	fileMenu->addAction(tr("打开"), this, SLOT(openFile()))->setShortcut(QKeySequence::Open);
	QMenu *saveMenu = fileMenu->addMenu(tr("保存为.."));
	saveMenu->setFont(font);
	saveMenu->addAction(tr("保存为图片"), this, SLOT(saveImg()))->setShortcut(QKeySequence::Save);
	saveMenu->addAction(tr("保存为Huffman文件"), this, SLOT(saveHuf()))->setShortcut(QKeySequence::SaveAs);

	QMenu *operateMenu = ui.menuBar->addMenu(tr("操作"));
	operateMenu->setFont(font);
	operateMenu->addAction(tr("哈夫曼压缩"), this, SLOT(huffmanCompress()));
}

int HuffmanImageCompression::openImg(QString const &fileName)
{
	int flag = inputBmp.load(fileName.toStdString());
	if (flag != 0)
		return flag;

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

	mImg = img;
	QGraphicsScene *scene = new QGraphicsScene(ui.imgView);
	scene->addPixmap(QPixmap::fromImage(img));
	ui.imgView->setScene(scene);

	return flag;
}

int HuffmanImageCompression::openHuf(QString const &fileName)
{
	std::ifstream in(fileName.toStdString(), std::ios::in | std::ios::binary);

	if (!in.is_open())
	{
		return -1;
	}

	std::vector<int>(256, 0).swap(rFreq);
	std::vector<int>(256, 0).swap(gFreq);
	std::vector<int>(256, 0).swap(bFreq);

	int height = 0, width = 0;
	in.read(reinterpret_cast<char *>(&height), 4);
	in.read(reinterpret_cast<char *>(&width), 4);

	if (height == 0 or width == 0)
		return -2;

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

	mImg = img;
	QGraphicsScene *scene = new QGraphicsScene(ui.imgView);
	scene->addPixmap(QPixmap::fromImage(img));
	ui.imgView->setScene(scene);

	return 0;
}

void HuffmanImageCompression::saveImg()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("保存为图像文件"), "", "*.jpg .png .bmp");
	if (fileName.isEmpty())
	{
		return;
	}

	int result = -1;
	QThread *saveTh = QThread::create([this, &fileName, &result] ()
		{
			mImg.save(fileName);
			result = 0;
		});

	saveTh->setParent(this);
	saveTh->start();
	saveTh->wait();

	switch (result)
	{
		case 0:
			QMessageBox::information(this, tr("提示"), tr("保存文件成功"));
			break;
		case -1:
			QMessageBox::warning(this, tr("警告"), tr("保存文件失败"));
			break;
		default:
			break;
	}
}

void HuffmanImageCompression::saveHuf()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("保存Huffman压缩文件"), "", "*.huf");

	if (fileName.isEmpty())
	{
		return;
	}

	int result = -1;
	QThread *readTh = QThread::create([this, &fileName, &result] ()
		{
			if (rHuffCoding == nullptr or gHuffCoding == nullptr or bHuffCoding == nullptr)
				return -2;

			std::ofstream out(fileName.toLocal8Bit(), std::ios::out | std::ios::binary);

			if (!out.is_open())
			{
				return -1;
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

			int allLen = 0;
			int nowBit = 0, k = 0;
			char data = 0;

			// 存储红色像素编码
			std::string rCodeStr = "";
			for (std::string const &s : rCode)
				rCodeStr += s;
			allLen = rCodeStr.length();
			int needByte = allLen / 8 + (allLen % 8 != 0 ? 1 : 0);
			out.write(reinterpret_cast<char const *>(&allLen), 4);		// 存储编码长度
			out.write(reinterpret_cast<char const *>(&needByte), 4);	// 存储编码需要的字节数
			char *buf = new char[needByte];

			nowBit = 0, k = 0, data = 0;
			for (int i = 0; i < allLen; ++ i)
			{
				char c = rCodeStr[i] - '0';

				if (nowBit == 8)
				{
					buf[k ++] = data;
					nowBit = 0, data = 0;
					i -= 1;
				}

				else if (nowBit < 8)
				{
					data += c;
					if (nowBit < 7)
						data <<= 1;
					nowBit += 1;
				}
			}
			while (nowBit < 7)
				data <<= 1, nowBit += 1;
			buf[k] = data;

			out.write(buf, needByte);
			delete[] buf;

			// 存储绿色像素编码
			std::string gCodeStr = "";
			for (std::string const &s : gCode)
				gCodeStr += s;
			allLen = gCodeStr.length();
			needByte = allLen / 8 + (allLen % 8 != 0 ? 1 : 0);
			out.write(reinterpret_cast<char const *>(&allLen), 4);		// 存储编码长度
			out.write(reinterpret_cast<char const *>(&needByte), 4);	// 存储编码需要的字节数
			buf = new char[needByte];

			nowBit = 0, k = 0, data = 0;
			for (int i = 0; i < allLen; ++ i)
			{
				char c = gCodeStr[i] - '0';

				if (nowBit == 8)
				{
					buf[k ++] = data;
					nowBit = 0, data = 0;
					i -= 1;
				}

				else if (nowBit < 8)
				{
					data += c;
					if (nowBit < 7)
						data <<= 1;
					nowBit += 1;
				}
			}
			while (nowBit < 7)
				data <<= 1, nowBit += 1;
			buf[k] = data;

			out.write(buf, needByte);
			delete[] buf;

			// 存储蓝色像素编码
			std::string bCodeStr = "";
			for (std::string const &s : bCode)
				bCodeStr += s;
			allLen = bCodeStr.length();
			needByte = allLen / 8 + (allLen % 8 != 0 ? 1 : 0);
			out.write(reinterpret_cast<char const *>(&allLen), 4);		// 存储编码长度
			out.write(reinterpret_cast<char const *>(&needByte), 4);	// 存储编码需要的字节数
			buf = new char[needByte];

			nowBit = 0, k = 0, data = 0;
			for (int i = 0; i < allLen; ++ i)
			{
				char c = bCodeStr[i] - '0';

				if (nowBit == 8)
				{
					buf[k ++] = data;
					nowBit = 0, data = 0;
					i -= 1;
				}

				else if (nowBit < 8)
				{
					data += c;
					if (nowBit < 7)
						data <<= 1;
					nowBit += 1;
				}
			}
			while (nowBit < 7)
				data <<= 1, nowBit += 1;
			buf[k] = data;

			out.write(buf, needByte);
			delete[] buf;

			out.close();
			result = 0;
		});

	readTh->setParent(this);
	readTh->start();
	readTh->wait();

	switch (result)
	{
		case 0:
			QMessageBox::information(this, tr("提示"), tr("保存文件成功"));
			break;
		case -1:
			QMessageBox::warning(this, tr("警告"), tr("保存文件失败"));
			break;
		case -2:
			QMessageBox::warning(this, tr("警告"), tr("未进行哈夫曼压缩"));
			break;
		default:
			break;
	}
}

void HuffmanImageCompression::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "", "*.bmp *.huf");
	if (fileName.isEmpty())
	{
		return;
	}

	clear();
	ui.imgView->setScene(nullptr);

	std::string suffix = fileName.toStdString().substr(fileName.toStdString().find_last_of("."), fileName.toStdString().length() - fileName.toStdString().find_last_of("."));

	QThread *readTh = nullptr;
	int result = 0;

	if (suffix == ".bmp")
	{
		readTh = QThread::create([this, fileName, &result] ()
			{
				result = openImg(fileName);
			});
	}

	else if (suffix == ".huf")
	{
		readTh = QThread::create([this, fileName, &result] ()
			{
				result = openHuf(fileName);
			});
	}

	else
	{
		return;
	}

	readTh->setParent(this);
	readTh->start();
	readTh->wait();

	switch (result)
	{
		case 0:
			QMessageBox::information(this, tr("提示"), tr("打开文件成功"));
			break;
		case -1:
			QMessageBox::warning(this, tr("警告"), tr("打开文件失败"));
			return;
		case -2:
			QMessageBox::warning(this, tr("警告"), tr("文件格式错误"));
			return;
		default:
			break;
	}
}

void HuffmanImageCompression::clear()
{
	delete rHuffCoding; rHuffCoding = nullptr;
	delete gHuffCoding; gHuffCoding = nullptr;
	delete bHuffCoding; bHuffCoding = nullptr;
}

void HuffmanImageCompression::huffmanCompress()
{
	if (inputBmp.imgData.empty())
		return;

	clear();

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

	ui.statusBar->showMessage(tr("经过哈夫曼编码"));
}