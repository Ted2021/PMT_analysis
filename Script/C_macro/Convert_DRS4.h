#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "TRandom.h"
#include "TCanvas.h"

void ConvertFile(TString filesrc, TString treename, TString new_ROOT_file, TString treename_new)
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
    tr->SetBranchAddress("wform1",wfp);
    tr->SetBranchAddress("wform0",wfn);
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

void ConvertFile2(TString filesrc, TString treename, TString treename_d, TString new_ROOT_file, TString ntr_s, TString ntr_d, Int_t cell = 1024)
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f_new = new TFile(new_ROOT_file, "recreate");
    TTree* tree_source = new TTree(ntr_s, ntr_s);

    //配列を格納するためのバッファ変数
    float time_s[cell];
    float wf_s[cell];
    Int_t length;
    

    //配列をbranchとしてTTreeに追加
    tree_source->Branch("length", &length, "length/I");
    tree_source->Branch("time", time_s, "time[length]/F");
    tree_source->Branch("wform", wf_s, "wform[length]/F");

    //ファイルを読み込む
    TFile* f  = TFile::Open(filesrc);
    //開いたファイルからTree名を取得
    TTree* tr = (TTree*)f->Get(treename);
    TTree* tr_d = (TTree*)f->Get(treename_d);

    //配列を用意
    float t_s[cell];
    float wfp_s[cell];//Positiveの波形のデータ格納場所
    float wfn_s[cell];//Negativeの波形のデータ格納場所
    int sc_s = 0;
    float t_d[cell];
    float wfp_d[cell];//Positiveの波形のデータ格納場所
    float wfn_d[cell];//Negativeの波形のデータ格納場所
    int sc_d = 0;

    //用意した配列とtree内イベントの対応付け
    tr->SetBranchAddress("time",t_s);
    tr->SetBranchAddress("wform1",wfp_s);
    tr->SetBranchAddress("wform0",wfn_s);
    tr->SetBranchAddress("stopcell",&sc_s);
    tr_d->SetBranchAddress("time",t_d);
    tr_d->SetBranchAddress("wform1",wfp_d);
    tr_d->SetBranchAddress("wform0",wfn_d);
    tr_d->SetBranchAddress("stopcell",&sc_d);


    int nEve = tr->GetEntries(); //tr内の全イベント数を格納
    int nEve_d = tr_d->GetEntries();

    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);
        length = cell;
        for(Int_t l=0;l<cell;l++)
        {
            if((l+sc_s)%1024==392)
            {
                wfp_s[l] = (wfp_s[l-1] + wfp_s[l+1])*0.5; 
                wfn_s[l] = (wfn_s[l-1] + wfn_s[l+1])*0.5;
            }
            time_s[l] = t_s[l];
            wf_s[l] = wfp_s[l] - wfn_s[l];
        }
        tree_source->Fill();
    }
    f_new->Write();


    TTree* tree_dark = new TTree(ntr_d, ntr_d);

    float time_d[cell];
    float wf_d[cell];
    Int_t length2;

    tree_dark->Branch("length", &length2, "length/I");
    tree_dark->Branch("time", time_d, "time[length]/F");
    tree_dark->Branch("wform", wf_s, "wform[length]/F");

    for(int i=0;i<nEve_d;i++)
    {
        tr_d->GetEntry(i);
        length2 = cell;
        for(Int_t l=0;l<cell;l++)
        {
            if((l+sc_d)%1024==392)
            {
                wfp_d[l] = (wfp_d[l-1] + wfp_d[l+1])*0.5; 
                wfn_d[l] = (wfn_d[l-1] + wfn_d[l+1])*0.5;
            }
            time_d[l] = t_d[l];
            wf_d[l] = wfp_d[l] - wfn_d[l];
        }
        tree_dark->Fill();
    }
    f_new->Write();

    f->Close();

}

void tree3w() {
 
   const Int_t kMaxTrack = 500;
   Int_t ntrack;
   Int_t stat[kMaxTrack];
   Int_t sign[kMaxTrack];
   Float_t px[kMaxTrack];
   Float_t py[kMaxTrack];
   Float_t pz[kMaxTrack];
   Float_t pt[kMaxTrack];
   Float_t zv[kMaxTrack];
   Float_t chi2[kMaxTrack];
   Double_t sumstat;
 
   TFile f("tree3.root","recreate");
   TTree *t3 = new TTree("t3","Reconst ntuple");
   t3->Branch("ntrack",&ntrack,"ntrack/I");
   t3->Branch("stat",stat,"stat[ntrack]/I");
   t3->Branch("sign",sign,"sign[ntrack]/I");
   t3->Branch("px",px,"px[ntrack]/F");
   t3->Branch("py",py,"py[ntrack]/F");
   t3->Branch("pz",pz,"pz[ntrack]/F");
   t3->Branch("zv",zv,"zv[ntrack]/F");
   t3->Branch("chi2",chi2,"chi2[ntrack]/F");
 
   TFile fr("tree3f.root","recreate");
   TTree *t3f = new TTree("t3f","a friend Tree");
   t3f->Branch("ntrack",&ntrack,"ntrack/I");
   t3f->Branch("sumstat",&sumstat,"sumstat/D");
   t3f->Branch("pt",pt,"pt[ntrack]/F");
 
   for (Int_t i=0;i<1000;i++) {
      Int_t nt = gRandom->Rndm()*(kMaxTrack-1);
      ntrack = nt;
      sumstat = 0;
      for (Int_t n=0;n<nt;n++) {
         stat[n] = n%3;
         sign[n] = i%2;
         px[n]   = gRandom->Gaus(0,1);
         py[n]   = gRandom->Gaus(0,2);
         pz[n]   = gRandom->Gaus(10,5);
         zv[n]   = gRandom->Gaus(100,2);
         chi2[n] = gRandom->Gaus(0,.01);
         sumstat += chi2[n];
         pt[n]   = TMath::Sqrt(px[n]*px[n] + py[n]*py[n]);
      }
      t3->Fill();
      t3f->Fill();
   }
   t3->Print();
   f.cd();
   t3->Write();
   fr.cd();
   t3f->Write();
}

void open(TString filesrc, TString treename)
{
    //ファイルを読み込む
    TFile* f  = TFile::Open(filesrc);
    //開いたファイルからTree名を取得
    TTree* tr = (TTree*)f->Get(treename);

    //配列を用意
    float time[1024] = {0};
    float wfp[1024] = {0};//Positiveの波形のデータ格納場所
    //float wfn[1024] = {0};//Negativeの波形のデータ格納場所
    //int sc = 0;

    //用意した配列とtree内イベントの対応付け
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress("wform",wfp);
    //tr->SetBranchAddress("wform0",wfn);
    //tr->SetBranchAddress("stopcell",&sc);
}