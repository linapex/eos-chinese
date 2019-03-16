
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:17</date>
//</624457049631100928>

#include <vector>
#include <iostream>

struct point
{
	double x;
	double y;
};

int main()
{
//在-1、-1到+1、+1范围内生成大量均匀分布的二维点。
	enum { numXSamples = 10000 };
	enum { numYSamples = 10000 };
	std::vector<point> points;
	points.reserve(numXSamples * numYSamples);
	for(int x = 0;x < numXSamples;++x)
	{
		for(int y = 0;y < numXSamples;++y)
		{
			point p = {-1.0 + 2.0 * x / (numXSamples-1),-1.0 + 2.0 * y / (numYSamples-1)};
			points.push_back(p);
		}
	}

//计算单位圆内点的比率。
	int numerator = 0;
	int denominator = 0;
	for(auto pointIt = points.begin();pointIt != points.end();++pointIt)
	{
		if(pointIt->x * pointIt->x + pointIt->y * pointIt->y < 1.0)
		{
			++numerator;
		}
		++denominator;
	}

//导出单位圆的面积。
	auto circleArea = 4.0 * (double)numerator / denominator;
	std::cout << "result: " << circleArea << std::endl;

	return 0;
}

