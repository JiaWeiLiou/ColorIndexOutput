#include <iostream>
#include <opencv2/opencv.hpp>  
#include <cmath>

using namespace std;
using namespace cv;


void makeColorbarIndex(vector<Scalar> &colorbarIndex);
void makeColorwheelIndex(vector<Scalar> &colorwheelIndex);
void colorwheel(Mat &colorwheelImage, int dim);

int main()
{

	return 0;
}

void makeColorbarIndex(vector<Scalar> &colorbarIndex)
{
	vector<Scalar> maincolor;

	maincolor.push_back(Scalar(127.5, 0, 0));      //深紅色
	maincolor.push_back(Scalar(255, 0, 0));		   //紅色
	maincolor.push_back(Scalar(255, 127.5, 0));	   //紅色至黃色
	maincolor.push_back(Scalar(255, 255, 0));	   //黃色
	maincolor.push_back(Scalar(127.5, 255, 0));	   //黃色至綠色
	maincolor.push_back(Scalar(0, 255, 0));		   //綠色
	maincolor.push_back(Scalar(0, 255, 127.5));	   //綠色至青色
	maincolor.push_back(Scalar(0, 255, 255));	   //青色
	maincolor.push_back(Scalar(0, 127.5, 255));	   //青色至藍色
	maincolor.push_back(Scalar(0, 0, 255));		   //藍色
	maincolor.push_back(Scalar(0, 0, 127.5));      //深藍色

	int layer = 15;		//各漸層漸變階層數

	for (int i = 0; i < maincolor.size() - 1; ++i)
		for (int j = 0; j < layer; j++)
		{
			double r = maincolor[i][0] + (maincolor[i + 1][0] - maincolor[i][0]) / layer*j;
			double g = maincolor[i][1] + (maincolor[i + 1][1] - maincolor[i][1]) / layer*j;
			double b = maincolor[i][2] + (maincolor[i + 1][2] - maincolor[i][2]) / layer*j;
			colorbarIndex.push_back(Scalar(r, g, b));
		}
}
void makeColorwheelIndex(vector<Scalar> &colorwheelIndex)
{
	vector<Scalar> maincolor;

	maincolor.push_back(Scalar(255, 0, 0));		   //紅色
	maincolor.push_back(Scalar(255, 255, 0));	   //黃色
	maincolor.push_back(Scalar(0, 255, 0));		   //綠色
	maincolor.push_back(Scalar(0, 255, 255));	   //青色
	maincolor.push_back(Scalar(0, 0, 255));		   //藍色
	maincolor.push_back(Scalar(255, 0, 255));	   //洋紅

	int layer = 15;		//各漸層漸變階層數

	for (int i = 0; i < maincolor.size(); ++i)
		for (int j = 0; j < layer; j++)
		{
			double r = maincolor[i][0] + (maincolor[(i + 1) % maincolor.size()][0] - maincolor[i][0]) / layer*j;
			double g = maincolor[i][1] + (maincolor[(i + 1) % maincolor.size()][1] - maincolor[i][1]) / layer*j;
			double b = maincolor[i][2] + (maincolor[(i + 1) % maincolor.size()][2] - maincolor[i][2]) / layer*j;
			colorwheelIndex.push_back(Scalar(r, g, b));
		}
}
void colorwheel(Mat &colorwheelImage, int dim)
{
	if (dim % 2 == 0)
	{
		dim = dim - 1;    //限制直徑為奇數
	}

	if (colorwheelImage.empty())
		colorwheelImage.create(dim, dim, CV_8UC4);

	vector<Scalar> colorwheelIndex; //Scalar r,g,b  
	makeColorwheelIndex(colorwheelIndex);

	for (int i = 0; i < dim - 1; ++i)
		for (int j = 0; j < dim - 1; ++j)
		{
			int x = j - (dim - 1) / 2;
			int y = i - (dim - 1) / 2;
			double rad = sqrt(x * x + y * y);
			double angle = atan2(-y, -x) / CV_PI;
			double fk = (angle + 1.0) / 2.0 * (colorwheelIndex.size() - 1);  //計算角度對應之漸層色的實際索引位置
			int k0 = (int)fk;											//計算角度對應之漸層色的索引位置下界
			int k1 = (k0 + 1) % colorwheelIndex.size();						//計算角度對應之漸層色的索引位置上界(if k0=89; k1 =0)
			float f = fk - k0;											//計算實際索引位置至索引位置下界的距離
																		//f = 0; // uncomment to see original color wheel  

																		/*設定半徑內不透明、半徑外透明*/
			if (rad <= (dim - 1) / 2) { colorwheelImage.at<Vec4b>(i, j)[3] = 255; }
			else { colorwheelImage.at<Vec4b>(i, j)[3] = 0; }

			for (int b = 0; b < 3; b++)
			{
				float col0 = colorwheelIndex[k0][b] / 255.0;
				float col1 = colorwheelIndex[k1][b] / 255.0;
				float col = (1 - f) * col0 + f * col1;					//漸層色內插
				if (rad <= (dim - 1) / 2) { col = 1 - (rad / ((dim - 1) / 2)) * (1 - col); }		// increase saturation with radius
				colorwheelImage.at<Vec4b>(i, j)[2 - b] = (int)(255.0 * col);
			}
		}
}
void colorbar(Mat &colorbarImage, int length, int width)
{
	if (colorbarImage.empty())
		colorbarImage.create(length, width, CV_8UC3);

	vector<Scalar> colorbarIndex; //Scalar r,g,b  
	makeColorbarIndex(colorbarIndex);

	for (int i = 0; i < length; ++i)
		for (int j = 0; j < width; ++j)
		{
			double fk = (double)i / (double)length*colorbarIndex.size();
			int k0 = floor(fk);						//計算長度對應之漸層色的索引位置下界
			int k1 = ceil(fk);						//計算長度對應之漸層色的索引位置上界
			float f = fk - k0;						//計算實際索引位置至索引位置下界的距離

			for (int b = 0; b < 3; b++)
			{
				float col0 = colorbarIndex[k0][b] / 255.0;
				float col1 = colorbarIndex[k1][b] / 255.0;
				float col = (1 - f) * col0 + f * col1;					//漸層色內插
				colorbarImage.at<Vec3b>(i, j)[2 - b] = (int)(255.0 * col);
			}
		}
}