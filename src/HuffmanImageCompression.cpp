#include "HuffmanImageCompression.h"

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
	fileMenu->addAction(tr("打开"), this, SLOT())->setShortcut(QKeySequence::Open);
	fileMenu->addAction(tr("保存"), this, SLOT())->setShortcut(QKeySequence::Save);

	QMenu *operateMenu = ui.menuBar->addMenu(tr("操作"));
	operateMenu->setFont(font);
	operateMenu->addAction(tr("哈夫曼压缩"), this, SLOT());
}

void HuffmanImageCompression::openImg()
{
}

void HuffmanImageCompression::saveResult()
{
}

void HuffmanImageCompression::huffmanCompress()
{
}