#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <map>
#include <set>
#include "ID.h"

using namespace cv;
using namespace std;


vector<Point2f> dotcenter;
vector<int>   dot_count;
map<int, vector<int>> dot_ID;

//voting
extern vector<Point2f> dot_clusters_position;//最后的dot position
extern multiset<int> id[150];



/*

vector<Point2f> dotcenter 点的中心
vector<int> dot_count 点的中心 对应的dot 数目

vector<Point2f> dotcenter_point 是 从四角形connect_quadrangle（连通性）里 查找点对应的中心点位置
vector<int> dotcenter_point_number 在连通性里 点的数目


*/


void vote(vector<Point2f> dotcenter_point, vector<int> ID) {


	/*for (int i = 0; i < 9; i++) {
	cout << dotcenter_point[i] << "->" << ID[i] << ",  ";
	}
	cout <<"vote!!!!!!!!!!!!!!!!!!!" <<endl;*/

	for (int i = 0; i < 9; i++) {

		vector<Point2f>::iterator it = find(dot_clusters_position.begin(), dot_clusters_position.end(), dotcenter_point[i]);
		if (it == dot_clusters_position.end()) {
			//如果没有
			id[it - dot_clusters_position.begin()].insert(ID[i]);
			dot_clusters_position.push_back(dotcenter_point[i]);

		}
		else {
			//如果有
			id[it - dot_clusters_position.begin()].insert(ID[i]);

		}

	}
}



void calculate_ID(int kwin, vector<Point2f> dotcenter_point) {

	define_ID();

	/*cout << "--------------calculate_ID kwin--------------" << endl;
	cout << kwin << endl;*/

	vector<int> ID(9);//"ID" is final ID  & "dotcenter_point" is final dotcenter position
	map<int, vector<int>>::iterator it;
	it = dot_ID.find(kwin);

	if (it != dot_ID.end()) {

		for (int i = 0; i < (*it).second.size(); i++) {

			ID[i] = (*it).second[i];
		}
	}
	else {
		return;

	}

	vote(dotcenter_point, ID);

}

void calculate_kwin(vector<int> dotcenter_point_numbers, vector<Point2f> dotcenter_point) {

	int kwin = 0;
	for (int i = 0; i < dotcenter_point_numbers.size(); i++) {
		int K;
		K = (dotcenter_point_numbers[i] - 1) * pow(4, i);

		kwin = kwin + K;

	}

	/*cout << "--------------kwin------------------" << endl;
	cout << kwin << endl;*/

	calculate_ID(kwin, dotcenter_point);


}


void dotnumber(vector<Point2f> dotcenter_point) {
	vector<int> dotcenter_point_numbers;  // dotcenter_point 坐标 对应的 点的数目
	for (int i = 0; i < dotcenter_point.size(); i++) {

		for (int k = 0; k < dotcenter.size(); k++) {
			if (dotcenter_point[i] == dotcenter[k]) {
				dotcenter_point_numbers.push_back(dot_count[k]);
			}

		}
	}

	//	cout << "-------- dotcenter_point_numbers----------" << endl;
	/*for (int i = 0; i < dotcenter_point_numbers.size(); i++) {
	cout << dotcenter_point_numbers[i] << " ,";

	}*/
	//cout << endl;

	calculate_kwin(dotcenter_point_numbers, dotcenter_point);
}



