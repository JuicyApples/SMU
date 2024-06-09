
class Tarf:
    def __init__(self, assetName, fixings, payoffFun, targetGain):
        self.fixings = fixings
        self.payoffFun = payoffFun
        self.assetName = assetName
        self.targetGain = targetGain
    def AllDates(self):
        return self.fixings
    def DiscountedMCPayoff(self, fobs):
        df = fobs["DF.USD"](self.fixings[-1])
        accum, discountedPO = 0, 0
        for t in self.fixings:
            df = fobs["DF.USD"](t)
            po = self.payoffFun(fobs[self.assetName](t))
            accum += po
            discountedPO += df * po
            if (accum > self.targetGain):
                break # triggers knockout
        return discountedPO
    def assetNames(self):
        return [self.assetName, "DF.USD"]