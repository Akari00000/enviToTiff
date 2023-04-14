#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <io.h>
#include <windows.h>
#include <algorithm>

using namespace std;
/*这个估计是大小端转换用的
#define MC_GET_16(__data__) ((__data__ & 0xFF) << 8) | (__data__ >> 8)
#define MC_GET_64(__data__) ((__data__ & 0xFF) << 24) | ((__data__ & 0xFF00) << 8) | ((__data__ & 0xFF0000) >> 8) | ((__data__ & 0xFF000000) >> 24)
*/
typedef struct
{
	uint16_t endian ; // 字节顺序标志位,值为II或者MM:II表示小字节在前,又称为little-endian,MM表示大字节在前,又称为big-endian
	uint16_t magic ;  // TIFF的标志位，一般都是42
	uint32_t ifdOffset ; // 第一个IFD的偏移量,可以在任意位置,但必须是在一个字的边界,也就是说必须是2的整数倍
}IFH;

typedef struct
{
	uint16_t tag ;  // 此TAG的唯一标识
	uint16_t type ; // 数据类型
	uint32_t size ; // 数量,通过类型和数量可以确定存储此TAG的数据需要占据的字节数
	uint32_t valOffset ;//如果占用的字节数少于4， 则数据直接存于此。 如果超过4个，则这里存放的是指向实际数据的指针
}DE;

typedef struct
{
	uint16_t deNumber; // 表示此IFD包含了多少个DE,假设数目为n
	DE* pDE ; // n个DE
	uint32_t nextIfdOffset; // 下一个IFD的偏移量,如果没有则置为0
}IFD;

typedef struct
{
	//我的图像有15个属性，我用的是tiff文档的变量命名
	/*子文件类型:
	  Bit0：如果该图像是此TIFF文件中另一图像的降低分辨率版本，则为1;否则位为0。
      Bit1：如果图像是多页图像中的一页，位1为1:否则位为0。
      Bit2：如果图像为该TIFF文件中的另一个图像定义了透明掩码，则比特2为1。
      PhotometricInterpretation值必须为4，表示透明掩码。
      这些值被定义为位标志，因为它们彼此独立。
      默认值为0
	*/

	/*请注意x，y不能被定义为32位以下，这是类或结构体的成员存储逻辑导致的*/
	//type对应class里定义的type的字节数，de里有每个数据的类型，我只按照存放数据的数字大小分配内存
	uint16_t subfileType ;//type4, 新的子文件类型
	uint16_t imageWidth ; // type4, 表示图像宽度，列
	uint16_t imageLength ;  // type4, 表示图像高度，行
	uint16_t bitsPerSample ;//type3, 比特采样率,每个波段的采样率。基线TIFF灰度图像的允许值为4和8，两者都允许16或256个不同的灰色阴影 
	uint16_t compression ;//type3, 图像是否压缩
	uint16_t photometricInterpretation ;//type3, 像素合成
	uint32_t stripOffsets ;//type4, 图像资料位置
	uint16_t orientation ;//type3, 图像相对于行列的方向
	uint16_t samplesPerPixel ;//type3, 每个像素的分级量,通道，对本项目来说就是光谱数
	uint16_t rowsPerStrip ;//type4, 每个条带中的行数,条带化数据存储
	uint32_t stripByteCounts ;//type4, 每个条带中的字节数,通俗来讲就是行数，数值等于行数*通道数*每个数据的字节数
	uint32_t xResolution ;//type5, X分辨率，XResolution(0x011a)/YResolution(0x011b)的单位. '1' 表示没有单位, '2' 意味着英寸, '3' 表示厘米. 缺省值是 '2'(英寸).
	uint32_t yResolution ;//type5, Y分辨率
	uint16_t planarConfiguration ;//type3, 每个像素的波段是如何存储的
	uint16_t sampleFormat ;//type3, 数据的类型
	
	uint16_t bytePerData;//每个数据的字节数
	uint32_t byteDataSum;//数据总字节数
	
}IMGINFO;

typedef struct
{
	uint16_t samples ;//列数
	uint16_t lines ;//行数
	uint16_t bands ;//波段数
	string  fileType ;
	uint16_t  dataTybe ;//数据字节数
	string  interleave ;
	string  sensorType ;
	string  waveLengthUnits ;
	vector<string> waveLength;

}HDR;

static vector<string>  readName;//全局字符串数组，用于存放处理完成的文件的文件名,vx
vector<string> resEnviLog;
/*
 测试机器是大端(big endian:返回false)还是小端(little endian:返回true)

inline bool mashineEndian(void)
{
	uint16_t a = 1;
	uint16_t* p = &a;
	return *((uint8_t*)p) == a;
}
*/

class EnviToTiff {
public:
	IFH _IFH{};
	DE _DE{};
	IFD _IFD{};
	IMGINFO _IMGINFO{};
	HDR _HDR{};

	short tagArray[15] = { 254,256,257,258,259,262,273,274,277,278,279,282,283,284,339 };
	/*
	 NewSubfileType              254  (FE.H),type4
	 ImageWidth                  256 (100.H),type4
	 ImageLength                 257 (101.H),type4
	 BitsPerSample               258 (102.H),type3
	 Compression                 259 (103.H),type3
	 PhotometricInterpretation   262 (106.H),type3
	 StripOffsets                273 (111.H),type4
	 Orientation                 274 (112.H),type3
	 SamplesPerPixel             277 (115.H),type3
	 RowsPerStrip                278 (116.H),type4
	 StripByteCounts             279 (117.H),type4
	 Xresolution                 282 (11A.H),type5
	 Yresolution                 283 (11B.H),type5
	 PlanarConfiguration         284 (11C.H),type3
	 SampleFormat                339 (153.H),type3
	*/

	uint16_t type3 = 3;
	uint32_t type4 = 4;
	uint64_t type5 = 5;
	/* 1 = BYTE 8-bit unsigned integer.
	   2 = ASCII 8-bit byte that contains a 7-bit ASCII code; the last byte
	   3 = SHORT 16-bit (2-byte) unsigned integer.
	   4 = LONG 32-bit (4-byte) unsigned integer.
	   5 = RATIONAL Two LONGs: the first represents the numerator of a fraction; the second, the denominator.
	*/

	char* data;//全局字符串数组，用于存储数据块
	uint16_t countMainEnviToTiff;//主函数计数器

	vector<string> tempReadName;//临时字符串数组，用于存放当前read函数检索的文件名，vy
	vector<string> resReadName;//临时字符串数组，用于存放deweightng_vector_string函数，在对vx，vy去重后的结果

	//函数运行的flag
	bool flagDeweightng_vector_string;
	bool flagRead;
	bool flagAnalyse;
	bool flagSave;
	bool flagMainEnviToTiff;
	
	string logTime();//格式化日志时间输出

	void recursiveCall(string readPath, string savePath, uint32_t count, uint32_t time);//递归调用，调用几次主函数mainEnviToTiff，每次间隔多长时间实现动态检索且只处理后续生成的envi图像。(count,次数，time，秒钟)
	                                                                                   //可能的方法还有1.数据库  
	void mainEnviToTiff(string readPath, string savePath);//主函数，运行一次，处理当前文件夹下的所有符合要求的ENVI图像
	void enviToTiff(string readPath);//mainEnviToTiff plus版，基于txt的log检索处理文件


	void get_need_file(string path, vector<string>& file, string ext);//https://baijiahao.baidu.com/s?id=1749468573339144199&wfr=spider&for=pc  readEnviFileName的方法，单独使用时请参考readEnviFileName函数

	
	void get_file(string path, vector<string>& file, string ext);//get_need_file plus版，基于时间检索排序搜索得的文件名

	void deweightng_vector_string(vector<string>& file0, vector<string>& file1);//https://blog.csdn.net/yang_yi520/article/details/125527602   可以单独调用，对比两个容器，删除相同元素，生成新容器，只支持顺序排列的名字
	void deweightng_vector_string1(vector<string>& file0, vector<string>& file1);//暂时修改

	void readEnviFileName(string readPath);//http://www.taodudu.cc/news/show-79819.html 索引目录下符合要求的ENVI图像的文件名，生成一个保存文件名的容器临时容器tempReadName，单独调用时请输入相对路径，两个文件必须在一个目录之下，命名必须一样

	void analyseEnvi(string hdrPath, string imgPath);//分析源文件数据和参数,单独调用时请输入绝对路径，两个文件必须在一个目录之下，命名必须一样
	void analyseHdr(string hdrPath);//自动提取.hdr的文件信息，数据保存在_HDR中,单独调用时请输入绝对路径
	void analyseImg(string imgPath);//读取.img，生成一个数据指针

	void saveTiff(string savePath);//不可单独调用，可以和analyseEnvi一起使用，使用顺序必须是analyseEnvi--saveTiff

	void dataProcess(string hdrPath, string enviClaisPath, string myPath);//数据分析脚本
	void deProcess(string txtPath, int DENUMBER);//DE处理脚本
	
	//数据指针和DE指针在使用前必须先释放
	~EnviToTiff() {
		free(data);
		data = nullptr;
		free(_IFD.pDE);
		_IFD.pDE = nullptr;
	}
private:

	//以下内容为TIFF头文件 bigEndian = 0x4d4d;
	const uint16_t littleEndian = 0x4949;
	const uint16_t magic = 0x002A;
};

string EnviToTiff::logTime() {
	time_t now = time(0);
	tm gmtm;
	char temp[50];
	localtime_s(&gmtm, &now);
	strftime(temp, sizeof(gmtm), "%Y%m%d_%H%M%S", &gmtm);

	string t = temp;

	return t;//获取拍摄时间
}

void EnviToTiff::get_need_file(string path, vector<string>& file, string ext)
{
	intptr_t file_handle = 0;
	struct _finddata_t file_info;
	string temp;
	if ((file_handle = _findfirst(temp.assign(path).append("/*" + ext).c_str(), &file_info)) != -1)
	{
		do
		{
			file.push_back(temp.assign(path).append("\\").append(file_info.name));
		} while (_findnext(file_handle, &file_info) == 0);
		_findclose(file_handle);
	}
}

bool Less(_finddata_t a, _finddata_t b)
{
	return a.time_create < b.time_create;
}
void EnviToTiff::get_file(string path, vector<string>& file, string ext)
{
	intptr_t file_handle = 0;
	struct _finddata_t file_info;
	string temp;
	vector< _finddata_t> tempInfo;

	if ((file_handle = _findfirst(temp.assign(path).append("/*" + ext).c_str(), &file_info)) != -1)
	{
		do
		{
			tempInfo.push_back(file_info);
		} while (_findnext(file_handle, &file_info) == 0);

		sort(tempInfo.begin(), tempInfo.end(), Less);

		for (int i = 0; i < tempInfo.size(); i++) {
			file.push_back(tempInfo[i].name);
		}

		_findclose(file_handle);
	}
}

void EnviToTiff::deweightng_vector_string(vector<string>& file0, vector<string>& file1) {

	flagDeweightng_vector_string = 1;

	vector<string>vx, vy;
	vx = file0;
	vy = file1;

	int x = file0.size();
	int y = file1.size();

	vector<string> res;

	int p1 = 0, p2 = 0;
	while (p1 < x && p2 < y) {
		if (vx[p1] > vy[p2]) {
			res.push_back(vy[p2]);
			p2++;
		}
		else if (vx[p1] < vy[p2]) {
			res.push_back(vx[p1]);
			p1++;
		}
		else {
			p1++;
			p2++;
		}
	}
	while (p1 < x) {
		res.push_back(vx[p1]);
		p1++;
	}
	while (p2 < y) {
		res.push_back(vy[p2]);
		p2++;
	}
	for (auto x : res) {
		resReadName.push_back(x);
	}

	flagDeweightng_vector_string = flagDeweightng_vector_string - 1;

}
void EnviToTiff::deweightng_vector_string1(vector<string>& file0, vector<string>& file1) {

	flagDeweightng_vector_string = 1;

	vector<string>vx, vy;
	vx = file0;
	vy = file1;

	int x = file0.size();
	int y = file1.size();

	vector<string> res;

	int p1 = 0, p2 = 0;
	while (p1 < x && p2 < y) {
		if (vx[p1] > vy[p2]) {
			res.push_back(vy[p2]);
			p2++;
		}
		else if (vx[p1] < vy[p2]) {
			res.push_back(vx[p1]);
			p1++;
		}
		else {
			p1++;
			p2++;
		}
	}
	while (p1 < x) {
		res.push_back(vx[p1]);
		p1++;
	}
	while (p2 < y) {
		res.push_back(vy[p2]);
		p2++;
	}
	for (auto x : res) {
		resEnviLog.push_back(x);
	}

	flagDeweightng_vector_string = flagDeweightng_vector_string - 1;

}

void EnviToTiff::readEnviFileName(string readPath) {

	flagRead = 1;

	if (flagRead) {
		cout << "\n" << "......读取中......" << "\n" << endl;
	}
	string file_path = readPath + "ENVI\\";
	vector<string> my_file0, my_file1;
	get_need_file(file_path, my_file0, ".hdr");
	get_need_file(file_path, my_file1, ".img");

	if ((my_file0.size() && my_file1.size()) == 0)
	{
		cout << "该目录下没有envi图像或envi图像不完整（必须同时有.img和.hdr）!" << endl;
	}
	else
	{
		if (my_file0.size() != my_file1.size())
		{
			cout << ".img必须和.hdr的数量相等!" << endl;
		}
		else
		{
			for (int i = 0; i < my_file0.size(); i++)
			{
				int ps0 = my_file0[i].find_last_of("\\");
				int pe0 = my_file0[i].find_last_of(".");
				string pic_name0 = my_file0[i].substr(ps0 + 1, pe0 - ps0 - 1);

				int ps1 = my_file1[i].find_last_of("\\");
				int pe1 = my_file1[i].find_last_of(".");
				string pic_name1 = my_file1[i].substr(ps1 + 1, pe1 - ps1 - 1);

				if (pic_name0 == pic_name1) {
					tempReadName.push_back(pic_name0);//输出该文件夹下有多少个文件，名字存到数组里
					cout << "ENVI" << i + 1 << " 目录：" << my_file0[i] << endl;
					cout <<  "            " << my_file1[i] << endl;
					cout << "文件" << i + 1 << ":" << tempReadName[i] << "\n\n" << endl;
				}
				else {
					cout << "存在命名不一样的ENVI图像，请检查目录，.hdr和.img必须命名一致，不识别未命名一致的ENVI图像";
				}

			}

		}

	}

	cout << "\n" << "该目录有下有" << my_file0.size() << "个符合要求的ENVI图像"  << "\n" << endl;
	cout << "\n" << "......读取完成！......" << "\n" << endl;
	flagRead = flagRead - 1;
}

void EnviToTiff::analyseEnvi(string hdrPath, string imgPath) {

	flagAnalyse = 1;

	analyseHdr(hdrPath);

	_IMGINFO.imageLength = _HDR.lines;//rows , type4
	_IMGINFO.imageWidth = _HDR.samples;//cols, type4
	_IMGINFO.samplesPerPixel = _HDR.bands;//channel, type3
	_IMGINFO.bytePerData = _HDR.dataTybe;//bytes per data 
	_IMGINFO.byteDataSum = _IMGINFO.imageLength * _IMGINFO.imageWidth * _IMGINFO.samplesPerPixel * _IMGINFO.bytePerData;


	//以下赋值按照内存存放的顺序
	//endian --> magic --> ifdoffset --> bitpersample --> stripoffset --> data --> stripbytecount --> x --> y --> sampleformat --> DE_COUNT(2) --> DE --> nextifdoffset(0000)


	_IFH.endian = littleEndian;//先默认为小端，转换先不写
	_IFH.magic = magic;//魔术位，值为42，别管了，哲学意义

	/*
	我有15个DE，从ifdoffset开始计算偏移量的十进制结果,
	字面意思就是计算从ifdoffset算起到ifdoffset指向的地址之间有多少个字节的数据（不单单指data，还包括索引的长度）
	以下计算是以我的DE为参照
	byteOffset = BitsPerSample + StripOffsets + Data + StripByteCounts + Xresolution + Yresolution + SampleFormat
	*/

	_IFH.ifdOffset = 8 + _IMGINFO.samplesPerPixel * sizeof(type3) + _IMGINFO.imageLength * sizeof(type4) + _IMGINFO.byteDataSum + _IMGINFO.imageLength * sizeof(type4) + 1 * sizeof(type5) + 1 * sizeof(type5) + _IMGINFO.samplesPerPixel * sizeof(type3);//算了IFH的8个字节，这是第一个ifd的偏移量

	
	//数据为short，compression为1，所以bitspersample为16，tiff文档查的  int   1   32
	switch (_IMGINFO.bytePerData) {

	case 2:_IMGINFO.bitsPerSample = 16; _IMGINFO.sampleFormat = 2;break;
	case 4:_IMGINFO.bitsPerSample = 32; _IMGINFO.sampleFormat = 3;break;

	};//通常为2，有符号整型数据的二进制补码    bytePerData, sampleFormat, bitsPerSample三个参数相互影响，3是浮点型IEEE
	
	_IMGINFO.stripOffsets = 8 + _IMGINFO.samplesPerPixel * sizeof(type3) + _IMGINFO.imageLength * sizeof(type4) ;//算法原理同上，不重复了,这是第一个offset的位置

	data = (char*)malloc(sizeof(char) * _IMGINFO.byteDataSum);

	analyseImg(imgPath);

	_IMGINFO.stripByteCounts = _IMGINFO.imageWidth * _IMGINFO.samplesPerPixel * _IMGINFO.bytePerData;//n行，每行字节大小是列数 * 波段数 * 2 ，条带装存储

	_IMGINFO.xResolution = 1;//无单位,暂时想不出它的意思
	_IMGINFO.yResolution = 1;//无单位

	_IFD.deNumber = 15;//我有15个DE

	_IFD.pDE = (DE*)malloc(sizeof(DE) * _IFD.deNumber);//使用结构体指针必须先设为空，再动态分配地址

	int i = 0;

	for (i; i < _IFD.deNumber; i++) {
		_IFD.pDE[i].tag = tagArray[i];
	}


	_IFD.pDE[0].type  = type4;
	_IFD.pDE[1].type  = type4;
	_IFD.pDE[2].type  = type4;
	_IFD.pDE[3].type  = type3;
	_IFD.pDE[4].type  = type3;
	_IFD.pDE[5].type  = type3;
	_IFD.pDE[6].type  = type4;
	_IFD.pDE[7].type  = type3;
	_IFD.pDE[8].type  = type3;
	_IFD.pDE[9].type  = type4;
	_IFD.pDE[10].type = type4;
	_IFD.pDE[11].type = type5;
	_IFD.pDE[12].type = type5;
	_IFD.pDE[13].type = type3;
	_IFD.pDE[14].type = type3;


	_IFD.pDE[0].size = 1;
	_IFD.pDE[1].size = 1;
	_IFD.pDE[2].size = 1;
	_IFD.pDE[3].size = _IMGINFO.samplesPerPixel;
	_IFD.pDE[4].size = 1;
	_IFD.pDE[5].size = 1;
	_IFD.pDE[6].size = _IMGINFO.imageLength;
	_IFD.pDE[7].size = 1;
	_IFD.pDE[8].size = 1;
	_IFD.pDE[9].size = 1;
	_IFD.pDE[10].size = _IMGINFO.imageLength;
	_IFD.pDE[11].size = 1;
	_IFD.pDE[12].size = 1;
	_IFD.pDE[13].size = 1;
	_IFD.pDE[14].size = _IMGINFO.samplesPerPixel;

	_IFD.pDE[0].valOffset = 0;
	_IFD.pDE[1].valOffset = _IMGINFO.imageWidth;
	_IFD.pDE[2].valOffset = _IMGINFO.imageLength;
	_IFD.pDE[3].valOffset = 8;////8是偏移地址，bitpersample的偏移地址 ,以下////都是偏移量
	_IFD.pDE[4].valOffset = 1;
	_IFD.pDE[5].valOffset = 2;
	_IFD.pDE[6].valOffset = 8 + _IMGINFO.samplesPerPixel * sizeof(type3);////
	_IFD.pDE[7].valOffset = 1;
	_IFD.pDE[8].valOffset = _IMGINFO.samplesPerPixel;
	_IFD.pDE[9].valOffset = 1;
	_IFD.pDE[10].valOffset = 8 + _IMGINFO.samplesPerPixel * sizeof(type3) + _IMGINFO.imageLength * sizeof(type4) + _IMGINFO.byteDataSum;////
	_IFD.pDE[11].valOffset = 8 + _IMGINFO.samplesPerPixel * sizeof(type3) + _IMGINFO.imageLength * sizeof(type4) + _IMGINFO.byteDataSum + _IMGINFO.imageLength * sizeof(type4);////
	_IFD.pDE[12].valOffset = 8 + _IMGINFO.samplesPerPixel * sizeof(type3) + _IMGINFO.imageLength * sizeof(type4) + _IMGINFO.byteDataSum + _IMGINFO.imageLength * sizeof(type4) + 1 * sizeof(type5);////
	_IFD.pDE[13].valOffset = 1;
	_IFD.pDE[14].valOffset = 8 + _IMGINFO.samplesPerPixel * sizeof(type3) + _IMGINFO.imageLength * sizeof(type4) + _IMGINFO.byteDataSum + _IMGINFO.imageLength * sizeof(type4) + 1 * sizeof(type5) + 1 * sizeof(type5);////

	_IFD.nextIfdOffset = 0;

	flagAnalyse = flagAnalyse - 1;
}
void EnviToTiff::analyseHdr(string hdrPath) {

	ifstream frd(hdrPath, ios::in | ios::binary);
	if (frd) {
		char temp[20];//按照目前我的.hdr看  20个字节长度是够的

		frd.getline(temp, 20, '=');
		frd.getline(temp, 20, ' ');
		frd.getline(temp, 20, '\n');//samples
		_HDR.samples = atoi(temp);

		frd.getline(temp, 20, '=');
		frd.getline(temp, 20, ' ');
		frd.getline(temp, 20, '\n');//lines
		_HDR.lines = atoi(temp);

		frd.getline(temp, 20, '=');
		frd.getline(temp, 20, ' ');
		frd.getline(temp, 20, '\n');//bands
		_HDR.bands = atoi(temp);

		frd.getline(temp, 20, '=');
		frd.getline(temp, 20, ' ');
		frd.getline(temp, 20, '\n');//file type
		_HDR.fileType = temp;

		frd.getline(temp, 20, '=');
		frd.getline(temp, 20, ' ');
		frd.getline(temp, 20, '\n');//data type
		_HDR.dataTybe = atoi(temp);

		frd.getline(temp, 20, '=');
		frd.getline(temp, 20, ' ');
		frd.getline(temp, 20, '\n');//interleave
		_HDR.interleave = temp;

		frd.getline(temp, 20, '=');
		frd.getline(temp, 20, ' ');
		frd.getline(temp, 20, '\n');//sensor type
		_HDR.sensorType = temp;

		frd.getline(temp, 20, '=');
		frd.getline(temp, 20, ' ');
		frd.getline(temp, 20, '\n');//wavelength units
		_HDR.waveLengthUnits = temp;

		frd.getline(temp, 20, '\n');


		for (int i = 0; i < _HDR.bands;i++) {
			frd.getline(temp, 20, '\n');
			_HDR.waveLength.push_back(temp);
		}
		frd.close();

		/*
		cout << _HDR.samples << endl;
		cout << _HDR.lines << endl;
		cout << _HDR.bands << endl;
		cout << _HDR.fileType << endl;
		cout << _HDR.dataTybe << endl;
		cout << _HDR.interleave << endl;
		cout << _HDR.sensorType << endl;
		cout << _HDR.waveLengthUnits << endl;
		*/
		
	}
	else {
		cout << "找不到hdr文件" << endl;
	}
}
void EnviToTiff::analyseImg(string imgPath) {

	//FILE* p;//读
	//char* idata = new char[_IMGINFO.imageWidth * _IMGINFO.imageLength * _IMGINFO.samplesPerPixel * 2];//每次读取一字节，总共有W*L*B*2字节数据（我的数据是两字节）

	//const char* pImgPath = imgPath.c_str();//把地址转换成char[]形式

	//fopen_s(&p, pImgPath, "rb");

	//data = idata;//指针传递

	//fread(idata, 1, _IMGINFO.imageWidth * _IMGINFO.imageLength * _IMGINFO.samplesPerPixel * 2, p);

	//fclose(p);
	char* idata = new char[_IMGINFO.byteDataSum];
	ifstream frd(imgPath, ios::in | ios::binary);
	if (frd) {
		frd.read(idata, _IMGINFO.byteDataSum);
	}
	else {
		cout << "找不到文件" << endl;
	}
	data = idata;//指针传递
	frd.close();

}

void EnviToTiff::saveTiff(string savePath) {

	flagSave = 1;
	
	ofstream fd{ savePath, ios::out | ios::binary };

	if (fd.is_open())
	{
		fd.seekp(0L, ios::beg);//从头开始写，以下按照内存顺序写入

		fd.write((char*)&(_IFH.endian), sizeof(type3));

		fd.write((char*)&(_IFH.magic), sizeof(type3));

		fd.write((char*)&(_IFH.ifdOffset), sizeof(type4));

		for (uint16_t i = 0; i < _IMGINFO.samplesPerPixel; i++) {
			fd.write((char*)&(_IMGINFO.bitsPerSample), sizeof(type3));
		}

		for (uint16_t i = 0; i < _IMGINFO.imageLength; i++) {
			fd.write((char*)&(_IMGINFO.stripOffsets), sizeof(type4));
			_IMGINFO.stripOffsets = _IMGINFO.stripOffsets + _IMGINFO.imageWidth * _IMGINFO.samplesPerPixel * _IMGINFO.bytePerData;//下一个数据条的位置=列数*通道数*字节数
		}

		fd.write(data, _IMGINFO.byteDataSum);//write（）对不起，是我叫的太大声了

		for (uint16_t i = 0; i < _IMGINFO.imageLength ; i++) {
			fd.write((char*)&(_IMGINFO.stripByteCounts), sizeof(type4));
		}
	
		fd.write((char*)& (_IMGINFO.xResolution), sizeof(type4));//注意，DE里面说明xResolution是数据类型是0005（8字节）

		fd.write((char*)&(_IMGINFO.xResolution), sizeof(type4));//采用投机取巧的方法，两个00 00 00 01拼在一起，意思不一定正确，但是至少目前的数据源上这玩意没有什么不同
		
		fd.write((char*)&(_IMGINFO.yResolution), sizeof(type4));//注意，DE里面说明yResolution是数据类型是0005（8字节）

		fd.write((char*)&(_IMGINFO.yResolution), sizeof(type4));//采用投机取巧的方法，两个00 00 00 01拼在一起，意思不一定正确，但是至少目前的数据源上这玩意没有什么不同

		for (uint16_t i = 0; i < _IMGINFO.samplesPerPixel; i++) {
			fd.write((char*)&(_IMGINFO.sampleFormat), sizeof(type3));
		}

		fd.write((char*)&(_IFD.deNumber), sizeof(type3));
	
		for (uint16_t i = 0; i < _IFD.deNumber; i++) {
			fd.write((char*)(&(_IFD.pDE[i].tag)), sizeof(_IFD.pDE->tag));
			fd.write((char*)(&(_IFD.pDE[i].type)), sizeof(_IFD.pDE->type));
			fd.write((char*)(&(_IFD.pDE[i].size)), sizeof(_IFD.pDE->size));
			fd.write((char*)(&(_IFD.pDE[i].valOffset)), sizeof(_IFD.pDE->valOffset));
			
			
		}

		fd.write((char*)(&(_IFD.nextIfdOffset)), sizeof(_IFD.nextIfdOffset));


		fd.close();
		flagSave = flagSave - 1;
	}
}

void EnviToTiff::recursiveCall(string readPath, string savePath, uint32_t count, uint32_t time) {

	count--;

	if (count >= 0)
	{
		recursiveCall(readPath, savePath, count, time);
		if (count != 0) {
			cout << "\n" << "暂停" << time << "秒" << "\n" << endl;
			Sleep(time * 1000);
		}
		mainEnviToTiff(readPath, savePath);
	}
}
void EnviToTiff::mainEnviToTiff(string readPath, string savePath) {

	countMainEnviToTiff++;
	cout << "\n" << "#######################" << "第" << countMainEnviToTiff << "次调用" << "#######################" << "\n" << endl;


	flagMainEnviToTiff = 1;
	if (flagMainEnviToTiff) {
		cout << "..............程序开始.............." << endl;
		cout << "\n" << "------------------------------------" << endl;
		cout << "------------------------------------" << "\n" << endl;
	}
	readEnviFileName(readPath);//先检索一次，得到初始长度

	cout << "\n" << "------------------------------------" << endl;
	cout << "------------------------------------" << "\n" << endl;

	if (!flagRead) {
		cout << "\n" << "..............开始去重.............." << "\n" << endl;
		deweightng_vector_string(readName, tempReadName);

		cout << "\n" << "..............去重结束.............." << "\n" << endl;
		cout << "\n" << "------------------------------------" << endl;
		cout << "------------------------------------" << "\n" << endl;

		if (!flagDeweightng_vector_string == 1) {

			for (int i = 0; i < resReadName.size(); i++) {

				cout << "正在分析第" << i + 1 << "个文件" << "\n" << endl;
				string hdrPath = readPath + resReadName[i] + ".hdr";
				string imgPath = readPath + resReadName[i] + ".img";
				string tiffPath = savePath + resReadName[i] + ".tif";

				analyseEnvi(hdrPath, imgPath);

				if (!flagAnalyse) {
					cout << "\n" << "正在保存第" << i + 1 << "个文件" << "\n" << endl;
					saveTiff(tiffPath);
				}
				
				readName.push_back(resReadName[i]);

				cout << "-------------------" << endl;

			}
		}
	}

	if (!flagSave) {
		tempReadName.clear();
		resReadName.clear();
	}
	flagMainEnviToTiff = flagMainEnviToTiff - 1;

	cout << "\n" << "..............处理完成.............." << "\n" << endl;
	cout << "\n" << "------------------------------------" << endl;
	cout << "------------------------------------" << "\n" << endl;

}
void EnviToTiff::enviToTiff(string path) {

	string processedEnviLogPath = path + "TIFF\\LOG\\SAVE\\processedEnviLog.txt";
	string enviLogPath = path + "ENVI\\LOG\\";

	vector<string> processedEnviLog;
	vector<string> enviLogPathFindLog;

	ifstream processedEnviLogfrd(processedEnviLogPath, ios::in | ios::binary);
	if (processedEnviLogfrd) {
		char temp[30] = {};
		do {
			processedEnviLogfrd.getline(temp, 30, '\n');
			if (strlen(temp) == 0) {
				break;
			}
			processedEnviLog.push_back(temp);
		} while (strlen(temp) != 0);
	}
	processedEnviLogfrd.close();
	get_file(enviLogPath, enviLogPathFindLog, ".txt");
	deweightng_vector_string1(processedEnviLog, enviLogPathFindLog);

	string tiffLogPath = path + "TIFF\\LOG\\" + "t" + logTime() + ".txt";
	ofstream tiffLogFd(tiffLogPath, ios::out | ios::binary);

	cout << "..............TIFF转换开始.............." << endl;
	cout << "\n" << "------------------------------------" << endl;
	cout << "------------------------------------" << "\n" << endl;

	for (int i = 0; i < resEnviLog.size(); i++) {
		string readEnviLogPath = enviLogPath + resEnviLog[i];
		vector<string> pendingEnviName;

		ifstream enviLogFrd(readEnviLogPath, ios::in | ios::binary);
		if (enviLogFrd) {
			char temp[30] = {};
			do {
				enviLogFrd.getline(temp, 30, '\n');
				if (strlen(temp) == 0) {
					break;
				}
				pendingEnviName.push_back(temp);
			} while (strlen(temp) != 0);
		}
		enviLogFrd.close();

		int pendingCount = 0;
		for (int j = 0; j < pendingEnviName.size(); j++) {
			pendingCount++;
			cout << "\n\n" << "正在分析第" << pendingCount << "个文件" << "\n" << endl;

			string hdrPath = path + "ENVI\\" + pendingEnviName[j];
			string imgPath = path + "ENVI\\" + pendingEnviName[j + 1];

			analyseEnvi(hdrPath, imgPath);

			pendingEnviName[j + 1].pop_back();
			pendingEnviName[j + 1].pop_back();
			pendingEnviName[j + 1].pop_back();
			pendingEnviName[j + 1].pop_back();
			tiffLogFd << pendingEnviName[j + 1] + ".tif\n";

			pendingEnviName[j].pop_back();
			pendingEnviName[j].pop_back();
			pendingEnviName[j].pop_back();
			pendingEnviName[j].pop_back();
			string tiffPath = path + "TIFF\\" + pendingEnviName[j] + ".tif";

			cout << "\n" << "正在保存第" << pendingCount << "个文件" << "\n" << endl;
			saveTiff(tiffPath);
			cout << "\n" << "第" << pendingCount << "个文件保存完成" << "\n" << endl;
			cout << "-------------------" << endl;
			j++;
		}
	}
	tiffLogFd.close();

	ofstream processedEnviLogfd(processedEnviLogPath, ios::app | ios::binary);

	for (int n = 0; n < resEnviLog.size(); n++) {
		processedEnviLogfd << resEnviLog[n] + "\n";
	}
	resEnviLog.clear();
	processedEnviLogfd.close();
}

void EnviToTiff::dataProcess(string readPath, string enviClaisPath, string myPath) {

	readEnviFileName(readPath);

	deweightng_vector_string(readName, tempReadName);

	for (int i = 0; i < resReadName.size(); i++) {

		string hdrPath = readPath + "ENVI\\" + resReadName[i] + ".hdr";

		string tiffEnviPath = enviClaisPath + resReadName[i] + ".tif";
		string tiffMyPath = myPath + resReadName[i] + ".tif";

		cout << "这是第" << i << "个文件" << endl;
		cout << "名字是" << resReadName[i] + ".tif" << "\n" << endl;

		analyseHdr(hdrPath);

		ifstream frd0(tiffEnviPath, ios::in | ios::binary);
		ifstream frd1(tiffMyPath, ios::in | ios::binary);

		if (frd0.is_open() && frd1.is_open()) {
			int j = 0;
			int flag0 = 0;
			int flag1 = 0;

			int n = 8 + _HDR.bands * 2 + _HDR.lines * 4 + _HDR.samples * _HDR.lines * _HDR.bands * _HDR.dataTybe + _HDR.lines * 4 + 1 * 8 + 1 * 8 + _HDR.bands * 2 + 2 + 12 * 15 + 4;

			char* idata0 = new char[n];
			char* idata1 = new char[n];
			frd0.read(idata0, n);
			frd1.read(idata1, n);

			for (j;j < n; j++)
			{
				if (idata0[j] != idata1[j])
				{
					cout << j << ":     " << "1" << ":" << idata0[j] << "     2:" << idata1[j] << endl;

					cout << "\n" << "!!!!有问题" << endl;
					flag0++;
				}
				flag1 = flag1++ - flag0;
			}

			if (flag1 == n) {
				cout << "没问题 !!!" << "\n" << endl;
			}
			else {
				cout << "有" << flag0 << "个问题 !!!" << "\n" << endl;
			}
			frd0.close();
			frd1.close();
		}
		else{
			cout << "有文件没打开，输入入径不对" << endl;
		}
	}

	cout << "处理完成！！！！！！！" << endl;
}
void EnviToTiff::deProcess(string txtPath, int DENUMBER) {

	string file_path = txtPath;
	vector<string> my_file0;

	get_need_file(file_path, my_file0, ".txt");

	for (int i = 0; i < my_file0.size(); i++)
	{
		int ps0 = my_file0[i].find_last_of("\\");
		int pe0 = my_file0[i].find_last_of(".");
		string pic_name0 = my_file0[i].substr(ps0 + 1, pe0 - ps0 - 1);

		cout << "txt" << i + 1 << " 目录：" << my_file0[i] << endl;
	
		string txtReadPath = txtPath + pic_name0 + ".txt";
		string txtSavePath = txtPath + pic_name0 + "-副本" + ".txt";

		ifstream frd(txtReadPath, ios::in);
		ofstream fd(txtSavePath, ios::out);
		if (frd.is_open()) {
			

			char temp[100];
			fd << "      ";
			frd.read(temp, 6);
			fd.write(temp, 6);
			fd << "\n";
			
			for (int i = 0; i < DENUMBER; i++)
			{
				fd << i << "    ";
				frd.read(temp, 6);
				fd.write(temp, 6);
				fd << "    ";

				frd.read(temp, 6);
				fd.write(temp, 6);
				fd << "    ";

				frd.read(temp, 12);
				fd.write(temp, 12);
				fd << "    ";

				frd.read(temp, 12);
				fd.write(temp, 12);
				fd << " ";
				fd << "\n";

			}
			fd << "        ";
			frd.read(temp, 6);
			fd.write(temp, 6);

			frd.read(temp, 6);
			fd << "    ";
			fd.write(temp, 6);
		}
		cout << "成功转换" << endl;
		frd.close();
		fd.close();
	}
}