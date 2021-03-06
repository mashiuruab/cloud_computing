#include<mpi.h>
#include<stdio.h>
#include<random>
#include<vector>
#include<fstream>
#include "gradient.h"

using namespace std;

#define BATCH_SIZE 10

void generate_data(double *data, int rank) {
    double lower_bound = 0;
    double upper_bound = BATCH_SIZE;
    std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
    std::default_random_engine re;

    for(int i=0;i<upper_bound;i++) {
        double a_random_double = unif(re);
        data[i] = double(rank * 100) + a_random_double;
    }
}

void print_buffer(double* recv_buffer, int data_size, int rank) {
    string file_name = to_string(rank) + ".txt";

    remove(file_name.c_str());

    ofstream file(file_name);

    for(int i=0;i<data_size;i++){
        file<<recv_buffer[i]<<endl;
    }

    file.close();
}

vector<TrainingExample> getData() {
    int M = 0, N = 0;
    vector<TrainingExample> ts;
    ifstream f;
    f.open("data.txt");
    if (!f.is_open())
    {
        cout << "File not opened" << endl;
        exit(-1);
    }
    f >> M >> N;
    cout << "M = " << M << ", N = " << N << endl;
    for (int i = 0; i < M; i++)
    {
        vector<int> feat(N+1, 1);
        int y = 0;
        for (int j = 0; j < N; j++)
            f >> feat[j+1];
        f >> y;

        TrainingExample te(feat, y);
        ts.push_back(te);
    }
    f.close();

    return ts;
}

vector<double> getTrainedTheta() {
    vector<TrainingExample> ts = getData();

    Hypothesis hyp(ts);
    vector<double> theta = hyp.gradientDescent();

    return theta;
}

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);

    int number_of_process;
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    double* send_buffer;
    send_buffer = (double*) malloc(BATCH_SIZE * sizeof(double));
    generate_data(send_buffer, my_rank);

    double* recv_buffer = (double*) malloc(number_of_process * BATCH_SIZE * sizeof(double));

    MPI_Allgather(send_buffer, BATCH_SIZE, MPI_DOUBLE, recv_buffer, BATCH_SIZE, MPI_DOUBLE, MPI_COMM_WORLD);

    print_buffer(recv_buffer, number_of_process * BATCH_SIZE, my_rank);

    free(send_buffer);
    free(recv_buffer);

    MPI_Finalize();

    if(my_rank == 0) {
        vector<double> theta = getTrainedTheta();

        cout << endl;
        for (size_t i = 0; i < theta.size(); i++)
            cout << "th" << i << " = " << theta[i] << " ";
        cout << endl;
    }

    return 0;
}
