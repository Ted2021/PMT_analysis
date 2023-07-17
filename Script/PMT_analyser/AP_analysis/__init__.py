import os
import ROOT as RT

from .Counts_AP_N import *
from .Counts_AP_S import *
from .Counts_AP_K import *
from .Combine import *

path = __file__.split("__init__.py")
RT.gROOT.LoadMacro(path[0] + "macro/Kiyomoto_method.h")
RT.gROOT.LoadMacro(path[0] + "macro/Nagayoshi_method.h")
RT.gROOT.LoadMacro(path[0] + "macro/Sakurai_method.h")

