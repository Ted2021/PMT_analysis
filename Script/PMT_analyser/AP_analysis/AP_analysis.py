import ROOT as RT
import numpy as np
import pickle
import sys

def Generate_Diff_file(work_dir, file_original, file_diff, avg_d, tr_o = "Treesource_0", tr_d = "Treediff_0"):
    av_wave_d = RT.std.vector(float)(avg_d)
    RT.CalcDiffWform4(work_dir+file_original, tr_o, work_dir+file_diff, tr_d, av_wave_d)

def Generate_Wform_file(work_dir, file_original, file_diff, avg_s, tr_o = "Treesource_0", tr_s = "Treesource_0"):
    av_wave = RT.std.vector(float)(avg_s)
    RT.CalcWform2(work_dir+file_analysis, tr_o, work_dir+file_sc, tr_s, av_wave)

def AP_find_N(work_dir, file, DIFF_THRES, int_s, int_e, N_pkl, tr = "Treediff_0"):
    event = RT.std.vector(int)()
    seg = RT.std.vector(int)()
    difwf = RT.std.vector(float)()
    counts = RT.std.vector(int)()
    RT.CountAPeventFromDiff(work_dir+file, tr, 1023, DIFF_THRES, int_s, int_e, event, seg, difwf, counts)

    N2 = pd.DataFrame({'event':np.array(event),
                      'seg':np.array(seg),
                      'difwf':np.array(difwf),
                      'counts':np.array(counts)
                     })
    N2.to_pickle(work_dir+N_pkl)

def AP_find_S(work_dir, file, PEAK_THES, CONTI_THES, int_s, int_e, S_pkl, tr = "Treesource_0"):
    event2 = RT.std.vector(int)()
    seg2 = RT.std.vector(int)()
    peak2 = RT.std.vector(float)()
    counts2 = RT.std.vector(int)()
    RT.CountAPeventFromPH4(work_dir+file,  tr, 1024, PEAK_THES, CONTI_THES, int_s, int_e, av_wave, event2, seg2, peak2, counts2)

    S3 = pd.DataFrame({'event':np.array(event2),
                    'seg':np.array(seg2),
                    'peak':np.array(peak2),
                    'counts':np.array(counts2)
                    })
    S3.to_pickle(work_dir + S_pkl)

def AP_find_K(work_dir, file_diff, file_wf, DIFF_THRES, PEAK_THRES, N2, K_pkl, tr_diff = "Treediff_0", tr_wf = "Treesource_0"):
    e = np.array(N2["event"])
    s = np.array(N2["seg"])
    event_list = RT.std.vector(int)(e)
    seg_list = RT.std.vector(int)(s)
    p1 = RT.std.vector(int)() 
    p2 = RT.std.vector(int)() 
    p3 = RT.std.vector(int)() 
    p4 = RT.std.vector(int)() 
    p5 = RT.std.vector(int)() 
    p6 = RT.std.vector(float)() 
    p7 = RT.std.vector(float)() 
    p8 = RT.std.vector(float)() 
    RT.JudgeAPevent(work_dir+file_diff, tr_diff, work_dir+file_sc, tr_wf, 1024, DIFF_THRES, PEAK_THRES, 2.4,2.4,event_list,seg_list,p1,p2,p3,p4,p5,p6,p7,p8)
    
    #これまでの解析結果をpd.Dataframeにまとめる
    K = pd.DataFrame({'event':e,
                      'seg':s,
                      'Label':np.array(p1),
                      'Start_cell':np.array(p2),
                      'peak_cell':np.array(p3),
                      'ankle_cell':np.array(p4),
                      'end_cell':np.array(p5),
                      'difwf':np.array(N2["difwf"]),
                      'Diff_peak':np.array(p6),
                      'Ph_peak':np.array(p7),
                      'ankle':np.array(p8)
                     })
    K.to_pickle(work_dir + K_pkl)
