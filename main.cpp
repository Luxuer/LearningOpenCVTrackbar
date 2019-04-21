/**
 * LearningOpenCVTrackBar ѧϰOpenCV�����TrackBar���ʹ��
 * �����ص������Ķ���, ����ص��������ݵĽṹ��Ķ���
 * ����ָ�����Mat�ļ���(��Ч��)
 *
 * ��f(x, y)��ԭͼ��ĳһ�������ֵ(R, G, B���߻Ҷ�), g(x, y)�Ǿ����������
 * ��Ӧ�������ֵ, ��ôͼ��ԱȶȺ����ȵ�����ʽΪ: 
 *				g(x, y) = alpha * f(x, y) + beta
 * ����alpha�����Աȶ�, ȡֵ��ΧΪ0.0-3.0; beta��������, ȡֵ��ΧΪ0-255.
 *
 *     ��Ȼ����Ĺ�ʽ������д, ��g(x, y)��f(x, y)��Ҫ��0-255�ķ�Χ��, 
 * ����ͨ���������ұ�֮��, ���һ��saturate_cast<uchar>()����֤����ֵ
 * �ں���ķ�Χ��, Ҳ���ǹ�ʽΪ:
 *				g(x, y) = saturate_cast<uchar>( alpha * f(x, y) + beta )
 */

#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

// ��������
#define WINDOW_NAME_DISPLAY "display"

// ����������
#define TRACKBAR_NAME_ALPHA "�Աȶ�"
#define TRACKBAR_NAME_BETA "����"

struct TrackbarData
{
	
	// �Աȶ�ֵ, ����ȡֵ��ΧӦ����0.0-3.0, ���ڻ�����λ��ֻ��������, 
	// ���Խ�������100��, alpha_valueȡֵ��ΧΪ0-300, ��ʵ�ʼ���Աȶȵ�ʱ��,
	// �Ὣ�����100, Ҳ���� alpha = alpha_value / max_alpha_value * 3.
	int alpha_value; 
	// ���Աȶ�ֵ, ����300
	int max_alpha_value; 
	// ����ֵ, ȡֵ��ΧΪ0-255
	int beta_value;
	// �������ֵ, ����255
	int max_beta_value;
	// Ҫ��ʾ��ԭͼ��
	Mat image;
	// ԭͼ����ĶԱȶȺ�����֮���ͼ��
	Mat dst_image;
	TrackbarData(int av, int mav, int bv, int mbv, Mat img, Mat dst_img)
		: alpha_value(av), max_alpha_value(mav), 
		  beta_value(bv), max_beta_value(mbv),
		  image(img), dst_image(dst_img) {}
};

// ��Ӧ�������Ļص�����
void OnTrackbar(int pos, void *userdata)
{
	TrackbarData *ptd = (TrackbarData*)userdata; // ����voidָ��ȡ���ṹ��
	double alpha = (ptd->alpha_value) / double(ptd->max_alpha_value) * 3; // ����Աȶ�
	double beta = ptd->beta_value; // ��������

	Mat image = (ptd->image); // ȡ��ԭͼ��
	// ȡ������չʾ��"����", �������ԭͼ��ı�ԱȶȺ�����֮���ͼ��
	Mat dst_image = (ptd->dst_image); 

	int nr = image.rows;
	int nl = image.cols*image.channels(); // ��3ͨ��ת��Ϊ1ͨ��
	/**
	 * ���һ��ͼ������������֮�䶼�������Ļ�, ���Խ���չ��һ������, 
	 * Ȼ����һ��ָ�������, ����Ч�ʱȽϸ�.
	 */
	if (image.isContinuous() && dst_image.isContinuous()) 
	{
		//printf("good!\n"); 
		nl *= nr;
		nr = 1;
	}
	
	for (int i = 0; i < nr; i++)
	{
		// ÿһ��ͼ���ָ��
		const uchar* const inData = image.ptr<uchar>(i); // ԭͼ��ָ��
		uchar * const outData = dst_image.ptr<uchar>(i); // ����չʾ��ͼ���ָ��
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
	moveWindow(WINDOW_NAME_DISPLAY, 0, 0); // ��չʾ���ڷŵ����ʵ�λ��
	resizeWindow(WINDOW_NAME_DISPLAY, 640, 480); // ����չʾ���ڵĴ�С

	// ����չʾ��ͼ�����
	Mat dst_image = Mat::zeros(input_image.size(), input_image.type());
	// ��Ż������ص��������ݵĽṹ��
	TrackbarData *ptd = new TrackbarData(0, 300, 0, 255, input_image, dst_image);

	/**
	 * �������������Թ���һ���ص�����, ��Ϊ��ĳ���������ı��ʱ��, 
	 * Ҫ���Ķ������¼���alpha��beta��ֵ, Ȼ���ػ�ͼ��; ��ͬ������, 
	 * һ����������������ı�alpha_value��ֵ, ��һ���ı�beta_value��ֵ. 
	 */
	// �����ԱȶȵĻ�����
	createTrackbar(TRACKBAR_NAME_ALPHA, WINDOW_NAME_DISPLAY, 
		&(ptd->alpha_value), ptd->max_alpha_value, OnTrackbar, (void*)ptd);
	// �������ȵĻ�����
	createTrackbar(TRACKBAR_NAME_BETA, WINDOW_NAME_DISPLAY,
		&(ptd->beta_value), ptd->max_beta_value, OnTrackbar, (void*)ptd);

	while (char(waitKey(30)) != 'q') // ��q�����˳�
	{
		imshow(WINDOW_NAME_DISPLAY, dst_image);
	}
	//waitKey(0);
	return 0;
}