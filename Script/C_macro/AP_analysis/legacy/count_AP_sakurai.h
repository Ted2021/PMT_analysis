#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "cmath"

//櫻井法でのAfterPulseのカウンティング
/*
###引数###
filesrc:sourceファイル
treenmae:sourceのTree名
threshold:APとノイズを区別する閾値(Pluse Height)
par:dark平均波形
par2:APのイベント
par3:APのセグメント
par4:ピーク波高値
par5:電荷量
par6:計算されたp.e.
start:スタートセル
*/
void count_sakurai(TString filesrc, TString treename, float thershold, std::vector<float> &par, std::vector<int> &par2, std::vector<int> &par3, std::vector<float> &par4, std::vector<float> &par5, std::vector<float> &par6, int start = 400)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[4504] = {0};
    float wave[4504] = {0};
    tr->SetBranchAddress("time", time);
    tr->SetBranchAddress("wave",wave);
    int nEve = tr->GetEntries();
    //double eve  = (double) nEve;
    //float thr = thershold*0.4;
    float thr = 0.74;

    double timerange = 3000;
    //static const double dtime = timerange/eve;
    double dtime = 0.04999378688808065;
    float dif_vol[4504] = {0};
    float chrg;
    
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        int event_b = 0;
        int event_a = 0;
        int cell_b = 0; 
        int cell_a = 0;
        //APのイベント開始segを250に設定
        for(int l=start;l<4505;l++)
        {
            dif_vol[l] = (wave[l]*1000 - par[l]);
            if(dif_vol[l] > thr)
            {
                event_a = i;
                cell_a = l;
                if((event_a == event_b) && (cell_a == cell_b +1))
                {
                    event_b = event_a;
                    cell_b = cell_a;
                }else
                {
                    par2.push_back(i);
                    par3.push_back(l);
                    //par4.push_back(dif_vol[l]);
                    par4.push_back(wave[l]*1000);
                    event_b = event_a;
                    cell_b = cell_a;
                }
            }
        }
        
    }
    //電荷量の計算
    int size = par2.size();
    float voltage = 0;
    int target_cell = 0;
    float ped = 0;
    float delt = 0;

    for(int h=0;h<size;h++)
    {
        tr->GetEntry(par2[h]);
        chrg = 0;
        target_cell = par3[h];
        for(int k=0;k<30;k++)
        {
            //ped = tako*std::pow(target_cell, 2.0) + boke*target_cell + nasu;
            ped = par[target_cell+k-15];
            voltage = wave[target_cell+k-15]*1000 - ped;
            delt = wave[target_cell+k-15] - wave[target_cell+k-15-1];
            chrg += (voltage)*(delt)*1000000000;
        }
        par5.push_back(chrg);
    } 
    //電荷量をp.e.で評価
    int size_c = par5.size();
    float ch_pe = 0;

    for(int ho=0;ho<size_c;ho++)
    {
        ch_pe = par5[ho] / thershold;
        par6.push_back(ch_pe);
    }

    f->Close();
}

void count_sakurai(TString filesrc, TString treename, float thershold, float thr, std::vector<float> &ave, std::vector<int> &par, std::vector<int> &par2, std::vector<float> &par3, std::vector<float> &par4, int start = 394)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[4504] = {0};
    float wave[4504] = {0};
    tr->SetBranchAddress("time", time);
    tr->SetBranchAddress("wave",wave);
    int nEve = tr->GetEntries();
    float sabun = 0;
    
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        //APのイベント開始segを250に設定
        for(int l=start;l<4504;l++)
        {
            sabun = wave[l]*1000 - ave[l];
            if(sabun > thr)
            {
                par.push_back(i);
                par2.push_back(l);
                par3.push_back(wave[l]);
                par4.push_back(sabun);
            }
        }
        
    }
    f->Close();
}