#pragma once

#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>

class BMP
{
public:
#pragma pack(2)
	// BMP �ļ�ͷ
	struct BMPFile
	{
		unsigned short type;		// �ļ�����
		unsigned long size;			// �ļ���С
		unsigned short reserved1;	// Ԥ������1
		unsigned short reserved2;	// Ԥ������2
		unsigned long offset;		// ͼ�����ݿ�ʼλ�õ�ƫ��
	};

	// BMP ��Ϣͷ
	struct BMPInfo
	{
		unsigned long size;				// ��Ϣͷ��С
		long width;						// ͼ����
		long height;					// ͼ��߶�
		unsigned short planes;			// ͼ��λ����
		unsigned short bits;			// ÿ�����ص�λ��
		unsigned long compression;		// ѹ������
		unsigned long sizeImage;		// ͼ���С��width * height * bits / 8
		long xPixelsPerMeter;			// ˮƽ�ֱ���
		long yPixelsPerMeter;			// ��ֱ�ֱ���
		unsigned long colorUsed;		// ʵ��ʹ�õ���ɫ���е���ɫ��
		unsigned long colorImportant;	// ��Ҫ��ɫ��
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
