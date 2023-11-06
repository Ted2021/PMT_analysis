import ROOT as RT
import numpy as np
import sys


#SETUP
args = sys.argv
INPUT_file = args[1]
Tree_list = ["TreeAfterPulseSeq1_0", "TreeAfterPulseSeq2_0", "TreeAfterPulseSeq3_0"]
Tree_list = ["TreeAfterPulseSeq1_0", "TreeAfterPulseSeq2_0"]
avg_wf = RT.std.vector(float)(np.zeros(1024))

#LOAD MACRO
nagayoshi_macro = "/Users/kiyomoto/git/Script/PMT_analyser/AP_analysis/macro/Nagayoshi_method.h"
sakurai_macro = "/Users/kiyomoto/git/Script/PMT_analyser/AP_analysis/macro/Sakurai_method.h"
RT.gROOT.LoadMacro(nagayoshi_macro)
RT.gROOT.LoadMacro(sakurai_macro)

#RUN the macro
for item in Tree_list:
    RT.CalcDiffWform_branch(INPUT_file, item, avg_wf)

for item in Tree_list:
    RT.ClacWf_branch(INPUT_file, item, avg_wf)

