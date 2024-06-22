#include "BMP.h"
#include <memory>

BMP::BMP(std::string const &path)
{
	load(path);
}

int BMP::load(std::string const &path)
{
	std::vector<std::vector<unsigned char>>().swap(imgData);

	std::ifstream in(path, std::ios::binary | std::ios::in);

	if (!in.is_open())
		return -1;

	in.read(reinterpret_cast<char *>(&file), sizeof(BMPFile));
	in.read(reinterpret_cast<char *>(&info), sizeof(BMPInfo));

	if (file.type != 0x4D42 or info.bits != 24)
		return -2;

	in.seekg(file.offset);

	std::unique_ptr<char[]> data = std::make_unique<char[]>(info.width * info.height * 3);
	in.read(data.get(), info.width * info.height * 3);

	char *ptr = data.get();

	for (int i = 0; i < info.height; i++)
	{
		for (int j = 0; j < info.width; j++)
		{
			std::vector<unsigned char> _rgb;
			_rgb.push_back(*ptr++);
			_rgb.push_back(*ptr++);
			_rgb.push_back(*ptr++);

			if (j == info.width - 1)
			{
				for (int k = 0; k < info.width % 4; k ++)
				{
					ptr++;
				}
			}

			imgData.push_back(_rgb);
		}
	}

	in.close();
	return 0;
}