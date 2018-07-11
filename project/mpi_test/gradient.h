#ifndef GRADIENT_H
#define GRADIENT_H

#include <vector>
#include <iostream>
#include <cmath>

using std::vector;
using std::cout;
using std::endl;

class TrainingExample
{
    private:
        vector<double> features;
        double target;
    public:
        TrainingExample(vector<double>& feat, double tar)
        {
            features = feat;
            target = tar;
        }

        double getFeature(int i) { return features.at(i); }
        vector<double>& getFeatures() { return features; }

        double getTarget() { return target; }
};

class Hypothesis
{
    private:
        vector<double> theta;
        vector<TrainingExample> ts;
        unsigned mExamples, nFeatures;

        double H(vector<double>& ntheta, vector<double>& features)
        {
            //cout << "H(x) = ";
            double sum = 0.0;
            for (unsigned i = 0; i < nFeatures; i++)
            {
                //cout << ntheta[i] << "*" << features[i] << " ";
                sum += ntheta[i]*features[i];
            }
            //cout << " = " << sum << endl;
            return sum;
        }

        double J()
        {
            double sum = 0.0;
            for (unsigned i = 0; i < mExamples; i++)
            {
                double diff = H(theta, ts[i].getFeatures()) - ts[i].getTarget();
                sum += diff*diff;
            }
            return sum / 2.0;
        }

    public:
        Hypothesis(vector<TrainingExample>& examples) //  mashiur : I think it would not used from our code
        {
            nFeatures = 0;
            ts = examples;
            mExamples = examples.size();
            if (mExamples > 0)
                nFeatures = examples[0].getFeatures().size();
            theta = vector<double>(nFeatures, 30.0);
        }

        Hypothesis(vector<TrainingExample>& examples, vector<double> &acc_theta)
        {
            nFeatures = 0;
            ts = examples;
            mExamples = examples.size();
            if (mExamples > 0)
                nFeatures = examples[0].getFeatures().size();
            theta = acc_theta;
        }

        vector<double> gradientDescent()
        {
            const double alpha = 0.0000001;
            const double eps   = 0.00001;
            bool converge = false;
            int debug = 0;
            while (!converge)
            {
                vector<double> newTheta = theta;

                // # mashiur :: cout << "J(theta) = " << J() << endl << endl;

                for (unsigned i = 0; i < mExamples; i++)
                {
                    //cout << "Using example" << i << endl;

                    TrainingExample ex = ts[i];
                    double HH = H(newTheta, ex.getFeatures()); // mashiur: y_hat
                    double diff = ex.getTarget() - HH; // mashiur : y_target - y_hat
                    diff *= alpha;

                    //cout << ex.getTarget() << "(T) - " << HH <<"(H) = " << diff <<endl;
                    // # mashiur : stochastic gradient descent
                    for (unsigned j = 0; j < nFeatures; j++)
                        newTheta[j] += diff * ex.getFeature(j);

                    //for (int k = 0; k < newTheta.size(); k++)
                    //    cout << "newTh" << k << " = " << newTheta[k] << " ";
                    //cout << endl;

                }

                converge = true;
                for (unsigned i = 0; i < theta.size(); i++)
                    converge = converge && fabs(theta[i]-newTheta[i]) < eps;

                theta = newTheta;
            }
            return theta;
        }
};

#endif

