import ROOT as RT
import numpy as np
import pickle
import sys
import pandas as pd


def Generate_DIFF_File(file, file_diff, avg_pkl, tree_dif="Treediff_0"):
    RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Nagayoshi_method.h")
    avg_wf_d = RT.std.vector(float)(np.array(pd.read_pickle(avg_pkl)))
    #print(avg_wf_d)
    RT.CalcDiffWform4(file, "Treesingle_0", file_diff, tree_dif, avg_wf_d)

def Counts_AP_N(file_diff, N_pd, DIFF_THRES, INT_s, INT_e, tree_dif="Treediff_0"):
    RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Nagayoshi_method.h")
    event = RT.std.vector(int)()
    seg = RT.std.vector(int)()
    difwf = RT.std.vector(float)()
    counts = RT.std.vector(int)()
    RT.CountAPeventFromDiff(file_diff, tree_dif, 1023, DIFF_THRES, INT_s, INT_e, event, seg, difwf, counts)

    #これまでの解析結果をpd.Dataframeにまとめる
    df = pd.DataFrame({'event':np.array(event),
                      'seg':np.array(seg),
                      'difwf':np.array(difwf),
                      'counts':np.array(counts)
                     })
    df.to_pickle(N_pd)

    print("##### Count AP by N_method!! #####")


if __name__ == '__main__':
    args = sys.argv
    Counts_AP_N(args[1], args[2], args[3], int(args[4]), int(args[5]), int(args[6]))