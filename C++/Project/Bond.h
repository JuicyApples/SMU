#pragma once
#include "Trade.h"

class Bond : public Trade {
public:
    Bond(std::string name, Date start, Date end, double _notional, double rate, double freq): Trade("BondTrade", start) {
        underlying = name;
        notional = _notional;
        frequency = freq;
        coupon = rate;
    }
    double Payoff(double s) const; // implement this
    double Pv(const Market& mkt) const; // implement this
    void generateSwapSchedule(); //implement this

private:
    std::string underlying;
    double notional;
    double tradePrice;
    double coupon;
    double frequency;

    Date startDate;
    Date maturityDate;
    vector<Date> bondSchedule;

};