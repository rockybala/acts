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
    parser.add_argument('-i1',
                        '--infile1',
                        dest = 'infile1',
                        help = 'input ROOT file 1')
    parser.add_argument('-i2',
                        '--infile2',
                        dest = 'infile2',
                        help = 'input ROOT file 2')
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
    infile1 = options.infile1
    infile2 = options.infile2
    path = os.path.join(dir_path, out_dir)
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)
    os.chdir(path)

    # Read input ROOT file
    infile1 = ROOT.TFile(infile1,"READ")
    infile2 = ROOT.TFile(infile2,"READ")

    # Create TCanvas
    canvas = ROOT.TCanvas('c1','c1')
    k1 = list(infile1.GetListOfKeys())
    k2 = list(infile2.GetListOfKeys())

    # Note, need to fix this with scatterplots
    for i in range(len(k1)):
        to_plot1 = k1[i].ReadObj()
        to_plot2 = k2[i].ReadObj()
        to_plot1.Draw("")
        to_plot1.SetLineColor(ROOT.kBlack)
        to_plot2.Draw("SAME")
        to_plot2.SetLineColor(ROOT.kRed)
        legend = ROOT.TLegend (0.7 ,0.6 ,0.85 ,0.75)
        legend.AddEntry(to_plot1,'EA Test Params')
        legend.AddEntry(to_plot2,'EA Prev Params')
        legend.SetLineWidth(0)
        legend.SetFillStyle(0)
        legend.Draw('same')
        canvas.SaveAs('{0}_superimposed.pdf'.format(k1[i].GetName()))
        canvas.Clear()

if __name__ == '__main__':
    main()
