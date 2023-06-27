#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "cmath"

int calc(float arr[])
{
    float sum = 0;
    
    for(int i=100;i<200; i++)
    {
        sum +=std::abs(arr[i]);
    }
    return sum;
}

void CountAPeventFromPH2(TString filesrc, TString treename, std::vector<float> &par)
{
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[1024];
    float wfp[1024];
    float wfn[1024];
    int sc = 0;
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress("wform1", wfp);
    tr->SetBranchAddress("wform0", wfn);
    tr->SetBranchAddress("stopcell", &sc);

    int nEve = tr->GetEntries();
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        float answer = calc(time);
        par.push_back(answer);    
    }
    f->Close();
}