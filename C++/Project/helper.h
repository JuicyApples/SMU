#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <locale> // tolower


using namespace std;

void inline readFromFile(const string& fileName, string& outPut)
{
    string lineText;
    ifstream MyReadFile(fileName);
    while (getline (MyReadFile, lineText)) {
    outPut.append(lineText);
    }
    MyReadFile.close();
}

std::vector<std::string> inline split(std::string s, std::string delimiter) 
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
};

string inline to_lower(string s) 
{        
    for(char &c : s)
        c = tolower(c);
    return s;
}

/*
a simple code to generate swap or bond leg schedule
input start/end date in year fraction, frequency as double, eg. 3m = 0.25
*/

void inline genSchedule(double start, double end, double freq, vector<double>& schedule) 
{
    if (start >= end || freq <= 0 || freq > 1)
        throw std::runtime_error("Error: start date is later than end date, or invalid frequency!");
    double seed = end;
    while(seed > start){
        schedule.push_back(seed);
        seed-=freq;
    }
    schedule.push_back(start);
    if (schedule.size()<2)
        throw std::runtime_error("Error: invalid schedule, check input!");
    
    std::reverse(schedule.begin(), schedule.end());
}

#endif