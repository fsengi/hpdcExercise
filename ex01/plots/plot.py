import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import ScalarFormatter

def moore():
    
    x = np.linspace(0,26,1000)
    y=func(415.53e15,x)

    plt.figure()
    plt.plot(x, y, label="R [Flop/s]")
    plt.gca().yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
    plt.hlines(1e18, 0, 26, linestyle='--', color='k', label="Rmax [Flop/s]")
    plt.title("Time until Exaflop starting June 2020")
    plt.xlabel("time [months]")
    plt.ylabel("Performance [Flop/s]")
    plt.legend()
    plt.savefig("pic/Moore.pdf")

def func(N, t):
    return N * 2**((1/18) * t) 

def predict():
    import datetime as dt
    from matplotlib import dates as mdt
    Rmax_2011 = 10510e12
    Rmax_2007 = 478.2e12
    x_data = [dt.date(2007,11,1), dt.date(2011,11,1)]
    y_data = [Rmax_2007, Rmax_2011]

    # get growth rate by linear fit to log values (ignore statistical nonsense due
    # to non-weightetness for small values... just an approximation)
    params = np.polyfit([2007,2011], [np.log(Rmax_2007), np.log(Rmax_2011)], 1)
    poly = np.poly1d(params)

    yfit = lambda x: np.exp(poly(x)) # transform values back from log domain
    x = [dt.date(2007,11,1),dt.date(2020,11,1)]
    y = [yfit(2007), yfit(2020)]

    plt.figure()
    plt.plot(x, y, label="Linear extrapolation")
    plt.plot(x_data, y_data, marker='x', linestyle='none', color='k', label="Rmax [Flop/s]")
    plt.hlines(1e18,x[0], x[1], linestyle='--', color='k')
    plt.yscale('log')
    plt.title("Extrapolating from past performance to Exaflop era")
    plt.xlabel("Year")
    plt.ylabel("Performance [Flop/s]")
    plt.legend()
    plt.savefig("pic/Prediction.pdf")


if __name__ == "__main__":
    moore()
    predict()