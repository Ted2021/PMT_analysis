def Convert_RT_to_numpy(RT_file, tree_name):
    branch = {}
    file = uproot.open(RT_file)
    tree = file[tree_name]
    
    branch["time"] = np.array(tree["time"])
    branch["wform"] = np.array(tree["wform1"]) - np.array(tree["wform0"])
    #branch["wfn"] = np.array(tree["wform0"])
    branch["stc"] = np.array(tree["stopcell"])
    
    return branch