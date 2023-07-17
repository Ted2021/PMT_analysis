import ROOT as RT
import numpy as np
import pickle
import sys
import pandas as pd

def Counts_AP_K(file_diff, file_wf, DIFF_THRES, PEAK_THRES, N_pd, K_pd, tree_dif = "Treediff_0", tree_wf = "Treesource_0"):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Kiyomoto_method.h")
    #清本法で、永吉法で得られたAP_Eventを検討する
    df = pd.read_pickle(N_pd)
    e = np.array(df["event"])
    s = np.array(df["seg"])
    event_list = RT.std.vector(int)(e)
    seg_list = RT.std.vector(int)(s)
    p1 = RT.std.vector(int)() 
    p2 = RT.std.vector(int)() 
    p3 = RT.std.vector(int)() 
    p4 = RT.std.vector(int)() 
    p5 = RT.std.vector(int)() 
    p6 = RT.std.vector(float)() 
    p7 = RT.std.vector(float)() 
    p8 = RT.std.vector(float)() 
    RT.JudgeAPevent2(file_diff, tree_dif, file_wf, tree_wf, 1024, DIFF_THRES, PEAK_THRES, 2.4,2.4,event_list,seg_list,p1,p2,p3,p4,p5,p6,p7,p8)
    
    #これまでの解析結果をpd.Dataframeにまとめる
    K = pd.DataFrame({'event':e,
                      'seg':s,
                      'Label':np.array(p1),
                      'Start_cell':np.array(p2),
                      'peak_cell':np.array(p3),
                      'ankle_cell':np.array(p4),
                      'end_cell':np.array(p5),
                      'difwf':np.array(df["difwf"]),
                      'Diff_peak':np.array(p6),
                      'Ph_peak':np.array(p7),
                      'ankle':np.array(p8)
                     })
    K.to_pickle(K_pd)

    print("##### Count AP by K_method!! #####")


def Counts_AP_K_QD(file_diff, file_wf, DIFF_THRES, PEAK_THRES, N_pd, K_pd, tree_dif = "Treediff_0", tree_wf = "Treesource_0"):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Kiyomoto_method.h")
    #清本法で、永吉法で得られたAP_Eventを検討する
    df = N_pd
    e = np.array(df["event"])
    s = np.array(df["end_cell"])
    event_list = RT.std.vector(int)(e)
    seg_list = RT.std.vector(int)(s)
    p1 = RT.std.vector(int)() 
    p2 = RT.std.vector(int)() 
    p3 = RT.std.vector(int)() 
    p4 = RT.std.vector(int)() 
    p5 = RT.std.vector(int)() 
    p6 = RT.std.vector(float)() 
    p7 = RT.std.vector(float)() 
    p8 = RT.std.vector(float)() 
    RT.JudgeAPevent2(file_diff, tree_dif, file_wf, tree_wf, 1024, DIFF_THRES, PEAK_THRES, 2.4,2.4,event_list,seg_list,p1,p2,p3,p4,p5,p6,p7,p8)
    
    #これまでの解析結果をpd.Dataframeにまとめる
    K = pd.DataFrame({'event':e,
                      'seg':s,
                      'Label':np.array(p1),
                      'Start_cell':np.array(p2),
                      'peak_cell':np.array(p3),
                      'ankle_cell':np.array(p4),
                      'end_cell':np.array(p5),
                      'Diff_peak':np.array(p6),
                      'Ph_peak':np.array(p7),
                      'ankle':np.array(p8)
                     })
    K.to_pickle(K_pd)

    print("##### Count AP by K_method!! #####")

def Calc_QD(file_diff, file_wf, DIFF_THRES, PEAK_THRES, path, K_pd, tree_dif = "Treediff_0", tree_wf = "Treesource_0"):
    QD_analysis = True
    i = 0

    df = pd.read_pickle(K_pd)
    df_qd = df[df["Label"]==2]
    if(len(df_qd) == 0):
        QD_analysis = False

    QD_file = []
    while QD_analysis == True:
        #df_name = K_pd.split("/")[-1].split(".pkl")[0] +"_"+str(i)+".pkl"
        df_name = path + K_pd.split("/")[-1].split(".pkl")[0] +"_QD"+str(i)+".pkl"
        QD_file.append(df_name)
        Counts_AP_K_QD(file_diff, file_wf, DIFF_THRES, PEAK_THRES, df_qd, df_name, tree_dif = "Treediff_0", tree_wf = "Treesource_0")
        df = pd.read_pickle(df_name)
        df_qd = df[df["Label"]==2]
        if(len(df_qd) == 0):
            QD_analysis = False
        else:
            i += 1
    return QD_file
    

def Compare_S(S_df, K_df, K_ndf):
    S = pd.read_pickle(S_df)
    e_S = RT.std.vector(int)(np.array(S["event"]))
    s_S = RT.std.vector(int)(np.array(S["seg"]))

    K = pd.read_pickle(K_df)
    K1= K[K["Label"]==1]
    e_K = RT.std.vector(int)(np.array(K1["event"]))
    s_K = RT.std.vector(int)(np.array(K1["peak_cell"]))

    e_K2 = RT.std.vector(int)()
    s_K2 = RT.std.vector(int)()

    label = RT.std.vector(int)()
    RT.Compare_SvsK(e_K, s_K, e_S, s_S, e_K2, s_K2, label)
    
    K1["S_label"] = np.array(label)
    K1.to_pickle(K_ndf)

