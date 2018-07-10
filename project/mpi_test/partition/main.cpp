#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

#define LINE_PER_FILE 100000

void csd_read(string file_name) { // comma separated data read
    ifstream file(file_name.c_str());
    ofstream fout;

    string line;
    unsigned int line_count = 0;
    unsigned int chunk_count = 0;

    while(getline(file,line))
    {
        if((line_count % LINE_PER_FILE) == 0) {
            if(fout.is_open()) {
                fout.close();
            }

            chunk_count++;
            string file_name = to_string(chunk_count);
            fout.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
        }
        istringstream linestream(line);
        string value;

        while(getline(linestream,value,','))
        {
            fout<< value << " ";
        }

        fout<< endl;

        line_count++;
    }

    if(fout.is_open()) {
        fout.close();
    }


    file.close();
}

int main(int argc, char** argv) {
    string  file_name = "sudeste.csv";

    csd_read(file_name);
    return 0;
}