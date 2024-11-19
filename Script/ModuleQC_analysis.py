"""
Update 23/11/25
AP_analysis Script for Module QC 2014
"""
#標準ライブラリ
import subprocess 
import os
import numpy as np
import uproot
import matplotlib.pyplot as plt
import pandas as pd
import sys
import uproot
import slackweb
import datetime
import pickle
import time
import ROOT as RT
from multiprocessing import Pool
import glob
import pypdf

#自作ライブラリ
import PMT_analyser.IO.Convert_DRS4 as DRS4
import PMT_analyser.Avg_wf.Calc_Avg_Wf as AVG
import PMT_analyser.Charge_analysis.Charge_Dist as Chrg
import PMT_analyser.Charge_analysis.Charge_AP as CAP
import PMT_analyser.Fitting.Fitting as Fit
import PMT_analyser.AP_analysis as AP

#標準出力の色を指定
RED = '\033[31m'
BLUE = '\033[34m'
GREEN = '\033[32m'
YELLOW = '\033[33m'
END = '\033[0m'

#Ana para
Tree_singlepe_s = "Events"
Tree_singlepe_d = "Events"


#Common Function

#次に進むかのFlag
def Flag_operate(telop, Stopper = True):
    if Stopper == True:
        stopflag = False
        cont_val = False
        while stopflag == False:
            message = input('{0} (y/n) >> '.format(telop))
            if message == "n":
                sys.exit()
            elif message == "y":
                stopflag = True
            elif message == "skip":
                stopflag = True
                cont_val = True
                return cont_val

#ファイルのヘッダーを定義
def Def_Header(input_file):
    if input_file.split("/")[6] == "APmeasurement003":
        date = "20231022"
    elif input_file.split("/")[6] == "APmeasurement002":
        date = "20231020"
    did = input_file.split("/")[-1].split("_")[-2]
    timing = input_file.split("/")[-1].split("_")[1]
    return date, did, timing

#inputデータ(.root)の存在を確認
def InputFile_Check(input_file, type):
    if os.path.exists(input_file) == False:
        print(RED + "ERROR! NO {0} ROOT_FILE (v file name below v)!!".format(type) + END)
        print(RED + input_file + END)
        sys.exit()

    else:
        print(GREEN  + "VVV Load Sucessfully {0} file VVV".format(type) + END)
        print(input_file)

#Outputするディレクトリを確認
def OuputFile_Check(path, type):
    if os.path.exists(path) == True:
        #print(RED +  "{0} Result Dir is Already Exsist! ".format(type) + END)
        Flag_operate("Continue Anyway?", False)
    else:
        os.makedirs(path)
        print(GREEN + "VVV Make New Dir for {0} VVV".format(type) + END)
        print("{0}".format(path))
    return 1

#Singleの解析
def SEAna_Values(input_file_s, input_file_d, tree_name, branch_name, 
               path, header,
               chrg_pkl = "SinglePE_chrg.pkl", histX_pkl = "SinglePE_histX_.pkl", histY_pkl = "SinglePE_histY_.pkl",
               para_pkl = "SinglePE_para.pkl", chrg_fig = "SinglePE_Chrg.png"):
    keys = {"inputRT_s":input_file_s,
            "inputRT_d":input_file_d,
            "tree":tree_name,
            "branch":branch_name,
            "chrg":path+header+chrg_pkl,
            "hstX":path+header+histX_pkl,
            "hstY":path+header+histY_pkl,
            "para":path+header+para_pkl,
            "Fig":path+header+chrg_fig}
    return keys

def Ana_SinglePE(keys, int_range=[15,25]):
    charge = Chrg.Calc_Charge(keys["inputRT_s"], keys["inputRT_d"], int_range[0], int_range[1], keys["chrg"], keys["tree"], keys["tree"], keys["branch"], keys["branch"])
    hist = Chrg.Plot_Hist(charge, -200, 500)
    #hist = plt.hist(charge, bins = 330, range=[-100,500])
    p = Fit.Find_Fit_Para(hist, keys["hstX"], keys["hstY"], 60)
    #print(BLUE + "{0} {1} {2}".format(p[0], p[1], p[2]) + END)
    para = Fit.Fit_trpl_Gauss(hist, p[0], p[1], p[2], [-200, 500], keys["para"], keys["Fig"])
    print(para)
    del charge, hist, p, para

    print(GREEN + "File : {0}, Branch: {1} ".format(keys["inputRT_s"], keys["branch"]) + "SinglePE_analysis Done!!" + END)

#入力と出力ファイルをDicに格納 出力ファイルのヘッダーはYYYYMMDD_DragonID_PMTSerial_Timing_ に統一する
def APAna_Values(input_file, tree_name, branch_name, 
               path, header, singlePE_chrg,
               output_file="conv.root", avg_s="avf_wf_s.pkl",
               mp_hst_x="MP_hst_x.pkl", mp_hst_y="MP_hst_y.pkl", mp_para = "MP_para.pkl", mp_chrg = "MP_chrg.pkl", mp_fig = "MP_fig.png",
               diff_para = "Diff_para.pkl", diff_fig = "Diff_fig.png",
               result_N="AP_result_N.pkl", result_S="AP_result_S.pkl", result_K="AP_result_K.pkl"):
    keys = {"inputRT":input_file,
            "tree":tree_name,
            "branch":branch_name,
            "SinglePE":singlePE_chrg,
            "path":path,
            "header":header,
            "outputRT":path+header+output_file,
            "avg_wf":path+header+avg_s,
            "MP_hstX":path+header+mp_hst_x,
            "MP_hstY":path+header+mp_hst_y,
            "MP_para":path+header+mp_para,
            "MP_chrg":path+header+mp_chrg,
            "MP_chrg_fig":path+header+mp_fig,
            "Diff_para":path+header+diff_para,
            "Diff_fig":path+header+diff_fig,
            "N":path+header+result_N,
            "S":path+header+result_S,
            "K":path+header+result_K}
    
    return keys

#差動電圧を出力
def Conv_RTfile(keys):
    #DRS4.Creat_ROOT_file(keys["inputRT"], keys["tree"], )
    DRS4.Copy_ROOT_file(keys["inputRT"], keys["tree"], keys["outputRT"], keys["tree"], "time", keys["branch"])
    subprocess.call(['ipython', '/Users/kiyomoto/git/Script/show_waveform.py', keys["outputRT"], keys["tree"], 'conv', '50000', '{0}{1}_dev_fig'.format(keys["path"], keys["header"])])
    print(GREEN + "##### COPY {0} {1} RT END #####".format(keys["inputRT"], keys["branch"]) + END)

#平均波形を出力
"""
def Calc_Avg_wf(keys):
    AVG.Calc_Avg_Wf_source(keys["outputRT"],keys["avg_wf"], keys["tree"])
    print(GREEN + "##### CALC AVG END #####" + END)
"""

def Calc_Avg_wf(keys):
    input_rt = keys["outputRT"]
    tree = keys["tree"]
    out_put_pkl = keys["avg_wf"]
    print(BLUE + input_rt)
    print(BLUE + tree)
    print(BLUE + out_put_pkl + END)
    tr = uproot.open(input_rt)[tree]
    print(RED + "{0}".format(tr.keys()) + END)
    wf = tr.arrays("wform", library="np")["wform"]
    avg = np.average(wf, axis=0)
    print(BLUE + "{0}".format(avg) + END)
    with open(out_put_pkl, mode = "wb") as fo:
        pickle.dump(avg, fo)
    #AVG.Calc_Avg_Wf_source(keys["outputRT"],keys["avg_wf"], keys["tree"])
    print(GREEN + "##### CALC {0} {1} AVG END #####".format(input_rt, keys["branch"]) + END)

#MPの推定
def Calc_MP_Charge(keys, branch_wf = "wform", int_range=[10,40]):
    rt = uproot.open(keys["outputRT"])
    tr = rt[keys["tree"]]
    wf = tr.arrays(branch_wf, library = "np")[branch_wf]
    dt = 1e3/1024
    bins = dt * wf[:,int_range[0]:int_range[1]]
    chrg = np.sum(bins, axis = 1)
    min_c = int(np.min(chrg))
    max_c = int(np.max(chrg))
    hist = Chrg.Plot_Hist(RT.std.vector(float)(chrg), min_c-10, max_c+10)
    p = Fit.Find_Fit_Para(hist, keys["MP_hstX"], keys["MP_hstY"])
    para = Fit.Fit_sgl_Gauss(hist, p[0], p[1], p[2], [min_c-50, max_c+50], keys["MP_para"], keys["MP_chrg_fig"])
    with open(keys["MP_chrg"], mode = "wb") as fo:
        pickle.dump(chrg, fo)
    print(GREEN + "##### CALC {0} {1} MP END #####".format(keys["outputRT"], keys["branch"]) + END)
        
#微分波形を追記
def Calc_Diff(keys):
    AP.Calc_DIFF_wf(keys["outputRT"], keys["tree"])
    subprocess.call(['ipython', '/Users/kiyomoto/git/Script/show_waveform.py', keys["outputRT"], keys["tree"], 'diff', '50000', '{0}{1}_dev_fig'.format(keys["path"], keys["header"])])
    print(GREEN + "##### CALC {0} {1} DIFF WF END #####".format(keys["outputRT"], keys["branch"]) + END)

#平均波形との偏差を追記
def Calc_Dev(keys):
    AP.Generate_WFORM_branch(keys["outputRT"], keys["tree"], keys["avg_wf"])
    subprocess.call(['ipython', '/Users/kiyomoto/git/Script/show_waveform.py', keys["outputRT"], keys["tree"], 'dev', '50000', '{0}{1}_dev_fig'.format(keys["path"], keys["header"])])
    print(GREEN + "##### CALC {0} {1} DEV WF END #####".format(keys["outputRT"], keys["branch"]) + END)

#永吉法解析
def AP_AnaN(keys):
    para = AP.Define_DIFF_THRES(keys["outputRT"], keys["tree"], True, keys["Diff_fig"])

    with open(keys["Diff_para"], "wb") as f:
        pickle.dump(para[0]+para[1], f)
        #pickle.dump(np.array(para[1]), f)
    DIFF_THRES = para[0][2] * 3
    print(RED + "DIFF_THRES IS => {0}".format(DIFF_THRES) + END)
    AP.Counts_AP_N(keys["outputRT"], keys["N"], DIFF_THRES, 100, 1000, keys["tree"])
    CAP.Calc_AP_Charge_ph(keys["outputRT"], keys["tree"], np.zeros(1024), keys["N"], "time", "wf_off", int_range=5)
    #CAP.Calc_AP_Charge_ph(keys["outputRT"], keys["tree"], pd.read_pickle(keys["avg_wf"]), keys["N"], "time", "wform")
    print(GREEN + "##### ANA {0} {1} N-method END #####".format(keys["outputRT"], keys["branch"]) + END)

def Call_singlePE(x,y):
    X = pd.read_pickle(x)
    Y = pd.read_pickle(y)
    df = pd.DataFrame({"x":X,"y":Y})
    val = df[df["y"] == np.max(df[df["x"]>80]["y"])]
    pe = float(val["x"])
    return pe

#櫻井法解析
def AP_AnaS(keys):
    #PEAK_THRES = pd.read_pickle(keys["SinglePE"])[4] * 0.3
    PEAK_THRES = keys["SinglePE"] * 0.3
    AP.Counts_AP_S(keys["outputRT"], keys["S"], PEAK_THRES, 3, 100, 1000, keys["avg_wf"], keys["tree"], "wf_off")
    CAP.Calc_AP_Charge_ph(keys["outputRT"], keys["tree"], np.zeros(1024), keys["S"], "time", "wf_off", int_range=5)
    #CAP.Calc_AP_Charge_ph(keys["outputRT"], keys["tree"], pd.read_pickle(keys["avg_wf"]), keys["S"], "time", "wform")
    print(GREEN + "##### ANA {0} {1} S-method END #####".format(keys["outputRT"], keys["branch"]) + END)

#清本法解析
def AP_AnaK(keys):
    DIFF_THRES = pd.read_pickle(keys["Diff_para"])[2] * 3
    #PEAK_THRES = pd.read_pickle(keys["SinglePE"])[4] * 0.3
    PEAK_THRES = keys["SinglePE"] * 0.3
    AP.Counts_AP_K(keys["outputRT"], keys["outputRT"], DIFF_THRES, PEAK_THRES, keys["N"], pd.read_pickle(keys["N"]), keys["K"], keys["tree"],  keys["tree"], "diffwf", "wf_off")
    CAP.Calc_AP_Charge_K2(keys["outputRT"], keys["tree"], np.zeros(1024), keys["K"], "time", "wf_off")
    AP.Compare_SvsK3(keys["S"], keys["K"])
    print(GREEN + "##### ANA {0} {1} K-method END #####".format(keys["outputRT"], keys["branch"]) + END)


#結果のマージ
def Merge_result(df_set, new_df, single_chrg, timing = [0,1000,2000,3000]):
    #single_pe = float(pd.read_pickle(single_chrg)[4])
    single_pe = single_chrg
    #AP.Merge_all_result(df_set, timing, new_df, True, single_pe)
    AP.Merge_all_result(df_set, timing, new_df, True, single_pe)

def Def_ALL_result(keys):
    N_set = sorted(glob.glob("{0}*AP_result_N.pkl".format(keys["path"])))
    S_set = sorted(glob.glob("{0}*AP_result_S.pkl".format(keys["path"])))
    K_set = sorted(glob.glob("{0}*AP_result_K.pkl".format(keys["path"])))
    N_all = "{0}N_Result.pkl".format(keys["path"])
    S_all = "{0}S_Result.pkl".format(keys["path"])
    K_all = "{0}K_Result.pkl".format(keys["path"])
    return N_set, S_set, K_set, N_all, S_all, K_all

def Merge_All(keys, tuples, s_pe, m_pe):
    Merge_result(tuples[0], tuples[3], s_pe)
    Merge_result(tuples[1], tuples[4], s_pe)
    Merge_result(tuples[2], tuples[5], s_pe)
    Show_Result(tuples[3], s_pe, m_pe, keys["path"]+"Result_N.pdf")
    Show_Result(tuples[4], s_pe, m_pe, keys["path"]+"Result_S.pdf")
    Show_Result(tuples[5], s_pe, m_pe, keys["path"]+"Result_K.pdf")
                
#解析結果の出力
def Show_Result(new_df, single_chrg, mp_chrg, pdf_name, event_num = 50000):
    #single_pe = pd.read_pickle(single_chrg)[4]
    single_pe = single_chrg
    #multi_pe = pd.read_pickle(mp_chrg)[1]
    multi_pe = mp_chrg
    AP.Plot_all_result(new_df, single_pe, multi_pe, event_num, "hoge", pdf_name)

#解析をガン回す
def APana_Seq(num, file, br, path, header, single, tr="Events"):
    val = APAna_Values(file, tr, br, path, header, single)
    #print(val)
    if num == 1:
        Conv_RTfile(val)
    
    if num == 2:
        Calc_Avg_wf(val)
        Calc_Diff(val)
        Calc_Dev(val)
    if num == 3:
        if "0-1" in val["header"]:
            Calc_MP_Charge(val)
        AP_AnaN(val)
        AP_AnaS(val)
        AP_AnaK(val)
    """
    """

#以下メイン関数
if __name__== "__main__":
    
    args = sys.argv
    ana_id = args[1]
    mode = int(args[2])

    if mode == 0:
        singlePE_RT_set = sorted(glob.glob("/Users/kiyomoto/reaserch/PMT_data/LST-1 QC/APmeasurement002/data/SinglePhe_NHV/*{0}*convroot.root".format(ana_id)))
        input_s = singlePE_RT_set[1]
        input_d = singlePE_RT_set[0]
        InputFile_Check(input_s,"SinglePE")
        InputFile_Check(input_d,"SinglePE")

        if singlePE_RT_set[0].split("/")[6] == "APmeasurement003":
            date = "20231022"
        elif singlePE_RT_set[0].split("/")[6] == "APmeasurement002":
            date = "20231020"
        did = singlePE_RT_set[0].split("/")[-1].split("_")[-2]

        p = Pool(30)
        for i in range(7):
            path = "/Users/kiyomoto/reaserch/231125/{0}/pix{1}/SinglePE/".format(did, i)
            f = OuputFile_Check(path,"SinglePE")
            header = "{0}_{1}_{2}_".format(date, did, "pix{0}".format(i))
            val = SEAna_Values(input_s, input_d, "Events", "waveH{0}".format(i), path, header)
            p.apply_async(Ana_SinglePE, args=(val,))
        p.close()
        p.join()
    

    rt_set = sorted(glob.glob("/Users/kiyomoto/reaserch/PMT_data/LST-1 QC/APmeasurement002/data/Afterpulse/*{0}*convroot.root".format(ana_id)))

    p = Pool(35)
    for i in range(7):
        for l in range(len(rt_set)):

            date, did, timing = Def_Header(rt_set[l])
            header = "{0}_{1}_{2}_{3}_".format(date, did, "pix{0}".format(i), timing)
            path = "/Users/kiyomoto/reaserch/231125/{0}/pix{1}/AP/".format(did, i)
            f = OuputFile_Check(path,"AP")

            single_path = glob.glob("/Users/kiyomoto/reaserch/231125/{0}/pix{1}/SinglePE/*para.pkl".format(did, i))[0]
            single_pe = float(pd.read_pickle(single_path)[4])

            if mode == 1 or mode == 2 or mode == 3:
                inkey = mode, rt_set[l], 'waveH{0}'.format(i), path, header, single_pe, 'Events'
                p.apply_async(APana_Seq, args=inkey)
                #APana_Seq(mode, rt_set[l], 'waveH{0}'.format(i), path, header, single, 'Events')

            if mode == 4:
                if l == 0:
                    multi_path = sorted(glob.glob("/Users/kiyomoto/reaserch/231125/{0}/pix{1}/AP/*MP_para.pkl".format(did, i)))[0]
                    multi_pe = float(pd.read_pickle(multi_path)[1])
                    val = APAna_Values(rt_set[l], 'Events', 'waveH{0}'.format(i), path, header, single_pe)
                    tpo = Def_ALL_result(val)
                    #print(tpo)
                    #inkey = val, tpo, single_pe, multi_pe
                    #p.apply_async(Merge_All, args=inkey)
                    Merge_All(val, tpo, single_pe, multi_pe)
    p.close()
    p.join()
    

    if mode == 4:
        merger = pypdf.PdfMerger()
        merger2 = pypdf.PdfMerger()
        merger3 = pypdf.PdfMerger()

        #did = "ID1-118"
        p = "/Users/kiyomoto/reaserch/231125/{0}/".format(did)
        
        for i in range(7):
            path = "/Users/kiyomoto/reaserch/231125/{0}/pix{1}/AP/".format(did, i)
            merger.append(path+"Result_N.pdf")
            merger2.append(path+"Result_S.pdf")
            merger3.append(path+"Result_K.pdf")
        
        merger.write(p + 'Result_all_N.pdf')
        merger.close()
        merger2.write(p + 'Result_all_S.pdf')
        merger2.close()
        merger3.write(p + 'Result_all_K.pdf')
        merger3.close()
        print(GREEN + "END Summarize {0}".format(did))
    

    #print('All subprocesses done.')