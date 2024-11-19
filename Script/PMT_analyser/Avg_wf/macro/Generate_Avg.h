#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include<math.h>
#include<stdio.h>
#include<string>

void Average_Make(TString filesrc, TString treename, TString branch_t, TString branch_wf, std::vector<float> &par, std::vector<float> &par2, int seg = 1024)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    //float time_e[1024] = {0};
    float data_time[seg];
    float wf[seg];
    tr->SetBranchAddress(branch_t, data_time);
    tr->SetBranchAddress(branch_wf,wf);
    int nEve = tr->GetEntries();

    float ave_wave[seg];
    
    for(int i=0;i<nEve;i++)
    {
        /*
        if(i==0)
        {
            for(int l=0;l<seg;l++)
            {

                ave_wave[l] = 0;
            }
        }*/
        tr->GetEntry(i);

        for(int l=0;l<seg;l++)
        {

            ave_wave[l] += (wf[l])/nEve;
        }
    }
    f->Close();
    //パラメータのReturn
    for(int i=0;i<seg;i++)
    {
        par2.push_back(ave_wave[i]);
        par.push_back(data_time[i]);
    }
}

void Average_Make2(TString filesrc, TString treename, TString branch_t, TString branch_wf, std::vector<float> &par, std::vector<float> &par2, int seg = 1024)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    /*
    float time_e[1024] = {0};
    float data_time[seg];
    float wf[seg];
    float ave_wave[seg];
    tr->SetBranchAddress(branch_t, data_time);
    tr->SetBranchAddress(branch_wf,wf);
    int nEve = tr->GetEntries();

    
    //string S = to_string(nEve);
    //std::cout << S << std::endl;

    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int l=0;l<1024;l++)
        {
            ave_wave[l] += (wf[l])/nEve;
        }
    }
    f->Close();
    for(int i=0;i<seg;i++)
    {
        par2.push_back(ave_wave[i]);
    }
    */
    /*
    
    
    for(int i=0;i<nEve;i++)
    {
        if(i==0)
        {
            for(int l=0;l<seg;l++)
            {

                ave_wave[l] = 0;
            }
        }
        tr->GetEntry(i);

        for(int l=0;l<seg;l++)
        {

            ave_wave[l] += (wf[l])/nEve;
        }
    }
    f->Close();
    //パラメータのReturn
    for(int i=0;i<seg;i++)
    {
        par2.push_back(ave_wave[i]);
        par.push_back(data_time[i]);
    }
    */
}

void Average_Make3(TString filesrc, TString treename, TString branch_t, TString branch_wf, std::vector<float> &par, std::vector<float> &par2, int seg = 1024)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    //float time_e[1024] = {0};
    float data_time[seg];
    float wf[seg];
    tr->SetBranchAddress(branch_t, data_time);
    tr->SetBranchAddress(branch_wf,wf);
    int nEve = tr->GetEntries();

    float ave_wave[seg];
    
    for(int i=0;i<nEve;i++)
    {
        /*
        if(i==0)
        {
            for(int l=0;l<seg;l++)
            {

                ave_wave[l] = 0;
            }
        }*/
        tr->GetEntry(i);

        for(int l=0;l<seg;l++)
        {
            if(wf[l]>3000 || wf[l] < -500)
            {
                ave_wave[l] += 0;
            }else
            {
                ave_wave[l] += (wf[l])/nEve;
            }
            
        }
    }
    f->Close();
    //パラメータのReturn
    for(int i=0;i<seg;i++)
    {
        par2.push_back(ave_wave[i]);
        par.push_back(data_time[i]);
    }
}