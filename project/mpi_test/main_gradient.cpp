#include<mpi.h>
#include<stdio.h>
#include<random>
#include<vector>
#include<fstream>
#include "gradient.h"

using namespace std;

#define BATCH_SIZE 10

void print_buffer(vector<double> &recv_buffer, int rank) {
    string file_name = to_string(rank) + ".txt";

    remove(file_name.c_str());

    ofstream file(file_name);

    for(int i=0;i<recv_buffer.size();i++){
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

vector<double> get_avg(vector<double> &acc_theta, int &number_of_process) {
    int batch_size = acc_theta.size() / number_of_process;

    vector<double> result;
    result.resize(batch_size);

    for(int i=0;i<batch_size;i++) {
        double sum = 0.0;
        int offset = 0;
        for(int j=0;j<number_of_process;j++) {
            sum += acc_theta[i + offset];
            offset += batch_size;
        }

        result[i] = sum / (double) number_of_process;
    }

    return result;
}

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);

    int number_of_process;
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    vector<double> theta = getTrainedTheta(); // send_buffer

    cout << endl;
    for (size_t i = 0; i < theta.size(); i++)
        cout << "th" << i << " = " << theta[i] << " ";
    cout << endl;

    vector<double> acc_theta;
    acc_theta.resize(number_of_process * theta.size());

    MPI_Allgather(&theta[0], theta.size(), MPI_DOUBLE, &acc_theta[0], theta.size(), MPI_DOUBLE, MPI_COMM_WORLD);

    vector<double> avg_theta = get_avg(acc_theta, number_of_process);

    print_buffer(avg_theta, my_rank);


    MPI_Finalize();

    return 0;
}
