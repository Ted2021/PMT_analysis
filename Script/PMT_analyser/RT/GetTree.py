import ROOT as RT
import glob
import sys

def GetTree(file):
    #ROOTファイルを開く
    root_file = RT.TFile(file, "READ")

    # ROOTファイル内のすべてのキーを取得
    keys = root_file.GetListOfKeys()

    # Treeオブジェクトを格納するためのリストを作成
    tree_list = []

    # キーを反復処理してTreeオブジェクトを取得
    for key in keys:
        obj = key.ReadObj()
        if isinstance(obj, RT.TTree):
            tree_list.append(obj)

    list_of_tree = []
    # Treeリストを表示
    for tree in tree_list:
        #print(f"Tree Name: {tree.GetName()}")
        list_of_tree.append(tree.GetName())

    # ROOTファイルを閉じる
    root_file.Close()
    
    #Treeリストを返す
    return list_of_tree

if __name__ == '__main__':
    args = sys.argv
    file = args[1]
    tree_list = GetTree(file)
    print(tree_list)