# 基于哈夫曼编码的图像压缩 / Image compression based on Huffman coding

此项目仅实现基于哈夫曼编码的图像压缩，并无过多讨论。

*暂时只能输入 24位 Bmp 图像。*

关键词：编程思维、数据结构、文件读写和 C++/Qt 的代码能力。

This project only implements image compression based on Huffman coding, without much discussion.

*Only 24-bit Bmp images can be entered for now.*

Key Words: Programming Thinking, Data Structures, File Reading and Writing, C++/Qt Coding

## 开发环境 / Development environment

- Visual Studio 2022
- Qt 6.6.1_msvc2019_64

## 项目结构 / Project structure

- example: 测试样例 / Test cases
- src: 源代码 / source codes

## 效果展示 / Effect display


测试1 / Test1：lena.bmp

![](example/lena.jpg)

| lena.bmp | lena.huf | lena.jpg | lena.png |
|:-:|:-:|:-:|:-:|
| 733 KB | 673 KB | 38 KB | 403 KB |

压缩率(之后/之前) / Compressibility (After / Before)：91.81%

测试2 / Test2：RGB.bmp

![](example/RGB.jpg)

| RGB.bmp | RGB.huf | RGB.jpg | RGB.png |
|:-:|:-:|:-:|:-:|
| 733 KB | 92 KB | 7 KB | 2 KB |

压缩率(之后/之前) / Compressibility (After / Before)：12.55%

测试3 / Test3：butterfly.bmp

![](example/butterfly.jpg)

| butterfly.bmp | butterfly.huf | butterfly.jpg | butterfly.png |
|:-:|:-:|:-:|:-:|
| 193 KB | 178 KB | 17 KB | 121 KB |

压缩率(之后/之前) / Compressibility (After / Before)：92.23%