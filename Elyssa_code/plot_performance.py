#!/usr/bin/env python
"""
Script to plot performance from CKF ACTS
"""
__author__ = "Elyssa Hofgard"


import ROOT
import argparse
import os,sys
import shutil

###############################################################################                                   
# Command line arguments
######################## 
def getArgumentParser():
    """ Get arguments from command line"""
    parser = argparse.ArgumentParser(description="Script to plot performance from CKF ACTS")
    parser.add_argument('-i',
                        '--infile',
                        dest = 'infile',
                        help = 'input ROOT file')
    parser.add_argument('-o',
                        '--outdir',
                        dest = 'outdir',
                        help = 'output directory for plot etc',
                        default = 'outdir')
    return parser

############################################################################### 

def main():
    options = getArgumentParser().parse_args()

    ### Make output dir
    dir_path = os.getcwd()
    out_dir = options.outdir
    infile = options.infile
    path = os.path.join(dir_path, out_dir)
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)
    os.chdir(path)

    # Read input ROOT file
    infile = ROOT.TFile(infile,"READ")

    # Create TCanvas
    canvas = ROOT.TCanvas()

    # Loop over keys and make a plot
    for k in infile.GetListOfKeys():
        to_plot = k.ReadObj()
        to_plot.Draw()
        canvas.SaveAs('{0}.pdf'.format(k.GetName()))
        canvas.Clear()

if __name__ == '__main__':
    main()


