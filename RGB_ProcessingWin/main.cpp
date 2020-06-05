#include <iostream>
#include <vector>
//Thread building blocks library
#include <tbb/task_scheduler_init.h>

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_reduce.h>
//Free Image library
#include <FreeImagePlus.h>

#include <chrono>
#include<thread>
#include <cmath>


using namespace std;
using namespace tbb;
using namespace std::chrono;

#define BLACK 0
#define WHITE 1


void Part1(const char *strImg1, const char *strImg2,int flag)
//pass in the location of the strings to load
{	
	fipImage img1, img2;
	img1.load(strImg1);
	img2.load(strImg2);

	img1.convertToFloat();
	img2.convertToFloat();

	auto width = img1.getWidth();
	auto height = img1.getHeight();

	const float* const img1Buffer = (float*)img1.accessPixels();
	const float* const img2Buffer = (float*)img2.accessPixels();

	//output
	fipImage outputImage;
	outputImage = fipImage(FIT_FLOAT, width, height, 32);
	float* outputBuffer = (float*)outputImage.accessPixels();

	uint64_t numElements = width * height;	

	for (int i = 0; i < numElements; i++)//if both imgaes pixcles are the same will set output to black
	{
		if (img1Buffer[i] == img2Buffer[i])
		{
			//black
			outputBuffer[i] = BLACK;
		}
		else
		{
			//white
			outputBuffer[i] = WHITE;
		}
		//outputBuffer[i] = inputBuffer[i];
	}
	//saving
	if (flag == 1)
	{
		cout << "Saving stage1_top...\n";

		outputImage.convertToType(FREE_IMAGE_TYPE::FIT_BITMAP);
		outputImage.convertTo24Bits();
		//outputImage.save("../Images/stage1_top.png");

		cout << "...done\n\n";
	}
	else
	{
		cout << "Saving stage1_bottom...\n";

		outputImage.convertToType(FREE_IMAGE_TYPE::FIT_BITMAP);
		outputImage.convertTo24Bits();
		//outputImage.save("../Images/stage1_bottom.png");

		cout << "...done\n\n";
	}

}

void Part1Combine(const char *strImg1, const char *strImg2)
{
	cout << "Combineing top and bottom" << endl;
	fipImage img1, img2;
	img1.load(strImg1);
	img2.load(strImg2);

	img1.convertToFloat();
	img2.convertToFloat();

	auto width = img1.getWidth();
	auto height = img1.getHeight();

	const float* const img1Buffer = (float*)img1.accessPixels();
	const float* const img2Buffer = (float*)img2.accessPixels();

	//output
	fipImage outputImage;
	outputImage = fipImage(FIT_FLOAT, width, height, 32);
	float* outputBuffer = (float*)outputImage.accessPixels();

	uint64_t numElements = width * height;

	for (int i = 0; i < numElements; i++)
	{
		outputBuffer[i] = (img1Buffer[i] / 2) + (img2Buffer[i] / 2);
	}

	cout << "Saving stage1_combined....\n";
	outputImage.convertToType(FREE_IMAGE_TYPE::FIT_BITMAP);
	outputImage.convertTo24Bits();
	//outputImage.save("../Images/stage1_combined.png");
	cout << "...done\n\n";
}
//------------------------------------------------------------------
float gaussian2D(float x = 0, float y = 0, float sigma = 10.0f)
{
	float PI = 3.141;
	return 1.0f / (2.0f * PI * sqrt(sigma)) * exp(-((sqrt(x) + sqrt(y)) / (2.0f * sqrt(sigma))));
}
void Part2Blur(int kernalSize = 3, int sigma = 10)
{
	fipImage img1;
	img1.load("../Images/stage1_combined.png");
	img1.convertToFloat();
	auto width = img1.getWidth();
	auto height = img1.getHeight();
	
	const float* const img1Buffer = (float*)img1.accessPixels();

	//output
	fipImage outputImage;
	outputImage = fipImage(FIT_FLOAT, width, height, 32);
	float* outputBuffer = (float*)outputImage.accessPixels();


	uint64_t numElements = width * height;

	//make gausan kernal	
	float**gaussian;

	gaussian = new float* [kernalSize]; // create first dimension (of pointers to 2nd dimension)
	for (int i = 0; i < kernalSize; i++) {
		gaussian[i] = new float[kernalSize]; // create second dimension(s) 
	}

	for (int i = 0; i < kernalSize; i++)
	{
		for (int y = 0; y < kernalSize; y++)
		{			
			gaussian[i][y] = gaussian2D(i, y);
			//cout << gaussian[i][y] <<"|i;"<<i<<"|y;"<<y<< endl;
		}
	}
	
	int start = kernalSize / 2;
	//loops throug pixel to be blurd
	for (int h = start; h < height - start; h++)//startshakf the kernal size in to stop it from going out of scope
	{
		for (int w = start; w < width - start; w++)
		{
			float output=0;
			//loop throu kernal size around output pixel | starts in top left of kernal blur range 
			for (int i = 0; i < kernalSize; i++)//row
			{
				for (int y = 0; y < kernalSize; y++)//collom
				{
					output += img1Buffer[(w+y-start) + ((h * width)+i-start)] * gaussian[i][y];
				}
			}
			outputBuffer[w + (h * width)] = output;
		}
	}	

	cout << "Saving stage2_blurred.png...\n";

	outputImage.convertToType(FREE_IMAGE_TYPE::FIT_BITMAP);
	outputImage.convertTo24Bits();
	//outputImage.save("../Images/stage2_blurred.png");

	cout << "...done\n\n";
}

void Part2BinaryThreshold()
{
	fipImage img1;

	img1.load("../Images/stage2_blurred.png");
	img1.convertToFloat();
	auto width = img1.getWidth();
	auto height = img1.getHeight();
	
	const float* const img1Buffer = (float*)img1.accessPixels();

	//output
	fipImage outputImage;
	outputImage = fipImage(FIT_FLOAT, width, height, 32);
	float* outputBuffer = (float*)outputImage.accessPixels();


	uint64_t numElements = width * height;

	cout << "loop start" << endl;
	for (int i = 0; i < numElements; i++)
	{
		if (img1Buffer[i] != BLACK)
		{
			
			outputBuffer[i] = WHITE;
		}
		else
		{
			//white
			outputBuffer[i] = img1Buffer[1];
		}
		//outputBuffer[i] = inputBuffer[i];
	}


	cout << "Saving stage2_threshold.png...\n";

	outputImage.convertToType(FREE_IMAGE_TYPE::FIT_BITMAP);
	outputImage.convertTo24Bits();
	//outputImage.save("../Images/stage2_threshold.png");

	cout << "...done\n\n";
}

void Part2BlurTBB(int kernalSize = 3, int sigma = 10)
{
	fipImage img1;
	img1.load("../Images/stage1_combined.png");
	img1.convertToFloat();
	auto width = img1.getWidth();
	auto height = img1.getHeight();

	const float* const img1Buffer = (float*)img1.accessPixels();

	//output
	fipImage outputImage;
	outputImage = fipImage(FIT_FLOAT, width, height, 32);
	float* outputBuffer = (float*)outputImage.accessPixels();


	uint64_t numElements = width * height;

	//make gausan kernal	
	float** gaussian;

	gaussian = new float* [kernalSize]; // create first dimension (of pointers to 2nd dimension)
	for (int i = 0; i < kernalSize; i++) {
		gaussian[i] = new float[kernalSize]; // create second dimension(s) 
	}
	
	parallel_for(blocked_range<int>(0, kernalSize), [&](const blocked_range<int>& range) {

		auto start = range.begin();
		auto end = range.end();

		for (int i = start; i < end; ++i)
		{
			for (int y = 0; y < kernalSize; y++)//dose one row at a time
			{
				gaussian[i][y] = gaussian2D(i, y);
			}
		}
	});

	int start = kernalSize / 2;
	
	cout << "loop start" << endl;
	/*parallel_for(blocked_range<int>(0, start), [&](const blocked_range<int>& range) {
	
		auto pStart = range.begin();
		auto end = range.end();

		for (int h = pStart; h < height - pStart; h++)//start at half the kernal size in to stop it from going out of scope
		{
			for (int w = start; w < width - start; w++)
			{
				float output = 0;
				//loop throu kernal size around output pixel | starts in top left of kernal blur range 
				for (int i = 0; i < kernalSize; i++)//row
				{
					for (int y = 0; y < kernalSize; y++)//collom
					{
						output += img1Buffer[(w + y - start) + ((h * width) + i - start)] * gaussian[i][y];
					}
				}
				outputBuffer[w + (h * width)] = output;
			}
		}

	});	*/

	
	parallel_for(blocked_range2d<int, int>(start, (height-start), 8, start, (width-start), 8), [&](const blocked_range2d<int, int>& r) {

		auto y1 = r.rows().begin();
		auto y2 = r.rows().end();
		auto x1 = r.cols().begin();
		auto x2 = r.cols().end();

		for (int h = y1; h < y2; h++)//h<y1 keep in current scope | h<height-start stop it from goiny out of bounds
		{
			for (int w = x1; w<x2; w++)
			{
				float output = 0;
				//loop throu kernal size around output pixel | starts in top left of kernal blur range 
				for (int i = 0; i < kernalSize; i++)//row
				{
					for (int y = 0; y < kernalSize; y++)//collom
					{
						output += img1Buffer[(w + y - start) + ((h * width) + i - start)] * gaussian[i][y];
					}
				}
				outputBuffer[w + (h * width)] = output;
			}
		}

	});
	cout << "Saving stage2_blurred.png...\n";

	outputImage.convertToType(FREE_IMAGE_TYPE::FIT_BITMAP);
	outputImage.convertTo24Bits();
	//outputImage.save("../Images/stage2_blurred.png");

	cout << "...done\n\n";
}

void Part2BinaryThresholdTBB()
{
	fipImage img1;

	img1.load("../Images/stage2_blurred.png");
	img1.convertToFloat();
	auto width = img1.getWidth();
	auto height = img1.getHeight();

	const float* const img1Buffer = (float*)img1.accessPixels();

	//output
	fipImage outputImage;
	outputImage = fipImage(FIT_FLOAT, width, height, 32);
	float* outputBuffer = (float*)outputImage.accessPixels();


	uint64_t numElements = width * height;


	parallel_for(blocked_range<int>(0, numElements), [&](const blocked_range<int>& range) {
	
		auto start = range.begin();
		auto end = range.end();

		for (int i = start; i < end; i++)
		{
			if (img1Buffer[i] != BLACK)
			{

				outputBuffer[i] = WHITE;
			}
			else
			{
				//white
				outputBuffer[i] = img1Buffer[1];
			}
			//outputBuffer[i] = inputBuffer[i];
		}

	});
	cout << "Saving stage2_threshold.png...\n";

	outputImage.convertToType(FREE_IMAGE_TYPE::FIT_BITMAP);
	outputImage.convertTo24Bits();
	//outputImage.save("../Images/stage2_threshold.png");

	cout << "...done\n\n";
}
//------------------------------------------------------------------

void Part3()
{	fipImage img1, img2threshold;
	img1.load("../Images/render_top_1.png");
	img2threshold.load("../Images/stage2_threshold.png");

	//img1.convertToFloat();
	//img2threshold.convertToFloat();
	img1.convertTo32Bits();
	img2threshold.convertTo32Bits();

	auto width = img1.getWidth();
	auto height = img1.getHeight();

	const RGBQUAD* const img1Buffer = (RGBQUAD*)img1.accessPixels();
	const RGBQUAD* const img2Buffer = (RGBQUAD*)img2threshold.accessPixels();

	//output
	fipImage outputImage;
	outputImage = fipImage(FIT_FLOAT, width, height, 32);
	RGBQUAD* outputBuffer = (RGBQUAD*)outputImage.accessPixels();

	int numElements = width * height;

	//-----white pixel count ------

	int whitepix = 0;
	
	for (int i = 0; i < numElements; i++)
	{
		if (img2Buffer[i].rgbRed == 255 && img2Buffer[i].rgbGreen == 255 && img2Buffer[i].rgbBlue == 255)//white
		{			
			whitepix++;
			//cout << "white" << endl;
		}
		
	}
	cout << "num of whit pixels:" << whitepix << endl;
	float percent = (((float)whitepix/(float)numElements) * 100);
	cout << "white persntage:" << percent << endl;
	
	for (int i = 0; i < numElements; i++)
	{
		if (img2Buffer[i].rgbRed == UCHAR_MAX && img2Buffer[i].rgbGreen == UCHAR_MAX && img2Buffer[i].rgbBlue == UCHAR_MAX)//is white
		{
			outputBuffer[i].rgbRed = UCHAR_MAX - img1Buffer[i].rgbRed;
			outputBuffer[i].rgbGreen = UCHAR_MAX - img1Buffer[i].rgbGreen;
			outputBuffer[i].rgbBlue = UCHAR_MAX - img1Buffer[i].rgbBlue;
			outputBuffer[i].rgbReserved = img1Buffer[i].rgbReserved;
			/*if (img1Buffer[i].r == 255)//white | spwates colour
			{
				outputBuffer[i].r = 0;//black
				outputBuffer[i].g = 0;
				outputBuffer[i].b = 0;
			}
			else
			{
				outputBuffer[i].r = 255;//white
				outputBuffer[i].g = 255;
				outputBuffer[i].b = 255;
			}*/
		}
		else//if not wihtpixle keep colour
		{
			outputBuffer[i].rgbRed = img1Buffer[i].rgbRed;
			outputBuffer[i].rgbGreen = img1Buffer[i].rgbGreen;
			outputBuffer[i].rgbBlue = img1Buffer[i].rgbBlue;
			outputBuffer[i].rgbReserved = img1Buffer[i].rgbReserved;
		}
	}

	cout << "Saving stage1_top_invert...\n";

	outputImage.convertToType(FREE_IMAGE_TYPE::FIT_BITMAP);
	outputImage.convertTo32Bits();
	//outputImage.save("../Images/stage1_top_invert.png");

	cout << "...done\n\n";
}

void Part3TBB()
{
	fipImage img1, img2threshold;
	img1.load("../Images/render_top_1.png");
	img2threshold.load("../Images/stage2_threshold.png");

	img1.convertTo32Bits();
	img2threshold.convertTo32Bits();

	auto width = img1.getWidth();
	auto height = img1.getHeight();

	const RGBQUAD* const img1Buffer = (RGBQUAD*)img1.accessPixels();
	const RGBQUAD* const img2Buffer = (RGBQUAD*)img2threshold.accessPixels();

	//output
	fipImage outputImage;
	outputImage = fipImage(FIT_FLOAT, width, height, 32);
	RGBQUAD* outputBuffer = (RGBQUAD*)outputImage.accessPixels();

	int numElements = width * height;
	

	int whitepix = 0;
	whitepix += parallel_reduce(blocked_range<int>(0, numElements),0,
		[&](const blocked_range<int>& range, int initValue)->int {
		auto rangeBegin = range.begin();
		auto rangeEnd = range.end();
		for (int i = rangeBegin; i != rangeEnd; i++) {
			
			if (img2Buffer[i].rgbRed == UCHAR_MAX && img2Buffer[i].rgbGreen == UCHAR_MAX && img2Buffer[i].rgbBlue == UCHAR_MAX)//is white
			{			
				initValue++;
			}
		}
		return initValue;
		},
		[&](int x, int y)->int {return (x + y); }
		);

	cout << "whitepix TBB:" << whitepix << endl;
	
	float percent = (((float)whitepix / (float)numElements) * 100);
	cout << "white persntage:" << percent << endl;

	parallel_for(blocked_range<int>(0, numElements), [&](const blocked_range<int>& range) {

		auto start = range.begin();
		auto end = range.end();

		
		for (int i = start; i < end; ++i)
		{
			if (img2Buffer[i].rgbRed == UCHAR_MAX && img2Buffer[i].rgbGreen == UCHAR_MAX && img2Buffer[i].rgbBlue == UCHAR_MAX)//is white
			{
				outputBuffer[i].rgbRed = UCHAR_MAX - img1Buffer[i].rgbRed;
				outputBuffer[i].rgbGreen = UCHAR_MAX - img1Buffer[i].rgbGreen;
				outputBuffer[i].rgbBlue = UCHAR_MAX - img1Buffer[i].rgbBlue;
				outputBuffer[i].rgbReserved = img1Buffer[i].rgbReserved;				
			}
			else//if not wihtpixle keep colour
			{
				outputBuffer[i].rgbRed = img1Buffer[i].rgbRed;
				outputBuffer[i].rgbGreen = img1Buffer[i].rgbGreen;
				outputBuffer[i].rgbBlue = img1Buffer[i].rgbBlue;
				outputBuffer[i].rgbReserved = img1Buffer[i].rgbReserved;
			}
		}			

	});//parrel_for end


	/*parallel_for(blocked_range2d<int, int>(0, height , 8, 0, width, 8), [&](const blocked_range2d<int, int>& r) {

		auto y1 = r.rows().begin();
		auto y2 = r.rows().end();
		auto x1 = r.cols().begin();
		auto x2 = r.cols().end();
		

		for (int h = y1; h < y2; h++)//h<y1 keep in current scope | h<height-start stop it from goiny out of bounds
		{
			for (int w = x1; w < x2; w++)
			{
				if (img2Buffer[w + (h * width)].rgbRed == UCHAR_MAX && img2Buffer[w + (h * width)].rgbGreen == UCHAR_MAX && img2Buffer[w + (h * width)].rgbBlue == UCHAR_MAX)//is white
				{
					outputBuffer[w + (h * width)].rgbRed = UCHAR_MAX - img1Buffer[w + (h * width)].rgbRed;
					outputBuffer[w + (h * width)].rgbGreen = UCHAR_MAX - img1Buffer[w + (h * width)].rgbGreen;
					outputBuffer[w + (h * width)].rgbBlue = UCHAR_MAX - img1Buffer[w + (h * width)].rgbBlue;
					outputBuffer[w + (h * width)].rgbReserved = img1Buffer[w + (h * width)].rgbReserved;
					
				}
				else//if not wihtpixle keep colour
				{
					outputBuffer[w + (h * width)].rgbRed = img1Buffer[w + (h * width)].rgbRed;
					outputBuffer[w + (h * width)].rgbGreen = img1Buffer[w + (h * width)].rgbGreen;
					outputBuffer[w + (h * width)].rgbBlue = img1Buffer[w + (h * width)].rgbBlue;
					outputBuffer[w + (h * width)].rgbReserved = img1Buffer[w + (h * width)].rgbReserved;
				}
			}
		}
	});*/

	cout << "Saving stage1_top_invert...\n";

	outputImage.convertToType(FREE_IMAGE_TYPE::FIT_BITMAP);
	outputImage.convertTo32Bits();
	outputImage.save("../Images/stage1_top_invert_tbb.png");

	cout << "...done\n\n";
}

//------------------------------------------------------------------

int main()
{
	int nt = task_scheduler_init::default_num_threads();
	task_scheduler_init T(nt);

	//Part 1 (Image Comparison): -----------------DO NOT REMOVE THIS COMMENT----------------------------//
	cout << "Toss a core to your picture" << endl;

	const char *img1 = "../Images/render_top_1.png";
	const char *img2 = "../Images/render_top_2.png";
	const char *img3 = "../Images/render_bottom_1.png";
	const char *img4 = "../Images/render_bottom_2.png";

	cout << "---Part 1Liear---" << endl;
	auto start = chrono::steady_clock::now();//timer start
	//cout << "stage1_top" << endl;
	Part1(img1, img2,1);

	//cout << "stage2_bottom" << endl;
	Part1(img3, img4,2);

	//cout << "stage1_combine" << endl;
	Part1Combine("stage1_top.png", "stage1_bottom.png");	

	auto end = chrono::steady_clock::now();//timmer end
	cout << "Part 1 liner time:" << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl<<endl;


	
	////-------------------------Threading-------------------------
	cout << "---Part 1 threading---" << endl;

	start = chrono::steady_clock::now();//start timer

	thread th1(Part1,img1, img2, 1);
	thread th2(Part1, img3, img4, 2);
	cout << "threads made \n";
	th2.join();
	th1.join();
	cout << "threads done"<<endl;

	Part1Combine("../Images/stage1_top.png", "../Images/stage1_bottom.png");

	end = chrono::steady_clock::now();//timmer end
	cout << "Part 1 thread time:" << chrono::duration_cast<chrono::milliseconds>(end - start).count()<<endl;

	//Part 2 (Blur & post-processing): -----------DO NOT REMOVE THIS COMMENT----------------------------//
	cout << "---part2---" << endl;
	cout << gaussian2D(0, 0)<<"|";
	cout << gaussian2D(1, 0) << "|";
	cout << gaussian2D(2,0) << endl;
	cout << gaussian2D(0, 1) << "|";
	cout << gaussian2D(1, 1) << "|";
	cout << gaussian2D(2, 1) << endl;
	cout << gaussian2D(0, 2) << "|";
	cout << gaussian2D(1, 2) << "|";
	cout << gaussian2D(2, 2) << endl;

	cout << endl << "kernal size >>";
	int kernalSize;
	cin >> kernalSize;

	cout << endl << "Sigma>>";
	int sigma;
	cin >> sigma;

	start = chrono::steady_clock::now();//start timer
	Part2Blur(kernalSize, sigma);	
	Part2BinaryThreshold();	
	 end = chrono::steady_clock::now();//timmer end
	 cout << "Part 2 liner time:" << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl << endl;
	
	 start = chrono::steady_clock::now();//start timer
	 Part2BlurTBB(kernalSize, sigma);
	 Part2BinaryThresholdTBB();
	 end = chrono::steady_clock::now();//timmer end
	 cout << "Part 2 TBB time:" << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl << endl;

	//Part 3 (Image Mask): -----------------------DO NOT REMOVE THIS COMMENT----------------------------//
	cout << "---part3---" << endl;

	start = chrono::steady_clock::now();//start timer
	Part3();
	 end = chrono::steady_clock::now();//timmer end
	cout << "Part 3 liner time:" << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl << endl;
	

	start = chrono::steady_clock::now();//start timer
	Part3TBB();
	end = chrono::steady_clock::now();//timmer end
	cout << "Part 3 TBB time:" << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl << endl;
	system("pause");
	return 0;
}