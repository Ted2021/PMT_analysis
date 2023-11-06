import ROOT as RT
import numpy as np
import pickle
import sys
import pandas as pd
import os

def Generate_WFORM_File(file, file_wf, avg_pkl, tree_s = "Treesource_0", tree_wf="Treesource_0"):
    #マクロの読み込み
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Sakurai_method.h")
    av_wave = RT.std.vector(float)(np.array(pd.read_pickle(avg_pkl)))
    RT.CalcWform2(file, tree_s, file_wf, tree_wf, av_wave)
    print('\033[31m' + "##### Generate wform ROOT file!! #####" + '\033[0m')

def Generate_WFORM_branch(file, tree, avg_pkl, length = 1024, branch_wf = "wform", name2 = "wf_off[1024]/F"):
    #マクロの読み込み
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Sakurai_method.h")
    av_wave = RT.std.vector(float)(np.array(pd.read_pickle(avg_pkl)))
    RT.ClacWf_branch(file, tree, av_wave, length, branch_wf, name2)
    print('\033[31m' + "##### Generate wform ROOT file!! #####" + '\033[0m')

def Counts_AP_S(file_wf, S_pd, PEAK_THES, CONTI_THES, INT_s, INT_e, avg_pkl, tree_wf="Treesource_0", branch = "wform"):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Sakurai_method.h")
    av_wave = RT.std.vector(float)(np.array(pd.read_pickle(avg_pkl)))
    event = RT.std.vector(int)()
    seg = RT.std.vector(int)()
    peak = RT.std.vector(float)()
    counts = RT.std.vector(int)()
    #PEAK_THES = 20
    #CONTI_THES = 3
    RT.CountAPeventFromPH4(file_wf, tree_wf, 1024, PEAK_THES, CONTI_THES, INT_s, INT_e, av_wave, event, seg, peak, counts, branch)


    df = pd.DataFrame({'event':np.array(event),
                      'seg':np.array(seg),
                      'peak':np.array(peak),
                      'counts':np.array(counts)
                     })
    df.to_pickle(S_pd)

    print("##### Count AP by S_method!! #####")
    
def Compare_K(S_df, K_df, K_qd):
    S = pd.read_pickle(S_df)
    e_S = RT.std.vector(int)(np.array(S["event"]))
    s_S = RT.std.vector(int)(np.array(S["seg"]))

    K = pd.read_pickle(K_df)
    e_K = RT.std.vector(int)(np.array(K["event"]))
    s_K = RT.std.vector(int)(np.array(K["peak_cell"]))

    if os.path.exists(K_qd) == True:
        K2 = pd.read_pickle(K_qd)
        e_K2 = RT.std.vector(int)(np.array(K2["event"]))
        s_K2 = RT.std.vector(int)(np.array(K2["peak_cell"]))
    else:
        e_K2 = RT.std.vector(int)()
        s_K2 = RT.std.vector(int)()

    label = RT.std.vector(int)()
    RT.Compare_SvsK(e_S, s_S, e_K, s_K, e_K2, s_K2, label)
    
    S["K_label"] = np.array(label)
    S.to_pickle(S_df)

    print("##### S=> Compare with K_method!! #####")
    
def Compare_K2(S_df, K_df):
    S = pd.read_pickle(S_df)
    e_S = RT.std.vector(int)(np.array(S["event"]))
    s_S = RT.std.vector(int)(np.array(S["seg"]))

    K = pd.read_pickle(K_df)
    e_K = RT.std.vector(int)(np.array(K["event"]))
    s_K = RT.std.vector(int)(np.array(K["seg"]))


    label = RT.std.vector(int)()
    RT.Compare_SvsK2(e_S, s_S, e_K, s_K, label)
    
    S["K_label"] = np.array(label)
    S.to_pickle(S_df)

    print("##### S=> Compare with K_method!! #####")
