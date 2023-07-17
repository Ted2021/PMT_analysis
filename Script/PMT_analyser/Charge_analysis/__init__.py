import os
import ROOT as RT

from .Charge_Dist import *
from .Charge_AP import *

path = __file__.split("__init__.py")
RT.gROOT.LoadMacro(path[0] + "macro/Make_Chrg_Dist.h")
RT.gROOT.LoadMacro(path[0] + "macro/Calc_AP_charge.h")