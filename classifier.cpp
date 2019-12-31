#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>
using namespace std;

#define batch 256

int main(int argc, char** argv) {
    int pip = atoi(argv[1]);
    // --------------------------------------------------------
    char c_path[batch+1];

    if(read(pip, c_path, batch) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    string path = string(c_path);

    ifstream ifs;
    ifs.open(path);
    ifs.close();
    mkfifo(c_path, 0666);
    ifs.open(path);

    string line;
    getline(ifs, line);

    vector< vector<float> > b;
    while (getline(ifs, line)) {
        replace(line.begin(),line.end(), ',', ' ');
        istringstream stream;
        stream.str(line);
        vector<float> bb;
        float bbb;
        while (stream >> bbb) {
            bb.push_back(bbb);
        }
        b.push_back(bb);
    }

    // for(int i=0; i<b.size(); i++) {
    //     for(int j=0; j<b[i].size(); j++)
    //         cout << b[i][j] << ' ';
    //     cout << endl;
    // }

    ifs.close();
    // --------------------------------------------------------
    // --------------------------------------------------------
    if(read(pip, c_path, batch) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    path = string(c_path);

    ifs.open(path);
    ifs.close();
    mkfifo(c_path, 0666);
    ifs.open(path);

    getline(ifs, line);

    vector< vector<float> > dataset;
    while (getline(ifs, line)) {
        replace(line.begin(),line.end(), ',', ' ');
        istringstream stream;
        stream.str(line);
        vector<float> bb;
        float bbb;
        while (stream >> bbb) {
            bb.push_back(bbb);
        }
        dataset.push_back(bb);
    }

    // for(int i=0; i<dataset.size(); i++) {
    //     for(int j=0; j<dataset[i].size(); j++)
    //         cout << dataset[i][j] << ' ';
    //     cout << endl;
    // }

    ifs.close();
    // --------------------------------------------------------
    // --------------------------------------------------------
    vector<int> result;
    for(int row=0; row < dataset.size(); row++) {
        vector<float> points;
        for(int c=0; c<b.size(); c++) {
            float sum = b[c][dataset[row].size()];
            for(int i=0; i<dataset[row].size(); i++)
                sum += dataset[row][i] * b[c][i];
            points.push_back(sum);
        }
        int max = 0;
        for(int c=0; c<points.size(); c++)
            if(points[c] > points[max])
                max = c;
        result.push_back(max);
    }
    // --------------------------------------------------------
    // --------------------------------------------------------
    if(read(pip, c_path, batch) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    path = string(c_path);

    ofstream ofs;
    ofs.open(path);
    ofs.close();
    mkfifo(c_path, 0666);
    ofs.open(path);

    for(int i=0; i<result.size(); i++) {
        ofs << result[i] << endl;
        // cout << result[i] << endl;
    }
    ofs.close();
    // --------------------------------------------------------
    close(pip);
    return 0;
}