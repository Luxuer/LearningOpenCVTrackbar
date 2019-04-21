/**
 * LearningOpenCVTrackBar 学习OpenCV里面的TrackBar如何使用
 * 包含回调函数的定义, 传入回调函数数据的结构体的定义
 * 还有指针遍历Mat的技巧(高效率)
 *
 * 设f(x, y)是原图像某一点的像素值(R, G, B或者灰度), g(x, y)是经过调整后的
 * 对应点的像素值, 那么图像对比度和亮度调整公式为: 
 *				g(x, y) = alpha * f(x, y) + beta
 * 其中alpha调整对比度, 取值范围为0.0-3.0; beta调整亮度, 取值范围为0-255.
 *
 *     虽然上面的公式是那样写, 但g(x, y)和f(x, y)都要在0-255的范围内, 
 * 所以通常计算完右边之后, 会加一个saturate_cast<uchar>()来保证像素值
 * 在合理的范围内, 也就是公式为:
 *				g(x, y) = saturate_cast<uchar>( alpha * f(x, y) + beta )
 */

#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

// 窗口名称
#define WINDOW_NAME_DISPLAY "display"

// 滑动条名称
#define TRACKBAR_NAME_ALPHA "对比度"
#define TRACKBAR_NAME_BETA "亮度"

struct TrackbarData
{
	
	// 对比度值, 本来取值范围应该是0.0-3.0, 由于滑动条位置只能是整数, 
	// 所以将其扩大100倍, alpha_value取值范围为0-300, 在实际计算对比度的时候,
	// 会将其除以100, 也就是 alpha = alpha_value / max_alpha_value * 3.
	int alpha_value; 
	// 最大对比度值, 等于300
	int max_alpha_value; 
	// 亮度值, 取值范围为0-255
	int beta_value;
	// 最大亮度值, 等于255
	int max_beta_value;
	// 要显示的原图像
	Mat image;
	// 原图像更改对比度和亮度之后的图像
	Mat dst_image;
	TrackbarData(int av, int mav, int bv, int mbv, Mat img, Mat dst_img)
		: alpha_value(av), max_alpha_value(mav), 
		  beta_value(bv), max_beta_value(mbv),
		  image(img), dst_image(dst_img) {}
};

// 响应滑动条的回调函数
void OnTrackbar(int pos, void *userdata)
{
	TrackbarData *ptd = (TrackbarData*)userdata; // 根据void指针取出结构体
	double alpha = (ptd->alpha_value) / double(ptd->max_alpha_value) * 3; // 计算对比度
	double beta = ptd->beta_value; // 计算亮度

	Mat image = (ptd->image); // 取出原图像
	// 取出用于展示的"画布", 用来存放原图像改变对比度和亮度之后的图像
	Mat dst_image = (ptd->dst_image); 

	int nr = image.rows;
	int nl = image.cols*image.channels(); // 将3通道转换为1通道
	/**
	 * 如果一个图像矩阵的行与行之间都是连续的话, 可以将其展成一行数组, 
	 * 然后用一个指针遍历它, 这样效率比较高.
	 */
	if (image.isContinuous() && dst_image.isContinuous()) 
	{
		//printf("good!\n"); 
		nl *= nr;
		nr = 1;
	}
	
	for (int i = 0; i < nr; i++)
	{
		// 每一行图像的指针
		const uchar* const inData = image.ptr<uchar>(i); // 原图像指针
		uchar * const outData = dst_image.ptr<uchar>(i); // 用于展示的图像的指针
		for (int j = 0; j < nl; j++)
			outData[j] = saturate_cast<uchar>(inData[j] * alpha + beta);
	}
}

int main(int argc, char **argv)
{
	String input_image_path = "flipped_actor_small.jpg";
	Mat input_image = imread(input_image_path);
	if (input_image.empty())
	{
		fprintf(stderr, "Error! No such picture.\n");
		return -1;
	}
	namedWindow(WINDOW_NAME_DISPLAY, WINDOW_NORMAL);
	moveWindow(WINDOW_NAME_DISPLAY, 0, 0); // 把展示窗口放到合适的位置
	resizeWindow(WINDOW_NAME_DISPLAY, 640, 480); // 设置展示窗口的大小

	// 用来展示的图像矩阵
	Mat dst_image = Mat::zeros(input_image.size(), input_image.type());
	// 存放滑动条回调函数数据的结构体
	TrackbarData *ptd = new TrackbarData(0, 300, 0, 255, input_image, dst_image);

	/**
	 * 两个滑动条可以共用一个回调函数, 因为当某个滑动条改变的时候, 
	 * 要做的都是重新计算alpha和beta的值, 然后重画图像; 不同点在于, 
	 * 一个滑动条会根据鼠标改变alpha_value的值, 另一个改变beta_value的值. 
	 */
	// 调整对比度的滑动条
	createTrackbar(TRACKBAR_NAME_ALPHA, WINDOW_NAME_DISPLAY, 
		&(ptd->alpha_value), ptd->max_alpha_value, OnTrackbar, (void*)ptd);
	// 调整亮度的滑动条
	createTrackbar(TRACKBAR_NAME_BETA, WINDOW_NAME_DISPLAY,
		&(ptd->beta_value), ptd->max_beta_value, OnTrackbar, (void*)ptd);

	while (char(waitKey(30)) != 'q') // 按q可以退出
	{
		imshow(WINDOW_NAME_DISPLAY, dst_image);
	}
	//waitKey(0);
	return 0;
}