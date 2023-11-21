#ifndef CSVREADER_HPP
#define CSVREADER_HPP
#include "Node.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class FileReader {
public:
    DataFrame read(string filePath);
};

DataFrame FileReader::read(string filePath) {
    string line;
    DataFrame array;
    ifstream f(filePath);
    while (getline(f, line)) {
        string val;
        vector<int> row;
        stringstream s(line);
        while (getline(s, val, ','))
            row.push_back(stoi(val));
        array.push_back(row);
    }
    f.close();
    return array;
}

#endif /* CSVREADER_HPP */
