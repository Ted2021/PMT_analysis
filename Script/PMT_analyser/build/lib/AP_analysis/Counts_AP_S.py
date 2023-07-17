import ROOT as RT
import numpy as np
import pickle
import sys
import pandas as pd

def Generate_WFORM_File(file, file_wf, avg_pkl, tree_s = "Treesource_0", tree_wf="Treesource_0"):
    #マクロの読み込み
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Sakurai_method.h")
    av_wave = RT.std.vector(float)(np.array(pd.read_pickle(avg_pkl)))
    RT.CalcWform2(file, tree_s, file_wf, tree_wf, av_wave)
    print("##### Generate wform ROOT file!! #####")

def Counts_AP_S(file_wf, S_pd, PEAK_THES, CONTI_THES, INT_s, INT_e, avg_pkl, tree_wf="Treesource_0"):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Sakurai_method.h")
    av_wave = RT.std.vector(float)(np.array(pd.read_pickle(avg_pkl)))
    event = RT.std.vector(int)()
    seg = RT.std.vector(int)()
    peak = RT.std.vector(float)()
    counts = RT.std.vector(int)()
    #PEAK_THES = 20
    #CONTI_THES = 3
    RT.CountAPeventFromPH4(file_wf, tree_wf, 1024, PEAK_THES, CONTI_THES, INT_s, INT_e, av_wave, event, seg, peak, counts)


    df = pd.DataFrame({'event':np.array(event),
                      'seg':np.array(seg),
                      'peak':np.array(peak),
                      'counts':np.array(counts)
                     })
    df.to_pickle(S_pd)

    print("##### Count AP by S_method!! #####")
    