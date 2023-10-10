import PMT_analyser.IO.Convert_DRS4 as DRS

original_file = "/Users/kiyomoto/reaserch/PMT_data/pmt/140929/201409291432ZQ5531.root"
Treelist = ["TreeAfterPulseSeq1_0", "TreeAfterPulseSeq2_0"]
path = "/Users/kiyomoto/reaserch/231010/"

for item in Treelist:
    DRS.Creat_ROOT_file(original_file, item, path + original_file.split(".root")[0] + "_" +item + ".root", item, "wform1", "wform0", 1400)

DRS.Merge_ROOT_file(Treelist[0], Treelist[1], original_file.split(".root")[0] + "_sabun" + ".root")

