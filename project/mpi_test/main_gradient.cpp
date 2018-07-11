#include<mpi.h>
#include<stdio.h>
#include<random>
#include<vector>
#include<fstream>
#include "gradient.h"

using namespace std;

#define MASTER_RANK 0

int current_process_rank;

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
        vector<double> feat(N+1, 1);
        int y = 0;
        for (int j = 0; j < N; j++)
        {
            f >> feat[j+1];
            //feat[j+1] = current_process_rank + feat[j+1]; // TODO:: need to remove
        }
        f >> y;

        TrainingExample te(feat, y);
        ts.push_back(te);
    }
    f.close();

    return ts;
}

vector<double> getTrainedTheta(vector<TrainingExample> &ts, vector<double> &theta) {
    Hypothesis hyp(ts, theta);
    vector<double> new_theta = hyp.gradientDescent();

    return new_theta;
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
    current_process_rank = my_rank;

    const double eps   = 0.00001;

    vector<TrainingExample> ts = getData();
    vector<double> theta(ts[0].getFeatures().size(),  30.0);// initializing the weight vector with a random value

    bool global_converge = false;
    unsigned int global_iteration = 0;

    while (!global_converge && global_iteration < 10) {
        vector<double> local_converged_theta = getTrainedTheta(ts, theta); // send_buffer

        vector<double> acc_theta;
        acc_theta.resize(number_of_process * local_converged_theta.size());

        MPI_Allgather(&local_converged_theta[0], local_converged_theta.size(), MPI_DOUBLE,
                      &acc_theta[0], local_converged_theta.size(), MPI_DOUBLE, MPI_COMM_WORLD);

        vector<double> avg_converged_theta = get_avg(acc_theta, number_of_process);

        //print_buffer(avg_converged_theta, my_rank);

        bool avg_theta_converge = true;
        for (unsigned i = 0; i < avg_converged_theta.size(); i++)
            avg_theta_converge = avg_theta_converge && fabs(avg_converged_theta[i] - local_converged_theta[i]) < eps;

        /* TODO:: allreduce the avg_theta_converge result of each machine of the cluster
         * Then based on that decide whether to loop through again or not variable "global_converge"
         * should be updated by the allreduce mpi function
         * */

        /*TODO :: debug message  need to remove or comment out*/
        cout << "Global Iteration = " << global_iteration << endl;
        cout << "In rank = " << my_rank << ", acc weight converged  " << avg_theta_converge << endl;

        MPI_Allreduce(&avg_theta_converge, &global_converge, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);

        theta = avg_converged_theta;
        global_iteration++;
    }

    if(my_rank == MASTER_RANK) {
        cout << endl;
        for (size_t i = 0; i < theta.size(); i++)
            cout << "th" << i << " = " << theta[i] << " ";
        cout << endl;
    }

    MPI_Finalize();

    return 0;
}
