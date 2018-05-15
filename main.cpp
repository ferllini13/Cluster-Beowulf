#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <opencv2/opencv.hpp>

using namespace cv;

int avrg(Mat src, int x, int y);
int main(int argc, char *argv[]){

	const int MASTER = 0;

	const int TAG_GENERAL = 1;
	int node;
	int source;
	int dest;
	int rc;
	int	count;
	int waitflag;

	MPI_Status Stat;

	MPI_Init(&argc, &argv);

	int numTasks;
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

	MPI_Comm_rank(MPI_COMM_WORLD, &node);

	// master node
	if (node == MASTER){
		Mat src = imread("pic.png", IMREAD_GRAYSCALE);

		int rows = src.rows;
		int rows_div = (int)(rows / (numTasks - 1));

		int work_count = 0;

		// broadcast to the sub-processes
		for (dest = 1; dest < numTasks; ++dest)
		{
			int start = work_count;
			work_count += rows_div;
			MPI_Send(&start, 1, MPI_INT, dest, TAG_GENERAL, MPI_COMM_WORLD);
			MPI_Send(&rows_div, 1, MPI_INT, dest, TAG_GENERAL, MPI_COMM_WORLD);
		}
		clock_t begin = clock();

		// get answer
		for (int x = 0; x < rows_div; ++x){
			for (int y = 0; y < src.cols - 1; ++y){
				for (dest = 1; dest < numTasks; ++dest){
					int rec = 0;
					MPI_Recv(&rec, 1, MPI_INT, dest, TAG_GENERAL, MPI_COMM_WORLD, &Stat);
					src.at<uchar>((rows_div * (dest - 1)) + x, y) = (uchar)rec;
				}
			}
		}

		clock_t end = clock();
		double elapsed_secs_wc = double(end - begin) / CLOCKS_PER_SEC;
		printf("Merge Time: %lf \n", elapsed_secs_wc);
		imwrite("filteredPic.jpg", src);//save picture
	}

	//Other nodes
	else
	{
		// to wait
		do{
			MPI_Iprobe(MASTER, 1, MPI_COMM_WORLD, &waitflag, MPI_STATUS_IGNORE);
		} while (!waitflag);
		int node_start = 0;
		rc = MPI_Recv(&node_start, 1, MPI_INT, MASTER, TAG_GENERAL, MPI_COMM_WORLD, &Stat);

		// to wait
		do{
			MPI_Iprobe(MASTER, 1, MPI_COMM_WORLD, &waitflag, MPI_STATUS_IGNORE);

		} while (!waitflag);
		int node_rows = 0;
		rc = MPI_Recv(&node_rows, 1, MPI_INT, MASTER, TAG_GENERAL, MPI_COMM_WORLD, &Stat);

		// get processor name
		char processor_name[MPI_MAX_PROCESSOR_NAME];

		int name_len;

		MPI_Get_processor_name(processor_name, &name_len);

		printf("[%s]: from %d, to %d \n", processor_name, node_start, node_start + node_rows);
		Mat src = imread("pic.png", IMREAD_GRAYSCALE);
		Mat dst = src.clone();

		clock_t begin = clock();//to get time

		//filter	
		for (int x = node_start; x < node_start + node_rows; ++x){
			for (int y = 4; y < src.cols - 4; ++y){
				if (x > 4 && x < src.rows - 4){
					uchar a = (uchar)avrg(src, x - 1, y - 1);
					uchar b = (uchar)avrg(src, x + 1, y - 1);
					uchar c = (uchar)avrg(src, x - 1, y + 1);
					uchar d = (uchar)avrg(src, x + 1, y + 1);
					uchar ori;
					int dif;
					uchar res;
					ori = src.at<uchar>(x, y);
					dif = 255;
					if (abs(ori - a) < dif){
						res = a;
						dif = abs(ori - a);
					}
					if (abs(ori - b) < dif){
						res = b;
						dif = abs(ori - b);
					}
					if (abs(ori - c) < dif){
						res = c;
						dif = abs(ori - c);
					}
					if (abs(ori - d) < dif){
						res = d;
						dif = abs(ori - d);
					}
					dst.at<uchar>(x, y) = res;
					ori = src.at<uchar>(x, y - 1);
					dif = 255;
					if (abs(ori - a) < dif){
						res = a;
						dif = abs(ori - a);
					}
					if (abs(ori - b) < dif){
						res = b;
						dif = abs(ori - b);
					}
					dst.at<uchar>(x, y - 1) = res;
					ori = src.at<uchar>(x, y + 1);
					dif = 255;
					if (abs(ori - c) < dif){
						res = c;
						dif = abs(ori - c);
					}
					if (abs(ori - d) < dif){
						res = d;
						dif = abs(ori - d);
					}
					dst.at<uchar>(x, y + 1) = res;
					ori = src.at<uchar>(x + 1, y);
					dif = 255;
					if (abs(ori - b) < dif){
						res = b;
						dif = abs(ori - b);
					}
					if (abs(ori - d) < dif){
						res = d;
						dif = abs(ori - d);
					}
					dst.at<uchar>(x + 1, y) = res;
					ori = src.at<uchar>(x - 1, y);
					dif = 255;
					if (abs(ori - a) < dif){
						res = a;
						dif = abs(ori - a);
					}
					if (abs(ori - c) < dif){
						res = c;
						dif = abs(ori - c);
					}
					dst.at<uchar>(x - 1, y) = res;
				}
			}
		}

		clock_t end = clock(); //to get time
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		printf("[%s]:Time: %lf \n", processor_name, elapsed_secs);
		for (int x = node_start; x < node_start + node_rows; ++x){
			for (int y = 0; y < src.cols - 1; ++y){
				uchar ori = dst.at<uchar>(x, y);
				MPI_Send(&ori, 1, MPI_INT, MASTER, TAG_GENERAL, MPI_COMM_WORLD);
			}
		}
	}
	MPI_Finalize();
}

int avrg(Mat src, int x, int y){
	int res = 0;
	for (int i = -1; i < 2; ++i){
		for (int j = -1; j < 2; ++j){
			res += (int)src.at<uchar>(x + i, y + j);
		}
	}
	return res/9;
}
