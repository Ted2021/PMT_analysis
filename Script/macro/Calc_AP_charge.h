#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "cmath"

//APの電荷量を計算する関数(APのピーク位置のみわかる場合)
void CalcAPCharge(TString filesrc, TString treename, int cell, std::vector<float> &av_wf, std::vector<int> &event, std::vector<int> &seg, std::vector<float> &chrg)
{
    //SourceとDarkのファイル読み込み
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[cell];
    float wfp[cell];
    float wfn[cell];
    int sc;
    tr->SetBranchAddress("time", time);
    tr->SetBranchAddress("wform1",wfp);
    tr->SetBranchAddress("wform0",wfn);
    tr->SetBranchAddress("stopcell", &sc);

    //int nEve = tr->GetEntries();

    std::size_t array_size = event.size();
    float charge = 0;
    int target_seg;
    int INT_RANGE = 15;


    for(int i=0;i<array_size;i++)
    {
        tr->GetEntry(event[i]);
        charge = 0;
        target_seg = seg[i];
        for(int l=target_seg-5;l<target_seg + INT_RANGE;l++)
        {
            if((l+sc)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5;
                wfn[l] = (wfp[l-1] + wfp[l+1])*0.5;
            }
            //charge += ((wfp[l] - wfn[l]) - av_wf[l])*(time[l] - time[l-1]);
            charge += (wfp[l] - wfn[l])*(time[l] - time[l-1]);
        }
        chrg.push_back(charge);

    }
    f->Close();
}

//APの電荷量を計算する関数(APの積分範囲がわかる場合)
void CalcAPCharge2(TString filesrc, TString treename, int cell,
    std::vector<float> &av_wf, std::vector<int> &event, 
    std::vector<int> &start_cell,std::vector<int> &end_cell, std::vector<float> &chrg)
{
    //SourceとDarkのファイル読み込み
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[cell];
    float wfp[cell];
    float wfn[cell];
    int sc;
    tr->SetBranchAddress("time", time);
    tr->SetBranchAddress("wform1",wfp);
    tr->SetBranchAddress("wform0",wfn);
    tr->SetBranchAddress("stopcell", &sc);

    //int nEve = tr->GetEntries();

    std::size_t array_size = event.size();
    float charge = 0;
    //int target_seg;
    //int INT_RANGE = 15;


    for(int i=0;i<array_size;i++)
    {
        tr->GetEntry(event[i]);
        charge = 0;
        for(int l=start_cell[i];l<end_cell[i];l++)
        {
            if((l+sc)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5;
                wfn[l] = (wfp[l-1] + wfp[l+1])*0.5;
            }
            charge += ((wfp[l] - wfn[l]) - av_wf[l])*(time[l] - time[l-1]);
            //charge += (wfp[l] - wfn[l])*(time[l] - time[l-1]);
        }
        chrg.push_back(charge);

    }
    f->Close();
}