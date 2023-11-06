import PMT_analyser.IO.Convert_DRS4 as DRS
import sys

#引数を指定
args = sys.argv
original_file = "/Users/kiyomoto/reaserch/PMT_data/pmt/141208/201412081640ZQ6094.root"
Treelist = ["TreeAfterPulseSeq1_0", "TreeAfterPulseSeq2_0", "TreeAfterPulseSeq3_0"]
path = "/Users/kiyomoto/reaserch/231010/ZQ6094/"

"""
original_file = "/Users/kiyomoto/reaserch/PMT_data/pmt/140929/201409291432ZQ5531.root"
Treelist = ["TreeAfterPulseSeq1_0", "TreeAfterPulseSeq2_0"]
path = "/Users/kiyomoto/reaserch/231010/"
"""

for item in Treelist:
    DRS.Creat_ROOT_file(original_file, item, path + item + ".root", item, "wformNeg", "wformPos", 0)

DRS.Merge_ROOT_file(path + Treelist[0] + ".root", path + Treelist[1] + ".root", path + "New.root")
DRS.Merge_ROOT_file(path + "New.root", path + Treelist[2] + ".root", path + "New2.root")


