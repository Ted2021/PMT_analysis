import ROOT as RT
import sys
import subprocess

def Create_ROOT_file_2385(original_file, dark_file, path, new_file, CH=10, Treename_s = "Treesingle_0", Treename_d = "Treedark_0"):
    RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/IO/Convert_DRS4.h")
    if CH == 10:
        branch_p = "wform1"
        branch_n = "wform0"
    elif CH == 32:
        branch_p = "wform3"
        branch_n = "wform2"

    RT.ConvertFile_2385(original_file, Treename_s, path+"temporary1.root", Treename_s, branch_p, branch_n)
    RT.ConvertFile_2385(dark_file, Treename_d, path+"temporary2.root", Treename_d, branch_p, branch_n)
    subprocess.run(["hadd", new_file, path+"temporary1.root", path+"temporary2.root"])
    subprocess.run(["rm", path+"temporary1.root"])
    subprocess.run(["rm", path+"temporary2.root"])

    print("##### Convert ROOT file! #####")

def Create_ROOT_file_2386(original_file, dark_file, path, new_file, CH=10, Treename_s = "Treesingle_0", Treename_d = "Treedark_0"):
    RT.gROOT.LoadMacro("/Users/kiyomoto/git/Script/C_macro/IO/Convert_DRS4.h")
    if CH == 10:
        branch_p = "wform1"
        branch_n = "wform0"
    elif CH == 32:
        branch_p = "wform3"
        branch_n = "wform2"

    RT.ConvertFile_2386(original_file, Treename_s, path+"temporary1.root", Treename_s, branch_p, branch_n)
    RT.ConvertFile_2386(dark_file, Treename_d, path+"temporary2.root", Treename_d, branch_p, branch_n)
    subprocess.run(["hadd", new_file, path+"temporary1.root", path+"temporary2.root"])
    subprocess.run(["rm", path+"temporary1.root"])
    subprocess.run(["rm", path+"temporary2.root"])

    print("##### Convert ROOT file! #####") 