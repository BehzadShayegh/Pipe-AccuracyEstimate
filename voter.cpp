#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <unistd.h> 
#include <bits/stdc++.h> 
#include <sys/stat.h>
using namespace std;

#define batch 256

int max(vector<int> a) {
    int m = 0;
    for(int i=0; i<a.size(); i++)
        if(a[i]>m)
            m = a[i];
    return m;
}

int argmax(int* a, int size) {
    int m = 0;
    for(int i=0; i<size; i++)
        if(a[i]>a[m])
            m = i;
    return m;
}

int vote(vector<int> a) {
    int m = max(a) + 1;
    int range[m] = {0};
    for(int i=0; i<a.size(); i++)
        range[a[i]]++;
    return argmax(range, m);
}


int main(int argc, char** argv) {
    int pip = atoi(argv[1]);
    int number_of_classifiers = atoi(argv[2]);
    // --------------------------------------------------------
    char c_path[batch+1];

    if(read(pip, c_path, batch) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    vector< vector<int> > results;
    for(int i=0; i<number_of_classifiers; i++) {
        vector<int> result;
        string path = string(c_path) + "result" + to_string(i) + ".csv";
        
        ifstream ifs;
        mkfifo(c_path, 0666);
        ifs.open(path);
        int r;
        while (ifs >> r)
            result.push_back(r);
        results.push_back(result);
        ifs.close();
    }

    // for(int i=0; i<number_of_classifiers; i++) {
    //     for(int j=0; j<1000; j++)
    //         cout << results[i][j] << ' ';
    //     cout << endl;
    // }

    vector<int> final_vote;
    for(int i=0; i<results[0].size(); i++) {
        vector<int> forvote;
        for(int j=0; j<number_of_classifiers; j++)
            forvote.push_back(results[j][i]);
        final_vote.push_back(vote(forvote));
    }

    // --------------------------------------------------------
    // --------------------------------------------------------

    if(read(pip, c_path, batch) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    string path = string(c_path);

    ofstream ofs;
    ofs.open(path);
    ofs.close();
    mkfifo(c_path, 0666);
    ofs.open(path);

    for(int i=0; i<final_vote.size(); i++) {
        ofs << final_vote[i] << endl;
        // cout << final_vote[i] << endl;
    }
    ofs.close();
    close(pip);

    return 0;
}