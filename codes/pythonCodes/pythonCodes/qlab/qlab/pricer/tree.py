import math
import numpy

############ binomial pricer and different binomial models
def simpleCRR(r, vol, t):
    u = math.exp(vol * math.sqrt(t))
    p = (math.exp(r * t) - (1 / u)) / (u - 1 / u)
    return (u, 1 / u, p)

def crrCalib(r, vol, t):
    b = math.exp(vol * vol * t + r * t) + math.exp(-r * t)
    u = (b + math.sqrt(b * b - 4)) / 2
    p = (math.exp(r * t) - (1 / u)) / (u - 1 / u)
    return (u, 1/u, p)

def jrrnCalib(r, vol, t):
    u = math.exp((r - vol * vol / 2) * t + vol * math.sqrt(t))
    d = math.exp((r - vol * vol / 2) * t - vol * math.sqrt(t))
    p = (math.exp(r * t) - d) / (u - d)
    return (u, d, p)

def jreqCalib(r, vol, t):
    u = math.exp((r - vol * vol / 2) * t + vol * math.sqrt(t))
    d = math.exp((r - vol * vol / 2) * t - vol * math.sqrt(t))
    return (u, d, 1/2)

def tianCalib(r, vol, t):
    v = math.exp(vol * vol * t)
    u = 0.5 * math.exp(r * t) * v * (v + 1 + math.sqrt(v*v + 2*v - 3))
    d = 0.5 * math.exp(r * t) * v * (v + 1 - math.sqrt(v*v + 2*v - 3))
    p = (math.exp(r * t) - d) / (u - d)
    return (u, d, p)

def binomialPricer(S, r, vol, trade, n, calib):
    t = trade.expiry / n
    (u, d, p) = calib(r, vol, t)
    # set up the last time slice, there are n+1 nodes at the last time slice
    vs = [trade.payoff(S * u ** (n - i) * d ** i) for i in range(n + 1)]
    # iterate backward
    for i in range(n - 1, -1, -1):
        # calculate the value of each node at time slide i, there are i nodes
        for j in range(i + 1):
            nodeS = S * u ** (i - j) * d ** j
            continuation = math.exp(-r * t) * (vs[j] * p + vs[j + 1] * (1 - p))
            vs[j] = trade.valueAtNode(t * i, nodeS, continuation)
    return vs[0]

# binomial, with multi states, for path dependent products
def binomialPricerX(S, r, vol, trade, n, calib):
    t = trade.expiry / n
    (u, d, p) = calib(r, vol, t)
    # set up the last time slice, there are n+1 nodes at the last time slice
    vs = [trade.valueAtNode(trade.expiry, S * u ** (n - i) * d ** i, None) for i in range(n + 1)]
    numStates = len(vs[0])
    # iterate backward
    for i in range(n - 1, -1, -1):
        # calculate the value of each node at time slide i, there are i nodes
        for j in range(i + 1):
            nodeS = S * u ** (i - j) * d ** j
            continuation = [math.exp(-r * t) * (vs[j][k] * p + vs[j + 1][k] * (1 - p)) for k in range(numStates)]
            vs[j] = trade.valueAtNode(t * i, nodeS, continuation)
    return vs[0][0]



def calib2D(r, q1, q2, vol1, vol2, rho, t):
    sqrtt = math.sqrt(t)
    v1 = r - q1 - vol1 * vol1 / 2
    v2 = r - q2 - vol2 * vol2 / 2
    x1 = vol1 * sqrtt
    x2 = vol2 * sqrtt
    a = x1 * x2
    b = x2 * v1 * t
    c = x1 * v2 * t
    d = rho * vol1 * vol2 * t
    puu = (a + b + c + d)/4/a
    pud = (a + b - c - d)/4/a
    pdu = (a - b + c - d)/4/a
    pdd = (a - b - c + d)/4/a
    return (x1, x2, puu, pud, pdu, pdd)

def binomialPricer2D(S1, S2, r, q1, q2, vol1, vol2, rho, trade, n):
    t = trade.expiry / n
    (x1, x2, puu, pud, pdu, pdd) = calib2D(r, q1, q2, vol1, vol2, rho, t)
    vs = numpy.zeros(shape=(n+1, n+1))
    for i in range(n+1):
        s1i = S1 * math.exp(x1 * (n - 2 * i))
        for j in range(n+1):
            s2j = S2 * math.exp(x2 * (n - 2*j))
            vs[i, j] = trade.payoff(s1i, s2j)
    # iterate backward
    for k in range(n - 1, -1, -1):
        # calculate the value of each node at time slide k, there are (k+1) x (k+1) nodes
        for i in range(k + 1):
            s1i = S1 * math.exp(x1 * (k - 2*i))
            for j in range(k + 1):
                s2j = S2 * math.exp(x2 * (k - 2*j))
                continuation = math.exp(-r * t) * (vs[i, j] * puu + vs[i, j+1] * pud + vs[i+1, j] * pdu + vs[i+1, j+1] * pdd)
                vs[i, j] = trade.valueAtNode(t * k, s1i, s2j, continuation)
    return vs[0, 0]

def trinomialPricer(S, r, q, vol, trade, n, lmda):
    t = trade.expiry / n
    u = math.exp(lmda * vol * math.sqrt(t))
    mu = r - q
    pu = 1 / 2 / lmda / lmda + (mu - vol * vol / 2) / 2 / lmda / vol * math.sqrt(t)
    pd = 1 / 2 / lmda / lmda - (mu - vol * vol / 2) / 2 / lmda / vol * math.sqrt(t)
    pm = 1 - pu - pd
    # set up the last time slice, there are 2n+1 nodes at the last time slice
    # counting from the top, the i-th node's stock price is S * u^(n - i), i from 0 to n+1
    vs = [trade.payoff(S * u ** (n - i)) for i in range(2*n + 1)]
    # iterate backward
    for i in range(n - 1, -1, -1):
        # calculate the value of each node at time slide i, there are i nodes
        for j in range(2*i + 1):
            nodeS = S * u ** (i - j)
            continuation = math.exp(-r * t) * (vs[j] * pu +  + vs[j+1] * pm + vs[j+2] * pd)
            vs[j] = trade.valueAtNode(t * i, nodeS, continuation)
    return vs[0]