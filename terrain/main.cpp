#include <windows.h>
#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include "../3d.h"

tdDraw tddraw;
tdDrawObject showobject;

using rgbc = std::array<unsigned char,3>;
double height(double x,double y);
tdDrawPolygon arraytopoly(std::array<double,9> a,rgbc color);


LRESULT CALLBACK WndProc(HWND hwnd , UINT msg , WPARAM wp , LPARAM lp) {
	HDC hdc;
	PAINTSTRUCT ps;

	RECT rect;
	
	static unsigned char* lpPixel;
	static BITMAPINFO bmpInfo;

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		tddraw.setObj(showobject);
		return 0;
	case WM_PAINT:
		GetWindowRect(hwnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		// std::cout << width << " " << height << std::endl; // output window size

		bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth=width;
		bmpInfo.bmiHeader.biHeight=height;
		bmpInfo.bmiHeader.biPlanes=1;
		bmpInfo.bmiHeader.biBitCount=32;
		bmpInfo.bmiHeader.biCompression=BI_RGB;
		hdc=BeginPaint(hwnd,&ps);
		{ // 3d
			lpPixel = tddraw.getImg(width,height);
			SetDIBitsToDevice(hdc,0,0,width,height,0,0,0,height,lpPixel,&bmpInfo,DIB_RGB_COLORS);
		}
		EndPaint(hwnd,&ps);
		delete[] lpPixel;
		return 0;
	}
	return DefWindowProc(hwnd,msg,wp,lp);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmdShow) {	


	std::array<int,4> mapa = {-100,-10,100,100};
	double buf = {};
	int psize = 6;
	mapa = {(int)std::floor(mapa[0]/psize)*psize,(int)std::floor(mapa[1]/psize)*psize,(int)std::floor(mapa[2]/psize)*psize,(int)std::floor(mapa[3]/psize)*psize};
	tdDrawObject obj = {};
	rgbc color = {255,255,255};
	rgbc water = {1,18,200};

		tddraw.setCamera({5,0,height(5,0)+30},{0,-0.3});
	int p = psize;
	for (int y=mapa[1]+psize;y<mapa[3]-psize;y+=psize*2) {
		for (int x=mapa[0]+psize;x<mapa[2]-psize;x+=psize*2) {
			double y_=y;double x_=x;
			// 地面
			double value = height(x,y);
			double cvalue = value*0.05;
			//std::cout << value << std::endl;
			rgbc color = {(unsigned char)(132-cvalue), (unsigned char)(131-cvalue), (unsigned char)(79-cvalue)};
        	obj.insert(obj.end(),arraytopoly({x_,y_,value,x_,y_-p,height(x,y-p),x_+p,y_-p,height(x+p,y-p)},color));
        	obj.insert(obj.end(),arraytopoly({x_,y_,value,x_+p,y_-p,height(x+p,y-p),x_+p,y_,height(x+p,y)},color));
        	obj.insert(obj.end(),arraytopoly({x_,y_,value,x_+p,y_,height(x+p,y),x_+p,y_+p,height(x+p,y+p)},color));
        	obj.insert(obj.end(),arraytopoly({x_,y_,value,x_+p,y_+p,height(x+p,y+p),x_,y_+p,height(x,y+p)},color));
        	obj.insert(obj.end(),arraytopoly({x_,y_,value,x_,y_+p,height(x,y+p),x_-p,y_+p,height(x-p,y+p)},color));
        	obj.insert(obj.end(),arraytopoly({x_,y_,value,x_-p,y_+p,height(x-p,y+p),x_-p,y_,height(x-p,y)},color));
        	obj.insert(obj.end(),arraytopoly({x_,y_,value,x_-p,y_,height(x-p,y),x_-p,y_-p,height(x-p,y-p)},color));
        	obj.insert(obj.end(),arraytopoly({x_,y_,value,x_-p,y_-p,height(x-p,y-p),x_,y_-p,height(x,y-p)},color));
			// // 水面
			// if (height(x,y)<2) {
			// 	water = [1,1,30-height(x,y)*0.2];
			// 	obj.push([[x_-p,y_-p,0],[x_+p,y_-p,0],[x_+p,y_+p,0],water]);
			// 	obj.push([[x_-p,y_-p,0],[x_+p,y_+p,0],[x_-p,y_+p,0],water]);
			// }
		}
	}
	showobject = obj;

	HWND hwnd;
	MSG msg;
	WNDCLASS winc;

	winc.style = CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc = WndProc;
	winc.cbClsExtra	= winc.cbWndExtra	= 0;
	winc.hInstance = hInstance;
	winc.hIcon = LoadIcon(NULL , IDI_APPLICATION);
	winc.hCursor = LoadCursor(NULL , IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName = NULL;
	winc.lpszClassName = TEXT("app");

	if (!RegisterClass(&winc)) return -1;

	hwnd = CreateWindow(
		TEXT("app") , TEXT("terrain generator") ,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE ,
		CW_USEDEFAULT , CW_USEDEFAULT ,
		500 , 400 , // window size
		NULL , NULL , hInstance , NULL
	);

	if (hwnd == NULL) return -1;

	while(GetMessage(&msg,NULL,0,0)) {DispatchMessage(&msg);}
	return msg.wParam;
}




double seed = 10;
using double2 = std::array<double,2>;
using double3 = std::array<double,3>;
double test8(double x,double y) {
    x+=10;
    double v = std::sin(x*10+y)+seed;
    double base = 5001;
    double ret = 0;
    double times = 1;
    for (double i=1;i<=times;i++) {
        ret += ((std::cos(v*base/std::pow(2,i)+i)))*std::pow(5,i);
        i++;
    }
    return std::fmod(std::abs(ret/times*4+128),1)*400-210;
}
double _getp(double3 a,double3 b,double3 c,double3 d,double3 p) {
    p[0] = 0.5*std::tanh(2*p[0]-1)+0.5+0.23*p[0]-0.115;
	p[1] = 0.5*std::tanh(2*p[1]-1)+0.5+0.23*p[1]-0.115;
    double a1 = (p[0]-a[0])*(p[1]-a[1]);double a2 = (p[0]-b[0])*(b[1]-p[1]);
	double a3 = (c[0]-p[0])*(c[1]-p[1]);double a4 = (d[0]-p[0])*(p[1]-d[1]);
    return (a[2]*a1+b[2]*a2+c[2]*a3+d[2]*a4);
}
double getp(double2 p,double s=1) {
    double minx = std::floor(p[0]/std::pow(2,s))*std::pow(2,s);double maxx = minx+std::pow(2,s);
    double miny = std::floor(p[1]/std::pow(2,s))*std::pow(2,s);double maxy = miny+std::pow(2,s);
    double res = _getp({0,0,test8(maxx+s,maxy)},{0,1,test8(maxx+s,miny)},{1,1,test8(minx+s,miny)},{1,0,test8(minx+s,maxy)},{(p[0]-minx)/std::pow(2,s),(p[1]-miny)/std::pow(2,s)});
    return res;
}
double getheight(double x,double y,double max=12,double min=1) {
    double res = 0;
    for (double i=min;i<=max;i++) {
        res += getp({x,y},i)/std::pow(2,max-i);
    }
    return (-10*std::tanh(0.09*res-0.5)+res-3)*4;
}
double height(double x,double y) {
    return getheight(x,y);
}
tdDrawPolygon arraytopoly(std::array<double,9> a,rgbc color) {
    return {{a[0],a[1],a[2]},{a[3],a[4],a[5]},{a[6],a[7],a[8]},color};
}