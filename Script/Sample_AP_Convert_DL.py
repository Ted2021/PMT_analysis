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
import csv
import slackweb

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


#ROOTファイルが存在するか確認する関数
def File_exists_check(dir):
    ap_timing = ["AP0000", "AP0950", "AP1850", "AP2750", "AP3650"]
    raw_datas = []
    for item in ap_timing:
        temp_dir = dir + "/20" + dir.split("/")[-2] + "_" + dir.split("/")[-1] + "_" + item + ".root"   #ROOTファイルの名前 ※ディレクトリ名に依存
        if os.path.exists(temp_dir) == False:
            print('\033[31m' + '"{0}" IS NOT EXISTS!!'.format(temp_dir) + "\033[0m")
        else:
            raw_datas.append(temp_dir)
            print("gerogero")
    return raw_datas
    


def File_check(dir):
    if os.path.exists(dir) == True:
        print("{0} is {1}".format(dir.split("/")[-1], os.path.exists(dir)))
    else:
        print('\033[31m' + "{0} is {1}".format(dir.split("/")[-1], os.path.exists(dir)) + "\033[0m")


def Def_filename(pmt, timing, file_n):
    today = datetime.date.today().strftime('%Y%m%d')
    print("{0}_{1}_{2}_{3}".format(today, pmt, timing, file_n))
    return "{0}_{1}_{2}_{3}".format(today, pmt, timing, file_n)


def Def_file_name(dir, file_i):
    today = datetime.date.today().strftime('%Y%m%d')
    #today.strftime('%Y%m%d')
    PMT = dir.split("/")[-3]
    Time = dir.split("/")[-2]
    
    print("{0}_{1}_{2}_{3}".format(today, PMT, Time, file_i))
    return "{0}_{1}_{2}_{3}".format(today, PMT, Time, file_i)


#生データから解析する形式に変換する関数
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

if __name__ == '__main__':

    args = sys.argv
    slack = slackweb.Slack(url="https://hooks.slack.com/services/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")

    #path = args[1]
    #file = args[2]

    global path
    global original_file
    global wform_file
    global AP_Diff_root_file
    global AP_Dev_root_file

    global AP_avg_wf_s
    global AP_avg_wf_d

    global N_Cat
    global S_Cat
    global K_Cat

    int_nsec_s = {"AP0000":60, "AP0950":21, "AP1850":52, "AP2750":52, "AP3650":52}
    int_nsec_e = {"AP0000":992, "AP0950":971, "AP1850":971, "AP2750":971, "AP3650":1000}

    #hoge = pd.read_csv(args[1])
    #mode = args[2]

    #生データのあるディレクトリ
    #raw_data_path = "/Users/kiyomoto/reaserch/PMT_data/pmt/230810/AC1952"   #Temporaryにおいておく
    raw_data_path = args[1]     #引数でとる
    raw_datas = File_exists_check(raw_data_path)    #APの測定データが存在するか確認

    #AfterPulseの解析結果をおいておくディレクトリ
    ap_analysis_result_path = raw_data_path + "/analysis"   #生データのあるディレクトリ直下におく

    #ap_analysis_result_pathが存在しない場合には、処理を終了する
    if os.path.exists(ap_analysis_result_path) == False:
        print('\033[31m' + '"{0}" IS NOT EXISTS!!'.format(ap_analysis_result_path) + "\033[0m")
        sys.exit()
    
    #ap_analysis_result_path直下にAPの解析結果を置くディレクトリを生成
    if os.path.exists(ap_analysis_result_path+"/AP") == False:
        ap_analysis_result_path += "/AP"
        os.mkdir(ap_analysis_result_path)
    else:
        ap_analysis_result_path += "/AP"

    #Slackで通知を飛ばす
    slack.notify(text="file_check_OK:{0}".format(raw_datas[0].split("/")[-2]), username = "Python3")

    #csvに書き出す (とりあえずヘッダーを書き込む)
    with open(ap_analysis_result_path+'/{0}_dataset.csv'.format(raw_datas[0].split("/")[-2]), 'w') as f:
            writer = csv.writer(f)
            writer.writerow(["Timing","Original RT_file","Path","Conver RT_file","Diff RT_file","Dev RT_file","Avg_s","Avg_d","N_Cat","S_Cat","K_Cat","int_s","int_e"])

    #raw_dates(ROOTファイル)の中身で回す
    for item in raw_datas:
        print(item)
        pmt_rot = item.split("/")[-2]       #pmtのロット番号
        ap_timing = item.split(".root")[0].split("_")[-1]       #AfterPulseのTriggerのタイミング
        
        #以下ファイル名を定義
        path = ap_analysis_result_path + "/"
        original_file = item
        wform_file = Def_filename(pmt_rot, ap_timing, "wform.root")
        AP_Diff_root_file = Def_filename(pmt_rot, ap_timing, "diff.root")
        AP_Dev_root_file = Def_filename(pmt_rot, ap_timing, "dev.root")
        AP_avg_wf_s = Def_filename(pmt_rot, ap_timing, "avg_wf_s.pkl")
        AP_avg_wf_d = Def_filename(pmt_rot, ap_timing, "avg_wf_d.pkl")
        N_Cat = Def_filename(pmt_rot, ap_timing, "N_cat.pkl")
        S_Cat = Def_filename(pmt_rot, ap_timing, "S_cat.pkl")
        K_Cat = Def_filename(pmt_rot, ap_timing, "K_cat.pkl")

        #csvに書き出す
        with open(ap_analysis_result_path+'/{0}_dataset.csv'.format(item.split("/")[-2]), 'a') as f:
            writer = csv.writer(f)
            writer.writerow([ap_timing, item, path, wform_file, AP_Diff_root_file, AP_Dev_root_file, AP_avg_wf_s, AP_avg_wf_d, N_Cat, S_Cat, K_Cat, int_nsec_s[ap_timing], int_nsec_e[ap_timing]])

        #処理を施す
        Generate_File(original_file, original_file)

        #正常に終了したらSlackに通知を飛ばす
        slack.notify(text="file_Convert_DONE!:{0}".format(item), username = "Python3")


    
