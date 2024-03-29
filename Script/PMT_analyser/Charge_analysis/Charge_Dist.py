import ROOT as RT
import numpy as np
import pickle
import sys

def Calc_Charge(file, dfile, t_min, t_max, name_c,
                treename = "Treesource_0", treedark = "Treedark_0",
                branch_wf_s = "wform", branch_wf_d = "wform", seg = 1024, mode = "multipe", 
                ped_min = 20, ped_max = 50):

    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/Charge_analysis/Make_Chrg_Dist.h")
    chrg = RT.std.vector(float)()
    if mode == "multipe":
        RT.Calc_Chrg(file, dfile, chrg, treename, treedark, branch_wf_s, branch_wf_d, t_min, t_max, seg)
    elif mode == "ap":
        RT.Calc_Chrg_Nodark(file, chrg, treename, branch_wf_s, t_min, t_max, ped_min, ped_max, seg)
    else:
        print("Error!")

    with open(name_c, "wb") as f:
        pickle.dump(np.array(chrg), f)
    print("##### Create chrg files #####")

    return chrg

def Plot_Hist(chrg, hist_min, hist_max, Title = "hst", bins = 330):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/Charge_analysis/Make_Chrg_Dist.h")
    hst = RT.Make_Charge_Hist(chrg, Title, bins, hist_min, hist_max)
    print("##### Create Histgram! #####")

    return hst