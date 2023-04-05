#include"EnviToTiff.h"



int main() {
	string a = "D:\\ENVI\\TEXT\\";
	string b = "D:\\ENVI\\PROCESS\\";
	//string c = "D:\\ENVI\\DATA1\\10.hdr";

	EnviToTiff IMG;

	IMG.mainEnviToTiff(a , b);

	cout << "!!!!" << endl;
	return 0;
}