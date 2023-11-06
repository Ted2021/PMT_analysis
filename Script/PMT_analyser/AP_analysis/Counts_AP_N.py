import ROOT as RT
import numpy as np
import pickle
import sys
import pandas as pd


def Generate_DIFF_File(file, file_diff, avg_pkl, tree_s = "Treesingle_0", tree_dif="Treediff_0"):
   #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Nagayoshi_method.h")
    avg_wf_d = RT.std.vector(float)(pd.read_pickle(avg_pkl))
    #print(avg_wf_d)
    RT.CalcDiffWform4(file, tree_s, file_diff, tree_dif, avg_wf_d)
    print('\033[31m' + "##### Generate Diff ROOT file!! #####" + '\033[0m')

def Calc_DIFF_wf(file, tree, avg_wf = RT.std.vector(float)(np.zeros(1024))):
    #avg_wf = RT.std.vector(float)(pd.read_pickle(avg_pkl))
    RT.CalcDiffWform_branch(file, tree, avg_wf)
    print('\033[31m' + "##### Generate Diff ROOT file!! #####" + '\033[0m')

def Define_DIFF_THRES(file_diff, tree_dif="Treediff_0", draw = False):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Nagayoshi_method.h")
    par = RT.std.vector(float)()
    par_e = RT.std.vector(float)()
    hist1 = RT.DiffWformProjection(file_diff, tree_dif, par, par_e, 60, 1000)
    if draw == True:
        canvas = RT.TCanvas("c", "c", 600, 600)
        hist1.Draw()
        canvas.SetLogy()
        canvas.SaveAs("Diff_wf_hist.png")
        #canvas.Draw()
    print("##### Difine DIFF_THRES!! #####")
    return par[2] * 3


def Counts_AP_N(file_diff, N_pd, DIFF_THRES, INT_s, INT_e, tree_dif="Treediff_0"):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Nagayoshi_method.h")
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