#pragma once

#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

class HuffmanCoding
{
private:
	struct HuffmanNode
	{
		unsigned char pixel;
		int frequency;
		HuffmanNode *left;
		HuffmanNode *right;

		HuffmanNode(unsigned char p, int f, HuffmanNode *l = nullptr, HuffmanNode *r = nullptr) : pixel(p), frequency(f), left(l), right(r) {}
	};

	struct HuffmanNodeComparator
	{
		bool operator() (HuffmanNode *l, HuffmanNode *r)
		{
			return l->frequency > r->frequency;
		}
	};

private:
	HuffmanNode *root{ nullptr };
	std::unordered_map<unsigned char, std::string> codes;

private:
	void destoryTree(HuffmanNode *node);

public:
	HuffmanCoding() = default;

	~HuffmanCoding();

	/**
	 * @brief 对给定的像素执行编码操作
	 * @param pixels 像素数组
	 */
	void coding(std::vector<unsigned char> const &pixels);

	/**
	 * @brief 对给定的像素执行编码操作
	 * @param freq 字符频率
	 */
	void coding(std::vector<int> const &freq);

	/**
	 * @brief 基于给定像素构建哈夫曼树
	 * @param pixels 像素数组
	 */
	void buildHuffmanTree(std::vector<unsigned char> const &pixels);

	/**
	 * @brief 基于给定像素构建哈夫曼树
	 * @param freq 字符频率
	 */
	void buildHuffmanTree(std::vector<int> const &freq);

	/**
	 * @brief 为给定的哈夫曼节点生成哈夫曼代码
	 * @param node 哈夫曼树根节点
	 * @param code 哈夫曼编码
	 */
	void generateCodes(HuffmanNode *node, std::string code);

	/**
	 * @brief 获取特定像素的哈夫曼代码
	 * @param pixel 特定像素
	 * @return String 哈夫曼编码
	 */
	std::string getHuffmanCode(unsigned char pixel);

	/**
	 * @brief 使用哈夫曼编码对编码数据进行解码
	 * @param encodeData 包含编码数据的字符串
	 * @return Vector 解码数组
	 */
	std::vector<unsigned char> decode(std::string const &encodeData);
};
