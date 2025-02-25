// EasureCode.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "dataOperation.h"
using namespace std;

const string fileName = "data.txt";


//生成原始数据文件data.txt，存放4*6个随机数生成的矩阵
void dataPreparation()
{

	srand(time(NULL));

	ifstream file(fileName);
	if (!file) {
		ofstream file(fileName);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 6; ++j) {
				file << rand() % 10 << " ";
			}
			file << "\n";
		}
		file.close();
	}
	else {
		cout << "元数据文件已存在" << endl;
	}
}


//定义6*4的编码矩阵
void encodeMatrixDef()
{
	encodeMatrix << 1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1,
					1, 1, 1, 1,
					1, 2, 4, 8;
}


//字符串分割函数split
vector<string> split(const string& src, string separate_character)
{
	vector<string> strs;
	int separate_characterLen = separate_character.size();//分割字符串的长度,这样就可以支持如“,,”多字符串的分隔符
	int lastPosition = 0, index = -1;

	while (-1 != (index = src.find(separate_character, lastPosition)))
	{
		strs.push_back(src.substr(lastPosition, index - lastPosition));
		lastPosition = index + separate_characterLen;
	}
	string lastString = src.substr(lastPosition);//截取最后一个分隔符后的内容
	if (!lastString.empty())
		strs.push_back(lastString);//如果最后一个分隔符后还有内容就入队

	return strs;
}


//数据从data.txt里读取出来并分块
void dataPartition()
{
	ifstream file(fileName);
	if (!file) {
		cout << "文件打开失败" << endl;
		exit(0);
	}
	else {
		string line;
		string del = " "; //设定分隔符
		int i = 0;

		while (getline(file, line)) {
			vector<string> strs = split(line, del);
			for (int j = 0; j < strs.size(); ++j) {
				stringstream sstr(strs[j]);
				double x;
				sstr >> x; //将字符串转为数字
				metaData(i,j) = x;
			}
			++i;
		}
	}
}


//编码矩阵和数据块相乘
void encodeProcess()
{
	encodeResult = encodeMatrix * metaData;
}


//保存编码结果于不同的文件中
void dataDeploy()
{
	vector<string> name = { "1","2","3","4","5","6" };

	for (int i = 0; i < 6; ++i) {
		string tmpName = name[i] + ".txt";

		ifstream infile(tmpName);
		if (infile) {
			cout << "文件已存在，准备删除并重新创建数据文件" << tmpName << endl;
			remove(tmpName.c_str());
		}
		ofstream outfile(tmpName);
		outfile << encodeResult.row(i);
		outfile.close();
	}
}


//模拟数据丢失
void simulator()
{
	//随机丢失一个数据块和校验块
	srand(time(NULL));
	stringstream data_name, chunk_name;
	int dataBlock = rand() % 4 + 1;
	int chunkBlock = 5 + rand() % 2;

	data_name << dataBlock;
	chunk_name << chunkBlock;

	string str_data = data_name.str() + ".txt";
	string str_chunk = chunk_name.str() + ".txt";
	remove(str_data.c_str());
	remove(str_chunk.c_str());

	ifstream file_data(str_data);
	ifstream file_chunk(str_chunk);
	if (!file_data && !file_chunk) {
		cout << "数据文件" << str_data << "丢失" << endl;
		cout << "校验块" << str_chunk << "丢失" << endl;
	}
}


//数据恢复
void decodeProcess()
{
	vector<string> name = { "1","2","3","4","5","6" };
	vector<int> existFile; //记录存在的数据文件的序号
	vector<int> loseFile;

	for (int i = 0; i < 6; ++i) {
		string tmpName = name[i] + ".txt";
		ifstream file(tmpName);
		if (!file) {
			cout << tmpName << "丢失" << endl;
			loseFile.push_back(i + 1);
		}
		else {
			existFile.push_back(i + 1);
		}
	}

	MatrixXd encodeResult_exist(4, 6); //数据丢失后的编码结果
	MatrixXd encodeMatrix_exist(4, 4); //数据丢失后的编码矩阵
	for (int i = 0; i < 4; ++i) {
		encodeMatrix_exist.row(i) = encodeMatrix.row(existFile[i] - 1);
		encodeResult_exist.row(i) = encodeResult.row(existFile[i] - 1);
	}

	MatrixXd tmpDataMatrix(4, 6);
	tmpDataMatrix = encodeMatrix_exist.inverse() * encodeResult_exist;
	
	//重新编码生成丢失的文件
	for (int i = 0; i < 2; ++i) {
		string tmpName = name[loseFile[i] - 1] + ".txt";
		ofstream file(tmpName);
		file << encodeMatrix.row(loseFile[i] - 1) * tmpDataMatrix;
		file.close();
	}
}


int main()
{
	cout << "数据编码开始！" << endl;

	dataPreparation(); //生成原始数据文件data.txt，存放4*6个随机数生成的矩阵
	encodeMatrixDef(); //定义6*4的编码矩阵
	dataPartition(); //数据从data.txt里读取出来并分块
	encodeProcess(); //编码过程
	dataDeploy(); //保存编码结果于不同文件

	cout << "开始模拟数据丢失" << endl;

	simulator();

	cout << "开始数据恢复" << endl;

	decodeProcess();

    return 0;
}

