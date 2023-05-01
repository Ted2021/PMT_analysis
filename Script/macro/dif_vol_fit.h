#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"

//微分波形の射影
TH1F* threshold3(TString filesrc, TString treename="AP", int int_start=394)
{
    TH1F* hst = new TH1F("hst","hst",500,-80,80);//Histgramの作成
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);

    float wave[4504] = {0};
    tr->SetBranchAddress("wave",wave);
    //TH1F* hst = new TH1F("hst","hst",400,0,800);//Histgramの作成


    float chrg = 0;

    //Pedestalの平均chargeの初期化
    //1 event毎に計算する
    int nEve = tr->GetEntries();

    for(int i=0;i<nEve;i++)//イベント回数で回す
    {
        tr->GetEntry(i);
        for(int j=int_start;j<4504;j++)//積分時間
        { 
            chrg = (wave[j] - wave[j-1])*1000;
            hst->Fill(chrg);
        }
    }
    //hst->Draw();
    f->Close();
    return hst;
}

//微分した波形のSingle Gaussian
void Fit_difVol(TH1F* hst,std::vector<float> &par,std::vector<float> &par_err)
{
    //初期値の設定
    //Double_t par_ped[3]={8e6,0};
    //Double_t par_single[3]={c_y,c_x,c_wid};
    //Double_t par_double[2]={14,32};
    par={};
    par_err={};
    
    //まずはSingle GausianでのPedestalのFitting
    TF1* f_ped = new TF1("gaus","gaus",-10,10);
    //Fittingパラメーターの設定
    //f_ped->SetParameters(par_ped);
    //Fitの実行
    hst->Fit(f_ped,"I","",-10, 10);   

    //パラメータのReturn
    for(int i=0;i<3;i++)
    {
        par.push_back(f_ped->GetParameter(i));
        //par_err.push_back(f_ped_double->GetParError(i));
    }
    
}