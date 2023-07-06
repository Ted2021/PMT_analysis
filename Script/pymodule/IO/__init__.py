import os
import ROOT as RT

from .Convert_DRS4 import *

path = __file__.split("_")
RT.gROOT.LoadMacro(path[0] + "macro/Convert_DRS4.h")