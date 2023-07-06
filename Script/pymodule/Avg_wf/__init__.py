import os
import ROOT as RT

from .Calc_Avg_Wf import *

path = __file__.split("_")
RT.gROOT.LoadMacro(path[0] + "macro/Generate_Avg.h")