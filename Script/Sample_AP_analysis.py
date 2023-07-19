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


def Def_file_name(path, file):
    today = datetime.date.today().strftime('%Y%m%d')
    #today.strftime('%Y%m%d')
    PMT = path.split("/")[-3]
    Time = path.split("/")[-2]
    
    print("{0}_{1}_{2}_{3}".format(today, PMT, Time, file))
    return "{0}_{1}_{2}_{3}".format(today, PMT, Time, file)


AP_avg_wf_s = Def_file_name(path, "avg_wf_s.pkl")
AP_avg_wf_d = Def_file_name(path, "avg_wf_d.pkl")

wform_file = Def_file_name(path, "wform.root")
AP_Diff_root_file = Def_file_name(path, "Diff.root")
AP_Dev_root_file = Def_file_name(path, "Dev.root")

N_Cat = Def_file_name(path, "N_Cat.pkl")
S_Cat = Def_file_name(path, "S_Cat.pkl")
K_Cat = Def_file_name(path, "K_Cat.pkl")

def Generate_File(file_s, file_d, path, Gain = 10):

    #inputしたROOTファイルのsource Tree名を取得
    tree = DRS4.Check_Tree(file_o)

    #DRS4の差動電圧を取ったROOTファイルを生成
    DRS4.Create_ROOT_file_2386(file_s, file_d, path, path+wform_file, CH=Gain, Treename_s = tree) 

    #差動電圧をとったROOTファイルでSourceとDarkの平均波形を生成
    AVG.Calc_Avg_Wf_all(path+wform_file, path+wform_file, path+AP_avg_wf_s, path+AP_avg_wf_d, tr_s = tree)

    #微分波形のROOTファイルを生成
    AP.Generate_DIFF_File(path+wform_file, path+AP_Diff_root_file, path+AP_avg_wf_d, tree_s = tree)

    #偏差を取ったROOTファイルを生成
    AP.Generate_WFORM_File(path+AP_wform_root_file, path+AP_Dev_root_file, path+AP_avg_wf_s, tree_s = tree)

def N_method_Counts(path, int_s=60, int_e=1000):
    tree = DRS4.Check_Tree(path+AP_Dev_root_file)
    DIFF_THRES = AP.Define_DIFF_THRES(path+AP_Diff_root_file)
    AP.Counts_AP_N(path+AP_Diff_root_file, path+N_Cat,  DIFF_THRES, int_s, int_e)
    APC.Calc_AP_Charge_ph(path+AP_Dev_root_file, tree, path+AP_avg_wf_d, path+N_Cat)


def S_method_Counts(path, conti=3, int_s=60, int_e = 1000):
    tree = DRS4.Check_Tree(path+AP_Dev_root_file)
    PEAK_THRES =  10    
    AP.Counts_AP_S(path+AP_Dev_root_file, path+S_Cat, PEAK_THRES, conti, int_s, int_e, path+AP_avg_wf_s)
    APC.Calc_AP_Charge_ph(path+AP_Dev_root_file, tree, path+AP_avg_wf_d, path+S_Cat)

def K_method_Counts(path):
    tree = DRS4.Check_Tree(path+AP_Dev_root_file)
    DIFF_THRES = AP.Define_DIFF_THRES(path+AP_Diff_root_file)
    PEAK_THRES =  10
    AP.Counts_AP_K(path+AP_Diff_root_file, path+AP_Dev_root_file, DIFF_THRES, PEAK_THRES, path+N_Cat, pd.read_pickle(path+N_Cat),path+K_Cat)
    APC.Calc_AP_Charge_K2(path+AP_Dev_root_file, tree, path+AP_avg_wf_d, path+K_Cat)

def Compare_to_S_K(path):
    Compare_SvsK3(path+S_Cat, path+K_Cat)


if __name__ == '__main__':

    global path = args[1]
    file = args[2]

    Generate_File(file_s, file_d, path, Gain = 10)
    N_method_Counts(path, int_s=60, int_e=1000)
    S_method_Counts(path, conti=3, int_s=60, int_e = 1000)
    K_method_Counts(path)
    Compare_to_S_K(path)
