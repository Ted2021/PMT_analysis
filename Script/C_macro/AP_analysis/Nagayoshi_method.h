#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"


//微分波形を求める関数(ROOTファイルにして書き出す)

/*

###INPUT###
AP_evnetが含まれるsourceのROOTファイル
AP_eventが含まれるdarkのROOTファイル
###########

###OUTPUT###
AP_evnetの微分波形のROOTファイル
############

###引数####
filesrc: AP_eventが含まれるsourceのROOTファイル
treename: filesrcのTree名
filesrc_d: AP_eventが含まれるdarkのROOTファイル
treename_d: filesrc_dのTree名
Diffwf_ROOT_file: 出力される微分波形のROOTファイルのパス (.rootまで記述すること)
length: ROOTファイルのセグメント数(DRS4で波形を取得した場合は、1024固定なので気にしなくて良い)
name: 出力するROOTファイルのBranch(時刻)の名前(オシロ等でセグメント数を1024以外にしたい場合はここの値を変更する)
name2: 出力するROOTファイルのBranch(微分電圧)の名前(nameと同様に基本的にそのままで良い)
##########

*/

//現状としては、3番目の関数を使用する
void CalcDiffWform(TString filesrc, TString treename, TString  filesrc_d, TString treename_d, TString Diffwf_ROOT_file, std::vector<float> &av_wf, int length = 1024, TString name = "time[1023]/F",  TString name2 = "diffwf[1023]/F")
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f = new TFile(Diffwf_ROOT_file, "recreate");
    TTree* tree = new TTree("Treediff_0", "Treediff_0");

    //配列を格納するためのバッファ変数
    float time[length-1];
    float diffwf[length-1];

    //配列をbranchとしてTTreeに追加
    tree->Branch("time", time, name);
    tree->Branch("diffwf", diffwf, name2);

    //読み込むTFileとTTree
    TFile* f_s = TFile::Open(filesrc);
    TTree* tr_s = (TTree*)f_s->Get(treename);
    TFile* f_d = TFile::Open(filesrc_d);
    TTree* tr_d = (TTree*)f_d->Get(treename_d);


    //配列を格納するためのバッファ変数(Source)
    float time_s[length];
    float wfp_s[length];
    float wfn_s[length];
    int sc_s = 0;
    tr_s->SetBranchAddress("time", time_s);
    tr_s->SetBranchAddress("wform1",wfp_s);
    tr_s->SetBranchAddress("wform0",wfn_s);
    tr_s->SetBranchAddress("stopcell", &sc_s);

    //配列を格納するためのバッファ変数(Dark)
    float time_d[length];
    float wfp_d[length];
    float wfn_d[length];
    int sc_d = 0;
    tr_d->SetBranchAddress("time", time_d);
    tr_d->SetBranchAddress("wform1",wfp_d);
    tr_d->SetBranchAddress("wform0",wfn_d);
    tr_d->SetBranchAddress("stopcell", &sc_d);

    //総イベント数を取得
    int nEve = tr_s->GetEntries(); 

    //イベントの数だけ繰り返す  
    for(int i=0;i<nEve;i++)
    {
        //i番目のイベントを読み込む
        tr_s->GetEntry(i);
        tr_d->GetEntry(i);

        //セグメントの数-1だけ繰り返す 差分を取るため、segment1から始める (DRS4では1024固定)
        for(int l=1;l<length;l++)
        {   
            if((l+sc_s)%1024==392)
            {
                wfp_s[l] = (wfp_s[l-1] + wfp_s[l+1])*0.5; 
                wfn_s[l] = (wfn_s[l-1] + wfn_s[l+1])*0.5;
            }
            if((l+sc_d)%1024==392)
            {
                wfp_d[l] = (wfp_d[l-1] + wfp_d[l+1])*0.5;
                wfn_d[l] = (wfn_d[l-1] + wfn_d[l+1])*0.5;
            }
            //取得時間を新しいROOTファイルのBranchにinput
            time[l] = time_s[l];
            //1イベント前との差動電圧の差(微分電圧)を新しいROOTファイルのBrachにinput
            diffwf[l] = (wfp_s[l] - wfn_s[l] - (wfp_d[l] - wfn_d[l])) - (wfp_s[l-1] - wfn_s[l-1] - (wfp_d[l-1] - wfn_d[l-1]));
        }
        tree->Fill();
    }
    //ROOTファイルの書き込み
    f->Write();

    f_s->Close();
    f_d->Close();
}

void CalcDiffWform3(TString filesrc, TString treename, TString Diffwf_ROOT_file, TString treename_dif, std::vector<float> &av_wf, int length = 1024, TString name = "time[1023]/F",  TString name2 = "diffwf[1023]/F")
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f = new TFile(Diffwf_ROOT_file, "recreate");
    TTree* tree = new TTree(treename_dif, treename_dif);

    //配列を格納するためのバッファ変数
    float time[length-1];
    float diffwf[length-1];

    //配列をbranchとしてTTreeに追加
    tree->Branch("time", time, name);
    tree->Branch("diffwf", diffwf, name2);

    //読み込むTFileとTTree
    TFile* f_s = TFile::Open(filesrc);
    TTree* tr_s = (TTree*)f_s->Get(treename);

    //配列を格納するためのバッファ変数(Source)
    //###ここを一般化しておきたい###
    float time_s[length];
    float wfp_s[length];
    float wfn_s[length];
    int sc_s = 0;
    tr_s->SetBranchAddress("time", time_s);
    tr_s->SetBranchAddress("wform1",wfp_s);
    tr_s->SetBranchAddress("wform0",wfn_s);
    tr_s->SetBranchAddress("stopcell", &sc_s);
    //###ここまで###

    //総イベント数を取得
    int nEve = tr_s->GetEntries(); 

    //イベントの数だけ繰り返す  
    for(int i=0;i<nEve;i++)
    {
        //i番目のイベントを読み込む
        tr_s->GetEntry(i);

        //セグメントの数-1だけ繰り返す 差分を取るため、segment1から始める (DRS4では1024固定)
        for(int l=1;l<length;l++)
        {   
            //###ここを一般化しておきたい###
            if((l+sc_s)%1024==392)
            {
                wfp_s[l] = (wfp_s[l-1] + wfp_s[l+1])*0.5; 
                wfn_s[l] = (wfn_s[l-1] + wfn_s[l+1])*0.5;
            }
            //###ここまで###

            //取得時間を新しいROOTファイルのBranchにinput
            time[l] = time_s[l];
            //1イベント前との差動電圧の差(微分電圧)を新しいROOTファイルのBrachにinput
            //###ここも修正必要あり
            diffwf[l] = (wfp_s[l] - wfn_s[l] - av_wf[l]) - (wfp_s[l-1] - wfn_s[l-1] - av_wf[l-1]);
        }
        tree->Fill();
    }
    //ROOTファイルの書き込み
    f->Write();

    f_s->Close();
}

void CalcDiffWform4(TString filesrc, TString treename, TString Diffwf_ROOT_file, TString treename_dif, std::vector<float> &av_wf, int length = 1024, TString name = "time[1023]/F",  TString name2 = "diffwf[1023]/F")
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f = new TFile(Diffwf_ROOT_file, "recreate");
    TTree* tree = new TTree(treename_dif, treename_dif);

    //配列を格納するためのバッファ変数
    float time[length-1];
    float diffwf[length-1];

    //配列をbranchとしてTTreeに追加
    tree->Branch("time", time, name);
    tree->Branch("diffwf", diffwf, name2);

    //読み込むTFileとTTree
    TFile* f_s = TFile::Open(filesrc);
    TTree* tr_s = (TTree*)f_s->Get(treename);

    //配列を格納するためのバッファ変数(Source)
    float time_s[length];
    float wform_s[length];
    tr_s->SetBranchAddress("time", time_s);
    //tr_s->SetBranchAddress("wform",wform_s);
    tr_s->SetBranchAddress("wave",wform_s);

    //総イベント数を取得
    int nEve = tr_s->GetEntries(); 

    //イベントの数だけ繰り返す  
    for(int i=0;i<nEve;i++)
    {
        //i番目のイベントを読み込む
        tr_s->GetEntry(i);

        //セグメントの数-1だけ繰り返す 差分を取るため、segment1から始める (DRS4では1024固定)
        for(int l=1;l<length;l++)
        {   

            //取得時間を新しいROOTファイルのBranchにinput
            time[l] = time_s[l];
            //1イベント前との差動電圧の差(微分電圧)を新しいROOTファイルのBrachにinput
            //###ここも修正必要あり
            diffwf[l] = (wform_s[l] - av_wf[l]) - (wform_s[l-1] - av_wf[l-1]);
        }
        tree->Fill();
    }
    //ROOTファイルの書き込み
    f->Write();

    f_s->Close();
}


void CalcDiffWform2(TString filesrc, TString treename, TString Diffwf_ROOT_file, std::vector<float> &av_wf, int length = 1024, TString name = "time[1023]/F",  TString name2 = "diffwf[1023]/F")
{
    //ROOTの初期設定
    gROOT -> Reset();
    gStyle -> SetOptStat(1001110);

    //新しいTFileとTTreeを定義する
    TFile* f = new TFile(Diffwf_ROOT_file, "recreate");
    TTree* tree = new TTree("Treediff_0", "Treediff_0");

    //配列を格納するためのバッファ変数
    float time[length-1];
    float diffwf[length-1];

    //配列をbranchとしてTTreeに追加
    tree->Branch("time", time, name);
    tree->Branch("diffwf", diffwf, name2);

    //読み込むTFileとTTree
    TFile* f_s = TFile::Open(filesrc);
    TTree* tr_s = (TTree*)f_s->Get(treename);

    //配列を格納するためのバッファ変数(Source)
    float time_s[length];
    float wfp_s[length];
    float wfn_s[length];
    int sc_s = 0;
    tr_s->SetBranchAddress("time", time_s);
    tr_s->SetBranchAddress("wform1",wfp_s);
    tr_s->SetBranchAddress("wform0",wfn_s);
    tr_s->SetBranchAddress("stopcell", &sc_s);

    //総イベント数を取得
    int nEve = tr_s->GetEntries(); 

    //イベントの数だけ繰り返す  
    for(int i=1;i<nEve;i++)
    {
        //i番目のイベントを読み込む
        tr_s->GetEntry(i);

        //セグメントの数-1だけ繰り返す 差分を取るため、segment1から始める (DRS4では1024固定)
        for(int l=1;l<length;l++)
        {   
            if((l+sc_s)%1024==392)
            {
                wfp_s[l] = (wfp_s[l-1] + wfp_s[l+1])*0.5; 
                wfn_s[l] = (wfn_s[l-1] + wfn_s[l+1])*0.5;
            }
            //取得時間を新しいROOTファイルのBranchにinput
            time[l] = time_s[l];
            //1イベント前との差動電圧の差(微分電圧)を新しいROOTファイルのBrachにinput
            diffwf[l] = (wfp_s[l] - wfn_s[l] - av_wf[l]) - (wfp_s[l-1] - wfn_s[l-1] - av_wf[l-1]);
        }
        tree->Fill();
    }
    //ROOTファイルの書き込み
    f->Write();

    f_s->Close();
}


//微分波形をY軸方向に射影する関数(+singleGaussianでFit)

/*

###INPUT###
AP_eventの微分波形のROOTファイル
###########

###OUTPUT###
微分波形を射影したヒストグラム
############

###引数###
filesrc:　微分波形のROOTファイル
treename:　filesrcのTree名
par: Fittingパラメータ
par_err: Fitting_errパラメータ
int_start: 計算範囲start
int_end: 計算範囲end
#########

*/
TH1F* DiffWformProjection(TString filesrc, TString treename, std::vector<float> &par, std::vector<float> &par_err, int int_start, int int_end, int seg = 1024, float h_max = 20, float fig_max = 80)
{
    //微分波形を射影するヒストグラムを作成
    TH1F* hst = new TH1F("fumufumu", "hogehoge", 200, -fig_max, fig_max);

    //微分波形が入ったROOTファイルを開く
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);

    //配列を格納するBranchのバッファ変数
    float difwf[seg];
    tr->SetBranchAddress("diffwf", difwf);
    
    //総イベント数
    int nEve = tr->GetEntries();

    //全イベントで回す
    for(int i=0;i<nEve;i++)
    {   
        //i番目のイベントを取得
        tr->GetEntry(i);
        //引数で指定したセグメントの範囲の微分波形を抜き出す
        for(int j=int_start;j<int_end;j++)
        {
            //ヒストグラムに詰める
            hst->Fill(difwf[j]);

        }
    }
    f->Close();

    //
    par={};
    par_err={};

    //SingleGaussianでPedestalのFittingを行う
    TF1* f_ped = new TF1("gaus", "gaus", -h_max, h_max);

    //Fittingを実行する
    hst->Fit(f_ped, "I", "", -h_max, h_max);

    //パラメータを返す
    for(int i=0;i<3;i++)
    {
        par.push_back(f_ped->GetParameter(i));
        par_err.push_back(f_ped->GetParError(i));
    } 


    return hst;
}

//微分波形からAPのカウンティングをする関数(微分波形を求めたROOTファイルから実行する場合)
//現状としては、1番目の関数を使用する
void CountAPeventFromDiff(TString filesrc, TString treename, int seg, float DIF_THRES, int int_start, int int_end, std::vector<int> &par, std::vector<int> &par2, std::vector<float> &par3, std::vector<int> & par4)
{
    //ROOTファイルの読み込み
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);

    //配列を格納するBranchの変数
    float time[seg];
    float diffwave[seg];
    tr->SetBranchAddress("time", time);
    tr->SetBranchAddress("diffwf", diffwave);

    //総イベント数を取得
    int nEve = tr->GetEntries();

    //全イベントで回す
    for(int i=0;i<nEve;i++)
    {
        //各変数の初期化
        int AP_event = i;   //APイベントのイベント番号
        int AP_segment = int_start; //APイベントのスタート位置(セグメント番号)
        float AP_diffwf = 0;    //APイベントスタート位置における微分波形の波高値
        int Counts = 0; //APイベントのカウンター(微分波形が連続していくつ閾値を超えたか)

        //i番目のイベントを読み込む
        tr->GetEntry(i);

        //int_startからint_endの範囲でループ
        for(int l=int_start;l<int_end;l++)
        {
            //微分波形が閾値より大きい場合
            if(diffwave[l] > DIF_THRES)
            {
                //1つ前のセグメントが閾値を超えていなかった場合
                if(Counts == 0)
                {
                    //APイベントの開始地点
                    AP_segment = l;
                    AP_diffwf = diffwave[l];
                }
                Counts += 1;
            }
            //微分波形が閾値を超えなかった場合
            else
            {
                //1つ前のセグメントが閾値を超えていた場合
                if(Counts != 0)
                {
                    //Vectorに各値を渡す
                    par.push_back(AP_event);
                    par2.push_back(AP_segment);
                    par3.push_back(AP_diffwf);
                    par4.push_back(Counts);

                    //APイベントのカウンターをリセット
                    Counts = 0;
                }
            }
        }
    }
    f->Close();
}

//微分波形からAPのカウンティングをする関数(生波形のROOTファイルから実行する場合)
void CountAPeventFromDiff2(TString filesrc, TString treename, int seg, float DIF_THRES, std::vector<float> &av_wf, int int_start, int int_end, std::vector<int> &par, std::vector<int> &par2, std::vector<float> &par3, std::vector<int> & par4)
{
    //ROOTファイルの読み込み
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);

    //配列を格納するBranchの変数
    float time[seg];
    float wfp[seg];
    float wfn[seg];
    int sc = 0;
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress("wform1", wfp);
    tr->SetBranchAddress("wform0", wfn);
    tr->SetBranchAddress("stopcell", &sc);


    //総イベント数を取得
    int nEve = tr->GetEntries();

    //全イベントで回す
    for(int i=0;i<nEve;i++)
    {
        //各変数の初期化
        int AP_event = i;   //APイベントのイベント番号
        int AP_segment = int_start; //APイベントのスタート位置(セグメント番号)
        float AP_diffwf = 0;    //APイベントスタート位置における微分波形の波高値
        int Counts = 0; //APイベントのカウンター(微分波形が連続していくつ閾値を超えたか)
        float diffwform;

        //i番目のイベントを読み込む
        tr->GetEntry(i);

        //int_startからint_endの範囲でループ
        for(int l=int_start;l<int_end;l++)
        {
            if((l+sc)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
            }
            else if ((l-1+sc)%1024==392)
            {
                wfp[l-1] = (wfp[l-2] + wfp[l])*0.5; 
                wfn[l-1] = (wfn[l-2] + wfn[l])*0.5;
            }

            diffwform = (wfp[l] - wfn[l] - av_wf[l]) - (wfp[l-1] - wfn[l-1] - av_wf[l-1]);
            //微分波形が閾値より大きい場合
            if(diffwform > DIF_THRES)
            {
                //1つ前のセグメントが閾値を超えていなかった場合
                if(Counts == 0)
                {
                    //APイベントの開始地点
                    AP_segment = l;
                    AP_diffwf = diffwform;
                }
                Counts += 1;
            }
            //微分波形が閾値を超えなかった場合
            else
            {
                //1つ前のセグメントが閾値を超えていた場合
                if(Counts != 0)
                {
                    //Vectorに各値を渡す
                    par.push_back(AP_event);
                    par2.push_back(AP_segment);
                    par3.push_back(AP_diffwf);
                    par4.push_back(Counts);

                    //APイベントのカウンターをリセット
                    Counts = 0;
                }
            }
        }
    }
    f->Close();
}

void CountAPeventFromDiff3(TString filesrc, TString treename, int seg, float DIF_THRES, std::vector<float> &av_wf, int int_start, int int_end, std::vector<int> &par, std::vector<int> &par2, std::vector<int> &par3, std::vector<float> & par4, std::vector<float> & par5,std::vector<int> & par6)
{
    //ROOTファイルの読み込み(微分波形)
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);

    //配列を格納するBranchの変数
    float time[seg];
    float wfp[seg];
    float wfn[seg];
    int sc = 0;
    tr->SetBranchAddress("time",time);
    tr->SetBranchAddress("wform1", wfp);
    tr->SetBranchAddress("wform0", wfn);
    tr->SetBranchAddress("stopcell", &sc);


    //総イベント数を取得
    int nEve = tr->GetEntries();

    //全イベントで回す
    for(int i=0;i<nEve;i++)
    {
        //各変数の初期化
        int AP_event = i;   //APイベントのイベント番号
        int AP_segment = int_start; //APイベントのスタート位置(セグメント番号)
        int peak_segment = int_start;
        float peak_ph = 0;
        float AP_diffwf = 0;    //APイベントスタート位置における微分波形の波高値
        int Counts = 0; //APイベントのカウンター(微分波形が連続していくつ閾値を超えたか)
        float wform;
        float diffwform;

        //i番目のイベントを読み込む
        tr->GetEntry(i);

        //int_startからint_endの範囲でループ
        for(int l=int_start;l<int_end;l++)
        {
            if((l+sc)%1024==392)
            {
                wfp[l] = (wfp[l-1] + wfp[l+1])*0.5; 
                wfn[l] = (wfn[l-1] + wfn[l+1])*0.5;
            }
            else if ((l-1+sc)%1024==392)
            {
                wfp[l-1] = (wfp[l-2] + wfp[l])*0.5; 
                wfn[l-1] = (wfn[l-2] + wfn[l])*0.5;
            }

            wform = (wfp[l] - wfn[l] - av_wf[l]);
            diffwform = (wfp[l] - wfn[l] - av_wf[l]) - (wfp[l-1] - wfn[l-1] - av_wf[l-1]);
            //微分波形が閾値より大きい場合
            if(diffwform > DIF_THRES)
            {
                //1つ前のセグメントが閾値を超えていなかった場合
                if(Counts == 0)
                {
                    //APイベントの開始地点
                    AP_segment = l;
                    peak_segment = l;
                    peak_ph = wform;
                    AP_diffwf = diffwform;
                }
                else
                {
                    if(wform > peak_ph)
                    {
                        peak_segment = l;
                        peak_ph = wform;
                    }
                }
                Counts += 1;
            }
            //微分波形が閾値を超えなかった場合
            else
            {
                //1つ前のセグメントが閾値を超えていた場合
                if(Counts != 0)
                {
                    //Vectorに各値を渡す
                    par.push_back(AP_event);
                    par2.push_back(AP_segment);
                    par3.push_back(peak_segment);
                    par4.push_back(AP_diffwf);
                    par5.push_back(peak_ph);
                    par6.push_back(Counts);

                    //APイベントのカウンターをリセット
                    Counts = 0;
                }
            }
        }
    }
    f->Close();
}


