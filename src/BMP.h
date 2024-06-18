#pragma once

#include <fstream>
#include <string>
#include <vector>

class BMP
{
public:
#pragma pack(2)
	// BMP 文件头
	struct BMPFile
	{
		unsigned short type;		// 文件类型
		unsigned long size;			// 文件大小
		unsigned short reserved1;	// 预留区域1
		unsigned short reserved2;	// 预留区域2
		unsigned long offset;		// 图像数据开始位置的偏移
	};

	// BMP 信息头
	struct BMPInfo
	{
		unsigned long size;				// 信息头大小
		long width;						// 图像宽度
		long height;					// 图像高度
		unsigned short planes;			// 图像位面数
		unsigned short bits;			// 每个像素的位数
		unsigned long compression;		// 压缩方法
		unsigned long sizeImage;		// 图像大小：width * height * bits / 8
		long xPixelsPerMeter;			// 水平分辨率
		long yPixelsPerMeter;			// 垂直分辨率
		unsigned long colorUsed;		// 实际使用的颜色表中的颜色数
		unsigned long colorImportant;	// 重要颜色数
	};
#pragma pack(8)

public:
	BMPFile file;
	BMPInfo info;
	std::vector<std::vector<unsigned char>> imgData;

public:
	BMP() = default;
	explicit BMP(std::string const &path);

	~BMP() = default;
	int load(std::string const &path);
};
