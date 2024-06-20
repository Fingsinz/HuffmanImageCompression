#include "HuffmanImageCompression.h"
#include "qimage.h"
#include <qthread.h>
#include <qfiledialog.h>
#include <qgraphicsview.h>
#include <qmessagebox.h>
#include <qthread.h>
#include <vector>

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
{}

void HuffmanImageCompression::saveImg(QString const &fileName)
{}

void HuffmanImageCompression::saveHuf(QString const &fileName)
{}

void HuffmanImageCompression::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "", "*.bmp *.huf");
	ui.statusBar->showMessage(tr("打开 ") + fileName);

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
}

void HuffmanImageCompression::saveFile()
{}

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

			rHuffCoding = new HuffmanCoding();
			rHuffCoding->coding(r);

			std::vector<int>(256, 0).swap(rFreq);

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
				{
					rCode.emplace_back(rHuffCoding->getHuffmanCode(inputBmp.imgData[(i * inputBmp.info.width) + j][2]));

					rFreq[inputBmp.imgData[(i * inputBmp.info.width) + j][2]] += 1;
				}
		});

	QThread *gChannel = QThread::create([this] ()
		{
			std::vector<unsigned char> g;

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
					g.emplace_back(inputBmp.imgData[(i * inputBmp.info.width) + j][1]);

			gHuffCoding = new HuffmanCoding();
			gHuffCoding->coding(g);

			std::vector<int>(256, 0).swap(gFreq);

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
				{
					gCode.emplace_back(gHuffCoding->getHuffmanCode(inputBmp.imgData[(i * inputBmp.info.width) + j][1]));

					gFreq[inputBmp.imgData[(i * inputBmp.info.width) + j][1]] += 1;
				}
		});

	QThread *bChannel = QThread::create([this] ()
		{
			std::vector<unsigned char> b;

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
					b.emplace_back(inputBmp.imgData[(i * inputBmp.info.width) + j][0]);

			bHuffCoding = new HuffmanCoding();
			bHuffCoding->coding(b);

			std::vector<int>(256, 0).swap(bFreq);

			for (int i = inputBmp.info.height - 1; i >= 0; i --)
				for (int j = 0; j < inputBmp.info.width; j ++)
				{
					bCode.emplace_back(bHuffCoding->getHuffmanCode(inputBmp.imgData[(i * inputBmp.info.width) + j][0]));

					bFreq[inputBmp.imgData[(i * inputBmp.info.width) + j][0]] += 1;
				}
		});

	rChannel->setParent(this), gChannel->setParent(this), bChannel->setParent(this);
	rChannel->start(), gChannel->start(), bChannel->start();

	ui.statusBar->showMessage(tr("已完成压缩"));
}