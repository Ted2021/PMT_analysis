import os
import ROOT as RT

from .Charge_Dist import *

path = __file__.split("_")
RT.gROOT.LoadMacro(path[0] + "macro/Make_Chrg_Dist.h")