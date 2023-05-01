
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "cmath"

void clac_chrg_PSP_1(TString filesrc, TString treename, TString filesrc_d, TString treename_d, std::vector<int> &event, std::vector<int> &s_silce, std::vector<int> &e_slice, std::vector<float> &charge)
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
    int start_cell;
    int end_cell;
    //float ped = 0;

    std::size_t list_size = event.size();

    for(int i=0;i<list_size;i++)
    {
        tr->GetEntry(event[i]);
        chrg = 0;
        start_cell = s_silce[i];
        end_cell = e_slice[i];
        //dtime = time[target_cell] - time[target_cell - 1];
        for(int k=start_cell; k<end_cell + 1; k++)
        {
            //ped = tako*std::pow(target_cell, 2.0) + boke*target_cell + nasu;
            //voltage = wave[target_cell+k-15] - ped;
            voltage = (wave[k] - wave_d[k])*1e3;
            chrg += voltage*(time[k] - time[k-1])*1e9;
        }
        charge.push_back(chrg);
    }
    
    f->Close();
    f_d->Close();
}

