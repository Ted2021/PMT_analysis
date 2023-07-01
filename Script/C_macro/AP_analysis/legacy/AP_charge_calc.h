#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "cmath"

//APの電荷量を計算する関数
/*
###引数###
filesrc:Sourceファイル
treename:SourceのTree名
filesrc_d:Darkファイル
treename_d:DarkのTree名
event:AfterPulseのイベント
segment:AfterPulseのスライス
count:AfterPulseEvnetの長さ
charge:計算したAfterPulseの電荷量
*/
void clac_chrg(TString filesrc, TString treename, TString filesrc_d, TString treename_d, std::vector<int> &event, std::vector<int> &segment, std::vector<int> &count, std::vector<float> &charge)
{
    TFile* f  = TFile::Open(filesrc);
    TFile* f_d = TFile::Open(filesrc_d);
    TTree* tr = (TTree*)f->Get(treename);
    TTree* tr_d = (TTree*)f_d->Get(treename_d);
    float time[4504] = {0};
    float time_d[4504] = {0};
    float wave[4504] = {0};
    float wave_d[4504] = {0};
    tr->SetBranchAddress("time", time);
    tr->SetBranchAddress("wave",wave);
    tr_d->SetBranchAddress("time", time_d);
    tr_d->SetBranchAddress("wave", wave_d);

    int nEve = tr->GetEntries();
    int nEve_d = tr_d->GetEntries();

    //double eve  = (double) nEve;

    //double timerange = 3000;
    //static const double dtime = timerange/eve;
    float dtime;
    float chrg;
    float voltage;
    int target_cell;
    int range_cell;
    //float ped = 0;

    std::size_t list_size = event.size();

    for(int i=0;i<list_size;i++)
    {
        tr->GetEntry(event[i]);
        chrg = 0;
        target_cell = segment[i];
        range_cell = count[i] + 5;
        dtime = time[target_cell] - time[target_cell - 1];
        for(int k=-range_cell; k<range_cell; k++)
        {
            //ped = tako*std::pow(target_cell, 2.0) + boke*target_cell + nasu;
            //voltage = wave[target_cell+k-15] - ped;
            voltage = wave[target_cell + k] - wave_d[target_cell + k];
            chrg += (voltage)*1e3*(dtime)*1e9;
        }
        charge.push_back(chrg);
    }
    
    f->Close();
    f_d->Close();
}