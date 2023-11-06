import ROOT as RT
import numpy as np
import pickle
import sys
import pandas as pd
import os

def Counts_AP_K(file_diff, file_wf, DIFF_THRES, PEAK_THRES, N_pd_path, N_pd, K_pd, tree_dif = "Treediff_0", tree_wf = "Treesource_0", branch_diff = "diffwf", branch_off = "wform", QD=False):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/AP_analysis/Kiyomoto_method.h")
    #清本法で、永吉法で得られたAP_Eventを検討する
    #df = pd.read_pickle(N_pd)
    df = N_pd
    e = np.array(df["event"])
    if QD == False:
        s = np.array(df["seg"])
    else:
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
    p9 = RT.std.vector(int)()
    RT.JudgeAPevent4(file_diff, tree_dif, file_wf, tree_wf, 1024, DIFF_THRES, PEAK_THRES, 2.4,2.4,event_list,seg_list,p1,p2,p3,p4,p5,p6,p7,p8,p9,5,10, branch_diff, branch_off)
    
    #これまでの解析結果をpd.Dataframeにまとめる
    K = pd.DataFrame({'event':e,
                      'seg':np.array(p3),
                      'Label':np.array(p1),
                      'QD':np.array(p9),
                      'Start_cell':np.array(p2),
                      'diff_cell':s,
                      'ankle_cell':np.array(p4),
                      'end_cell':np.array(p5),
                      'Diff_peak':np.array(p6),
                      'Ph_peak':np.array(p7),
                      'ankle':np.array(p8)
                     })
    

    if QD == False:
        K.query('Label == 1 or Label == 2').to_pickle(K_pd)

        df["K_label"] = np.array(p1)
        df.to_pickle(N_pd_path)

        print("##### Count AP by K_method!! #####")
    else:
        K.to_pickle(K_pd)
        

#こちらはおそらくいらない
def Calc_QD(file_diff, file_wf, DIFF_THRES, PEAK_THRES, path, K_pd, QD_pd, tree_dif = "Treediff_0", tree_wf = "Treesource_0"):
    QD_analysis = True
    i = 0

    df = pd.read_pickle(K_pd)
    df_qd = df[df["Label"]==2]
    if(len(df_qd) == 0):
        QD_analysis = False

    QD_file = []
    while QD_analysis == True:
        df_name = path + K_pd.split("/")[-1].split(".pkl")[0] +"_QD"+str(i)+".pkl"
        QD_file.append(df_name)
        Counts_AP_K(file_diff, file_wf, DIFF_THRES, PEAK_THRES, df_qd, df_qd, df_name, tree_dif = "Treediff_0", tree_wf = "Treesource_0", QD = True)
        df = pd.read_pickle(df_name)
        df_qd = df[df["Label"]==2]
        if(len(df_qd) == 0):
            QD_analysis = False
            QD_all = []
            for item in QD_file:
                QD_all.append(pd.read_pickle(item))
            QD_df = pd.concat(QD_all)
            QD_df.to_pickle(QD_pd)
            print("##### Count QD max:{0}!! #####".format(str(i+1)))

        else:
            i += 1
    
    

def Compare_S(S_df, K_df, QD_df):
    S = pd.read_pickle(S_df)
    e_S = RT.std.vector(int)(np.array(S["event"]))
    s_S = RT.std.vector(int)(np.array(S["seg"]))

    K = pd.read_pickle(K_df)
    e_K = RT.std.vector(int)(np.array(K["event"]))
    s_K = RT.std.vector(int)(np.array(K["peak_cell"]))

    label = RT.std.vector(int)()
    RT.Compare_SvsK(e_K, s_K, e_S, s_S, RT.std.vector(int)(), RT.std.vector(int)(), label)
    
    K["S_label"] = np.array(label)
    K.to_pickle(K_df)
    print("##### K=> Compare with S_method!! #####")
    
    if os.path.exists(QD_df) == True:
            QD = pd.read_pickle(QD_df)
            e_QD = RT.std.vector(int)(np.array(QD["event"]))
            s_QD = RT.std.vector(int)(np.array(QD["peak_cell"]))

            label = RT.std.vector(int)()
            RT.Compare_SvsK(e_QD, s_QD, e_S, s_S, RT.std.vector(int)(), RT.std.vector(int)(), label)

            QD["S_label"] = np.array(label)
            QD.to_pickle(QD_df)
            print("##### QD => Compare with S_method!! #####")

def Compare_S2(S_df, K_df):
    S = pd.read_pickle(S_df)
    e_S = RT.std.vector(int)(np.array(S["event"]))
    s_S = RT.std.vector(int)(np.array(S["seg"]))

    K = pd.read_pickle(K_df)
    e_K = RT.std.vector(int)(np.array(K["event"]))
    s_K = RT.std.vector(int)(np.array(K["seg"]))

    label = RT.std.vector(int)()
    RT.Compare_SvsK2(e_K, s_K, e_S, s_S, label)
    
    K["S_label"] = np.array(label)
    K.to_pickle(K_df)
    print("##### K=> Compare with S_method!! #####")

def Compare_SvsK(S_df, K_df):
    S = pd.read_pickle(S_df)
    list_s = []
    for i in range(len(S)):
        list_s.append([np.array(S["event"])[i], np.array(S["seg"])[i]])

    K = pd.read_pickle(K_df)
    list_k = []
    for i in range(len(K)):
        list_k.append([np.array(K["event"])[i], np.array(K["seg"])[i]])


    label_s = []
    for i in range(len(list_s)):
        bool_check = list_s[i] in list_k
        if bool_check == False:
            label_s.append(0)
        else:
            label_s.append(1)
    
    S["K_label"] = label_s
    S.to_pickle(S_df)
    print("##### S=> Compare with K_method!! #####")

    del label_s

    label_k = []
    for i in range(len(list_k)):
        bool_check = list_k[i] in list_s
        if bool_check == False:
            label_k.append(0)
        else:
            label_k.append(1)

    
    K["S_label"] = label_k
    K.to_pickle(K_df)
    print("##### K=> Compare with S_method!! #####")
    del label_k
    del list_k, list_s


def Compare_SvsK3(S_df, K_df):
    S = pd.read_pickle(S_df)
    K = pd.read_pickle(K_df)
    Diff = pd.merge(K,S, on=['event', 'seg', 'charge'], how='outer',indicator=True)
    
    S_only = Diff[Diff["_merge"] == "right_only"][S.columns.values]
    S_only["K_label"] = np.full(len(S_only), False)
    S_both = Diff[Diff["_merge"] == "both"][S.columns.values]
    S_both["K_label"] = np.full(len(S_both), True)
    
    K_only = Diff[Diff["_merge"] == "left_only"][K.columns.values]
    K_only["S_label"] = np.full(len(K_only), False)
    K_both = Diff[Diff["_merge"] == "both"][K.columns.values]
    K_both["S_label"] = np.full(len(K_both), True)
    
    
    S_label = pd.concat([S_both, S_only])
    K_label = pd.concat([K_both, K_only])
    
    S_label.to_pickle(S_df)
    K_label.to_pickle(K_df)

    print("#### Compare S & K method ####")