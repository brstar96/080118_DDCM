#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <map>
#include "identification.h"

using namespace cv;
using namespace std;


//extern char judge_connected_quadrangle_size;



bool GreaterSort(double a, double b) { return (a > b); }

bool quadrangleSort_Y(Point2f a, Point2f b) { return (a.y > b.y); }
double product(Point2f a, Point2f b) {
	return a.x * b.x + a.y * b.y;

	cout << a.x << endl;
}


void define_A_B_C_D(vector<vector<Point2f>>& final_quadrangle_point) {


	map<int, vector<Point2f>> map_ABCD_point2f;
	vector<Point2f> A, B, C, D; //为了计算lsq 用到的 A,B,C,D
	vector<Point2f> AB, AD, CB, CD, AC, DB; //为了计算 lsq
	vector<Point2f> NAB, NAD, NCB, NCD, NAC, NDB; //normolize AB,AD,CB,CD,AC,DB
	vector<double> Lsq;
	vector<int> Lsq_normal;//int 是第几个四方行
	vector<double> Lsq_org;



	for (int i = 0; i < final_quadrangle_point.size(); i++) {
		A.push_back(final_quadrangle_point[i][0]);
		B.push_back(final_quadrangle_point[i][2]);
		C.push_back(final_quadrangle_point[i][1]);
		D.push_back(final_quadrangle_point[i][3]);
	}

	for (int i = 0; i < final_quadrangle_point.size(); i++) {
		AB.push_back(Point2f(B[i].x - A[i].x, B[i].y - A[i].y));
		AD.push_back(Point2f(D[i].x - A[i].x, D[i].y - A[i].y));
		CB.push_back(Point2f(B[i].x - C[i].x, B[i].y - C[i].y));
		CD.push_back(Point2f(D[i].x - C[i].x, D[i].y - C[i].y));
		AC.push_back(Point2f(C[i].x - A[i].x, C[i].y - A[i].y));
		DB.push_back(Point2f(B[i].x - D[i].x, B[i].y - D[i].y));
	}



	//normolize AB,AD,CB,CD,AC,DB
	for (int i = 0; i < final_quadrangle_point.size(); i++) {
		NAB.push_back(Point2f(AB[i].x / sqrt(AB[i].x * AB[i].x + AB[i].y * AB[i].y), AB[i].y / sqrt(AB[i].x * AB[i].x + AB[i].y * AB[i].y)));
		NAD.push_back(Point2f(AD[i].x / sqrt(AD[i].x * AD[i].x + AD[i].y * AD[i].y), AD[i].y / sqrt(AD[i].x * AD[i].x + AD[i].y * AD[i].y)));
		NCB.push_back(Point2f(CB[i].x / sqrt(CB[i].x * CB[i].x + CB[i].y * CB[i].y), CB[i].y / sqrt(CB[i].x * CB[i].x + CB[i].y * CB[i].y)));
		NCD.push_back(Point2f(CD[i].x / sqrt(CD[i].x * CD[i].x + CD[i].y * CD[i].y), CD[i].y / sqrt(CD[i].x * CD[i].x + CD[i].y * CD[i].y)));
		NAC.push_back(Point2f(AC[i].x / sqrt(AC[i].x * AC[i].x + AC[i].y * AC[i].y), AC[i].y / sqrt(AC[i].x * AC[i].x + AC[i].y * AC[i].y)));
		NDB.push_back(Point2f(DB[i].x / sqrt(DB[i].x * DB[i].x + DB[i].y * DB[i].y), DB[i].y / sqrt(DB[i].x * DB[i].x + DB[i].y * DB[i].y)));


	}

	// for test
	//for (int i = 0; i < final_quadrangle_point.size(); i++) {
	//	cout << NAB[i] << endl;
	//}

	//计算最终的lsq
	for (int i = 0; i < final_quadrangle_point.size(); i++) {
		double p = 1 - product(NAB[i], NAD[i]) * product(NAB[i], NAD[i]) / 3
			- product(NCB[i], NCD[i]) * product(NCB[i], NCD[i]) / 3
			- product(NAC[i], NDB[i]) * product(NAC[i], NDB[i]) / 3;

		Lsq.push_back(p);
	}


	//计算正常的四角形
	//	int normal_lsq = 0;
	for (int i = 0; i < Lsq.size(); i++) {
		if (Lsq[i] > 0.95) {
			Lsq_normal.push_back(i);
		}
	}

	cout << "Lsq_normal:" << Lsq_normal.size() << endl;



	//用visit 来 标记 lsq最大的四角形 已被访问 《0》是被访问 《1》是没有访问
	vector<int>  visit(Lsq_normal.size(), 1);



	for (int t = 0; t < Lsq_normal.size(); t++) {
		stack<int> stack_q;
		vector<vector<int>> connect_quadrangle(35, vector<int>(35, -1));


		if (visit[t] == 1) {
			connect_quadrangle[16][16] = Lsq_normal[t];
			visit[t] = 0;
			stack_q.push(Lsq_normal[t]);
			cout << "stack_top:" << Lsq_normal[t] << endl;
		}
		else {
			continue;
		}


		cout << "这里-----------------------------" << endl;
		//while--------------------------------------------------------------------------------------------
		while (!stack_q.empty()) {
			vector<Point2f> SA, SB, SC, SD;
			vector<Point2f> ABCD_point;
			map<int, vector<Point2f>>::iterator it;
			int stack_push_count = 0;
			int node_top = stack_q.top();
			//sorting Y
			sort(final_quadrangle_point[node_top].begin(), final_quadrangle_point[node_top].end(), quadrangleSort_Y);


			//sorting SA,SB,SC,SD 
			//  cout << "sorting quadrangle point" << endl;
			// for (int i = 0; i < final_quadrangle_point[stack_q.top()].size(); i++) {
			//  cout << final_quadrangle_point[stack_q.top()][i] << endl;
			//   }

			if (final_quadrangle_point[stack_q.top()][0].x > final_quadrangle_point[stack_q.top()][1].x) {
				SC.push_back(final_quadrangle_point[stack_q.top()][0]);
				SD.push_back(final_quadrangle_point[stack_q.top()][1]);
			}
			else {
				SD.push_back(final_quadrangle_point[stack_q.top()][0]);
				SC.push_back(final_quadrangle_point[stack_q.top()][1]);
			}
			if (final_quadrangle_point[stack_q.top()][2].x > final_quadrangle_point[stack_q.top()][3].x) {
				SB.push_back(final_quadrangle_point[stack_q.top()][2]);
				SA.push_back(final_quadrangle_point[stack_q.top()][3]);
			}
			else {
				SA.push_back(final_quadrangle_point[stack_q.top()][2]);
				SB.push_back(final_quadrangle_point[stack_q.top()][3]);
			}


			//把ABCD_point sort完之后 放入 map<int, vector<Point2f>> map_ABCD_point2f里面
			it = map_ABCD_point2f.find(stack_q.top());
			if (it == map_ABCD_point2f.end()) {
				ABCD_point.push_back(SA[0]);
				ABCD_point.push_back(SB[0]);
				ABCD_point.push_back(SC[0]);
				ABCD_point.push_back(SD[0]);
				map_ABCD_point2f.insert(pair<int, vector<Point2f>>(node_top, ABCD_point));
			}


			//查找与 BC共边的四角形------------------------------------------------------------------------------------------------------------------
			vector<int> common_edge_quad_ID1;
			for (int i = 0; i < Lsq_normal.size(); i++) {
				//if (Lsq_org[i] > 0.95) {
				int count = 0;
				if (final_quadrangle_point[Lsq_normal[i]][0] == SB[0] || final_quadrangle_point[Lsq_normal[i]][0] == SC[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][1] == SB[0] || final_quadrangle_point[Lsq_normal[i]][1] == SC[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][2] == SB[0] || final_quadrangle_point[Lsq_normal[i]][2] == SC[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][3] == SB[0] || final_quadrangle_point[Lsq_normal[i]][3] == SC[0]) {
					count++;
				}
				if (count == 2) {
					common_edge_quad_ID1.push_back(Lsq_normal[i]);
				}
				//	}
			}
			// 共边四角形为2个时 判断哪个是未访问的四角形 然后加到stack里面
			int count_i = 0, count_j = 0;

			//查找stack_q.top() 在 二维 connect_quadrangle[][] 的哪个位置

			for (int i = 0; i < connect_quadrangle.size(); i++) {
				for (int j = 0; j < connect_quadrangle[i].size(); j++) {

					if (connect_quadrangle[i][j] == node_top) {
						count_i = i;
						count_j = j;
						break;
					}

				}
			}


			if (common_edge_quad_ID1.size() == 2) {
				vector<int>::iterator Lsq_normal_it1 = find(Lsq_normal.begin(), Lsq_normal.end(), common_edge_quad_ID1[0]);
				if (Lsq_normal.size() != Lsq_normal_it1 - Lsq_normal.begin()) {
					if (visit[Lsq_normal_it1 - Lsq_normal.begin()] == 1) {
						connect_quadrangle[count_i][count_j + 1] = common_edge_quad_ID1[0];
						stack_q.push(common_edge_quad_ID1[0]);
						stack_push_count++;
						visit[Lsq_normal_it1 - Lsq_normal.begin()] = 0;
					}
				}
				vector<int>::iterator Lsq_normal_it2 = find(Lsq_normal.begin(), Lsq_normal.end(), common_edge_quad_ID1[1]);
				if (Lsq_normal.size() != Lsq_normal_it2 - Lsq_normal.begin()) {
					if (visit[Lsq_normal_it2 - Lsq_normal.begin()] == 1) {
						connect_quadrangle[count_i][count_j + 1] = common_edge_quad_ID1[1];
						stack_q.push(common_edge_quad_ID1[1]);
						stack_push_count++;
						visit[Lsq_normal_it2 - Lsq_normal.begin()] = 0;
					}
				}
			}


			//查找与 DC共边的四角形-----------------------------------------------------------------------------------------------------------------
			vector<int> common_edge_quad_ID2;
			for (int i = 0; i < Lsq_normal.size(); i++) {
				//if (Lsq_org[i] > 0.95) {
				int count = 0;
				if (final_quadrangle_point[Lsq_normal[i]][0] == SD[0] || final_quadrangle_point[Lsq_normal[i]][0] == SC[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][1] == SD[0] || final_quadrangle_point[Lsq_normal[i]][1] == SC[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][2] == SD[0] || final_quadrangle_point[Lsq_normal[i]][2] == SC[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][3] == SD[0] || final_quadrangle_point[Lsq_normal[i]][3] == SC[0]) {
					count++;
				}
				if (count == 2) {
					common_edge_quad_ID2.push_back(Lsq_normal[i]);
				}
				//	}
			}
			// 共边四角形为2个时 判断哪个是未访问的四角形 然后加到stack里面
			int count_ii = 0, count_jj = 0;

			//查找stack_q.top() 在 二维 connect_quadrangle[][] 的哪个位置

			for (int i = 0; i < connect_quadrangle.size(); i++) {
				for (int j = 0; j < connect_quadrangle[i].size(); j++) {

					if (connect_quadrangle[i][j] == node_top) {
						count_ii = i;
						count_jj = j;
						break;
					}

				}
			}
			if (common_edge_quad_ID2.size() == 2) {
				vector<int>::iterator Lsq_normal_it3 = find(Lsq_normal.begin(), Lsq_normal.end(), common_edge_quad_ID2[0]);
				if (Lsq_normal.size() != Lsq_normal_it3 - Lsq_normal.begin()) {
					if (visit[Lsq_normal_it3 - Lsq_normal.begin()] == 1) {
						connect_quadrangle[count_ii + 1][count_jj] = common_edge_quad_ID2[0];
						stack_q.push(common_edge_quad_ID2[0]);
						stack_push_count++;
						visit[Lsq_normal_it3 - Lsq_normal.begin()] = 0;
					}
				}
				vector<int>::iterator Lsq_normal_it4 = find(Lsq_normal.begin(), Lsq_normal.end(), common_edge_quad_ID2[1]);
				if (Lsq_normal.size() != Lsq_normal_it4 - Lsq_normal.begin()) {
					if (visit[Lsq_normal_it4 - Lsq_normal.begin()] == 1) {
						connect_quadrangle[count_ii + 1][count_jj] = common_edge_quad_ID2[1];
						stack_q.push(common_edge_quad_ID2[1]);
						stack_push_count++;

						visit[Lsq_normal_it4 - Lsq_normal.begin()] = 0;
					}
				}
			}


			//查找与 DA共边的四角形-----------------------------------------------------------------------------------------------------------------
			vector<int> common_edge_quad_ID3;
			for (int i = 0; i < Lsq_normal.size(); i++) {
				//if (Lsq_org[i] > 0.95) {
				int count = 0;
				if (final_quadrangle_point[Lsq_normal[i]][0] == SD[0] || final_quadrangle_point[Lsq_normal[i]][0] == SA[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][1] == SD[0] || final_quadrangle_point[Lsq_normal[i]][1] == SA[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][2] == SD[0] || final_quadrangle_point[Lsq_normal[i]][2] == SA[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][3] == SD[0] || final_quadrangle_point[Lsq_normal[i]][3] == SA[0]) {
					count++;
				}
				if (count == 2) {
					common_edge_quad_ID3.push_back(Lsq_normal[i]);
				}
				//	}
			}
			// 共边四角形为2个时 判断哪个是未访问的四角形 然后加到stack里面
			int count_iii = 0, count_jjj = 0;

			//查找stack_q.top() 在 二维 connect_quadrangle[][] 的哪个位置

			for (int i = 0; i < connect_quadrangle.size(); i++) {
				for (int j = 0; j < connect_quadrangle[i].size(); j++) {

					if (connect_quadrangle[i][j] == node_top) {
						count_iii = i;
						count_jjj = j;
						break;
					}

				}
			}
			if (common_edge_quad_ID3.size() == 2) {
				vector<int>::iterator Lsq_normal_it5 = find(Lsq_normal.begin(), Lsq_normal.end(), common_edge_quad_ID3[0]);
				if (Lsq_normal.size() != Lsq_normal_it5 - Lsq_normal.begin()) {
					if (visit[Lsq_normal_it5 - Lsq_normal.begin()] == 1) {
						//	connect_quadrangle[count_iii].insert(connect_quadrangle[count_iii].begin() + count_jjj, common_edge_quad_ID3[0]);
						connect_quadrangle[count_iii][count_jjj - 1] = common_edge_quad_ID3[0];
						stack_q.push(common_edge_quad_ID3[0]);
						stack_push_count++;
						//visit[common_edge_quad_ID3[0]] = 0;
						visit[Lsq_normal_it5 - Lsq_normal.begin()] = 0;
					}
				}
				vector<int>::iterator Lsq_normal_it6 = find(Lsq_normal.begin(), Lsq_normal.end(), common_edge_quad_ID3[1]);
				if (Lsq_normal.size() != Lsq_normal_it6 - Lsq_normal.begin()) {
					if (visit[Lsq_normal_it6 - Lsq_normal.begin()] == 1) {
						//	connect_quadrangle[count_iii].insert(connect_quadrangle[count_iii].begin() + count_jjj, common_edge_quad_ID3[1]);
						connect_quadrangle[count_iii][count_jjj - 1] = common_edge_quad_ID3[1];
						stack_q.push(common_edge_quad_ID3[1]);
						stack_push_count++;
						//visit[common_edge_quad_ID3[1]] = 0;
						visit[Lsq_normal_it6 - Lsq_normal.begin()] = 0;
					}
				}
			}

			//查找与 AB共边的四角形----------------------------------------------------------------------------------------------------------------------------
			vector<int> common_edge_quad_ID4;
			for (int i = 0; i < Lsq_normal.size(); i++) {
				//if (Lsq_org[i] > 0.95) {
				int count = 0;
				if (final_quadrangle_point[Lsq_normal[i]][0] == SA[0] || final_quadrangle_point[Lsq_normal[i]][0] == SB[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][1] == SA[0] || final_quadrangle_point[Lsq_normal[i]][1] == SB[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][2] == SA[0] || final_quadrangle_point[Lsq_normal[i]][2] == SB[0]) {
					count++;
				}
				if (final_quadrangle_point[Lsq_normal[i]][3] == SA[0] || final_quadrangle_point[Lsq_normal[i]][3] == SB[0]) {
					count++;
				}
				if (count == 2) {
					common_edge_quad_ID4.push_back(Lsq_normal[i]);
				}
				//	}
			}
			// 共边四角形为2个时 判断哪个是未访问的四角形 然后加到stack里面

			int count_iiii = 0, count_jjjj = 0;

			//查找stack_q.top() 在 二维 connect_quadrangle[][] 的哪个位置

			for (int i = 0; i < connect_quadrangle.size(); i++) {
				for (int j = 0; j < connect_quadrangle[i].size(); j++) {

					if (connect_quadrangle[i][j] == node_top) {
						count_iiii = i;
						count_jjjj = j;
						break;
					}

				}
			}
			if (common_edge_quad_ID4.size() == 2) {
				vector<int>::iterator Lsq_normal_it7 = find(Lsq_normal.begin(), Lsq_normal.end(), common_edge_quad_ID4[0]);
				if (Lsq_normal.size() != Lsq_normal_it7 - Lsq_normal.begin()) {
					if (visit[Lsq_normal_it7 - Lsq_normal.begin()] == 1) {
						connect_quadrangle[count_iiii - 1][count_jjjj] = common_edge_quad_ID4[0];
						stack_q.push(common_edge_quad_ID4[0]);
						stack_push_count++;
						visit[Lsq_normal_it7 - Lsq_normal.begin()] = 0;
					}
				}
				vector<int>::iterator Lsq_normal_it8 = find(Lsq_normal.begin(), Lsq_normal.end(), common_edge_quad_ID4[1]);
				if (Lsq_normal.size() != Lsq_normal_it8 - Lsq_normal.begin()) {
					if (visit[Lsq_normal_it8 - Lsq_normal.begin()] == 1) {
						connect_quadrangle[count_iiii - 1][count_jjjj] = common_edge_quad_ID4[1];
						stack_q.push(common_edge_quad_ID4[1]);
						stack_push_count++;
						visit[Lsq_normal_it8 - Lsq_normal.begin()] = 0;
					}
				}
			}



			if (stack_push_count == 0) {
				stack_q.pop();
			}

			//	cout << "finish------------------ -" << endl;
		}//while



		 // cout ->connect_quadrangle
		 /*for (int i = 0; i < connect_quadrangle.size() ; i++) {
		 for (int k = 0; k < connect_quadrangle[i].size() ; k++) {
		 cout << connect_quadrangle[i][k] << ",";
		 }
		 cout << endl;
		 }*/


		 //按着identification window 循环查找ID
		for (int i = 0; i < connect_quadrangle.size() - 1; i++) {
			for (int j = 0; j < connect_quadrangle.size() - 1; j++) {
				if (connect_quadrangle[i][j] != -1 && connect_quadrangle[i][j + 1] != -1 && connect_quadrangle[i + 1][j] != -1 && connect_quadrangle[i + 1][j + 1] != -1) {
					vector<Point2f> dotcenter_point(9, Point2f(0, 0));
					// vector<int> dotcenter_point_numbers(9, 0);  // dotcenter_point 坐标 对应的 点的数目
					map<int, vector<Point2f>>::iterator it;
					it = map_ABCD_point2f.find(connect_quadrangle[i][j]);
					if (it != map_ABCD_point2f.end()) {
						dotcenter_point[0] = (*it).second[0];
						dotcenter_point[1] = (*it).second[1];
						dotcenter_point[4] = (*it).second[2];
						dotcenter_point[3] = (*it).second[3];
					}
					it = map_ABCD_point2f.find(connect_quadrangle[i][j + 1]);
					if (it != map_ABCD_point2f.end()) {
						dotcenter_point[2] = (*it).second[1];
						dotcenter_point[5] = (*it).second[2];
					}
					it = map_ABCD_point2f.find(connect_quadrangle[i + 1][j]);
					if (it != map_ABCD_point2f.end()) {
						dotcenter_point[6] = (*it).second[3];
						dotcenter_point[7] = (*it).second[2];
					}
					it = map_ABCD_point2f.find(connect_quadrangle[i + 1][j + 1]);
					if (it != map_ABCD_point2f.end()) {
						dotcenter_point[8] = (*it).second[2];
					}

					//cout << "---------dotcenter_point--------------" << endl;

					//for (int j = 0; j < dotcenter_point.size(); j++) {
					//	cout << dotcenter_point[j] << " ,";
					//}
					//cout << endl;

					dotnumber(dotcenter_point);
				}
			}
		}


	}
}
