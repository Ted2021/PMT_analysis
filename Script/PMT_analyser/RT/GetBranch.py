import ROOT
import sys
import glob
from argparse import ArgumentParser

def GetBranch(file, branch):
    # ROOTファイルを開く
    root_file = ROOT.TFile(file, "READ")

    # ファイル内のツリー（またはツリーの名前）を指定
    tree = root_file.Get(branch)

    # ブランチのリストを取得
    branch_list = tree.GetListOfBranches()

    # ブランチの数を取得
    num_branches = branch_list.GetEntries()

    list_of_branch = []
    # すべてのブランチ名を表示
    for i in range(num_branches):
        branch = branch_list.At(i)
        #print(branch.GetName())
        list_of_branch.append(branch.GetName())

    # ファイルを閉じる
    root_file.Close()

    return list_of_branch

if __name__ == '__main__':
    args = sys.argv
    file = args[1]
    branch = "TreeAfterPulseSeq1_0"

    print(file)
    list_of_branch = GetBranch(file, branch)
    print(list_of_branch)