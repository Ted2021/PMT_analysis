#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"

//0.3p.e.イベントの波高値を求める関数
void CalcPEPulseHeight(TString filesrc, TString treename, int int_start, int int_end, std::vector<float> &av_wf, std::vector<float> &par)
{
    //ファイルを開く
    TFile* f  = TFile::Open(filesrc);
    //開いたファイルからTree名を取得
    TTree* tr = (TTree*)f->Get(treename);
    //配列を用意
    float time_e[1024] = {0};
    float wfp[1024] = {0};//Positiveの波形のデータ格納場所
    float wfn[1024] = {0};//Negativeの波形のデータ格納場所
    int sc = 0;
    //用意した配列とtree内イベントの対応付け
    tr->SetBranchAddress("time",time_e);
    tr->SetBranchAddress("wform1",wfp);
    tr->SetBranchAddress("wform0",wfn);
    tr->SetBranchAddress("stopcell",&sc);
    
    int int_wnd[2] = {int_start,int_end};//64-74 ns
    int nEve = tr->GetEntries(); //tr内の全イベント数を格納
    float wform;
    float peak_wf;

    for(int i=0;i<nEve;i++)//イベント回数で回す
    {
        tr->GetEntry(i);
        peak_wf = 0;

        for(int l=int_wnd[0];l<int_wnd[1];l++)
        {
            if((sc+l)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
            }
            wform = wfp[l]-wfn[l] - av_wf[l];
            if (wform > peak_wf)
            {
                peak_wf = wform;
            }
        }
        par.push_back(peak_wf);
    }
    f->Close();

}

//とりあえず２番目を使用する
//生波形とSouceの平均波形の差分を求める関数(ROOTファイルとして書き出す)
void CalcWform(TString filesrc, TString treename, TString New_ROOT_file, TString treename_new, std::vector<float> &av_wf, int length = 1024)
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f = new TFile(New_ROOT_file, "recreate");
    TTree* tree = new TTree(treename_new, treename_new);

    //配列を格納するためのバッファ変数
    float time[length];
    float wform[length];

    //配列をbranchとしてTTreeに追加
    tree->Branch("time", time, "time[1024]/F");
    tree->Branch("wform", wform, "wform[1024]/F");

    //読み込むTFileとTTree
    TFile* f_s = TFile::Open(filesrc);
    TTree* tr_s = (TTree*)f_s->Get(treename);

    //配列を格納するためのバッファ変数(Source)

    //####ここを一般化したい####
    float time_s[length];
    float wfp_s[length];
    float wfn_s[length];
    int sc_s = 0;
    tr_s->SetBranchAddress("time", time_s);
    tr_s->SetBranchAddress("wform1",wfp_s);
    tr_s->SetBranchAddress("wform0",wfn_s);
    tr_s->SetBranchAddress("stopcell", &sc_s);
    //####ここまで####

    //総イベント数を取得
    int nEve = tr_s->GetEntries();


    //イベントの数だけ繰り返す  
    for(int i=0;i<nEve;i++)
    {
        //i番目のイベントを読み込む
        tr_s->GetEntry(i);

        //セグメントの数-1だけ繰り返す 差分を取るため、segment1から始める (DRS4では1024固定)
        for(int l=0;l<length;l++)
        {   
            //###ここを一般化したらいらない####
            if((l+sc_s)%1024==392)
            {
                wfp_s[l] = (wfp_s[l-1] + wfp_s[l+1])*0.5; 
                wfn_s[l] = (wfn_s[l-1] + wfn_s[l+1])*0.5;
            }
            //###ここまで###

            //取得時間を新しいROOTファイルのBranchにinput
            time[l] = time_s[l];
            //1イベント前との差動電圧の差(微分電圧)を新しいROOTファイルのBrachにinput
            wform[l] = (wfp_s[l] - wfn_s[l] - av_wf[l]);
        }
        tree->Fill();
    }
    //ROOTファイルの書き込み
    f->Write();

    f_s->Close();

}

//生波形とSouceの平均波形の差分を求める関数(ROOTファイルとして書き出す)
void CalcWform2(TString filesrc, TString treename, TString New_ROOT_file, TString treename_new, std::vector<float> &av_wf, int length = 1024)
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f = new TFile(New_ROOT_file, "recreate");
    TTree* tree = new TTree(treename_new, treename_new);

    //配列を格納するためのバッファ変数
    float time[length];
    float wform[length];

    //配列をbranchとしてTTreeに追加
    tree->Branch("time", time, "time[1024]/F");
    tree->Branch("wform", wform, "wform[1024]/F");

    //読み込むTFileとTTree
    TFile* f_s = TFile::Open(filesrc);
    TTree* tr_s = (TTree*)f_s->Get(treename);

    //配列を格納するためのバッファ変数(Source)

    //####ここを一般化したい####
    float time_s[length];
    float wform_s[length];
    tr_s->SetBranchAddress("time", time_s);
    tr_s->SetBranchAddress("wform",wform_s);
    //####ここまで####

    //総イベント数を取得
    int nEve = tr_s->GetEntries();


    //イベントの数だけ繰り返す  
    for(int i=0;i<nEve;i++)
    {
        //i番目のイベントを読み込む
        tr_s->GetEntry(i);

        //セグメントの数-1だけ繰り返す 差分を取るため、segment1から始める (DRS4では1024固定)
        for(int l=0;l<length;l++)
        {   

            //取得時間を新しいROOTファイルのBranchにinput
            time[l] = time_s[l];
            //1イベント前との差動電圧の差(微分電圧)を新しいROOTファイルのBrachにinput
            wform[l] = (wform_s[l] - av_wf[l]);
        }
        tree->Fill();
    }
    //ROOTファイルの書き込み
    f->Write();

    f_s->Close();

}

//とりあえず4番目を使用する
//生波形の波高値からAP_eventをカウントする関数
void CountAPeventFromPH(TString filesrc, TString treename, int seg, float PH_THRES, int int_start, int int_end, std::vector<int> &par, std::vector<int> &par2, std::vector<float> &par3, std::vector<int> & par4)
{
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[seg];
    float wfp[seg];
    float wfn[seg];
    int sc = 0;
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress("wform1", wfp);
    tr->SetBranchAddress("wform0", wfn);
    tr->SetBranchAddress("stopcell", &sc);

    int nEve = tr->GetEntries();

    for(int i=0;i<nEve;i++)
    {
        //各変数の初期化
        int AP_event = i;   //APイベントのイベント番号
        int AP_segment = int_start; //APイベントのスタート位置(セグメント番号)
        float AP_PH_max = 0;    //APイベントスタート位置における微分波形の波高値
        float wform;
        int Counts = 0;

        tr->GetEntry(i);
        for(int l=int_start;l<int_end;l++)
        {
            if((l+sc)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
            }

            wform = wfp[l] - wfn[l];
            if(wform > PH_THRES)
            {
                if(Counts == 0)
                {
                    AP_segment = l;
                    AP_PH_max = wform;
                }
                else
                {
                    if(wform > AP_PH_max)
                    {
                        AP_PH_max = wform;
                    }
                }
                Counts += 1;
            }
            else
            {
                if(Counts !=0)
                {
                    par.push_back(AP_event);
                    par2.push_back(AP_segment);
                    par3.push_back(AP_PH_max);
                    par4.push_back(Counts);

                    Counts = 0;
                }
            }
        }    
    }
    f->Close();
}

//生波形とSourceの平均波の差分からAP_eventをカウントする関数
void CountAPeventFromPH2(TString filesrc, TString treename, int seg, float PH_THRES, int int_start, int int_end, std::vector<float> &av_wf, std::vector<int> &par, std::vector<int> &par2, std::vector<float> &par3, std::vector<int> & par4)
{
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[seg];
    float wfp[seg];
    float wfn[seg];
    int sc = 0;
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress("wform1", wfp);
    tr->SetBranchAddress("wform0", wfn);
    tr->SetBranchAddress("stopcell", &sc);

    int nEve = tr->GetEntries();

    for(int i=0;i<nEve;i++)
    {
        //各変数の初期化
        int AP_event = i;   //APイベントのイベント番号
        int AP_segment = int_start; //APイベントのピーク位置(セグメント番号)
        float AP_PH_max = 0;    //APイベントスタート位置における微分波形の波高値
        float wform;
        int Counts = 0;

        tr->GetEntry(i);
        for(int l=int_start;l<int_end;l++)
        {
            if((l+sc)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
            }

            wform = wfp[l] - wfn[l] - av_wf[l];
            if(wform > PH_THRES)
            {
                if(Counts == 0)
                {
                    AP_segment = l;
                    AP_PH_max = wform;
                }
                else
                {
                    if(wform > AP_PH_max)
                    {
                        AP_segment = l;
                        AP_PH_max = wform;
                    }
                }
                Counts += 1;
            }
            else
            {
                if(Counts !=0)
                {
                    par.push_back(AP_event);
                    par2.push_back(AP_segment);
                    par3.push_back(AP_PH_max);
                    par4.push_back(Counts);

                    Counts = 0;
                }
            }
        }    
    }
    f->Close();
}

//生波形とSourceの平均波の差分からAP_eventをカウントする関数(連続してn点を超えた場合)
void CountAPeventFromPH3(TString filesrc, TString treename, int seg, float PH_THRES, int CONTI_THES, int int_start, int int_end, std::vector<float> &av_wf, std::vector<int> &par, std::vector<int> &par2, std::vector<float> &par3, std::vector<int> & par4)
{
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[seg];
    float wfp[seg];
    float wfn[seg];
    int sc = 0;
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress("wform1", wfp);
    tr->SetBranchAddress("wform0", wfn);
    tr->SetBranchAddress("stopcell", &sc);

    int nEve = tr->GetEntries();

    for(int i=0;i<nEve;i++)
    {
        //各変数の初期化
        int AP_event = i;   //APイベントのイベント番号
        int AP_segment = int_start; //APイベントのピーク位置(セグメント番号)
        float AP_PH_max = 0;    //APイベントスタート位置における微分波形の波高値
        float wform;
        int Counts = 0;

        tr->GetEntry(i);
        for(int l=int_start;l<int_end;l++)
        {
            if((l+sc)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
            }

            wform = wfp[l] - wfn[l];
            if(wform - av_wf[l] > PH_THRES)
            {
                if(Counts == 0)
                {
                    AP_segment = l;
                    AP_PH_max = wform;
                }
                else
                {
                    if(wform > AP_PH_max)
                    {
                        AP_segment = l;
                        AP_PH_max = wform;
                    }
                }
                Counts += 1;
            }
            else
            {
                if(Counts !=0)
                {
                    if (Counts >= CONTI_THES)
                    {
                        par.push_back(AP_event);
                        par2.push_back(AP_segment);
                        par3.push_back(AP_PH_max);
                        par4.push_back(Counts);
                        Counts = 0;
                    }
                    else
                    {
                        Counts = 0;
                    }
                }
            }
        }    
    }
    f->Close();
}

//CalcWformで得たROOTファイルを用いてAP_eventをカウントする関数(連続してn点を超えた場合)
void CountAPeventFromPH4(TString filesrc, TString treename, int seg, float PH_THRES, int CONTI_THES, int int_start, int int_end, std::vector<float> &av_wf, std::vector<int> &par, std::vector<int> &par2, std::vector<float> &par3, std::vector<int> & par4)
{
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time[seg];
    float wform[seg];
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress("wform", wform);

    int nEve = tr->GetEntries();

    for(int i=0;i<nEve;i++)
    {
        //各変数の初期化
        int AP_event = i;   //APイベントのイベント番号
        int AP_segment = int_start; //APイベントのピーク位置(セグメント番号)
        float AP_PH_max = 0;    //APイベントスタート位置における微分波形の波高値
        //float wform;
        int Counts = 0;

        tr->GetEntry(i);
        for(int l=int_start;l<int_end;l++)
        {

            if(wform[l]  > PH_THRES)
            {
                if(Counts == 0)
                {
                    AP_segment = l;
                    AP_PH_max = wform[l];
                }
                else
                {
                    if(wform[l] > AP_PH_max)
                    {
                        AP_segment = l;
                        AP_PH_max = wform[l];
                    }
                }
                Counts += 1;
            }
            else
            {
                if(Counts !=0)
                {
                    if (Counts >= CONTI_THES)
                    {
                        par.push_back(AP_event);
                        par2.push_back(AP_segment);
                        par3.push_back(AP_PH_max);
                        par4.push_back(Counts);
                        Counts = 0;
                    }
                    else
                    {
                        Counts = 0;
                    }
                }
            }
        }    
    }
    f->Close();
}