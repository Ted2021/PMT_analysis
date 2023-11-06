import uproot
import numpy as np

def Convert_RT_to_numpy(RT_file, tree_name, branch_list):
    branch = {}
    file = uproot.open(RT_file)
    tree = file[tree_name]
    
    for item in branch_list:
        branch[item] = np.array(tree[item])

    """
    branch["time"] = np.array(tree["time"])
    #branch["wform"] = np.array(tree["wform1"]) - np.array(tree["wform0"])
    branch["wform"] = np.array(tree["wform"])
    #branch["stc"] = np.array(tree["stopcell"])
    """
    
    return branch