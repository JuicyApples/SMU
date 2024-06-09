from context import qlab

import numpy as np

from qlab.tradeable.types import PayoffType
from qlab.tradeable.european import EuropeanOption
from qlab.tradeable.asian import AsianOption
from qlab.tradeable.tarf import Tarf

from qlab.pricer.analytic import bsPrice
from qlab.pricer.tree import binomialPricer, crrCalib
from qlab.pricer.mcpricer import LocalVolMC, mcPricer, BlackScholesMC, DeterminsticIR
from qlab.pricer.pde import pdePricerX
from qlab.volsurf.impliedvol import FlatVol
from qlab.volsurf.localvol import LocalVol

assetCode = "ASSET1"
df = "DF.USD"

S = 100.0
K = 100.0
r = 0.01
q = 0.0
vol = 0.1
T = 1.0
opt = EuropeanOption(assetCode, T, K, PayoffType.Call)
payoff = lambda A: max(A - 100.0, 0)
asiaOpt = AsianOption(assetCode, "Stock1", [0.2, 0.4, 0.6, 0.8, 1.0], payoff, None, T)
targetGain = 10.0
tarfOpt = Tarf(assetCode, [0.2, 0.4, 0.6, 0.8, 1.0], payoff,targetGain)

def testBSMC():
    print("BS analytic European Price/Std : ", bsPrice(S, r, q, vol, opt.expiry, opt.strike, opt.payoffType))
    print("CRR binomial European Price/Std : ", binomialPricer(S, r, vol, opt, 500, crrCalib))
    models = {
        assetCode: BlackScholesMC(S, vol, r, q),
        df: DeterminsticIR(r),
    }
    corrMat = np.identity(1)
    npaths = 100# 256*1024
    print("BS mc European Price/Std ", mcPricer(opt, models, corrMat, npaths))
    print("BS mc asian Option Price/Std ", mcPricer(asiaOpt, models, corrMat, npaths))
    print("BS mc Tarf Price/Std ", mcPricer(tarfOpt, models, corrMat, npaths))
    
def testLVMC():
    flatvol = FlatVol(vol)
    lv = LocalVol(flatvol, S, r, q)
    print("pde lv price: ", pdePricerX(S, r, q, lv, NX = 100, NT = 100, w=0.5, trade=opt))

    lvmodels = {
        assetCode:LocalVolMC(S, T, lv, r, q),
        #assetCode: BlackScholesMC(S, vol, r, q),
        df: DeterminsticIR(r),
    }
    corrMat = np.identity(1)
    npaths = 1000# 256*1024
    print("mc LV European Price/Std price: ", mcPricer(opt, lvmodels, corrMat, npaths))
    print("mc LV Tarf Price/Std ", mcPricer(tarfOpt, lvmodels, corrMat, npaths))
testBSMC()
testLVMC()


