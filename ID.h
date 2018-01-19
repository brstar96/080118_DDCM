#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <map>

using namespace cv;
using namespace std;

extern map<int, vector<int>> dot_ID;

void define_ID() {

	//在二维放入ID
	int number = 1;
	vector<vector<int>> dot(8, vector < int>(12));
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 12; j++) {

			dot[i][j] = number;
			number++;

		}
	}
	//在二维放入dot_number
	vector<vector<int>> dot_number(8);
	dot_number[0] = { 2,1,2,3,1,2,4,1,2,3,2,1 };
	dot_number[1] = { 1,2,3,1,3,1,2,4,3,4,1,3 };
	dot_number[2] = { 4,1,4,3,4,3,4,1,4,1,3,4 };
	dot_number[3] = { 1,4,1,4,2,4,3,2,3,2,1,3 };
	dot_number[4] = { 3,2,3,2,1,3,1,4,2,3,4,2 };
	dot_number[5] = { 2,1,4,1,4,1,4,1,4,1,3,4 };
	dot_number[6] = { 4,3,1,3,2,4,3,2,1,2,1,3 };
	dot_number[7] = { 1,4,2,1,3,1,2,3,2,4,2,4 };

	vector<int> kwin1;
	vector<vector<int>> id1(60);
	vector<int> kwin2;
	vector<vector<int>> id2(60);
	vector<int> kwin3;
	vector<vector<int>> id3(60);
	vector<int> kwin4;
	vector<vector<int>> id4(60);

	int window = 1;
	for (int i = 0; i < dot.size() - 2; i++) {
		for (int j = 0; j < dot[i].size() - 2; j++) {
			//	cout << "window:" << window << endl;
			window++;
			int kwin_1 = 0;
			int kwin_2 = 0;
			int kwin_3 = 0;
			int kwin_4 = 0;

			//1
			/*	cout << dot[i][j] << "," << dot[i][j + 1] << "," << dot[i][j + 2] << ","
			<< dot[i + 1][j] << "," << dot[i + 1][j + 1] << "," << dot[i + 1][j + 2] << ","
			<< dot[i + 2][j] << "," << dot[i + 2][j + 1] << "," << dot[i + 2][j + 2] << endl;*/
			vector<int > dot_number_1(9);
			dot_number_1 = { dot_number[i][j], dot_number[i][j + 1], dot_number[i][j + 2],
				dot_number[i + 1][j] , dot_number[i + 1][j + 1] ,dot_number[i + 1][j + 2] ,
				dot_number[i + 2][j] , dot_number[i + 2][j + 1] , dot_number[i + 2][j + 2] };
			for (int i = 0; i < dot_number_1.size(); i++) {
				int K;
				K = (dot_number_1[i] - 1) * pow(4, i);

				kwin_1 = kwin_1 + K;
			}
			//cout << "kwin_1:" << kwin_1 << endl;
			kwin1.push_back(kwin_1);
			id1[window - 2].push_back(dot[i][j]); id1[window - 2].push_back(dot[i][j + 1]); id1[window - 2].push_back(dot[i][j + 2]);
			id1[window - 2].push_back(dot[i + 1][j]); id1[window - 2].push_back(dot[i + 1][j + 1]); id1[window - 2].push_back(dot[i + 1][j + 2]);
			id1[window - 2].push_back(dot[i + 2][j]); id1[window - 2].push_back(dot[i + 2][j + 1]); id1[window - 2].push_back(dot[i + 2][j + 2]);


			//2
			/*cout << dot[i][j + 2] << "," << dot[i + 1][j + 2] << "," << dot[i + 2][j + 2] << ","
			<< dot[i][j + 1] << "," << dot[i + 1][j + 1] << "," << dot[i + 2][j + 1] << ","
			<< dot[i][j] << "," << dot[i + 1][j] << "," << dot[i + 2][j] << endl;*/
			vector<int > dot_number_2(9);
			dot_number_2 = { dot_number[i][j + 2] ,dot_number[i + 1][j + 2] , dot_number[i + 2][j + 2] ,
				dot_number[i][j + 1] , dot_number[i + 1][j + 1] , dot_number[i + 2][j + 1] ,
				dot_number[i][j], dot_number[i + 1][j], dot_number[i + 2][j] };
			for (int i = 0; i < dot_number_2.size(); i++) {
				int K;
				K = (dot_number_2[i] - 1) * pow(4, i);

				kwin_2 = kwin_2 + K;

			}
			//	cout << "kwin_2:" << kwin_2 << endl;
			kwin2.push_back(kwin_2);
			id2[window - 2].push_back(dot[i][j + 2]); id2[window - 2].push_back(dot[i + 1][j + 2]); id2[window - 2].push_back(dot[i + 2][j + 2]);
			id2[window - 2].push_back(dot[i][j + 1]); id2[window - 2].push_back(dot[i + 1][j + 1]); id2[window - 2].push_back(dot[i + 2][j + 1]);
			id2[window - 2].push_back(dot[i][j]); id2[window - 2].push_back(dot[i + 1][j]); id2[window - 2].push_back(dot[i + 2][j]);


			//3
			/*	cout << dot[i + 2][j + 2] << "," << dot[i + 2][j + 1] << "," << dot[i + 2][j] << ","
			<< dot[i + 1][j + 2] << "," << dot[i + 1][j + 1] << "," << dot[i + 1][j] << ","
			<< dot[i][j + 2] << "," << dot[i][j + 1] << "," << dot[i][j] << endl;*/
			vector<int > dot_number_3(9);
			dot_number_3 = { dot_number[i + 2][j + 2], dot_number[i + 2][j + 1] , dot_number[i + 2][j],
				dot_number[i + 1][j + 2], dot_number[i + 1][j + 1] , dot_number[i + 1][j] ,
				dot_number[i][j + 2] , dot_number[i][j + 1], dot_number[i][j] };
			for (int i = 0; i < dot_number_3.size(); i++) {
				int K;
				K = (dot_number_3[i] - 1) * pow(4, i);

				kwin_3 = kwin_3 + K;

			}
			//	cout << "kwin_3:" << kwin_3 << endl;
			kwin3.push_back(kwin_3);
			id3[window - 2].push_back(dot[i + 2][j + 2]); id3[window - 2].push_back(dot[i + 2][j + 1]); id3[window - 2].push_back(dot[i + 2][j]);
			id3[window - 2].push_back(dot[i + 1][j + 2]); id3[window - 2].push_back(dot[i + 1][j + 1]); id3[window - 2].push_back(dot[i + 1][j]);
			id3[window - 2].push_back(dot[i][j + 2]); id3[window - 2].push_back(dot[i][j + 1]); id3[window - 2].push_back(dot[i][j]);




			//4
			/*cout << dot[i + 2][j] << "," << dot[i + 1][j] << "," << dot[i][j] << ","
			<< dot[i + 2][j + 1] << "," << dot[i + 1][j + 1] << "," << dot[i][j + 1] << ","
			<< dot[i + 2][j + 2] << "," << dot[i + 1][j + 2] << "," << dot[i][j + 2] << endl;*/
			vector<int > dot_number_4(9);
			dot_number_4 = { dot_number[i + 2][j] , dot_number[i + 1][j] , dot_number[i][j] ,
				dot_number[i + 2][j + 1] , dot_number[i + 1][j + 1], dot_number[i][j + 1] ,
				dot_number[i + 2][j + 2], dot_number[i + 1][j + 2] , dot_number[i][j + 2] };
			for (int i = 0; i < dot_number_4.size(); i++) {
				int K;
				K = (dot_number_4[i] - 1) * pow(4, i);

				kwin_4 = kwin_4 + K;
			}
			//	cout << "kwin_4:" << kwin_4 << endl;
			kwin4.push_back(kwin_4);
			id4[window - 2].push_back(dot[i + 2][j]); id4[window - 2].push_back(dot[i + 1][j]); id4[window - 2].push_back(dot[i][j]);
			id4[window - 2].push_back(dot[i + 2][j + 1]); id4[window - 2].push_back(dot[i + 1][j + 1]); id4[window - 2].push_back(dot[i][j + 1]);
			id4[window - 2].push_back(dot[i + 2][j + 2]); id4[window - 2].push_back(dot[i + 1][j + 2]); id4[window - 2].push_back(dot[i][j + 2]);
			//cout << endl;
		}
		//cout << "-------------------------------------------------------------" << endl;
	}

	for (int i = 0; i < 60; i++) {
		dot_ID.insert(pair<int, vector<int>>(kwin1[i], id1[i]));
		dot_ID.insert(pair<int, vector<int>>(kwin2[i], id2[i]));
		dot_ID.insert(pair<int, vector<int>>(kwin3[i], id3[i]));
		dot_ID.insert(pair<int, vector<int>>(kwin4[i], id4[i]));

	}




}