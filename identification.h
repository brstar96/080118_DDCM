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
extern vector<Point2f> dot_clusters_position;//����dot position
extern multiset<int> id[150];



/*

vector<Point2f> dotcenter �������
vector<int> dot_count ������� ��Ӧ��dot ��Ŀ

vector<Point2f> dotcenter_point �� ���Ľ���connect_quadrangle����ͨ�ԣ��� ���ҵ��Ӧ�����ĵ�λ��
vector<int> dotcenter_point_number ����ͨ���� �����Ŀ


*/


void vote(vector<Point2f> dotcenter_point, vector<int> ID) {


	/*for (int i = 0; i < 9; i++) {
	cout << dotcenter_point[i] << "->" << ID[i] << ",  ";
	}
	cout <<"vote!!!!!!!!!!!!!!!!!!!" <<endl;*/

	for (int i = 0; i < 9; i++) {

		vector<Point2f>::iterator it = find(dot_clusters_position.begin(), dot_clusters_position.end(), dotcenter_point[i]);
		if (it == dot_clusters_position.end()) {
			//���û��
			id[it - dot_clusters_position.begin()].insert(ID[i]);
			dot_clusters_position.push_back(dotcenter_point[i]);

		}
		else {
			//�����
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
	vector<int> dotcenter_point_numbers;  // dotcenter_point ���� ��Ӧ�� �����Ŀ
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



