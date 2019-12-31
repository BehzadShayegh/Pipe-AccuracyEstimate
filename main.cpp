#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <unistd.h> 
#include <bits/stdc++.h> 
#include<sys/wait.h>
#include <sys/stat.h>
using namespace std;

#define batch 256

struct PIPE {int read; int write;};

int main(int argc, char** argv) {
    if(argc < 3) {
        cout << "too few arguments\n";
        exit(EXIT_FAILURE);
    }
    const char* dataset_path = argv[1];
    const char* weights_path = argv[2];

    int number_of_classifires = -2;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (weights_path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            // printf ("%s\n", ent->d_name);
            number_of_classifires++;
        }
    }

    vector<int> childs;
    vector<PIPE> pips;
    for(int i=0; i<number_of_classifires; i++) {
        int pip[2];
        if (pipe(pip) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        } else {
            PIPE newpip;
            newpip.read = pip[0];
            newpip.write = pip[1];
            pips.push_back(newpip);
        }

        int pid = fork();
        if(pid < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        } else if(pid==0) {
            string temp = to_string(pip[0]);
            if(execl("./classifier.out","./classifier.out", temp.c_str(), NULL) < 0) {
                perror("execl");
                exit(EXIT_FAILURE);
            }
        } else {
            string msg = string(weights_path) + "/classifier_" + to_string(i) + ".csv";
            write(pip[1], msg.c_str(), batch);
            msg = string(dataset_path) + "/dataset.csv";
            write(pip[1], msg.c_str(), batch);
            msg = "./result" + to_string(i) + ".csv";
            write(pip[1], msg.c_str(), batch);
            continue;
        }
    }

    while (wait(NULL) >= 0);
    for(int i=0; i<pips.size(); i++) {
        close(pips[i].read);
        close(pips[i].write);
    }

    int pip[2];
    if (pipe(pip) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if(pid < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    } else if(pid==0) {
        string temp = to_string(pip[0]);
        string noc = to_string(number_of_classifires);
        if(execl("./voter.out","./voter.out", temp.c_str(), noc.c_str(), NULL) < 0) {
            perror("execl2");
            exit(EXIT_FAILURE);
        }
    } else {
        string msg = "./";
        write(pip[1], msg.c_str(), batch);
        msg = "./final.csv";
        write(pip[1], msg.c_str(), batch);
    }

    if (wait(&pid) < 0) {
        perror("wait");
        exit(EXIT_FAILURE);
    }
    close(pip[0]);
    close(pip[1]);
    // --------------------------------------------------
    vector<int> result;
    string path = "./final.csv";

    ifstream ifs;
    mkfifo(path.c_str(), 0666);
    ifs.open(path);
    int r;
    while (ifs >> r)
        result.push_back(r);
    ifs.close();
    // --------------------------------------------------
    vector<int> label;
    path = string(argv[1])+"/labels.csv";

    ifs.open(path);
    ifs.close();
    mkfifo(path.c_str(), 0666);
    ifs.open(path);
    string trash;
    getline(ifs, trash);
    while (ifs >> r)
        label.push_back(r);
    ifs.close();
    // --------------------------------------------------
    int trues = 0;
    for(int i=0; i<result.size(); i++)
        if(result[i] == label[i])
            trues++;

    float acc = float(trues) * 100 / result.size();
    cout << "Accuracy: " << fixed << setprecision(2) << acc << "%\n";

    return 0;
}