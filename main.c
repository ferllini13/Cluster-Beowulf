// Header File of mpi library
#include <mpi.h>

// Common input/output and variable types in C++
#include <stdio.h>
#include <stdlib.h>

// Time Measure Library
#include <ctime>

// OpenCV Library
#include <opencv2/opencv.hpp>

using namespace cv;

// Computes the average in a 9 pixels area
int avrg(Mat src, int x, int y);
int main(int argc, char *argv[])
{

	// Master node is always 0
	const int MASTER = 0;

	// Common nodes variables
	const int TAG_GENERAL = 1;
	int rank;
	int source;
	int dest;
	int rc;
	int
		count;
	int dataWaitingFlag;

	MPI_Status Stat;

	// Initialize the MPI stack and pass 'argc' and 'argv' to each slave node
	MPI_Init(&argc, &argv);

	// Gets number of tasks/processes that this program is running on
	int numTasks;
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

	// Gets the rank (process/task number) that this program is running on
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// If the master node
	if (rank == MASTER)
	{
		char imagen[] = "fullHD.jpg";

		Mat src = imread(imagen, IMREAD_GRAYSCALE);

		int rows = src.rows;
		int rows_div = (int)(rows / (numTasks - 1));

		int work_count = 0;

		// Send out messages to all the sub-processes
		for (dest = 1; dest < numTasks; dest++)
		{
			int start = work_count;
			work_count += rows_div;
			MPI_Send(&start, 1, MPI_INT, dest, TAG_GENERAL, MPI_COMM_WORLD);
			MPI_Send(&rows_div, 1, MPI_INT, dest, TAG_GENERAL, MPI_COMM_WORLD);
		}
		clock_t begin = clock();

		// Receive all messages from the sub-processes
		for (int x = 0; x < rows_div; x++)
		{
			for (int y = 0; y < src.cols - 1; y++)
			{
				for (dest = 1; dest < numTasks; dest++)
				{
					int rec = 0;
					MPI_Recv(&rec, 1, MPI_INT, dest, TAG_GENERAL, MPI_COMM_WORLD, &Stat);
					src.at<uchar>((rows_div * (dest - 1)) + x, y) = (uchar)rec;
				}
			}
		}

		clock_t end = clock();
		double elapsed_secs_wc = double(end - begin) / CLOCKS_PER_SEC;
		printf("elapsed time with comunication: %lf \n", elapsed_secs_wc);
		// Save image in disk
		imwrite("fullHD_filt.jpg", src);
	}

	// Else a slave node
	else
	{
		// Wait until a message is there to be received
		do
		{
			MPI_Iprobe(MASTER, 1, MPI_COMM_WORLD, &dataWaitingFlag, MPI_STATUS_IGNORE);
		} while (!dataWaitingFlag);
		int node_start = 0;
		rc = MPI_Recv(&node_start, 1, MPI_INT, MASTER, TAG_GENERAL, MPI_COMM_WORLD, &Stat);

		// Wait until a message is there to be received
		do
		{
			MPI_Iprobe(MASTER, 1, MPI_COMM_WORLD, &dataWaitingFlag, MPI_STATUS_IGNORE);

		} while (!dataWaitingFlag);
		int node_rows = 0;
		rc = MPI_Recv(&node_rows, 1, MPI_INT, MASTER, TAG_GENERAL, MPI_COMM_WORLD, &Stat);

		// Get the name of the processor
		char processor_name[MPI_MAX_PROCESSOR_NAME];

		int name_len;

		MPI_Get_processor_name(processor_name, &name_len);

		printf("[%s]: trabajando desde la fila %d, hasta %d \n", processor_name, node_start, node_start + node_rows);

		String imagen = "fullHD.jpg";

		Mat src = imread(imagen, IMREAD_GRAYSCALE);

		Mat dst = src.clone();

		clock_t begin = clock();

		// KUWAHARA FILTER
		for (int x = node_start; x < node_start + node_rows; x++)
		{
			for (int y = 4; y < src.cols - 4; y++)
			{
				if (x > 4 && x < src.rows - 4)
				{
					uchar a = (uchar)avrg(src, x - 1, y - 1);
					uchar b = (uchar)avrg(src, x + 1, y - 1);
					uchar c = (uchar)avrg(src, x - 1, y + 1);
					uchar d = (uchar)avrg(src, x + 1, y + 1);
					uchar ori;
					int dif;
					uchar res;
					ori = src.at<uchar>(x, y);
					dif = 255;
					if (abs(ori - a) < dif)
					{
						res = a;
						dif = abs(ori - a);
					}
					if (abs(ori - b) < dif)
					{
						res = b;
						dif = abs(ori - b);
					}
					if (abs(ori - c) < dif)
					{
						res = c;
						dif = abs(ori - c);
					}
					if (abs(ori - d) < dif)
					{
						res = d;
						dif = abs(ori - d);
					}
					dst.at<uchar>(x, y) = res;
					ori = src.at<uchar>(x, y - 1);
					dif = 255;
					if (abs(ori - a) < dif)
					{
						res = a;
						dif = abs(ori - a);
					}
					if (abs(ori - b) < dif)
					{
						res = b;
						dif = abs(ori - b);
					}
					dst.at<uchar>(x, y - 1) = res;
					ori = src.at<uchar>(x, y + 1);
					dif = 255;
					if (abs(ori - c) < dif)
					{
						res = c;
						dif = abs(ori - c);
					}
					if (abs(ori - d) < dif)
					{
						res = d;
						dif = abs(ori - d);
					}
					dst.at<uchar>(x, y + 1) = res;
					ori = src.at<uchar>(x + 1, y);
					dif = 255;
					if (abs(ori - b) < dif)
					{
						res = b;
						dif = abs(ori - b);
					}
					if (abs(ori - d) < dif)
					{
						res = d;
						dif = abs(ori - d);
					}
					dst.at<uchar>(x + 1, y) = res;
					ori = src.at<uchar>(x - 1, y);
					dif = 255;
					if (abs(ori - a) < dif)
					{
						res = a;
						dif = abs(ori - a);
					}
					if (abs(ori - c) < dif)
					{
						res = c;
						dif = abs(ori - c);
					}
					dst.at<uchar>(x - 1, y) = res;
				}
			}
		}

		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		printf("[%s]: elapsed time: %lf \n", processor_name, elapsed_secs);
		for (int x = node_start; x < node_start + node_rows; x++)
		{
			for (int y = 0; y < src.cols - 1; y++)
			{
				uchar ori = dst.at<uchar>(x, y);
				MPI_Send(&ori, 1, MPI_INT, MASTER, TAG_GENERAL, MPI_COMM_WORLD);
			}
		}
	}
	MPI_Finalize();
}

int avrg(Mat src, int x, int y)
{
	int res = 0;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			res += (int)src.at<uchar>(x + i, y + j);
		}
	}
	return res / 9;
}
