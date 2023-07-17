#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include<math.h>

void Average_Make_AP(TString filesrc, TString treename, int seg, std::vector<float> &par, std::vector<float> &par2)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time_e[4504] = {};
    float data_time[4504] = {};
    float wf[4504] = {};
    //float wfn[1024] = {0};
    tr->SetBranchAddress("time", data_time);
    tr->SetBranchAddress("wave",wf);
    //tr->SetBranchAddress("wform0",wfn);
    int nEve = tr->GetEntries();

    float ave_wave[4504] = {};
    
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int l=0;l<seg;l++)
        {
            ave_wave[l] += (wf[l]*1000)/nEve;
        }
    }
    f->Close();
    //パラメータのReturn
    
    for(int i=0;i<seg;i++)
    {
        par.push_back(ave_wave[i]);
        par2.push_back(data_time[i]);
    }
}

void Average_Make_single(TString filesrc, TString treename, int seg, std::vector<float> &par, std::vector<float> &par2)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time_e[1200] = {};
    float data_time[1200] = {};
    float wf[1200] = {};
    //float wfn[1024] = {0};
    tr->SetBranchAddress("time", data_time);
    tr->SetBranchAddress("wave",wf);
    //tr->SetBranchAddress("wform0",wfn);
    int nEve = tr->GetEntries();

    float ave_wave[1200] = {};
    
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int l=0;l<seg;l++)
        {
            ave_wave[l] += (wf[l]*1000)/nEve;
        }
    }
    f->Close();
    //パラメータのReturn
    
    for(int i=0;i<seg;i++)
    {
        par.push_back(ave_wave[i]);
        par2.push_back(data_time[i]);
    }
}

void return_sigma(TString filesrc, TString treename, int seg, std::vector<float> &par, std::vector<float> &par2)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time_e[4504] = {};
    float data_time[4504] = {};
    float wf[4504] = {};
    //float wfn[1024] = {0};
    tr->SetBranchAddress("time", data_time);
    tr->SetBranchAddress("wave",wf);
    //tr->SetBranchAddress("wform0",wfn);
    int nEve = tr->GetEntries();

    float ave_wave[4504] = {};
    float ave_sigma[4504] = {};
    
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int l=0;l<seg;l++)
        {
            ave_wave[l] += (wf[l]*1000)/nEve;
        }
    }
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int k=0;k<seg;k++)
        {
            ave_sigma[k] += (pow((wf[k]*1000 - ave_wave[k]),2))/nEve;
        }
    }
    f->Close();
    //パラメータのReturn
    for(int i=0;i<seg;i++)
    {
        par.push_back(ave_wave[i]);
        par2.push_back(sqrt(ave_sigma[i]));
    }
}

void Average_Make_dark(TString filesrc, TString treename, int seg, std::vector<float> &par, std::vector<float> &par2)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    
    float data_time[1200] = {};
    float wf[1200] = {};
    float ave_wave[1200] = {};
    
    tr->SetBranchAddress("time", data_time);
    tr->SetBranchAddress("wave",wf);
    int nEve = tr->GetEntries();
    
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int l=0;l<seg;l++)
        {
            ave_wave[l] += (wf[l]*1000)/nEve;
        }
    }
    f->Close();
    //パラメータのReturn
    
    for(int i=0;i<seg;i++)
    {
        par.push_back(ave_wave[i]);
        par2.push_back(data_time[i]);
    }
}