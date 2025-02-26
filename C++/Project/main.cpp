#include <fstream>
#include <ctime>
#include <chrono>

#include "Market.h"
#include "Pricer.h"
#include "RiskEngine.h"
#include "Factory.h"
#include "thread_pool.h"

using namespace std;

int main()
{
  //task 1, create an market data object, and update the market data from from txt file
  // load 2 days market data
  std::time_t t = std::time(0);
  auto now_ = std::localtime(&t);
  Date valueDate;
  valueDate.year = now_->tm_year + 1900;
  valueDate.month = now_->tm_mon + 1;
  valueDate.year = now_->tm_mday;

  Market mkt = Market(valueDate);
  /*
  load data from file and update market object with data
  */

  //task 2, create a portfolio of bond, swap, european option, american option
  vector<std::shared_ptr<Trade>> myPortfolio;
  
  auto bFactory = std::make_unique<BondFactory>();
  auto sFactory = std::make_unique<SwapFactory>();
  auto eFactory = std::make_unique<EurOptFactory>();
  auto aFactory = std::make_unique<AmericanOptFactory>();
  auto bond = bFactory->createTrade("usd-gpv",Date(2024, 1, 1), Date(2034, 1, 1),1000000, 0.035, 0.6, OptionType::None);
  auto swap = sFactory->createTrade("usd-sofr",Date(2024, 1, 1), Date(2034, 1, 1),-1000000, 0.03, 1.0, OptionType::None);
  auto eCall = eFactory->createTrade("appl", Date(2024, 1, 1), Date(2025, 1, 1), 10000, 530, 0,OptionType::Call);
  auto aPut = aFactory->createTrade("appl", Date(2024, 1, 1), Date(2026, 1, 1), 10000, 525, 0,OptionType::Put);

  myPortfolio.push_back(bond);
  myPortfolio.push_back(swap);
  myPortfolio.push_back(eCall);
  myPortfolio.push_back(aPut);

  //task 3, creat a pricer and price the portfolio, output the pricing result of each deal 
  //3.1 compute the NPV of deal as of market date 1
  //3.2 compute the NPV of deal as of market date 2, and then compute the daily Pnl for each deal uisng NPV(date2) - NPV (date1), and output the result in file
  auto pricer = new CRRBinomialTreePricer(100);
  for (size_t i = 0; i<myPortfolio.size(); i++) {
    auto& trade = myPortfolio[i];
    double pv = pricer->Price(mkt, trade);
    //log pv details out in a file
  }

  //task 4, compute the Greeks of DV01, and Vega risk as of market date 1
  // 4.1 compute risk using risk engine
  // 4.2 use idea of multi-threading
  // analyzing the risk of PnL and risk of your portfolio and come out some simple strategy of a) square off the risk, b) maximize the PnL

  // sample code for risk computation
  double curve_shock = 0.0001;// 1 bp of zero rate
  double vol_shock = 0.01; //1% of log normal vol
  double price_shock = 1.0; // shock in abs price of stock
  
  
  // example 1, simple example of computing one point dv01 for one swap
  string risk_id = "usd-sofr:on";
  double shockUp = 0.0001;
  double shockDown = -0.0001;
  auto testShockUp = MarketShock();
  testShockUp.market_id ="usd-sofr";
  testShockUp.shock = make_pair(Date(), shockUp);
  auto testShockDown = MarketShock();
  testShockDown.market_id ="usd-sofr";
  testShockDown.shock = make_pair(Date(), shockDown);

  auto shockedUpCurveUp = CurveDecorator(mkt, testShockUp);
  auto shockedUpCurveDown = CurveDecorator(mkt, testShockDown);
  
  unordered_map<string, double> thisDealDv01;
  double pv_up, pv_down;
  auto m_up = shockedUpCurveUp.getMarketUp();
  pv_up = swap->Pv(m_up);
  auto m_down = shockedUpCurveDown.getMarketDown();
  pv_down = swap->Pv(m_down);
  double dv01 = (pv_up - pv_down)/2.0;
  thisDealDv01.emplace(risk_id, dv01);

  //example2, using risk engine to compute full set of dv01 for a swap
  RiskEngine re(mkt, curve_shock, vol_shock, price_shock);
  re.computeRisk("dv01", swap, true);
  auto dv01_of_swap = re.getResult();

  //example 3, demo using thread pool
  ThreadPool pool(4); 
  map<string, double> swapDv01;
  auto pv_job = [&swapDv01, risk_id, &swap, &m_up, &m_down]() {
    cout << "Task is running on thread: " << this_thread::get_id() << endl; 
    auto pricer = std::make_unique<CRRBinomialTreePricer>(100);
    double pv_u = pricer->Price(m_up, swap);
    double pv_d = pricer->Price(m_down, swap);
    double dv01 = (pv_u - pv_d) / 2.;
    swapDv01.emplace(std::make_pair(risk_id, dv01));
    this_thread::sleep_for(chrono::milliseconds(100)); 
  }; 

  for (int i = 0; i < 5; ++i) { 
      pool.enqueue(pv_job); 
  } 
  
  //final
  cout << "Project build successfully!" << endl;
  
  return 0;

}
