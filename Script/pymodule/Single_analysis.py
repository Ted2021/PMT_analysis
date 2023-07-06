import sys
import numpy as np
sys.path.append("/Users/kiyomoto/reaserch/8dy_Noise/scripts/python/")
import pmtanalysis as pmt
import matplotlib.pyplot as plt

def Calc_Single_Chrg(file, treesingle, treedark):
    PMT = pmt.Pmtanalysis(file)

    PMT.GetAverage(treesingle, "wform1", "wform0")
    PMT.GetAverage(treedark, "wform1", "wform0", "dark")

    peak = np.argmax(PMT.avg_wfs)
    #print(peak-50)
    #print(peak+75)

    fig,ax = plt.subplots()
    plt.plot(PMT.time, PMT.avg_wfs-PMT.avg_wfd, label = "Avg wfrom")
    ax.axvspan(PMT.time[peak-50], PMT.time[peak+75], color="gray", alpha=0.15, label = "Int range")
    plt.xlim(PMT.time[peak-100], PMT.time[peak+100])
    plt.xlabel("time (ns)")
    plt.ylabel("Vol (mV)")
    plt.legend()
    plt.show()
    
    single_hist = PMT.SingleChargeDist(treesingle, treedark, int(peak-50), int(peak+75), -20, 120)
    temp = PMT.SingleChargeFit(single_hist)
    PMT.SingleGain(temp[0])
    hh = PMT.SingleChargePlot(temp[0])
    #hh.show()
    print("0.3pe: " + str(PMT.l_para[4] * 0.3))
    return hh