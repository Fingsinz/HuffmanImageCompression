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
    Ui::HuffmanImageCompressionClass ui;
};
