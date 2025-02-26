#ifndef MARKET_H
#define MARKET_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include "Date.h"

using namespace std;

class RateCurve {
public:
  RateCurve(){};
  RateCurve(const string& _name): name(_name) {} ; 
  void addRate(Date tenor, double rate);
  void shock(Date tenor, double value) {}; //implement this
  double getRate(Date tenor) const; //implement this function using linear interpolation
  double getDf(Date _date) const; // using df = exp(-rt), and r is getRate function
  void display() const;
  
  std::string name;
  Date _asOf;//same as market data date

private:
  vector<Date> tenors;
  vector<double> rates; //zero coupon rate or continous compounding rate
};

class VolCurve { // atm vol curve without smile
public:
  VolCurve(){}
  VolCurve(const string& _name): name(_name) {} ; 
  void addVol(Date tenor, double rate); //implement this
  double getVol(Date tenor) const; //implement this function using linear interpolation
  void display() const; //implement this
  void shock(Date tenor, double value); //implement this
  
  string name;
  Date _asOf;

private:
  vector<Date> tenors;
  vector<double> vols;
};

class Market
{
public:
  Date asOf;
  Market(){
    cout<< "default constructor is called" <<endl;
  };
  Market(const Date& now): asOf(now) {};
  Market(const Market& mkt){
    this->asOf = mkt.asOf;
    //etc


  } //implement this

  void Print() const;
  void addCurve(const std::string& curveName, const RateCurve& curve);//implement this
  void addVolCurve(const std::string& curveName, const VolCurve& curve);//implement this
  void addBondPrice(const std::string& bondName, double price);//implement this
  void addVolCurve(const std::string& stockName, double price);//implement this
  
  inline void shockPrice(const string& underlying, double shock) { stockPrices[underlying] +=shock; }
  inline RateCurve getCurve(const string& name) { return curves[name]; };
  inline VolCurve getVolCurve(const string& name) { return vols[name]; };


private:
  unordered_map<string, VolCurve> vols;
  unordered_map<string, RateCurve> curves;
  unordered_map<string, double> bondPrices;
  unordered_map<string, double> stockPrices;

};

std::ostream& operator<<(std::ostream& os, const Market& obj);
std::istream& operator>>(std::istream& is, Market& obj);

#endif
