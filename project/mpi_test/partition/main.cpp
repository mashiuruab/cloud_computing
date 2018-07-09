#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
    ifstream infile("input.csv");
    ofstream out("out.csv");

    while (!infile.eof()) {
        string line;
        infile >> line;
        out << line;
    }

    infile.close();
    out.close();
    return 0;
}