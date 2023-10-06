import ROOT as RT
import subprocess
import sys

#ファイル名,ツリー名
args = sys.argv
AP_file = args[1]
tree = args[2]
CH = args[3]
events = args[4]
output_file = args[5]

#ROOTファイルを開き描画する
file = RT.TFile.Open(AP_file)
tree_O = file.Get(tree)
c1 = RT.TCanvas("c1", "c1", 800, 500)
#tree_O.Draw("wform1-wform0:time")
#tree_O.Draw("wave:time>>(4504,-0.5e-6,3e-6,1000,-0.05,0.35)","","colz", 50001, 0)
if CH == "single":
    tree_O.Draw("wform1-wform0:time>>(1024,0,200,1024,-20,50)","","colz", int(events), 0)
elif CH == "multi":
    tree_O.Draw("wform3-wform2:time>>(1024,0,200,1024,-20,500)","","colz", int(events), 0)
elif CH == "AP":
    tree_O.Draw("wform1-wform0:time>>(1024,0,1000,1024,-20,200)","","colz",int(events), 0)
else:
    print("ERROR: please set correct args!! 10 or 32")
    sys.exit()

#ヒストグラムのオブジェクト名を入手
obj = c1.FindObject("")
#obj.SetStats(0)
text = args[1].split('.')
title = text[0] + " " + args[2] + " " + "CH" + CH[0] +" - CH" + CH[-1] + ";" + "time (ns);Vol (mV);Counts"
#title = "CH1-CH0" + " " + args[2] + ";" + "time (ns);Vol (mV);Counts"
obj.SetTitle(title)
c1.SetLogz()
c1.Draw()
#pic_name  = text[0] + "_" + args[2] + ".png"
#pic_name2 = text[0] + "_" + args[2] + ".pdf"
#pic_name2 = text[0] + "_" + args[2] + "_" + "CH" + CH[0] +" - CH" + CH[-1] + ".pdf"
#pic_name2 = args[2] + "_" + "CH" + CH[0] +" - CH" + CH[-1] + ".pdf"
pic_name2 = output_file + ".pdf"
#c1.SaveAs("/Users/cta/work/dora_data/pmt/230222/ZQ7568/fig/" + pic_name)
#save_loc = "/Users/cta/work/dora_data/pmt/230518/ZQ7568/fig/" + pic_name2
cp = subprocess.run("pwd", capture_output=True, text=True)
save_loc = cp.stdout.split("\n")[0] +"/" + pic_name2
#save_loc = "/Users/cta/work/dora_data/pmt/230525/fig/" + pic_name2
#save_loc = cp.stdout +"/hoge.pdf"
c1.SaveAs(save_loc)
c1.ls()
#subprocess.run(['open', save_loc])
#subprocess.run('open hoge.png')