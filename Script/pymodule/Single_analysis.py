import sys
import numpy as np
sys.path.append("/Users/kiyomoto/reaserch/8dy_Noise/scripts/python/")
import pmtanalysis as pmt

def Calc_Single_Chrg(file, treesingle, treedark):
    PMT = pmt.Pmtanalysis(file)

    PMT.GetAverage(treesingle, "wform1", "wform0")
    PMT.GetAverage(treedark, "wform1", "wform0", "dark")

    peak = np.argmax(PMT.avg_wfs)
    print(peak-50)
    print(peak+75)
    
    single_hist = PMT.SingleChargeDist(treesingle, treedark, int(peak-50), int(peak+75), -20, 120)
    temp = PMT.SingleChargeFit(single_hist)

    return PMT.l_para[4] * 0.3