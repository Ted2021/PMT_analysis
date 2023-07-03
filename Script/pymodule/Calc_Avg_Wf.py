import ROOT as RT
import numpy as np
import pickle
import sys

RT.gROOT.LoadMacro("/Users/kiyomoto/reaserch/8dy_Noise/scripts/macros/average_wave.h")

def Calc_Avg_Wf(file, name_s, name_d):
    #darkの平均波形を求める
    av_wave_d = RT.std.vector(float)()
    av_time_d = RT.std.vector(float)()
    RT.Average_Make(file, "Treedark_0", "wform1", "wform0", av_wave_d, av_time_d)

    #sourceの平均波形を求める
    av_wave = RT.std.vector(float)()
    av_time = RT.std.vector(float)()
    RT.Average_Make(file, "Treesingle_0", "wform1", "wform0", av_wave, av_time)
    
    with open(name_s, "wb") as f:
        pickle.dump(np.array(av_wave), f)

    with open(name_d, "wb") as f:
        pickle.dump(np.array(av_wave_d), f)


if __name__ == '__main__':
    args = sys.argv
    Calc_Avg_Wf(args[1], args[2], args[3])