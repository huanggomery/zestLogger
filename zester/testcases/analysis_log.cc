/* 用于分析test_multi_threads程序产生的日志文件 */
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;


int main(int argc, char **argv)
{
    if (argc < 2) {
        cout << "Need log file name" << endl;
        exit(-1);
    }
    ifstream ifs(argv[1]);
    if (!ifs.is_open()) {
        cout << "Can't open the file: " << argv[1] << endl;
        exit(-1);
    }

    string logline;
    int count = 0;
    unordered_map<int, vector<int>> um;

    while (getline(ifs, logline)) {
        int pos = logline.find("thread:");
        string thread_and_number = logline.substr(pos+8);
        pos = thread_and_number.find(' ');
        int thread = stoi(thread_and_number.substr(0, pos));
        int number = stoi(thread_and_number.substr(pos));
        um[thread].push_back(number);
        ++count;
    }

    for (auto &p : um) {
        bool is_break = false;
        int last_number = p.second.back();
        for (int i = 1; i < p.second.size(); ++i) {
            if (p.second[i] != p.second[i-1]+1)
                is_break = true;
        }
        cout << "thread: " << p.first << " count: " << p.second.size() 
             << " last_number: " << last_number << " is_break: " << is_break << endl; 
    }

    ifs.close();
}