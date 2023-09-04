import PMT_analyser.IO.Convert_DRS4 as DRS4
import PMT_analyser.Avg_wf.Calc_Avg_Wf as AVG
import PMT_analyser.Charge_analysis.Charge_Dist as Chrg
import PMT_analyser.Charge_analysis.Charge_AP as CAP
import PMT_analyser.Fitting.Fitting as Fit
import PMT_analyser.AP_analysis as AP
import sys
import pandas as pd
import os
import subprocess
import ROOT as RT
import numpy as np
import pickle
import glob
import matplotlib.pyplot as plt
import datetime
import slackweb

slack = slackweb.Slack(url = "https://hooks.slack.com/services/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")
def notify(title, text, color):
    attachments = [{"title": title,
                    "text": text,
                    "color": color, #good, warning, danger
                    "footer": "Send from Python",}]
    slack.notify(text=None, attachments=attachments)

"""
AP_avg_wf_s = "avg_wf_s.pkl"
AP_avg_wf_d = "avg_wf_d.pkl"

wform_file = "wform.root"
AP_Diff_root_file = "Diff.root"
AP_Dev_root_file = "Dev.root"

N_Cat = "N_Cat.pkl"
S_Cat = "S_Cat.pkl"
K_Cat = "K_Cat.pkl"
"""

def File_check(dir):
    if os.path.exists(dir) == True:
        print("{0} is {1}".format(dir.split("/")[-1], os.path.exists(dir)))
        notify(args[1], "{0} is {1}".format(dir.split("/")[-1], os.path.exists(dir)), "good")
    else:
        print('\033[31m' + "{0} is {1}".format(dir.split("/")[-1], os.path.exists(dir)) + "\033[0m")
        notify(args[1], '\033[31m' + "{0} is {1}".format(dir.split("/")[-1], os.path.exists(dir)) + "\033[0m", "bad")

    



def Def_file_name(dir, file_i):
    today = datetime.date.today().strftime('%Y%m%d')
    #today.strftime('%Y%m%d')
    PMT = dir.split("/")[-3]
    Time = dir.split("/")[-2]
    
    print("{0}_{1}_{2}_{3}".format(today, PMT, Time, file_i))
    return "{0}_{1}_{2}_{3}".format(today, PMT, Time, file_i)

def Generate_File(file_s, file_d,Gain = 10):

    #inputしたROOTファイルのsource Tree名を取得
    tree = DRS4.Check_Tree(file_s)

    #DRS4の差動電圧を取ったROOTファイルを生成
    DRS4.Create_ROOT_file_2386(file_s, file_d, path, path+wform_file, CH=Gain, Treename_s = tree) 

    #差動電圧をとったROOTファイルでSourceとDarkの平均波形を生成
    AVG.Calc_Avg_Wf_all(path+wform_file, path+wform_file, path+AP_avg_wf_s, path+AP_avg_wf_d, tr_s = tree)

    #微分波形のROOTファイルを生成
    AP.Generate_DIFF_File(path+wform_file, path+AP_Diff_root_file, path+AP_avg_wf_d, tree_s = tree)

    #偏差を取ったROOTファイルを生成
    AP.Generate_WFORM_File(path+wform_file, path+AP_Dev_root_file, path+AP_avg_wf_s, tree_s = tree)

def N_method_Counts(int_s=60, int_e=1000):
    tree = DRS4.Check_Tree(path+AP_Dev_root_file)
    DIFF_THRES = AP.Define_DIFF_THRES(path+AP_Diff_root_file)
    AP.Counts_AP_N(path+AP_Diff_root_file, path+N_Cat,  DIFF_THRES, int_s, int_e)
    CAP.Calc_AP_Charge_ph(path+AP_Dev_root_file, tree, path+AP_avg_wf_d, path+N_Cat)


def S_method_Counts(conti=3, int_s=60, int_e = 1000):
    tree = DRS4.Check_Tree(path+AP_Dev_root_file)
    PEAK_THRES = float(pd.read_pickle(path.split("AP")[0]+"single/"+"para.pkl")[4] * 0.4)
    AP.Counts_AP_S(path+AP_Dev_root_file, path+S_Cat, PEAK_THRES, conti, int_s, int_e, path+AP_avg_wf_s)
    CAP.Calc_AP_Charge_ph(path+AP_Dev_root_file, tree, path+AP_avg_wf_d, path+S_Cat)

def K_method_Counts():
    tree = DRS4.Check_Tree(path+AP_Dev_root_file)
    DIFF_THRES = AP.Define_DIFF_THRES(path+AP_Diff_root_file)
    #DIFF_THRES  =5
    print("### DIFF_THRES: {0} mV ###".format(DIFF_THRES))
    print("### type of DIFF_THRES: {0}".format(type(DIFF_THRES)))
    PEAK_THRES = float(pd.read_pickle(path.split("AP")[0]+"single/"+"para.pkl")[4] * 0.4)
    print("### PEAK_THRES: {0} mV ###".format(PEAK_THRES))
    print("### type of PEAK_THRES: {0}".format(type(PEAK_THRES)))
    AP.Counts_AP_K(path+AP_Diff_root_file, path+AP_Dev_root_file, DIFF_THRES, PEAK_THRES, path+N_Cat, pd.read_pickle(path+N_Cat),path+K_Cat)
    CAP.Calc_AP_Charge_K2(path+AP_Dev_root_file, tree, path+AP_avg_wf_d, path+K_Cat)

def Compare_to_S_K():
    AP.Compare_SvsK3(path+S_Cat, path+K_Cat)


if __name__ == '__main__':

    args = sys.argv

    #関数内部でも使えるようにファイル名はglobal変数にしておく
    global path
    global file
    global wform_file
    global AP_Diff_root_file
    global AP_Dev_root_file
    global AP_avg_wf_s
    global AP_avg_wf_d
    global N_Cat
    global S_Cat
    global K_Cat

    hoge = pd.read_csv(args[1])     #ファイル名が入ったCSVファイルを読み込む
    mode = args[2]                  #debugモードで実行するならば、debugを打ち込む

    #APの解析をする範囲を表示
    for i in range(len(hoge)):
        print('\033[34m' + '{0} is t={1}ns ~ t={2}ns'.format(hoge.loc[i]["Timing"], 
                                                            int(hoge.loc[i]["int_s"])*1000/1024 + int(hoge.loc[i]["Timing"].split("AP")[-1]), 
                                                            int(hoge.loc[i]["int_e"])*1000/1024 + int(hoge.loc[i]["Timing"].split("AP")[-1])) + '\033[0m')
    
    #ファイルが存在するか確認する
    for i in range(len(hoge)):
        print("Analysis Target => {0}".format(hoge.loc[i]["Timing"]))

        #CSVファイルからファイル名を取得
        path = hoge.loc[i]["Path"]
        file = hoge.loc[i]["Original RT_file"]
        wform_file = hoge.loc[i]["Conver RT_file"]
        AP_Diff_root_file = hoge.loc[i]["Diff RT_file"]
        AP_Dev_root_file = hoge.loc[i]["Dev RT_file"]
        AP_avg_wf_s = hoge.loc[i]["Avg_s"] 
        AP_avg_wf_d = hoge.loc[i]["Avg_d"] 
        N_Cat = hoge.loc[i]["N_Cat"] 
        S_Cat = hoge.loc[i]["S_Cat"]
        K_Cat = hoge.loc[i]["K_Cat"]
        start = int(hoge.loc[i]["int_s"])
        end = int(hoge.loc[i]["int_e"])

        #ファイルが存在するか確認
        File_check(file)
        File_check(path+wform_file)
        File_check(path+AP_Diff_root_file)
        File_check(path+AP_Dev_root_file)
        File_check(path+AP_avg_wf_s)
        File_check(path+AP_avg_wf_d)
        File_check(path+N_Cat)
        File_check(path+S_Cat)
        File_check(path+K_Cat)
        print("\033[32m" + "analysis range seg:{0} => seg:{1}".format(start, end) + "\033[0m")
        

        if mode != "debug":
            #永吉法、櫻井法、清本法のそれぞれ解析を実行する
            N_method_Counts(int_s=start, int_e=end)
            S_method_Counts(conti=3, int_s=start, int_e = end)
            K_method_Counts()
            Compare_to_S_K()
            print('\033[31m' + "Analysis {0} is Finish!!".format(hoge.loc[i]["Timing"]) + "\033[0m")
            notify(args[1], "Analysis {0} is Finish!!".format(hoge.loc[i]["Timing"]), "good")

    #AP_avg_wf_s = Def_file_name(path, "avg_wf_s.pkl")
    #AP_avg_wf_s = 'avg_s.pkl'
    
    #AP_avg_wf_d = Def_file_name(path, "avg_wf_d.pkl")
    #AP_avg_wf_d = 'avg_d.pkl'
    
    #wform_file = Def_file_name(path, "wform.root")
    #wform_file = 'new.root'
    
    #AP_Diff_root_file = Def_file_name(path, "Diff.root")
    #AP_Diff_root_file = 'diff.root'
    
    #AP_Dev_root_file = Def_file_name(path, "Dev.root")
    #AP_Dev_root_file = 'wform.root'
    
    #N_Cat = Def_file_name(path, "N_Cat.pkl")
    
    #S_Cat = Def_file_name(path, "S_Cat.pkl")
    
    #K_Cat = Def_file_name(path, "K_Cat.pkl")

    #print(path)

    #Generate_File(file, file, path, Gain = 10)
    #N_method_Counts(int_s=60, int_e=1000)
    #S_method_Counts(conti=3, int_s=60, int_e = 1000)
    #K_method_Counts()
    #Compare_to_S_K()
