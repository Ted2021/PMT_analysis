#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"


//波形の微分を行う関数
void Average_Make(TString filesrc, TString treename, std::vector<std::vector<float> > &par, std::vector<float> &par2)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[4504] = {0};
    float wave[4504] = {0};
    tr->SetBranchAddress("time", time);
    tr->SetBranchAddress("wave",wave);
    int nEve = tr->GetEntries();
    
    //int wnd[2] = {250, 4504};
    
    //vector<vector<float>> dif_wave(50001, vector<float>(4504));
    float dtime = 3000/4504;
    float dif_vol[4503] = {0};
    
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        //一つ前の差分をとるので、segmentは、1番目からスタート (配列の長さは、4503になる)
        for(int l=1;l<4504;l++)
        {
            if(i==0)
            {
                par2.push_back(time[l]*1000000000);
            }
            //一つ前のsegmentとの差分をとる(mvで返す)
            dif_vol[l] = (wave[l] - wave[l-1])*1000;
            par.at(i).push_back(dif_vol[l]);
        }
        
    }
    
}


void Dif_vol_Count(TString filesrc, TString treename, float thr, std::vector<int> &par, std::vector<int> &par2, std::vector<float> &par3, std::vector<float> &par4, int int_start=394)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[4504] = {0};
    float wave[4504] = {0};
    tr->SetBranchAddress("time", time);
    tr->SetBranchAddress("wave",wave);
    int nEve = tr->GetEntries();
    
    float dif_vol = 0;
    
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int l=int_start;l<4504;l++)
        {
            //一つ前のsegmentとの差分をとる(mvで返す)
            dif_vol = (wave[l] - wave[l-1])*1000;
            if(dif_vol > thr)
            {
                par.push_back(i);
                par2.push_back(l);
                par3.push_back(wave[l]);
                par4.push_back(dif_vol);
            }
        }
        
    }
}

