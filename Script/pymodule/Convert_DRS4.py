import ROOT as RT
import sys
import subprocess

def Create_ROOT_file(original_file, path, new_file):
    RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/Convert_DRS4.h")
    Treename_s = "Treesingle_0"
    Treename_d = "Treedark_0"
    RT.ConvertFile(original_file, Treename_s, path+"temporary1.root", Treename_s)
    RT.ConvertFile(original_file, Treename_d, path+"temporary2.root", Treename_d)
    subprocess.run(["hadd", new_file, path+"temporary1.root", path+"temporary2.root"])
    subprocess.run(["rm", path+"temporary1.root"])
    subprocess.run(["rm", path+"temporary2.root"])

    print("##### Convert ROOT file! #####")