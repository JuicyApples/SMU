#ifndef DATE_H
#define DATE_H

#include <iostream>
#include "helper.h"

class Date
{
public:
  int year;
  int month;
  int day;
  Date(int y, int m, int d) : year(y), month(m), day(d) {};
  Date(){};
};

//this function return the year franction between 2 date object
double operator-(const Date& d1, const Date& d2);
bool operator>(const Date& d1, const Date& d2);
bool operator<(const Date& d1, const Date& d2);
bool operator==(const Date& d1, const Date& d2);
std::ostream& operator<<(std::ostream& os, const Date& d);
std::istream& operator>>(std::istream& is, Date& d);
Date dateAddTenor(const Date& start, const string& tenorStr);

#endif
