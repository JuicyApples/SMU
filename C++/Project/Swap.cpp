#include "Swap.h"
#include "Market.h"

void Swap::generateSwapSchedule()
{
  if (startDate == maturityDate || frequency <= 0 || frequency > 1)
        throw std::runtime_error("Error: start date is later than end date, or invalid frequency!");
    
    string tenorStr;
    if (frequency == 0.25)
      tenorStr="3M";
    else if (frequency == 0.5) 
      tenorStr="6M";
    else
      tenorStr="1Y";
    
    Date seed = startDate;
    while(seed < maturityDate){
        swapSchedule.push_back(seed);
        seed=dateAddTenor(seed, tenorStr);
    }
    swapSchedule.push_back(maturityDate);
    if (swapSchedule.size()<2)
        throw std::runtime_error("Error: invalid schedule, check input!");

}

double Swap::Payoff(double s) const 
{
  //this is using annutiy to compute pv
  return (s - tradeRate) * getAnnuity();
}

double Swap::getAnnuity() const 
{
  return 0;
}

double Swap::Pv(const Market& mkt) const
{
  //using cash flow discunting
  auto thisMkt = const_cast<Market&>(mkt);
  double fixPv = 0;

  auto rateCurve = thisMkt.getCurve("usd-sofr");
  double df = rateCurve.getDf(maturityDate);
  double fltPv = (-notional + notional * df);
  for (auto& dt: swapSchedule) {
    if (dt == startDate)
      continue;
    double tau = dt - startDate;
    df = rateCurve.getDf(dt);
    fixPv += notional * tau * tradeRate * df;
  }

  return fixPv + fltPv;
}




