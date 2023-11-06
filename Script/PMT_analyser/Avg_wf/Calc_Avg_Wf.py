import ROOT as RT
import numpy as np
import pickle
import sys
import matplotlib.pyplot as plt

def Calc_Avg_Wf(file, dfile, name_s, name_d,
                tr_s="Treesource_0", tr_d="Treedark_0", branch_t="time", branch_w="wform", seg = 1024, mode = "all"):
    #マクロの読み込み
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/Avg_wf/Generate_Avg.h")

    #sourceの平均波形を求める
    def calc_source_avg(file, tr_s, branch_t, branch_w, seg, name_s):
        
        #ROOTマクロで計算
        av_wave = RT.std.vector(float)()
        av_time = RT.std.vector(float)()
        RT.Average_Make(file, tr_s, branch_t, branch_w, av_time, av_wave, seg)

        #pklファイルに書き出す
        with open(name_s, "wb") as f:
            pickle.dump(np.array(av_wave), f)
        print('\033[31m' + "##### Create Avg files(Source)! #####" + '\033[0m')

        return av_wave

    #darkの平均波形を求める
    def calc_dark_avg(dfile, tr_d, branch_t, branch_w, seg, name_d):
        av_wave_d = RT.std.vector(float)()
        av_time_d = RT.std.vector(float)()
        RT.Average_Make(dfile, tr_d, branch_t, branch_w, av_time_d, av_wave_d, seg)

        #pklファイルに書き出す
        with open(name_d, "wb") as f:
            pickle.dump(np.array(av_wave_d), f)
        print('\033[31m' + "##### Create Avg files(Dark)! #####" + '\033[0m')

        return av_wave_d

    if mode == "all":
        calc_source_avg(file, tr_s, branch_t, branch_w, seg, name_s)
        calc_dark_avg(dfile, tr_d, branch_t, branch_w, seg, name_d)
    elif mode == "source":
        calc_source_avg(file, tr_s, branch_t, branch_w, seg, name_s)
    elif mode == "dark":
        calc_dark_avg(dfile, tr_d, branch_t, branch_w, seg, name_d)
    else:
        print("MODE ERROR! PLEASE INPUT CORRECT MODE!")
        sys.exit(1)

def Calc_Avg_Wf_all(file, dfile, name_s, name_d,
                tr_s="Treesource_0", tr_d="Treedark_0", branch_t="time", branch_w="wform", seg = 1024):
    #マクロの読み込み
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/Avg_wf/Generate_Avg.h")

    #darkの平均波形を求める
    av_wave_d = RT.std.vector(float)()
    av_time_d = RT.std.vector(float)()
    RT.Average_Make(dfile, tr_d, branch_t, branch_w, av_time_d, av_wave_d, seg)

    #sourceの平均波形を求める
    av_wave = RT.std.vector(float)()
    av_time = RT.std.vector(float)()
    RT.Average_Make(file, tr_s, branch_t, branch_w, av_time, av_wave, seg)
    
    with open(name_s, "wb") as f:
        pickle.dump(np.array(av_wave), f)
    print("##### Create Avg files(Source)! #####")

    with open(name_d, "wb") as f:
        pickle.dump(np.array(av_wave_d), f)
    print("##### Create Avg files(Dark)! #####")

    return av_time, av_wave, av_wave_d

def Calc_Avg_Wf_source(file, name_s,
                tr_s="Treesource_0", tr_d="Treedark_0", branch_t="time", branch_w="wform", seg = 1024):
    #マクロの読み込み
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/Avg_wf/Generate_Avg.h")

    #sourceの平均波形を求める
    av_wave = RT.std.vector(float)()
    av_time = RT.std.vector(float)()
    RT.Average_Make(file, tr_s, branch_t, branch_w, av_time, av_wave, seg)
    
    with open(name_s, "wb") as f:
        pickle.dump(np.array(av_wave), f)
    print("##### Create Avg files(Source)! #####")

    return av_wave


def Calc_Avg_Wf_dark(dfile, name_d,
                tr_s="Treesource_0", tr_d="Treedark_0", branch_t="time", branch_w="wform", seg = 1024):
    #マクロの読み込み
    #RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/Avg_wf/Generate_Avg.h")

    #darkの平均波形を求める
    av_wave_d = RT.std.vector(float)()
    av_time_d = RT.std.vector(float)()
    RT.Average_Make(dfile, tr_d, branch_t, branch_w, av_time_d, av_wave_d, seg)

    with open(name_d, "wb") as f:
        pickle.dump(np.array(av_wave_d), f)
    print("##### Create Avg files(Dark)! #####")

    return av_wave_d

def Plot_Avg(av_wave, av_wave_d,fig_name, span = [-25, 50], lim = [-100, 100], ped = [20, 50], title = "Avg_wf"):
    fig, ax = plt.subplots()
    x = np.linspace(0, 200, 1024)
    max = int(np.argmax(np.array(av_wave)))
    plt.plot(x,np.array(av_wave)-np.array(av_wave_d), label = "Avg wf.")
    ax.axvspan(x[max+span[0]], x[max+span[1]], color="gray", alpha = 0.15, label= "Int range")
    ax.axvspan(x[ped[0]], x[ped[1]], color="cyan", alpha = 0.15, label= "Ped range")
    plt.axvline(x[max], linestyle = "--", color = "red")
    plt.text(x[max], 100, str(max))
    plt.text(x[ped[0]], 100, str(ped[0]))
    plt.text(x[ped[1]], 100, str(ped[1]))
    plt.xlim(x[lim[0]], x[lim[1]])
    plt.xlabel("time (ns)")
    plt.ylabel("Volt (mV)")
    plt.title(title)
    plt.legend()
    plt.savefig(fig_name)
    #plt.show()
    print("##### Create Avg plot! #####")


    return max

if __name__ == '__main__':
    args = sys.argv
    Calc_Avg_Wf(args[1], args[2], args[3])