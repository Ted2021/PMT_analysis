import pandas as pd
import os
import subprocess
import sys
#import ROOT as RT
import numpy as np
import pickle
import matplotlib.pyplot as plt

def gau(x, p0, p1, p2):
            return p0*np.exp(-0.5*((x-p1)/p2)**2)

def triple_gau_2(x, l_para):
    p0, p1, p2, p3, p4, p5, p6, p7, p8 = l_para[0], l_para[1], l_para[2], l_para[3], l_para[4], l_para[5], l_para[6], 2*l_para[4], l_para[7]
    return gau(x, p0, p1, p2)+gau(x, p3, p4, p5)+gau(x, p6, p7, p8)


if __name__ == '__main__':

    #作業ディレクトリ
    args = sys.argv
    types = args[1]
    work_dir = args[2]

    if types == "single":
        #平均波の表示
        avg_s_pkl = "avg_s.pkl"
        avg_d_pkl = "avg_d.pkl"
        single = pd.read_pickle(work_dir + avg_s_pkl)
        multi = pd.read_pickle(work_dir + avg_d_pkl)

        hist_x = "histX_.pkl"
        hist_y = "histY_.pkl"
        X = pd.read_pickle(work_dir + hist_x)
        Y = pd.read_pickle(work_dir + hist_y)

        p = "para.pkl"
        para = pd.read_pickle(work_dir + p)

        fig_name = "joh.png"
        fig_name2 = "kk.png"
        
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

    elif types == "multi":
        #平均波の表示
        avg_s_pkl = "avg_s_multi.pkl"
        avg_d_pkl = "avg_d_multi.pkl"
        single = pd.read_pickle(work_dir + avg_s_pkl)
        multi = pd.read_pickle(work_dir + avg_d_pkl)

        hist_x = "histX_.pkl"
        hist_y = "histY_.pkl"
        X = pd.read_pickle(work_dir + hist_x)
        Y = pd.read_pickle(work_dir + hist_y)

        p = "para_multi.pkl"
        para = pd.read_pickle(work_dir + p)

        fig_name = "joh.png"
        fig_name2 = "kk.png"
        
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
