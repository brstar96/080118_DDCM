#include <Windows.h> 
#include <math.h> 


#include "include\GL\gl.h"
#include "include\GL\glu.h"
#include "include\GL\glut.h"
#include "include\GL\freeglut.h"
#include "bmp.h"
#include <algorithm>
#include <iterator>
#include <sstream>

#include <opencv\highgui.h>
#include <opencv2\core\core.hpp> 
#include <opencv2\highgui\highgui.hpp> 
#include <opencv2\opencv.hpp> 
#include <vector> 
#include <iostream> 
#include "connectivity.h"
#include <time.h>



#define MAX_NO_TEXTURES 1 
#define RowDotCount 12;
#define ColsDotCount 8;
GLuint texture[MAX_NO_TEXTURES];

using namespace cv;
using namespace std;
using namespace cv::ml;

int thresh = 50;
RNG rng(12345);
extern vector<Point2f> dotcenter;
extern vector<int>   dot_count;

vector<Point2f> dot_clusters_position;//最后的dot position
multiset<int> id[150];
vector<int> final_id;//最后的dot id

					 //-----------------------------h
Size sampleSize(160, 160);//样本的大小
int train_samples = 16;
int classes = 4;

Mat trainData;
Mat trainClasses;

//申明全局函数
Mat readImageSaveContour(Mat src);
void getData();

//--------------------



struct point
{
	int x, y;
};
double g_dConArea;






//---------------------------------OpenGL Render-------------------------------------


class DDCM_Values {
public:
	GLfloat VBO_DotClusterCoordData;
	GLfloat VBO_PlaneMeshUV;
	GLint PlaneMeshTexture;

	int w = 1920, h = 1080; //Horizontal & Vertical Window Size
};

AUX_RGBImageRec *LoadBMP(char *Filename)    // Loads A Bitmap Image
{
	FILE *File = NULL;         // File Handle
	if (!Filename)          // Make Sure A Filename Was Given
	{
		return NULL;         // If Not Return NULL
	}
	File = fopen(Filename, "r");       // Check To See If The File Exists
	if (File)           // Does The File Exist?
	{
		fclose(File);         // Close The Handle
		return auxDIBImageLoad(Filename);    // Load The Bitmap And Return A Pointer
	}
	return NULL;          // If Load Failed Return NULL
}


void reshape(int width, int height) {

	DDCM_Values data;

	if (height == 0) //Prevent dividing by 0 when window is very small
		height = 1;
	double ratio = 1.0 * width / height;


	glMatrixMode(GL_PROJECTION);//Initialize coordinate system before modifying
	glLoadIdentity();
	//glViewport(-width/2, height/2, data.w, data.h); //Set the viewport to the full size of the window. (Lower left x, lower left y, upper right x, upper right y);
	glViewport(0, 0, width, height); //Set the viewport to the full size of the window. (Lower left x, lower left y, upper right x, upper right y);

									 //gluPerspective(45.0f, ratio, 1.0f, 1000.0f); //default value
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f); //default fovy value is 45.0f

	gluLookAt(0.0f, 0.0f, 900.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f); //The first three values are the OpenGL camera position value(default value)
																		//gluLookAt(30.0f, 0.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f); //The first three values are the OpenGL camera position value
}


bool FindID(vector<int> final_id, int CurrentIDNum) {

	bool found = FALSE;

	//Searching N-th final_id vector
	//then if there is no N-th final_id vector, return FALSE.

	for (int IDIndexNumber = 0; IDIndexNumber < final_id.size(); IDIndexNumber++) {
		if (find(final_id.begin(), final_id.end(), CurrentIDNum) != final_id.end()) {
			found = TRUE;
		}
		else {
			found = FALSE;
		}
	}
	return found;
}

/*
//-------------------test code-------------------------------------------------
cout << "test0" << endl;
for (int i = 0; i < dot_clusters_position.size(); i++) {
cout << dot_clusters_position[i] << "  -> " << final_id[i] << "      ";
}
//-------------------test code-------------------------------------------------
*/



void display() {

	DDCM_Values value;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear scene
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[0]); //texture binding

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	//int scaleconstX = 3, scaleconstY = 2;
	//glScalef(3 / 2 * scaleconstX, -3 / 2 * scaleconstY, 2);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(180, 0, 1, 0); //Y绵 扁霖栏肺 180档 雀傈
	glTranslatef(-(value.w / 6), -(value.h / 6), 1);


	int _RowDotCount = RowDotCount - 1;
	int _ColsDotCount = ColsDotCount - 3;
	int Desc_TexRowVertCount = RowDotCount - 1;
	int Desc_TexColsVertCount = ColsDotCount - 1;
	int InitNum = 96, DescNum = 12;


	for (int Row = 0; Row < 7; Row++) {
		int _DescNum = DescNum*Row;
		cout << "Descnum捞" << _DescNum << "栏肺 函版凳" << endl;
		int CurrentRowIDInitNum = InitNum - _DescNum; //95-12....
		int CurrentIDNumber = CurrentRowIDInitNum; //2锅掳 for巩 倒府扁 傈俊 檬扁拳


		for (int QuadCount = 0; QuadCount < 11; QuadCount++) {


			glBegin(GL_QUADS);
			if (FindID(final_id, CurrentIDNumber) == true) {

				//if RefVec_IDwithDCPosition function's return value is 'TRUE', draw 1st Quad
				bool found = FindID(final_id, CurrentIDNumber);


				glTexCoord2f(0.0f, 1.0f);
				//glVertex2f(dot_clusters_position[CurrentIDNumber].x, dot_clusters_position[CurrentIDNumber].y);
				vector<int>::iterator map1 = find(final_id.begin(), final_id.end(), CurrentIDNumber);

				glVertex2f(dot_clusters_position[map1 - final_id.begin()].x, dot_clusters_position[map1 - final_id.begin()].y);
				cout << endl;
				cout << "-------------------------------------" << endl;
				cout << "≮CurrentRowIDInitNum : " << CurrentRowIDInitNum << endl;
				cout << "dot_clusters_position苞 final_id啊 96锅老锭 : " << dot_clusters_position[96] << "  -> " << final_id[96] << endl;
				cout << "dot_clusters_position苞 final_id啊 95锅老锭 : " << dot_clusters_position[95] << "  -> " << final_id[95] << endl;
				cout << "dot_clusters_position苞 final_id啊 94锅老锭 : " << dot_clusters_position[94] << "  -> " << final_id[94] << endl;
				cout << "dot_clusters_position苞 final_id啊 0锅老锭 : " << dot_clusters_position[0] << "  -> " << final_id[0] << endl;
				cout << endl;
				cout << "∝Vertex1 Current CurrentIDNumber: " << CurrentIDNumber << endl;
				cout << "Vertex1 Current DotClsutersPosition Index: " << CurrentIDNumber - 1 << endl;
				cout << "test1 DotPosition (x,y) : " << dot_clusters_position[map1 - final_id.begin()].x << ", " << dot_clusters_position[map1 - final_id.begin()].y << endl;
				if (found == true) {
					cout << "Vertex1 FindID : TRUE" << endl;
				}
				else {
					cout << "FALSE" << endl;
				}
			}
			if (FindID(final_id, CurrentIDNumber - 12) == true) { //if RefVec_IDwithDCPosition function's return value is 'TRUE', draw 2nd Quad
				bool found = FindID(final_id, CurrentIDNumber - 12);


				glTexCoord2f(0.0f, 0.0f);
				//	glVertex2f(dot_clusters_position[CurrentIDNumber - 12].x, dot_clusters_position[CurrentIDNumber - 12].y);
				vector<int>::iterator map2 = find(final_id.begin(), final_id.end(), CurrentIDNumber - 12);

				glVertex2f(dot_clusters_position[map2 - final_id.begin()].x, dot_clusters_position[map2 - final_id.begin()].y);
				cout << "∝Vertex2 Current CurrentIDNumber: " << CurrentIDNumber - 12 << endl;
				cout << "Vertex2 Current DotClsutersPosition Index: " << CurrentIDNumber - 13 << endl;
				cout << "test2 DotPosition (x,y) : " << dot_clusters_position[map2 - final_id.begin()].x << ", " << dot_clusters_position[map2 - final_id.begin()].y << endl;
				if (found == true) {
					cout << "Vertex2 FindID : TRUE" << endl;
				}
				else {
					cout << "FALSE" << endl;
				}
			}
			if (FindID(final_id, CurrentIDNumber - 13) == true) { //if RefVec_IDwithDCPosition function's return value is 'TRUE', draw 3rd Quad
				bool found = FindID(final_id, CurrentIDNumber - 13);


				glTexCoord2f(1.0f, 0.0f);
				//glVertex2f(dot_clusters_position[CurrentIDNumber - 13].x, dot_clusters_position[CurrentIDNumber - 13].y);
				vector<int>::iterator map3 = find(final_id.begin(), final_id.end(), CurrentIDNumber - 13);

				glVertex2f(dot_clusters_position[map3 - final_id.begin()].x, dot_clusters_position[map3 - final_id.begin()].y);
				cout << "∝Vertex3 Current CurrentIDNumber: " << CurrentIDNumber - 13 << endl;
				cout << "Vertex3 Current DotClsutersPosition Index: " << CurrentIDNumber - 14 << endl;
				cout << "test3 DotPosition (x,y) : " << dot_clusters_position[map3 - final_id.begin()].x << ", " << dot_clusters_position[map3 - final_id.begin()].y << endl;
				if (found == true) {
					cout << "Vertex3 FindID : TRUE" << endl;
				}
				else {
					cout << "FALSE" << endl;
				}
			}
			if (FindID(final_id, CurrentIDNumber - 1) == true) { //if RefVec_IDwithDCPosition function's return value is 'TRUE', draw 4th Quad
				bool found = FindID(final_id, CurrentIDNumber - 1);


				glTexCoord2f(1.0f, 1.0f);
				//glVertex2f(dot_clusters_position[CurrentIDNumber - 1].x, dot_clusters_position[CurrentIDNumber - 1].y);
				vector<int>::iterator map4 = find(final_id.begin(), final_id.end(), CurrentIDNumber - 1);

				glVertex2f(dot_clusters_position[map4 - final_id.begin()].x, dot_clusters_position[map4 - final_id.begin()].y);
				cout << "∝Vertex4 Current CurrentIDNumber: " << CurrentIDNumber - 1 << endl;
				cout << "Vertex4 Current DotClsutersPosition Index: " << CurrentIDNumber - 2 << endl;
				cout << "test4 DotPosition (x,y) : " << dot_clusters_position[map4 - final_id.begin()].x << ", " << dot_clusters_position[map4 - final_id.begin()].y << endl;
				if (found == true) {
					cout << "Vertex4 FindID : TRUE" << endl;
				}
				else {
					cout << "FALSE" << endl;
				}
				cout << endl;
				cout << "泅犁 DescNum : " << _DescNum << endl;
				cout << "current QuadCount : " << QuadCount << endl;
				cout << "current Row : " << Row << endl;


			}
			glEnd();
			//CurrentIDInitNum--;
			//Desc_TexRowVertCount--;
			CurrentIDNumber--;
		} //draw GL_QUADs for 1 row (contains 11 GL_Quad)
		Desc_TexColsVertCount--;
	} //draw GL_QUADs for 7 cols (contains 77 GL_Quad)


	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers(); //swap between front buffer and back buffer 
}

void init(void) {
	DDCM_Values value;
	int Status = FALSE;

	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(MAX_NO_TEXTURES, texture);

	AUX_RGBImageRec *TextureImage[1]; //create a memory space for textureimage
	memset(TextureImage, 0, sizeof(void *) * MAX_NO_TEXTURES);   //initialize the generated memory space to NULL
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // This Will Clear The Background Color
	TextureImage[0] = LoadBMP("ImageResource/modelS1920by1080px.bmp"); //pass image to LoadBMP function

																	   //flip texture
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(-1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

	glGenTextures(MAX_NO_TEXTURES, texture); //generate texture 
	glBindTexture(GL_TEXTURE_2D, texture[0]); //bind texture to texture[0].
	glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}


//---------------------------------OpenGL Render-------------------------------------









static void draw_subdiv(Mat& img, Subdiv2D& subdiv, Scalar delaunay_color)
{

	bool draw;
	vector<vector<Point2f>> final_quadrangle_point;
	vector<Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	vector<Point2f> pt(3);
	vector<Vec6f> quadrangle;


	for (size_t k = 0; k < triangleList.size(); k++)
	{
		Vec6f t = triangleList[k];
		pt[0] = Point2f(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point2f(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point2f(cvRound(t[4]), cvRound(t[5]));
		draw = true;
		for (int i = 0; i < 3; i++) {
			if (pt[i].x > 1800 || pt[i].y > 1800 || pt[i].x < 0 || pt[i].y < 0)
				draw = false;
		}
		if (draw) {
			line(img, pt[0], pt[1], delaunay_color, 1, LINE_AA, 0);
			line(img, pt[1], pt[2], delaunay_color, 1, LINE_AA, 0);
			line(img, pt[2], pt[0], delaunay_color, 1, LINE_AA, 0);
			//	cout << triangleList[k] << endl;
			quadrangle.push_back(triangleList[k]);
		}
	}
	imshow("delaunay", img);
	imwrite("result.png", img);

	//在quandrangle里面放入 正规的三角形坐标
	cout << "-----------" << endl;
	cout << "quadrangle.size" << quadrangle.size() << endl;
	/*for (int i = 0; i < quadrangle.size(); i++) {
	cout << quadrangle[i] << endl;
	}*/

	//查出四角形 把坐标放入quadranglef_pt
	int count_quadrangle = 0;
	for (int i = 0; i < quadrangle.size(); i++) {
		int j = i;
		for (j; j < quadrangle.size() - 1; j++) {
			int count = 0;
			vector<Point2f> quadranglef_pt;
			for (int k = 0; k < 5; k = k + 2) {
				if (quadrangle[i][k] == quadrangle[j + 1][0]) {
					if (quadrangle[i][k + 1] == quadrangle[j + 1][1]) {
						count++;
						quadranglef_pt.push_back(Point2f(quadrangle[i][k], quadrangle[i][k + 1]));
					}
				}
				if (quadrangle[i][k] == quadrangle[j + 1][2]) {
					if (quadrangle[i][k + 1] == quadrangle[j + 1][3]) {
						count++;
						quadranglef_pt.push_back(Point2f(quadrangle[i][k], quadrangle[i][k + 1]));
					}
				}
				if (quadrangle[i][k] == quadrangle[j + 1][4]) {
					if (quadrangle[i][k + 1] == quadrangle[j + 1][5]) {
						count++;
						quadranglef_pt.push_back(Point2f(quadrangle[i][k], quadrangle[i][k + 1]));
					}
				}
			}
			if (count == 2) {
				for (int k = 0; k < 6; k = k + 2) {
					if (Point2f(quadrangle[i][k], quadrangle[i][k + 1]) == quadranglef_pt[0] ||
						Point2f(quadrangle[i][k], quadrangle[i][k + 1]) == quadranglef_pt[1]) {
					}
					else {
						quadranglef_pt.push_back(Point2f(quadrangle[i][k], quadrangle[i][k + 1]));
					}
					if (Point2f(quadrangle[j + 1][k], quadrangle[j + 1][k + 1]) == quadranglef_pt[0] ||
						Point2f(quadrangle[j + 1][k], quadrangle[j + 1][k + 1]) == quadranglef_pt[1]) {
					}
					else {
						quadranglef_pt.push_back(Point2f(quadrangle[j + 1][k], quadrangle[j + 1][k + 1]));
					}
				}
			}
			if (quadranglef_pt.size() == 4) {
				//cout << "quadrangle point" << endl;
				count_quadrangle++;
				/*for (int i = 0; i < quadranglef_pt.size(); i++) {
				cout << quadranglef_pt[i] << endl;
				}*/
				final_quadrangle_point.push_back(quadranglef_pt);
				/*		cout << "finish  " << count_quadrangle << endl;
				cout << endl;*/
			}
		}
	}

	/*for (int i = 0; i < final_quadrangle_point.size(); i++) {
	cout << final_quadrangle_point[i] << endl;
	}*/
	cout << "......................." << endl;
	define_A_B_C_D(final_quadrangle_point);
}
int main(int argc, char* argv[])
{

	//计算时间
	clock_t start, finish;
	double totaltime;
	start = clock();

	//Mat src = imread("queshi_mark.png", 0); //lost marker
 	Mat src = imread("modify_mark.png", 0); //full marker

	getData();
	//定义SVM
	/*CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);*/

	Ptr<SVM> SVM_params = SVM::create();  // CvSVMParams结构用于定义基本参数  

	SVM_params->setType(SVM::C_SVC);     // SVM类型  
	SVM_params->setKernel(SVM::LINEAR); // 不做映射  
	SVM_params->setDegree(0);
	SVM_params->setGamma(1);
	SVM_params->setCoef0(0);
	SVM_params->setC(1);
	SVM_params->setNu(0);
	SVM_params->setP(0);
	SVM_params->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 0.01));



	SVM_params->train(trainData, ROW_SAMPLE, trainClasses);

	//得到图像进行预测
	Mat show;
	cvtColor(src, show, 8);//8 表示灰度图到彩色图
	Mat imageWhite;
	threshold(src, imageWhite, 100, 255, 8);
	imshow("threshold", imageWhite);
	imageWhite = 255 - imageWhite;
	imshow("src", src);
	// imshow("255 - threshold ", imageWhite); 


	/*Mat imageWhite;
	Mat frame_gray;
	blur(src, frame_gray, Size(3, 3));
	Canny(src, imageWhite, 80, 120, 3);
	imshow("threshold", imageWhite);*/
	//imageWhite = 255 - imageWhite;
	//imshow("255 - threshold ", imageWhite);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(imageWhite, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	//	for (int index = contours.size() - 1; index >= 1; index--)

	for (int index = contours.size() - 1; index >= 0; index--)
	{
		Rect rectangleTem = boundingRect(contours[index]);
		Mat image;
		image = Mat::zeros(src.size(), CV_8UC1);
		drawContours(image, contours, index, Scalar(255), 2, 8, hierarchy);
		Mat tem = image(rectangleTem);

		//	imshow("tem", tem);

		Mat imageNewSize;
		resize(tem, imageNewSize, sampleSize, CV_INTER_LINEAR);

		//	imshow("imageNewSize",imageNewSize);
		image.release();
		image = imageNewSize.reshape(1, 1);

		image.convertTo(image, CV_32FC1);
		int response = (int)SVM_params->predict(image);
		if (response == 0)
		{
			//cout << "    circle" << endl;
			string str = "circle";
			putText(show, str, Point(rectangleTem.x + rectangleTem.width / 2, rectangleTem.y + rectangleTem.height / 2),
				FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, 8);

			dotcenter.push_back(Point(rectangleTem.x + rectangleTem.width / 2, rectangleTem.y + rectangleTem.height / 2));
			dot_count.push_back(4);
		}
		else if (response == 1)
		{
			//cout << "    rectangle" << endl;
			string str = "rec";
			putText(show, str, Point(rectangleTem.x + rectangleTem.width / 2, rectangleTem.y + rectangleTem.height / 2),
				FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, 8);

			dotcenter.push_back(Point(rectangleTem.x + rectangleTem.width / 2, rectangleTem.y + rectangleTem.height / 2));
			dot_count.push_back(3);
		}
		else if (response == 2)
		{
			//cout << "    triangle" << endl;
			string str = "tri";
			putText(show, str, Point(rectangleTem.x + rectangleTem.width / 2, rectangleTem.y + rectangleTem.height / 2),
				FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, 8);

			dotcenter.push_back(Point(rectangleTem.x + rectangleTem.width / 2, rectangleTem.y + rectangleTem.height / 2));
			dot_count.push_back(2);
		}
		else if (response == 3)
		{
			//cout << "    cross" << endl;
			string str = "cross";
			putText(show, str, Point(rectangleTem.x + rectangleTem.width / 2, rectangleTem.y + rectangleTem.height / 2),
				FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, 8);

			dotcenter.push_back(Point(rectangleTem.x + rectangleTem.width / 2, rectangleTem.y + rectangleTem.height / 2));
			dot_count.push_back(1);
		}
	}


	/*cout << "dotcenter.size: " << dotcenter.size() << endl;
	for (int i = 0; i < dotcenter.size(); i++) {
	cout << dotcenter[i] << " , ";
	}
	cout << endl;*/

	/*cout << "dot_count.size: " << dot_count.size() << endl;
	for (int i = 0; i < dot_count.size(); i++) {
	cout << dot_count[i] << " , ";
	}
	cout << endl;*/


	imshow("result", show);
	imwrite("result.png", show);
	//-----------------------------------------------------------

	//triangulation
	Scalar active_facet_color(0, 0, 255), delaunay_color(255, 255, 255);
	Rect rect(0, 0, 1800, 1800);
	Subdiv2D subdiv(rect);

	Mat delaunay(src.size(), CV_8UC3);

	// 修改step1的时候 改的部分
	//	Mat delaunay(canny.size(), CV_8UC3);
	delaunay = Scalar::all(0);
	for (int i = 0; i < dotcenter.size(); i++)
	{
		subdiv.insert(dotcenter[i]);
	}

	draw_subdiv(delaunay, subdiv, active_facet_color);


	//voting-----------------------------------------------------
	multiset <int> ::iterator idit;
	for (int i = 0; i < dot_clusters_position.size(); i++) {
		size_t countmax = 0;
		size_t graymax = 0;
		for (idit = id[i].begin(); idit != id[i].end(); idit++)
		{

			if (std::count(id[i].begin(), id[i].end(), *idit) > countmax)
			{
				countmax = std::count(id[i].begin(), id[i].end(), *idit);
				graymax = *idit;
			}
		}
		final_id.push_back(graymax);
	}

	/*for (int i = 0; i < dot_clusters_position.size(); i++) {
	cout << dot_clusters_position[i] << "  -> " << final_id[i] << "      ";
	}
	cout << endl;*/

	cout << "sorting-------" << endl;
	// sorting-------------------------

	for (int i = 1; i < 97; i++) {
		vector<int>::iterator sortit = find(final_id.begin(), final_id.end(), i);

		if (sortit != final_id.end()) {
			cout << dot_clusters_position[sortit - final_id.begin()] << "  -> " << final_id[sortit - final_id.begin()] << "      ";
		}
	}

	//---------------------------------OpenGL Render-------------------------------------
	//code written by MyeongGyu.Lee, 20180112
	//mapping(dot_clusters_position, final_id);

	DDCM_Values value;


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GL_DEPTH_BUFFER_BIT);
	glutInitWindowSize(value.w, value.h);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("DDCM_Render(x64, 0117 V0.6_2a)");

	//CameraCalibration(); //call camera calibration function before initializing glut.
	init();
	glutDisplayFunc(display); //display callback function
	glutReshapeFunc(reshape);
	glutMainLoop();
	//---------------------------------OpenGL Render-------------------------------------

	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	cout << "轮廓总数：" << contours.size() << endl;

	cout << "结束一帧-------------------------" << endl;

	char c = cvWaitKey(0);






}

Mat readImageSaveContour(Mat src)
{
	Mat imageWhite;
	threshold(src, imageWhite, 100, 255, 8);
	imageWhite = 255 - imageWhite;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(imageWhite, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	//最大轮廓
	double maxarea = 0;
	int maxAreaIdx = 0;
	for (int index = contours.size() - 1; index >= 0; index--)
	{
		double tmparea = fabs(contourArea(contours[index]));
		if (tmparea>maxarea)
		{
			maxarea = tmparea;
			maxAreaIdx = index;
		}
	}
	Rect rectangleTem = boundingRect(contours[maxAreaIdx]);
	Mat image;
	image = Mat::zeros(src.size(), CV_8UC1);
	drawContours(image, contours, 0, Scalar(255), 2, 8, hierarchy);
	//Rect newRectangleTem(rectangleTem.x - 1, rectangleTem.y - 1, rectangleTem.width + 2, rectangleTem.height+2);
	Mat tem = image(rectangleTem);
	//imshow("tem", tem);


	return tem;
}

void getData()
{

	trainData.create(train_samples*classes, sampleSize.width*sampleSize.height, CV_32FC1);
	trainClasses.create(train_samples*classes, 1, CV_32SC1);

	//trainData.create(train_samples*classes, sampleSize.width*sampleSize.height, CV_32FC1);
	//trainClasses.create(train_samples*classes, 1, CV_32FC1);
	Mat src_image;
	char file[255];
	int i, j;
	for (i = 0; i<classes; i++)
	{
		for (j = 0; j< train_samples; j++)
		{
			sprintf(file, "samples/s%d/%d.png", i, j);
			src_image = imread(file, 0);
			if (src_image.empty())
			{
				printf("Error: Cant load image %s\n", file);
				//exit(-1);
			}

			Mat image = readImageSaveContour(src_image);
			//	imshow("image", image);
			Mat imageNewSize;
			resize(image, imageNewSize, sampleSize, CV_INTER_LINEAR);
			image.release();
			image = imageNewSize.reshape(1, 1);
			image.convertTo(trainData(Range(i*train_samples + j, i*train_samples + j + 1), Range(0, trainData.cols)), CV_32FC1);
			trainClasses.at<int>(i*train_samples + j, 0) = i;
		}

	}




}