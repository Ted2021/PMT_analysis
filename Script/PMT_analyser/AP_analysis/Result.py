#import ROOT as RT
import numpy as np
import pickle
import sys
import pandas as pd
import os
import glob
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors
from matplotlib.colors import Normalize


#標準出力の色を指定
RED = '\033[31m'
BLUE = '\033[34m'
GREEN = '\033[32m'
YELLOW = '\033[33m'
END = '\033[0m'


def Plot_all_result(df_path, single_pe, multi_pe, event, title_txt, fig_name):
    fig = plt.figure(figsize = (18,10))

    ax1 = fig.add_subplot(2,2,1)
    ax2 = fig.add_subplot(2,2,2)
    ax3 = fig.add_subplot(2,2,3)

    df = pd.read_pickle(df_path)
    df_t = df[df["event"] != 0]
    df_h = df_t[df_t["charge"]/single_pe >= 4]
    df_over0 = df_t[df_t["charge"]/single_pe >= 1]

    ax1.hist(df["charge"]/single_pe, bins = 100, histtype = "step", color = "black")
    ax1.set_title("{0}".format(title_txt))
    ax1.set_yscale("log")
    ax1.set_xlabel('p.e.')
    ax1.set_ylabel('Counts')
    AP_rate = len(df_h) / ((multi_pe/single_pe)* event)
    ax1.text(5, 8e2, "APP > 4p.e. = {0}".format(AP_rate), size = 16)
    
    ax2.hist(df_over0["time"], bins = 100, histtype = "step", color = "black")
    ax2.hist(df_h["time"], bins = 100, histtype = "step", color = "red")
    ax2.set_title("{0}".format(title_txt))
    ax2.set_yscale("log")
    ax2.set_xlabel('time (ns)')
    ax2.set_ylabel('Counts')
    ax2.text(3e3, 1e3, "AP_Events : {0}".format(len(df)), size = 16)
    ax2.text(3e3, 6e2, "AP_counts : {0}".format(len(df_h)), color = "red", size = 16)
    
    
    x_scale=(0, 5000, 500)
    y_scale=(0, 40, 50)
    H = ax3.hist2d(np.array(df_over0["time"]), 
                  np.array(df_over0["charge"])/single_pe, 
                  bins=[np.linspace(x_scale[0], x_scale[1], x_scale[2]),
                        np.linspace(y_scale[0], y_scale[1], y_scale[2])], 
                  norm=matplotlib.colors.LogNorm(), cmap=cm.jet)
    
    ax3.set_title("{0}".format(title_txt))
    ax3.set_xlabel('time (ns)')
    ax3.set_ylabel('p.e.')

    fig.colorbar(H[3], ax=ax3)
    H[3].set_clim(1e0,1e4)
    
    plt.savefig(fig_name)
    #plt.show()

    print(GREEN + "Dump the Result!!" + END)