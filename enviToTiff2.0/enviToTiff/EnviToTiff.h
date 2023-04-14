#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <io.h>
#include <windows.h>
#include <algorithm>

using namespace std;
/*��������Ǵ�С��ת���õ�
#define MC_GET_16(__data__) ((__data__ & 0xFF) << 8) | (__data__ >> 8)
#define MC_GET_64(__data__) ((__data__ & 0xFF) << 24) | ((__data__ & 0xFF00) << 8) | ((__data__ & 0xFF0000) >> 8) | ((__data__ & 0xFF000000) >> 24)
*/
typedef struct
{
	uint16_t endian ; // �ֽ�˳���־λ,ֵΪII����MM:II��ʾС�ֽ���ǰ,�ֳ�Ϊlittle-endian,MM��ʾ���ֽ���ǰ,�ֳ�Ϊbig-endian
	uint16_t magic ;  // TIFF�ı�־λ��һ�㶼��42
	uint32_t ifdOffset ; // ��һ��IFD��ƫ����,����������λ��,����������һ���ֵı߽�,Ҳ����˵������2��������
}IFH;

typedef struct
{
	uint16_t tag ;  // ��TAG��Ψһ��ʶ
	uint16_t type ; // ��������
	uint32_t size ; // ����,ͨ�����ͺ���������ȷ���洢��TAG��������Ҫռ�ݵ��ֽ���
	uint32_t valOffset ;//���ռ�õ��ֽ�������4�� ������ֱ�Ӵ��ڴˡ� �������4�����������ŵ���ָ��ʵ�����ݵ�ָ��
}DE;

typedef struct
{
	uint16_t deNumber; // ��ʾ��IFD�����˶��ٸ�DE,������ĿΪn
	DE* pDE ; // n��DE
	uint32_t nextIfdOffset; // ��һ��IFD��ƫ����,���û������Ϊ0
}IFD;

typedef struct
{
	//�ҵ�ͼ����15�����ԣ����õ���tiff�ĵ��ı�������
	/*���ļ�����:
	  Bit0�������ͼ���Ǵ�TIFF�ļ�����һͼ��Ľ��ͷֱ��ʰ汾����Ϊ1;����λΪ0��
      Bit1�����ͼ���Ƕ�ҳͼ���е�һҳ��λ1Ϊ1:����λΪ0��
      Bit2�����ͼ��Ϊ��TIFF�ļ��е���һ��ͼ������͸�����룬�����2Ϊ1��
      PhotometricInterpretationֵ����Ϊ4����ʾ͸�����롣
      ��Щֵ������Ϊλ��־����Ϊ���Ǳ˴˶�����
      Ĭ��ֵΪ0
	*/

	/*��ע��x��y���ܱ�����Ϊ32λ���£��������ṹ��ĳ�Ա�洢�߼����µ�*/
	//type��Ӧclass�ﶨ���type���ֽ�����de����ÿ�����ݵ����ͣ���ֻ���մ�����ݵ����ִ�С�����ڴ�
	uint16_t subfileType ;//type4, �µ����ļ�����
	uint16_t imageWidth ; // type4, ��ʾͼ���ȣ���
	uint16_t imageLength ;  // type4, ��ʾͼ��߶ȣ���
	uint16_t bitsPerSample ;//type3, ���ز�����,ÿ�����εĲ����ʡ�����TIFF�Ҷ�ͼ�������ֵΪ4��8�����߶�����16��256����ͬ�Ļ�ɫ��Ӱ 
	uint16_t compression ;//type3, ͼ���Ƿ�ѹ��
	uint16_t photometricInterpretation ;//type3, ���غϳ�
	uint32_t stripOffsets ;//type4, ͼ������λ��
	uint16_t orientation ;//type3, ͼ����������еķ���
	uint16_t samplesPerPixel ;//type3, ÿ�����صķּ���,ͨ�����Ա���Ŀ��˵���ǹ�����
	uint16_t rowsPerStrip ;//type4, ÿ�������е�����,���������ݴ洢
	uint32_t stripByteCounts ;//type4, ÿ�������е��ֽ���,ͨ������������������ֵ��������*ͨ����*ÿ�����ݵ��ֽ���
	uint32_t xResolution ;//type5, X�ֱ��ʣ�XResolution(0x011a)/YResolution(0x011b)�ĵ�λ. '1' ��ʾû�е�λ, '2' ��ζ��Ӣ��, '3' ��ʾ����. ȱʡֵ�� '2'(Ӣ��).
	uint32_t yResolution ;//type5, Y�ֱ���
	uint16_t planarConfiguration ;//type3, ÿ�����صĲ�������δ洢��
	uint16_t sampleFormat ;//type3, ���ݵ�����
	
	uint16_t bytePerData;//ÿ�����ݵ��ֽ���
	uint32_t byteDataSum;//�������ֽ���
	
}IMGINFO;

typedef struct
{
	uint16_t samples ;//����
	uint16_t lines ;//����
	uint16_t bands ;//������
	string  fileType ;
	uint16_t  dataTybe ;//�����ֽ���
	string  interleave ;
	string  sensorType ;
	string  waveLengthUnits ;
	vector<string> waveLength;

}HDR;

static vector<string>  readName;//ȫ���ַ������飬���ڴ�Ŵ�����ɵ��ļ����ļ���,vx
vector<string> resEnviLog;
/*
 ���Ի����Ǵ��(big endian:����false)����С��(little endian:����true)

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

	char* data;//ȫ���ַ������飬���ڴ洢���ݿ�
	uint16_t countMainEnviToTiff;//������������

	vector<string> tempReadName;//��ʱ�ַ������飬���ڴ�ŵ�ǰread�����������ļ�����vy
	vector<string> resReadName;//��ʱ�ַ������飬���ڴ��deweightng_vector_string�������ڶ�vx��vyȥ�غ�Ľ��

	//�������е�flag
	bool flagDeweightng_vector_string;
	bool flagRead;
	bool flagAnalyse;
	bool flagSave;
	bool flagMainEnviToTiff;
	
	string logTime();//��ʽ����־ʱ�����

	void recursiveCall(string readPath, string savePath, uint32_t count, uint32_t time);//�ݹ���ã����ü���������mainEnviToTiff��ÿ�μ���೤ʱ��ʵ�ֶ�̬������ֻ����������ɵ�enviͼ��(count,������time������)
	                                                                                   //���ܵķ�������1.���ݿ�  
	void mainEnviToTiff(string readPath, string savePath);//������������һ�Σ�����ǰ�ļ����µ����з���Ҫ���ENVIͼ��
	void enviToTiff(string readPath);//mainEnviToTiff plus�棬����txt��log���������ļ�


	void get_need_file(string path, vector<string>& file, string ext);//https://baijiahao.baidu.com/s?id=1749468573339144199&wfr=spider&for=pc  readEnviFileName�ķ���������ʹ��ʱ��ο�readEnviFileName����

	
	void get_file(string path, vector<string>& file, string ext);//get_need_file plus�棬����ʱ��������������õ��ļ���

	void deweightng_vector_string(vector<string>& file0, vector<string>& file1);//https://blog.csdn.net/yang_yi520/article/details/125527602   ���Ե������ã��Ա�����������ɾ����ͬԪ�أ�������������ֻ֧��˳�����е�����
	void deweightng_vector_string1(vector<string>& file0, vector<string>& file1);//��ʱ�޸�

	void readEnviFileName(string readPath);//http://www.taodudu.cc/news/show-79819.html ����Ŀ¼�·���Ҫ���ENVIͼ����ļ���������һ�������ļ�����������ʱ����tempReadName����������ʱ���������·���������ļ�������һ��Ŀ¼֮�£���������һ��

	void analyseEnvi(string hdrPath, string imgPath);//����Դ�ļ����ݺͲ���,��������ʱ���������·���������ļ�������һ��Ŀ¼֮�£���������һ��
	void analyseHdr(string hdrPath);//�Զ���ȡ.hdr���ļ���Ϣ�����ݱ�����_HDR��,��������ʱ���������·��
	void analyseImg(string imgPath);//��ȡ.img������һ������ָ��

	void saveTiff(string savePath);//���ɵ������ã����Ժ�analyseEnviһ��ʹ�ã�ʹ��˳�������analyseEnvi--saveTiff

	void dataProcess(string hdrPath, string enviClaisPath, string myPath);//���ݷ����ű�
	void deProcess(string txtPath, int DENUMBER);//DE����ű�
	
	//����ָ���DEָ����ʹ��ǰ�������ͷ�
	~EnviToTiff() {
		free(data);
		data = nullptr;
		free(_IFD.pDE);
		_IFD.pDE = nullptr;
	}
private:

	//��������ΪTIFFͷ�ļ� bigEndian = 0x4d4d;
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

	return t;//��ȡ����ʱ��
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
		cout << "\n" << "......��ȡ��......" << "\n" << endl;
	}
	string file_path = readPath + "ENVI\\";
	vector<string> my_file0, my_file1;
	get_need_file(file_path, my_file0, ".hdr");
	get_need_file(file_path, my_file1, ".img");

	if ((my_file0.size() && my_file1.size()) == 0)
	{
		cout << "��Ŀ¼��û��enviͼ���enviͼ������������ͬʱ��.img��.hdr��!" << endl;
	}
	else
	{
		if (my_file0.size() != my_file1.size())
		{
			cout << ".img�����.hdr���������!" << endl;
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
					tempReadName.push_back(pic_name0);//������ļ������ж��ٸ��ļ������ִ浽������
					cout << "ENVI" << i + 1 << " Ŀ¼��" << my_file0[i] << endl;
					cout <<  "            " << my_file1[i] << endl;
					cout << "�ļ�" << i + 1 << ":" << tempReadName[i] << "\n\n" << endl;
				}
				else {
					cout << "����������һ����ENVIͼ������Ŀ¼��.hdr��.img��������һ�£���ʶ��δ����һ�µ�ENVIͼ��";
				}

			}

		}

	}

	cout << "\n" << "��Ŀ¼������" << my_file0.size() << "������Ҫ���ENVIͼ��"  << "\n" << endl;
	cout << "\n" << "......��ȡ��ɣ�......" << "\n" << endl;
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


	//���¸�ֵ�����ڴ��ŵ�˳��
	//endian --> magic --> ifdoffset --> bitpersample --> stripoffset --> data --> stripbytecount --> x --> y --> sampleformat --> DE_COUNT(2) --> DE --> nextifdoffset(0000)


	_IFH.endian = littleEndian;//��Ĭ��ΪС�ˣ�ת���Ȳ�д
	_IFH.magic = magic;//ħ��λ��ֵΪ42������ˣ���ѧ����

	/*
	����15��DE����ifdoffset��ʼ����ƫ������ʮ���ƽ��,
	������˼���Ǽ����ifdoffset����ifdoffsetָ��ĵ�ַ֮���ж��ٸ��ֽڵ����ݣ�������ָdata�������������ĳ��ȣ�
	���¼��������ҵ�DEΪ����
	byteOffset = BitsPerSample + StripOffsets + Data + StripByteCounts + Xresolution + Yresolution + SampleFormat
	*/

	_IFH.ifdOffset = 8 + _IMGINFO.samplesPerPixel * sizeof(type3) + _IMGINFO.imageLength * sizeof(type4) + _IMGINFO.byteDataSum + _IMGINFO.imageLength * sizeof(type4) + 1 * sizeof(type5) + 1 * sizeof(type5) + _IMGINFO.samplesPerPixel * sizeof(type3);//����IFH��8���ֽڣ����ǵ�һ��ifd��ƫ����

	
	//����Ϊshort��compressionΪ1������bitspersampleΪ16��tiff�ĵ����  int   1   32
	switch (_IMGINFO.bytePerData) {

	case 2:_IMGINFO.bitsPerSample = 16; _IMGINFO.sampleFormat = 2;break;
	case 4:_IMGINFO.bitsPerSample = 32; _IMGINFO.sampleFormat = 3;break;

	};//ͨ��Ϊ2���з����������ݵĶ����Ʋ���    bytePerData, sampleFormat, bitsPerSample���������໥Ӱ�죬3�Ǹ�����IEEE
	
	_IMGINFO.stripOffsets = 8 + _IMGINFO.samplesPerPixel * sizeof(type3) + _IMGINFO.imageLength * sizeof(type4) ;//�㷨ԭ��ͬ�ϣ����ظ���,���ǵ�һ��offset��λ��

	data = (char*)malloc(sizeof(char) * _IMGINFO.byteDataSum);

	analyseImg(imgPath);

	_IMGINFO.stripByteCounts = _IMGINFO.imageWidth * _IMGINFO.samplesPerPixel * _IMGINFO.bytePerData;//n�У�ÿ���ֽڴ�С������ * ������ * 2 ������װ�洢

	_IMGINFO.xResolution = 1;//�޵�λ,��ʱ�벻��������˼
	_IMGINFO.yResolution = 1;//�޵�λ

	_IFD.deNumber = 15;//����15��DE

	_IFD.pDE = (DE*)malloc(sizeof(DE) * _IFD.deNumber);//ʹ�ýṹ��ָ���������Ϊ�գ��ٶ�̬�����ַ

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
	_IFD.pDE[3].valOffset = 8;////8��ƫ�Ƶ�ַ��bitpersample��ƫ�Ƶ�ַ ,����////����ƫ����
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
		char temp[20];//����Ŀǰ�ҵ�.hdr��  20���ֽڳ����ǹ���

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
		cout << "�Ҳ���hdr�ļ�" << endl;
	}
}
void EnviToTiff::analyseImg(string imgPath) {

	//FILE* p;//��
	//char* idata = new char[_IMGINFO.imageWidth * _IMGINFO.imageLength * _IMGINFO.samplesPerPixel * 2];//ÿ�ζ�ȡһ�ֽڣ��ܹ���W*L*B*2�ֽ����ݣ��ҵ����������ֽڣ�

	//const char* pImgPath = imgPath.c_str();//�ѵ�ַת����char[]��ʽ

	//fopen_s(&p, pImgPath, "rb");

	//data = idata;//ָ�봫��

	//fread(idata, 1, _IMGINFO.imageWidth * _IMGINFO.imageLength * _IMGINFO.samplesPerPixel * 2, p);

	//fclose(p);
	char* idata = new char[_IMGINFO.byteDataSum];
	ifstream frd(imgPath, ios::in | ios::binary);
	if (frd) {
		frd.read(idata, _IMGINFO.byteDataSum);
	}
	else {
		cout << "�Ҳ����ļ�" << endl;
	}
	data = idata;//ָ�봫��
	frd.close();

}

void EnviToTiff::saveTiff(string savePath) {

	flagSave = 1;
	
	ofstream fd{ savePath, ios::out | ios::binary };

	if (fd.is_open())
	{
		fd.seekp(0L, ios::beg);//��ͷ��ʼд�����°����ڴ�˳��д��

		fd.write((char*)&(_IFH.endian), sizeof(type3));

		fd.write((char*)&(_IFH.magic), sizeof(type3));

		fd.write((char*)&(_IFH.ifdOffset), sizeof(type4));

		for (uint16_t i = 0; i < _IMGINFO.samplesPerPixel; i++) {
			fd.write((char*)&(_IMGINFO.bitsPerSample), sizeof(type3));
		}

		for (uint16_t i = 0; i < _IMGINFO.imageLength; i++) {
			fd.write((char*)&(_IMGINFO.stripOffsets), sizeof(type4));
			_IMGINFO.stripOffsets = _IMGINFO.stripOffsets + _IMGINFO.imageWidth * _IMGINFO.samplesPerPixel * _IMGINFO.bytePerData;//��һ����������λ��=����*ͨ����*�ֽ���
		}

		fd.write(data, _IMGINFO.byteDataSum);//write�����Բ������ҽе�̫������

		for (uint16_t i = 0; i < _IMGINFO.imageLength ; i++) {
			fd.write((char*)&(_IMGINFO.stripByteCounts), sizeof(type4));
		}
	
		fd.write((char*)& (_IMGINFO.xResolution), sizeof(type4));//ע�⣬DE����˵��xResolution������������0005��8�ֽڣ�

		fd.write((char*)&(_IMGINFO.xResolution), sizeof(type4));//����Ͷ��ȡ�ɵķ���������00 00 00 01ƴ��һ����˼��һ����ȷ����������Ŀǰ������Դ��������û��ʲô��ͬ
		
		fd.write((char*)&(_IMGINFO.yResolution), sizeof(type4));//ע�⣬DE����˵��yResolution������������0005��8�ֽڣ�

		fd.write((char*)&(_IMGINFO.yResolution), sizeof(type4));//����Ͷ��ȡ�ɵķ���������00 00 00 01ƴ��һ����˼��һ����ȷ����������Ŀǰ������Դ��������û��ʲô��ͬ

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
			cout << "\n" << "��ͣ" << time << "��" << "\n" << endl;
			Sleep(time * 1000);
		}
		mainEnviToTiff(readPath, savePath);
	}
}
void EnviToTiff::mainEnviToTiff(string readPath, string savePath) {

	countMainEnviToTiff++;
	cout << "\n" << "#######################" << "��" << countMainEnviToTiff << "�ε���" << "#######################" << "\n" << endl;


	flagMainEnviToTiff = 1;
	if (flagMainEnviToTiff) {
		cout << "..............����ʼ.............." << endl;
		cout << "\n" << "------------------------------------" << endl;
		cout << "------------------------------------" << "\n" << endl;
	}
	readEnviFileName(readPath);//�ȼ���һ�Σ��õ���ʼ����

	cout << "\n" << "------------------------------------" << endl;
	cout << "------------------------------------" << "\n" << endl;

	if (!flagRead) {
		cout << "\n" << "..............��ʼȥ��.............." << "\n" << endl;
		deweightng_vector_string(readName, tempReadName);

		cout << "\n" << "..............ȥ�ؽ���.............." << "\n" << endl;
		cout << "\n" << "------------------------------------" << endl;
		cout << "------------------------------------" << "\n" << endl;

		if (!flagDeweightng_vector_string == 1) {

			for (int i = 0; i < resReadName.size(); i++) {

				cout << "���ڷ�����" << i + 1 << "���ļ�" << "\n" << endl;
				string hdrPath = readPath + resReadName[i] + ".hdr";
				string imgPath = readPath + resReadName[i] + ".img";
				string tiffPath = savePath + resReadName[i] + ".tif";

				analyseEnvi(hdrPath, imgPath);

				if (!flagAnalyse) {
					cout << "\n" << "���ڱ����" << i + 1 << "���ļ�" << "\n" << endl;
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

	cout << "\n" << "..............�������.............." << "\n" << endl;
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

	cout << "..............TIFFת����ʼ.............." << endl;
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
			cout << "\n\n" << "���ڷ�����" << pendingCount << "���ļ�" << "\n" << endl;

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

			cout << "\n" << "���ڱ����" << pendingCount << "���ļ�" << "\n" << endl;
			saveTiff(tiffPath);
			cout << "\n" << "��" << pendingCount << "���ļ��������" << "\n" << endl;
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

		cout << "���ǵ�" << i << "���ļ�" << endl;
		cout << "������" << resReadName[i] + ".tif" << "\n" << endl;

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

					cout << "\n" << "!!!!������" << endl;
					flag0++;
				}
				flag1 = flag1++ - flag0;
			}

			if (flag1 == n) {
				cout << "û���� !!!" << "\n" << endl;
			}
			else {
				cout << "��" << flag0 << "������ !!!" << "\n" << endl;
			}
			frd0.close();
			frd1.close();
		}
		else{
			cout << "���ļ�û�򿪣������뾶����" << endl;
		}
	}

	cout << "������ɣ�������������" << endl;
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

		cout << "txt" << i + 1 << " Ŀ¼��" << my_file0[i] << endl;
	
		string txtReadPath = txtPath + pic_name0 + ".txt";
		string txtSavePath = txtPath + pic_name0 + "-����" + ".txt";

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
		cout << "�ɹ�ת��" << endl;
		frd.close();
		fd.close();
	}
}