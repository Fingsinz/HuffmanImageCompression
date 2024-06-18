#include "HuffmanCoding.h"

HuffmanCoding::~HuffmanCoding()
{
	destoryTree(root);
}

void HuffmanCoding::destoryTree(HuffmanNode *node)
{
	if (node == nullptr)
		return;

	destoryTree(node->left);
	destoryTree(node->right);
	delete node;
}

void HuffmanCoding::coding(std::vector<unsigned char> const &pixels)
{
	buildHuffmanTree(pixels);
	generateCodes(root, "");
}

void HuffmanCoding::buildHuffmanTree(std::vector<unsigned char> const &pixels)
{
	std::unordered_map<unsigned char, int> freq;

	for (unsigned char pixel : pixels)
		freq[pixel]++;

	std::priority_queue < HuffmanNode *, std::vector<HuffmanNode *>, HuffmanNodeComparator > pq;

	// 排序
	for (auto &it : freq)
	{
		pq.push(new HuffmanNode(it.first, it.second));
	}

	// 构建哈夫曼树
	while (pq.size() > 1)
	{
		HuffmanNode *left = pq.top();
		pq.pop();
		HuffmanNode *right = pq.top();
		pq.pop();
		pq.push(new HuffmanNode(-1, left->frequency + right->frequency, left, right));
	}

	root = pq.top();
}

void HuffmanCoding::generateCodes(HuffmanNode *node, std::string code)
{
	if (node == nullptr)
	{
		return;
	}

	if (node->pixel != -1)
	{
		codes[node->pixel] = code;
	}

	generateCodes(node->left, code + "0");
	generateCodes(node->right, code + "1");
}

std::string HuffmanCoding::getHuffmanCode(unsigned char pixel)
{
	return codes[pixel];
}

std::vector<unsigned char> HuffmanCoding::decode(std::string const &encodeData)
{
	std::vector<unsigned char> decodedPixels;
	HuffmanNode *current = root;

	for (char bit : encodeData)
	{
		if (bit == '0' and current->left != nullptr)
		{
			current = current->left;
		}

		else if (bit == '1' and current->right != nullptr)
		{
			current = current->right;
		}

		else
		{
			return {};
		}

		if (current->left == nullptr and current->right == nullptr)
		{
			decodedPixels.push_back(current->pixel);
			current = root;
		}
	}

	return decodedPixels;
}