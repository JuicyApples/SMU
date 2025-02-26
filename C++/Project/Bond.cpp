#include "Bond.h"
#include "Market.h"

void Bond::generateSwapSchedule()
{
  //implement this

}

double Bond::Payoff(double s) const
{
  double pv = notional * (s - tradePrice);
  return pv;
}

double Bond::Pv(const Market& mkt) const
{
  //using cash flow discunting
  // implement this
  return 0;
}



