#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "TRandom.h"
#include "TCanvas.h"

void ConvertFile_2385(TString filesrc, TString treename, TString new_ROOT_file, TString treename_new,
                TString branch_p = "wform1", TString branch_n = "wform0")
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f_new = new TFile(new_ROOT_file, "recreate");
    TTree* tree_new = new TTree(treename_new, treename_new);

    //配列を格納するためのバッファ変数
    float time_n[1024];
    float wf[1024];

    //配列をbranchとしてTTreeに追加
    tree_new->Branch("time", time_n, "time[1024]/F");
    tree_new->Branch("wform", wf, "wform[1024]/F");

    //ファイルを読み込む
    TFile* f  = TFile::Open(filesrc);
    //開いたファイルからTree名を取得
    TTree* tr = (TTree*)f->Get(treename);

    //配列を用意
    float time[1024] = {0};
    float wfp[1024] = {0};//Positiveの波形のデータ格納場所
    float wfn[1024] = {0};//Negativeの波形のデータ格納場所
    int sc = 0;

    //用意した配列とtree内イベントの対応付け
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress(branch_p,wfp);
    tr->SetBranchAddress(branch_n,wfn);
    tr->SetBranchAddress("stopcell",&sc);

    int nEve = tr->GetEntries(); //tr内の全イベント数を格納

    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int l=0;l<1024;l++)
        {
            if((l+sc)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
            }
            time_n[l] = time[l];
            wf[l] = wfp[l] - wfn[l];
        }
        tree_new->Fill();
    }
    f_new->Write();

    f->Close();

}

void ConvertFile_2386(TString filesrc, TString treename, TString new_ROOT_file, TString treename_new,
                TString branch_p = "wform1", TString branch_n = "wform0")
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f_new = new TFile(new_ROOT_file, "recreate");
    TTree* tree_new = new TTree(treename_new, treename_new);

    //配列を格納するためのバッファ変数
    float time_n[1024];
    float wf[1024];

    //配列をbranchとしてTTreeに追加
    tree_new->Branch("time", time_n, "time[1024]/F");
    tree_new->Branch("wform", wf, "wform[1024]/F");

    //ファイルを読み込む
    TFile* f  = TFile::Open(filesrc);
    //開いたファイルからTree名を取得
    TTree* tr = (TTree*)f->Get(treename);

    //配列を用意
    float time[1024] = {0};
    float wfp[1024] = {0};//Positiveの波形のデータ格納場所
    float wfn[1024] = {0};//Negativeの波形のデータ格納場所
    int sc = 0;

    //用意した配列とtree内イベントの対応付け
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress(branch_p,wfp);
    tr->SetBranchAddress(branch_n,wfn);
    tr->SetBranchAddress("stopcell",&sc);

    int nEve = tr->GetEntries(); //tr内の全イベント数を格納

    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int l=0;l<1024;l++)
        {
            time_n[l] = time[l];
            wf[l] = wfp[l] - wfn[l];
        }
        tree_new->Fill();
    }
    f_new->Write();

    f->Close();

}


void ConvertFile(TString filesrc, TString treename, TString new_ROOT_file, TString treename_new,
                TString branch_p = "wform1", TString branch_n = "wform0", int board_num = 2385)
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f_new = new TFile(new_ROOT_file, "recreate");
    TTree* tree_new = new TTree(treename_new, treename_new);

    //配列を格納するためのバッファ変数
    float time_n[1024];
    float wf[1024];

    //配列をbranchとしてTTreeに追加
    tree_new->Branch("time", time_n, "time[1024]/F");
    tree_new->Branch("wform", wf, "wform[1024]/F");

    //ファイルを読み込む
    TFile* f  = TFile::Open(filesrc);
    //開いたファイルからTree名を取得
    TTree* tr = (TTree*)f->Get(treename);

    //配列を用意
    float time[1024] = {0};
    float wfp[1024] = {0};//Positiveの波形のデータ格納場所
    float wfn[1024] = {0};//Negativeの波形のデータ格納場所
    int sc = 0;

    //用意した配列とtree内イベントの対応付け
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress(branch_p,wfp);
    tr->SetBranchAddress(branch_n,wfn);
    tr->SetBranchAddress("stopcell",&sc);

    int nEve = tr->GetEntries(); //tr内の全イベント数を格納

    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        for(int l=0;l<1024;l++)
        {
            switch(board_num)
            {
                case 2385:
                    if((l+sc)%1024==392)
                    {
                        wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                        wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
                    }
                    break;
                
                case 2386:
                    if((l+sc)%1024==430)
                    {
                        wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                        wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
                    }
                    break;
                
                default:
                    break;
            }
            time_n[l] = time[l];
            wf[l] = wfp[l] - wfn[l];
        }
        tree_new->Fill();
    }
    f_new->Write();

    f->Close();

}