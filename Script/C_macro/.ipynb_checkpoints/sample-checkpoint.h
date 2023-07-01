#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "cmath"
#include "string"

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

void CreateROOTFile(TString filesrc, TString treename, TString New_ROOT_file, TString treename_new, int length)
{
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f_n = new TFile(New_ROOT_file, "recreate");
    TTree* tree_n = new TTree(treename_new, treename_new);

    //配列を格納するためのバッファ変数
    float time[length];
    float wform[length];

    //配列をbranchとしてTTreeに追加
    tree_n->Branch("time", time, "time[1024]/F");
    tree_n->Branch("wform", wform, "wform[1024]/F");


    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);

    //####ここを一般化したい####
    float time_s[length];
    float wfp[length];
    float wfn[length];
    int sc = 0;
    tr->SetBranchAddress("time", time_s);
    tr->SetBranchAddress("wform1",wfp);
    tr->SetBranchAddress("wform0",wfn);
    tr->SetBranchAddress("stopcell", &sc);

    //総イベント数を取得
    int nEve = tr->GetEntries();

    //イベントの数だけ繰り返す  
    for(int i=0;i<nEve;i++)
    {
        //i番目のイベントを読み込む
        tr->GetEntry(i);

        //セグメントの数-1だけ繰り返す 差分を取るため、segment1から始める (DRS4では1024固定)
        for(int l=0;l<length;l++)
        {   
            //###ここを一般化したらいらない####
            if((l+sc)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
            }
            //###ここまで###

            //取得時間を新しいROOTファイルのBranchにinput
            time[l] = time_s[l];
            //1イベント前との差動電圧の差(微分電圧)を新しいROOTファイルのBrachにinput
            wform[l] = wfp[l] - wfn[l];
        }
        tree_n->Fill();
    }
    //ROOTファイルの書き込み
    f_n->Write();

    f->Close();
    


}

void CreateROOTFile2(std::vector<std::string>& RT_files, TString treename, TString New_ROOT_file, TString treename_new, int length, int num)
{
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f_n = new TFile(New_ROOT_file, "recreate");
    TTree* tree_n = new TTree(treename_new, treename_new);

    //配列を格納するためのバッファ変数
    float time[2048];
    float wform[2048];

    //配列をbranchとしてTTreeに追加
    tree_n->Branch("time", time, "time[2048]/F");
    tree_n->Branch("wform", wform, "wform[2048]/F");

    int cell = 0;

    for(int i=0;i<num;i++)
    {
        TFile* f = TFile::Open(RT_files[i]);
        TTree* tr = (TTree*)f->Get(treename);

        //####ここを一般化したい####
        float time_s[length];
        float wfp[length];
        float wfn[length];
        int sc = 0;
        tr->SetBranchAddress("time", time_s);
        tr->SetBranchAddress("wform1",wfp);
        tr->SetBranchAddress("wform0",wfn);
        tr->SetBranchAddress("stopcell", &sc);
        //総イベント数
        int nEve = tr->GetEntries();
        for(int j=0;j<nEve;j++)
        {
            //i番目のイベントを読み込む
            tr->GetEntry(j);

            //セグメントの数-1だけ繰り返す 差分を取るため、segment1から始める (DRS4では1024固定)
            for(int l=0;l<length;l++)
            {   
                //###ここを一般化したらいらない####
                if((l+sc)%1024==392)
                {
                    wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                    wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
                }
                //###ここまで###

                //取得時間を新しいROOTファイルのBranchにinput
                time[cell+l] = time_s[l];
                //1イベント前との差動電圧の差(微分電圧)を新しいROOTファイルのBrachにinput
                wform[cell+l] = wfp[l] - wfn[l];
            }
            tree_n->Fill();
        }
        f->Close();
        cell += 1024
    }
    //ROOTファイルの書き込み
    f_n->Write();

}


void show_arr(std::vector<std::string>& arr, int num)
{
    for(int i=0;i<num;i++)
    {
        cout << arr[i] << "\n";
    }
}