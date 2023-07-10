import pymodule.IO.Convert_DRS4 as DRS4
import pymodule.Avg_wf.Calc_Avg_Wf as AVG
import pymodule.Charge_analysis.Charge_Dist as Chrg
import pymodule.Fitting.Fitting as Fit
import sys
import pandas as pd
import os
import subprocess
import ROOT as RT
import numpy as np
import pickle
import matplotlib.pyplot as plt
import glob

def gau(x, p0, p1, p2):
            return p0*np.exp(-0.5*((x-p1)/p2)**2)

def triple_gau_2(x, l_para):
    p0, p1, p2, p3, p4, p5, p6, p7, p8 = l_para[0], l_para[1], l_para[2], l_para[3], l_para[4], l_para[5], l_para[6], 2*l_para[4], l_para[7]
    return gau(x, p0, p1, p2)+gau(x, p3, p4, p5)+gau(x, p6, p7, p8)

def Single_analysis(file, work_dir):
    tr = "Treesource_0"
    tr_d = "Treedark_0"

    new_file = "new.root"
    avg_s_pkl = "avg_s.pkl"
    avg_d_pkl = "avg_d.pkl"
    avg_fig = "Avg.png"
    chrg_pkl = "chrg.pkl"
    chrg_fig = "Chrg.png"
    histX_pkl = "histX_.pkl"
    histY_pkl = "histY_.pkl"
    para_pkl = "para.pkl"

    create = DRS4.Create_ROOT_file_2386(file, file, work_dir, work_dir+new_file, 10, tr, tr_d)
    del create
    avg_wf = AVG.Calc_Avg_Wf_all(work_dir+new_file, work_dir+new_file, work_dir+avg_s_pkl, work_dir+avg_d_pkl)
    int_max  = AVG.Plot_Avg(avg_wf[1], avg_wf[2], work_dir+avg_fig)
    charge = Chrg.Calc_Charge(work_dir+new_file, work_dir+new_file, int_max - 25, int_max + 50, work_dir+chrg_pkl)
    hist = Chrg.Plot_Hist(charge, -20, 120)
    p = Fit.Find_Fit_Para(hist, work_dir+histX_pkl, work_dir+histY_pkl)
    para = Fit.Fit_trpl_Gauss(hist, p[0], p[1], p[2], [-20, 100], work_dir+para_pkl)

def Multi_analysis(file, work_dir):
    tr = "Treesource_0" 
    tr_d = "Treedark_0"

    new_file = "new_multi.root"
    avg_s_pkl = "avg_s_multi.pkl"
    avg_d_pkl = "avg_d_multi.pkl"
    avg_fig = "Avg_multi.png"
    chrg_pkl = "chrg_multi.pkl"
    chrg_fig = "Chrg_multi.png"
    histX_pkl = "histX_.pkl"
    histY_pkl = "histY_.pkl"
    para_pkl = "para_multi.pkl"

    create = DRS4.Create_ROOT_file_2386(file, file, work_dir, work_dir+new_file, 32, tr, tr_d)
    avg_wf = AVG.Calc_Avg_Wf_all(work_dir+new_file, work_dir+new_file, work_dir+avg_s_pkl, work_dir+avg_d_pkl)
    int_max  = AVG.Plot_Avg(avg_wf[1], avg_wf[2], work_dir+avg_fig)

    charge = Chrg.Calc_Charge(work_dir+new_file, work_dir+new_file, int_max - 20, int_max + 50, work_dir+chrg_pkl)
    min_c = int(np.min(np.array(charge)))
    max_c = int(np.max(np.array(charge)))
    hist = Chrg.Plot_Hist(charge, min_c-10, max_c+10)

    p = Fit.Find_Fit_Para(hist, work_dir+histX_pkl, work_dir+histY_pkl)
    para = Fit.Fit_sgl_Gauss(hist, p[0], p[1], p[2], [min_c-50, max_c+50], work_dir+para_pkl)

def plot_single(work_dir):
    #平均波の表示
        avg_s_pkl = "avg_s.pkl"
        avg_d_pkl = "avg_d.pkl"
        single = pd.read_pickle(work_dir + avg_s_pkl)
        multi = pd.read_pickle(work_dir + avg_d_pkl)

        histX_pkl = "histX_.pkl"
        histY_pkl = "histY_.pkl"
        X = pd.read_pickle(work_dir + histX_pkl)
        Y = pd.read_pickle(work_dir + histY_pkl)

        p = "para.pkl"
        para = pd.read_pickle(work_dir + p)

        fig_name = "single_avg.png"
        fig_name2 = "single_chrg.png"
        
        fig, ax = plt.subplots()
        x = np.linspace(0, 200, 1024)
        max = int(np.argmax(single))
        plt.plot(x,single-multi, label = "Avg wf.")
        ax.axvspan(x[max-25], x[max+50], color="gray", alpha = 0.15, label= "Int range")
        plt.xlim(x[max-100], x[max+100])
        plt.xlabel("time (ns)")
        plt.ylabel("Volt (mV)")
        plt.legend()
        plt.savefig(work_dir + fig_name)
        #plt.show()

        #電荷量の表示
        fig, ax = plt.subplots()
        ax.plot(X,Y,c="blue", label="Charge Dist.")
        y_best = triple_gau_2(X, para)
        plt.plot(X,y_best,c="red",label="Best Fit")
        ax.set_yscale("log")
        ax.grid(which="both")
        ax.set_ylim(0.6, np.max(y_best)+100)
        plt.xlabel("Charge (mV*ns)")
        plt.ylabel("#Events")
        plt.legend()
        plt.savefig(work_dir + fig_name2)
        #plt.show()

def plot_multi(work_dir):
    #平均波の表示
        avg_s_pkl = "avg_s_multi.pkl"
        avg_d_pkl = "avg_d_multi.pkl"
        single = pd.read_pickle(work_dir + avg_s_pkl)
        multi = pd.read_pickle(work_dir + avg_d_pkl)

        histX_pkl = "histX_.pkl"
        histY_pkl = "histY_.pkl"
        X = pd.read_pickle(work_dir + histX_pkl)
        Y = pd.read_pickle(work_dir + histY_pkl)

        p = "para_multi.pkl"
        para = pd.read_pickle(work_dir + p)

        fig_name = "multi_avg.png"
        fig_name2 = "multi_chrg.png"
        
        fig, ax = plt.subplots()
        x = np.linspace(0, 200, 1024)
        max = int(np.argmax(single))
        plt.plot(x,single-multi, label = "Avg wf.")
        ax.axvspan(x[max-25], x[max+50], color="gray", alpha = 0.15, label= "Int range")
        plt.xlim(x[max-100], x[max+100])
        plt.xlabel("time (ns)")
        plt.ylabel("Volt (mV)")
        plt.legend()
        plt.savefig(work_dir + fig_name)
        #plt.show()

        #電荷量の表示
        fig, ax = plt.subplots()
        ax.plot(X,Y,c="blue", label="Charge Dist.")
        y_best = gau(X, para[0], para[1], para[2])
        plt.plot(X,y_best,c="red",label="Best Fit")
        ax.set_yscale("log")
        ax.grid(which="both")
        ax.set_ylim(0.6, np.max(y_best)+100)
        plt.xlabel("Charge (mV*ns)")
        plt.ylabel("#Events")
        plt.legend()
        plt.savefig(work_dir + fig_name2)
        #plt.show()

if __name__ == '__main__':
    # file_setup
    new_dir_path = "/Users/kiyomoto/reaserch/230710/"
    """
    for name in glob.glob('/Users/kiyomoto/reaserch/PMT_data/pmt/*/*/*single.root'):
        sfile_path = name
        PMT = name.split("_single.root")[0].split("_")[-1]
        os.mkdir(new_dir_path+PMT)
        dir_path = new_dir_path+PMT + "/single/"
        os.mkdir(dir_path)
        print("Target" + sfile_path)
        Single_analysis(sfile_path, dir_path)
        plot_single(dir_path)
    """

    for name in glob.glob('/Users/kiyomoto/reaserch/PMT_data/pmt/*/*/*multi.root'):
        mfile_path = name
        PMT = name.split("_multi.root")[0].split("_")[-1]
        dir_path = new_dir_path + PMT + "/multi/"
        os.mkdir(dir_path)
        print("Target" + mfile_path)
        Multi_analysis(mfile_path, dir_path)
        plot_multi(dir_path)

