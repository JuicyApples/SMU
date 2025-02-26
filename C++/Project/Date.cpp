#include "Date.h"

//this function return the year franction between 2 date object
double operator-(const Date& d1, const Date& d2)
{
  int yearDiff = d1.year - d2.year;
  int monthDiff = (d1.month - d2.month);
  int dayDiff = d1.day - d2.day;
  return yearDiff + monthDiff / 12.0 + dayDiff / 360.0;
};

bool operator>(const Date& d1, const Date& d2)
{
  double gap = d1 - d2;
  if (gap > 0)
    return true;
  else
    return false;
};

bool operator<(const Date& d1, const Date& d2)
{
  double gap = d1 - d2;
  if (gap < 0)
    return true;
  else
    return false;
};

bool operator==(const Date& d1, const Date& d2)
{
  double gap = d1 - d2;
  if (gap == 0)
    return true;
  else
    return false;
};

std::ostream& operator<<(std::ostream& os, const Date& d)
{
  os << d.year << " " << d.month << " " << d.day << std::endl;
  return os;
}

std::istream& operator>>(std::istream& is, Date& d)
{
  is >> d.year >> d.month >> d.day;
  return is;
}

Date dateAddTenor(const Date& start, const string& tenorStr)
{
  Date newdate = start;
  auto tenorUnit = tenorStr.back();
  int numUnit = stoi(tenorStr.substr(0, tenorStr.size()-1));
  
  if (to_lower(tenorStr) == "on" || to_lower(tenorStr) == "o/n"){
    newdate.day +=1;
    if (newdate.day > 30){
      newdate.month +=1;
      newdate.day=1;
    }
  }
  else if (tenorUnit == 'W'){
    int newDay = newdate.day + numUnit * 7;
    if (newDay > 30) {
      newdate.month +=1;
      newDay = newDay-30;
      newdate.day=newDay;
    }
  }
  else if(tenorUnit == 'M'){
    int newMonth = newdate.month + numUnit;
    if (newMonth > 12) {
      newdate.year +=1;
      newMonth = newMonth-12;
      newdate.month=newMonth;
    }
  }
  else if(tenorUnit =='Y'){
    newdate.year += numUnit;
  }
  else
    throw std::runtime_error("Error: found unsupported tenor: "+ tenorStr);
  
  return newdate;
}