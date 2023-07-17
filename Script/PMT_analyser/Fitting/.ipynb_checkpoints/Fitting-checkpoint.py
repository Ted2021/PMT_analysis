import ROOT as RT
import numpy as np
import pickle
import sys
import matplotlib.pyplot as plt
import pandas as pd

def Find_Fit_Para(hst, name_h1, name_h2):
    #hist = plt.hist(chrg, bins = nbins)
    hist_value = pd.DataFrame({'X':[hst.GetXaxis().GetBinCenter(i) for i in range(hst.GetNbinsX())],
                                'Y':[hst.GetBinContent(i) for i in range(hst.GetNbinsX())]
                            })
    mean = np.mean(hist_value[hist_value["Y"] == np.max(hist_value[hist_value["X"] > 10]["Y"])]["X"])
    height = np.mean(hist_value[hist_value["Y"] == np.max(hist_value[hist_value["X"] > 10]["Y"])]["Y"])
    sigma = mean/2

    with open(name_h1, "wb") as f:
        pickle.dump(np.array(hist_value["X"]), f)
    with open(name_h2, "wb") as f:
        pickle.dump(np.array(hist_value["Y"]), f)

    print("##### determine the syokiti #####")
    return mean, height, sigma, np.array(hist_value["X"]), np.array(hist_value["Y"])

def Fit_trpl_Gauss(hst, mean, height, sigma, fit_range, name_f):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/Fitting/Fitting.h")
    fit_para = RT.std.vector(float)()
    fit_para_e = RT.std.vector(float)()
    RT.Fit_trpl_Gauss(hst, fit_para, fit_para_e, mean, height, sigma, fit_range[0], fit_range[1])

    with open(name_f, "wb") as f:
        pickle.dump(np.array(fit_para), f)
        pickle.dump(np.array(fit_para_e), f)

    print("##### Complete Fitting #####")
    return list(fit_para), list(fit_para_e)

def Fit_sgl_Gauss(hst, mean, height, sigma, fit_range, name_f):
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/Fitting/Fitting.h")
    fit_para = RT.std.vector(float)()
    fit_para_e = RT.std.vector(float)()
    RT.Fit_sgl_Gauss(hst, fit_para, fit_para_e, mean, height, sigma, fit_range[0], fit_range[1])

    with open(name_f, "wb") as f:
        pickle.dump(np.array(fit_para), f)
        pickle.dump(np.array(fit_para_e), f)

    print("##### Complete Fitting #####")

    return list(fit_para), list(fit_para_e)


def gau(x, p0, p1, p2):
            return p0*np.exp(-0.5*((x-p1)/p2)**2)

def double_gau(x, l_para):
    p0, p1, p2, p3, p4, p5 = l_para[0], l_para[1], l_para[2], l_para[3], l_para[4], l_para[5]
    return gau(x, p0, p1, p2)+gau(x, p3, p4, p5)

def triple_gau(x, l_para):
    p0, p1, p2, p3, p4, p5, p6, p7, p8 = l_para[0], l_para[1], l_para[2], l_para[3], l_para[4], l_para[5], l_para[6], l_para[7], l_para[8]
    return gau(x, p0, p1, p2)+gau(x, p3, p4, p5)+gau(x, p6, p7, p8)

def triple_gau_2(x, l_para):
    p0, p1, p2, p3, p4, p5, p6, p7, p8 = l_para[0], l_para[1], l_para[2], l_para[3], l_para[4], l_para[5], l_para[6], 2*l_para[4], l_para[7]
    return gau(x, p0, p1, p2)+gau(x, p3, p4, p5)+gau(x, p6, p7, p8)


def Fig_Plot(X, Y, para, fig_name, bins = 330):
    fig, ax = plt.subplots()
    ax.plot(X,Y,c="blue", label="Charge Dist.")
    y_best = triple_gau_2(X, para)
    plt.plot(X,y_best,c="red",label="Best Fit")
    ax.set_yscale("log")
    ax.grid(which="both")
    ax.set_ylim(0.6, np.max(y_best))
    plt.xlabel("Charge (mV*ns)")
    plt.ylabel("#Events")
    plt.legend()
    plt.savefig(fig_name)
    #plt.show()
    print("##### Create charge dist! #####")

def Fig_Plot_multi(X, Y, para, fig_name, bins = 330):
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
    plt.savefig(fig_name)
    #plt.show()
    print("##### Create charge dist! #####")

