#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "cmath"
#include "tuple"
#include "algorithm"

/*
###CONTENTS###
FindPeakValue: パルスのピークを探索する関数
CheckConv: パルスの収束を探索する関数(指定したcell数の範囲で収束したか)
JudgeAPevent:　あらかじめ求めておいたAP_eventがノイズか否か判定する関数
##############
*/


/*パルスのピーク値を探す関数(一つ先の値と比較する)
MIN_OR_MAX == 1の時は、最大値のピークを返す
MIN_OR_MAX == 0の時は、最小値のピークを返す

###引数###
flaot arr[]: 波形の入った配列(微分波形でも生波形でも良い)
int seg_start: ピーク探査を始めるsegmentの値
int MIN_OR_MAX: ピークの最大値(=1)or最小値(=0)
#########
*/
int FindPeakValue(float arr[], int seg_start, int MIN_OR_MAX = 1)
{
    int i = seg_start;
    if(MIN_OR_MAX == 0)
    {
        while(arr[i+1] < arr[i])
        {
            i++;
        }
    }
    else if (MIN_OR_MAX == 1)
    {
        while (arr[i+1] > arr[i])
        {
            i++;
        }
        
    }
    return i;
}

/*波形が収束したか確認する関数
FW_OR_BWD: パルスの立ち下がり(=1)、パルスの立ち上がり(=0)で、パルスの両端の位置を探索する

###引数###
arr[]: 波形の入った配列(生波形)
arr2[]: 波形の入った配列(微分波形)
seg_start: ピーク探査を始めるsegmentの値
limit: パルスの探索範囲
DIF_THRES: 微分波形の立ち上がりの閾値
PH_CONV: パルス波形の収束閾値
DIFF_CONV: 微分波形の収束閾値
FW_OR_BWD: パルスの立ち上がりor立ち下がり
#########

*/
int CheckConv(float arr[], float arr2[], int seg_start, int limit, float DIF_THRES, float PH_CONV, float DIFF_CONV, int FW_OR_BWD = 1)
{
    int i = seg_start;
    if(FW_OR_BWD == 1)
    {
        while(i < i + limit)
        {
            if(arr2[i] > DIF_THRES)
            {
                i = 0;
                break;
            }
            else if(std::abs(arr[i]) <= PH_CONV && std::abs(arr2[i]) <= DIFF_CONV)
            {
                break;
            }
            else
            {
                i++;
            }
        }
    }
    else if(FW_OR_BWD == 0)
    {
        while (i > i - limit)
        {
            if(std::abs(arr[i]) <= PH_CONV && std::abs(arr2[i]) <= DIFF_CONV)
            {
                break;
            }
            else
            {
                i--;
            }
        }
        
    }

    return i;

}

int CheckConv2(float arr[], float arr2[], int seg_start, int limit, float DIF_THRES, float PH_CONV, float DIFF_CONV, int FW_OR_BWD = 1, bool QD = false)
{
    int i = seg_start;
    if(FW_OR_BWD == 1)
    {
        while(i < i + limit)
        {
            //if(arr2[i] > DIF_THRES)
            if(std::abs(arr[i]) <= PH_CONV && std::abs(arr2[i]) <= DIFF_CONV)
            {
                //QD = true;
                break;
            }
            else if(arr2[i] > DIF_THRES)
            //else if(std::abs(arr[i]) <= PH_CONV && std::abs(arr2[i]) <= DIFF_CONV)
            {
                QD = true;
                break;
            }
            else
            {
                i++;
            }
        }
    }
    else if(FW_OR_BWD == 0)
    {
        while (i > i - limit)
        {
            if(std::abs(arr[i]) <= PH_CONV && std::abs(arr2[i]) <= DIFF_CONV)
            {
                break;
            }
            else
            {
                i--;
            }
        }
        
    }

    return i;

}

std::tuple<int, bool> CheckConv3(float arr[], float arr2[], int seg_start, int limit, float DIF_THRES, float PH_CONV, float DIFF_CONV, int FW_OR_BWD = 1, bool QD = false)
{
    int i = seg_start;
    if(FW_OR_BWD == 1)
    {
        while(i < i + limit)
        {
            //if(arr2[i] > DIF_THRES)
            if(std::abs(arr[i]) <= PH_CONV && std::abs(arr2[i]) <= DIFF_CONV)
            {
                //QD = true;
                break;
            }
            else if(arr2[i] > DIF_THRES)
            //else if(std::abs(arr[i]) <= PH_CONV && std::abs(arr2[i]) <= DIFF_CONV)
            {
                QD = true;
                break;
            }
            else
            {
                i++;
            }
        }
    }
    else if(FW_OR_BWD == 0)
    {
        while (i > i - limit)
        {
            if(std::abs(arr[i]) <= PH_CONV && std::abs(arr2[i]) <= DIFF_CONV)
            {
                break;
            }
            else
            {
                i--;
            }
        }
        
    }

    return std::forward_as_tuple(i, QD);

}


//APイベントをノイズ or AP_evnet or QuickDouble(連続してピークが到来した)の3つにGratingする関数

/*
###引数###
filesrc_s: 生波形のROOTファイル
treename_s: 生波形のROOTファイルのTree名
filesrc_d:　微分波形のROOTファイル
treename_d: 微分波形のROOTファイルのTree名
slice: ROOTファイルのsegの長さ(1024)
DIF_THRES: 微分波形の閾値
PH_THRES: 生波形の閾値
PH_CONV: 生波形の収束閾値
DIFF_CONV: 微分波形の収束閾値
event: AP_eventのイベントが入った配列
seg: AP_evnetのイベントが入った配列
par: Gratingした結果(1:ノイズ、2:AP_event, 3:AP_event(QuickDouble))
par2: AP_eventの開始位置
par3: AP_evneのピーク位置
par4: AP_eventのankle位置
par5: AP_eventの終了位置
par6: AP_eventの微分波形のピーク値
par7: AP_eventのピーク値
par8: AP_evnetのankle値
limit_d: 収束範囲閾値(開始位置)
limit_u: 収束範囲閾値(終了位置)

*/
void JudgeAPevent(TString filesrc_s, TString treename_s, TString filesrc_d, TString treename_d, 
    int slice, float DIF_THRES, float PH_THRES, float PH_CONV, float DIFF_CONV, 
    std::vector<int> &event, std::vector<int> &seg,
    std::vector<int> &par, std::vector<int> &par2, std::vector<int> &par3, std::vector<int> &par4,
    std::vector<int> &par5, std::vector<float> &par6, std::vector<float> &par7,std::vector<float> &par8,
    int limit_d = 5, int limit_u = 10)
{
    //ROOTファイルの読み込み
    TFile* f_s = TFile::Open(filesrc_s);
    TTree* tr_s = (TTree*)f_s->Get(treename_s);
    TFile* f_d = TFile::Open(filesrc_d);
    TTree* tr_d = (TTree*)f_d->Get(treename_d);

    //配列を用意
    float time_s[slice];
    float diff_wf[slice];
    float time_d[slice];
    float wform[slice];
    tr_s->SetBranchAddress("time",time_s);
    tr_s->SetBranchAddress("diffwf",diff_wf);
    tr_d->SetBranchAddress("time",time_d);
    tr_d->SetBranchAddress("wform", wform);

    //永吉法で見つかったAP_eventの長さ
    std::size_t size = event.size();


    //イベントラベル(0:ノイズ, 1:APevent, 2:QuickDouble)
    int Event_label = 1;

    int start_AP_event_seg;
    int diff_peak_seg;
    int ph_peak_seg;
    int diff_ankle_seg;
    int end_AP_event_seg;

    float diff_peak;
    float ph_peak;
    float wf_ankle;


    for(int i=0;i<size;i++)
    {
        //永吉法で見つかったAPのイベントを読み込む
        tr_s->GetEntry(event[i]);
        tr_d->GetEntry(event[i]);

        diff_peak_seg = FindPeakValue(diff_wf, seg[i]); //微分波形のピーク位置を探す
        diff_peak = diff_wf[diff_peak_seg];   //微分波形のピーク値を求める
        
        ph_peak_seg = FindPeakValue(wform, diff_peak_seg);  //生波形のピーク位置を探す
        ph_peak = wform[ph_peak_seg]; //生波形のピーク値を求める
        if(ph_peak > PH_THRES)  //櫻井法の閾値に基づきノイズでないかどうか判別する
        {
            //閾値より大きかった場合、生波形の折れ曲がりについて調べる
            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める

            end_AP_event_seg = CheckConv(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1); //APのイベントの収束位置を探す
            if(end_AP_event_seg != 0)   //QuickDoubleeが発生していないか確認
            {
                //QuickDoubleなし、通常のAPイベント
                Event_label = 1;
            }
            else
            {
                //QuickDoubleあり
                Event_label = 2;
            }
            //APの開始位置を求める
            start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);
        }
        else
        {
            //櫻井法の波高値より低い場合は、ノイズとみなす
            Event_label = 0;

            //一応、AP_eventの開始位置と終了位置を求める
            start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);

            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める

            end_AP_event_seg = CheckConv(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1);
            //end_AP_event_seg = CheckConv(wform, diff_wf, ph_peak_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1);
            //ankleは求めていないので、0を代入する
            //diff_ankle_seg = 0;
            //wf_ankle = 0;
        }

        //これまでの結果を返す
            par.push_back(Event_label);
            par2.push_back(start_AP_event_seg);
            par3.push_back(ph_peak_seg);
            par4.push_back(diff_ankle_seg);
            par5.push_back(end_AP_event_seg);
            par6.push_back(diff_peak);
            par7.push_back(ph_peak);
            par8.push_back(wf_ankle);
    }

    f_s->Close();
    f_d->Close();

}

// こちらを採用(QDの判定を少し変更)
void JudgeAPevent2(TString filesrc_s, TString treename_s, TString filesrc_d, TString treename_d, 
    int slice, float DIF_THRES, float PH_THRES, float PH_CONV, float DIFF_CONV, 
    std::vector<int> &event, std::vector<int> &seg,
    std::vector<int> &par, std::vector<int> &par2, std::vector<int> &par3, std::vector<int> &par4,
    std::vector<int> &par5, std::vector<float> &par6, std::vector<float> &par7,std::vector<float> &par8,
    std::vector<int> &par9,
    int limit_d = 5, int limit_u = 10)
{
    //ROOTファイルの読み込み
    TFile* f_s = TFile::Open(filesrc_s);
    TTree* tr_s = (TTree*)f_s->Get(treename_s);
    TFile* f_d = TFile::Open(filesrc_d);
    TTree* tr_d = (TTree*)f_d->Get(treename_d);

    //配列を用意
    float time_s[slice];
    float diff_wf[slice];
    float time_d[slice];
    float wform[slice];
    tr_s->SetBranchAddress("time",time_s);
    tr_s->SetBranchAddress("diffwf",diff_wf);
    tr_d->SetBranchAddress("time",time_d);
    tr_d->SetBranchAddress("wform",wform);

    //永吉法で見つかったAP_eventの長さ
    std::size_t size = event.size();


    //イベントラベル(0:ノイズ, 1:APevent, 2:QuickDoubleの可能性あり)
    int Event_label = 1;
    //QDラベル(1:QuickDouble有り, 0:QuickDoubleなし)
    int QD_label =  0;

    int start_AP_event_seg;
    int diff_peak_seg;
    int ph_peak_seg;
    int diff_ankle_seg;
    int end_AP_event_seg;

    float diff_peak;
    float ph_peak;
    float wf_ankle;
    bool QD_c;


    for(int i=0;i<size;i++)
    {
        //永吉法で見つかったAPのイベントを読み込む
        tr_s->GetEntry(event[i]);
        tr_d->GetEntry(event[i]);
        //bool QD_c = false;
        
        //APの開始位置を求める
        start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);

        //QuickDoubleイベントかどうか判別
        if(i != 0)
        {
            //一つ前に処理したイベントから連続しているかどうか
            if(event[i] ==  event[i-1] && seg[i] == par5[par5.size() - 1])
            {
                QD_label = 1;  //QuickDoubleの続き
                start_AP_event_seg = par5[par5.size() - 1];
            }
            else
            {
                QD_label = 0;  //独立したイベント
            }
        }
        
        diff_peak_seg = FindPeakValue(diff_wf, seg[i]); //微分波形のピーク位置を探す
        
        diff_peak = diff_wf[diff_peak_seg];   //微分波形のピーク値を求める
        
        ph_peak_seg = FindPeakValue(wform, diff_peak_seg);  //生波形のピーク位置を探す
        ph_peak = wform[ph_peak_seg]; //生波形のピーク値を求める
        if(ph_peak > PH_THRES)  //櫻井法の閾値に基づきノイズでないかどうか判別する
        {
            
            //閾値より大きかった場合、生波形の折れ曲がりについて調べる
            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める
            
            std::tie(end_AP_event_seg, QD_c) = CheckConv3(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1, false); //APのイベントの収束位置を探す
            if(QD_c == false)   //QuickDoubleeが発生していないか確認
            {
                //QuickDoubleなし、通常のAPイベント
                Event_label = 1;
            }
            else
            {
                //QuickDoubleあり
                Event_label = 2;
            }
            //APの開始位置を求める
            //start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);
        }
        else
        {
            //櫻井法の波高値より低い場合は、ノイズとみなす
            Event_label = 0;

            //一応、AP_eventの開始位置と終了位置を求める
            //start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);

            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める

            end_AP_event_seg = CheckConv2(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1, QD_c);
            //end_AP_event_seg = CheckConv(wform, diff_wf, ph_peak_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1);
            //ankleは求めていないので、0を代入する
            //diff_ankle_seg = 0;
            //wf_ankle = 0;
        }

        //これまでの結果を返す
            par.push_back(Event_label);
            par9.push_back(QD_label);
            par2.push_back(start_AP_event_seg);
            par3.push_back(ph_peak_seg);
            par4.push_back(diff_ankle_seg);
            par5.push_back(end_AP_event_seg);
            par6.push_back(diff_peak);
            par7.push_back(ph_peak);
            par8.push_back(wf_ankle);
    }

    f_s->Close();
    f_d->Close();

}


void Compare_SvsK(std::vector<int> &e_S, std::vector<int> &s_S, 
                std::vector<int> &e_K, std::vector<int> &s_K,
                std::vector<int> &e_K2, std::vector<int> &s_K2, 
                std::vector<int> &par)
{
    std::size_t size = e_S.size();
    int label;

    for(int i=0;i<size;i++)
    {
        int key_e = e_S[i];
        int key_s = s_S[i];

        if (std::find(e_K.begin(), e_K.end(), key_e) != e_K.end() && std::find(s_K.begin(), s_K.end(), key_s) != s_K.end())
        {
            label = 1;
        }
        else if (std::find(e_K2.begin(), e_K2.end(), key_e) != e_K2.end() && std::find(s_K2.begin(), s_K2.end(), key_s) != s_K2.end())
        {
            label = 2;
        }
        else
        {
            label = 0;
        }
        
        par.push_back(label);

    }
}


// (QDの判定を少し変更)
void JudgeAPevent3(TString filesrc_s, TString treename_s, TString filesrc_d, TString treename_d, 
    int slice, float DIF_THRES, float PH_THRES, float PH_CONV, float DIFF_CONV, 
    std::vector<int> &event, std::vector<int> &seg,
    std::vector<int> &par, std::vector<int> &par2, std::vector<int> &par3, std::vector<int> &par4,
    std::vector<int> &par5, std::vector<float> &par6, std::vector<float> &par7,std::vector<float> &par8,
    std::vector<int> &par9,
    int limit_d = 5, int limit_u = 10)
{
    //ROOTファイルの読み込み
    TFile* f_s = TFile::Open(filesrc_s);
    TTree* tr_s = (TTree*)f_s->Get(treename_s);
    TFile* f_d = TFile::Open(filesrc_d);
    TTree* tr_d = (TTree*)f_d->Get(treename_d);

    //配列を用意
    float time_s[slice];
    float diff_wf[slice];
    float time_d[slice];
    float wform[slice];
    tr_s->SetBranchAddress("time",time_s);
    tr_s->SetBranchAddress("diffwf",diff_wf);
    tr_d->SetBranchAddress("time",time_d);
    tr_d->SetBranchAddress("wform",wform);

    //永吉法で見つかったAP_eventの長さ
    std::size_t size = event.size();


    //イベントラベル(0:ノイズ, 1:APevent, 2:QuickDoubleの可能性あり, 3:重複)
    int Event_label = 1;
    //QDラベル(1:QuickDouble有り, 0:QuickDoubleなし)
    int QD_label =  0;

    int start_AP_event_seg;
    int diff_peak_seg;
    int ph_peak_seg;
    int diff_ankle_seg;
    int end_AP_event_seg;

    float diff_peak;
    float ph_peak;
    float wf_ankle;
    bool QD_c;


    for(int i=0;i<size;i++)
    {
        //永吉法で見つかったAPのイベントを読み込む
        tr_s->GetEntry(event[i]);
        tr_d->GetEntry(event[i]);
        //bool QD_c = false;
        
        //APの開始位置を求める
        start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);

        //QuickDoubleイベントかどうか判別
        if(i != 0)
        {
            //一つ前に処理したイベントから連続しているかどうか
            if(event[i] ==  event[i-1] && seg[i] == par5[par5.size() - 1])
            {
                QD_label = 1;  //QuickDoubleの続き
                start_AP_event_seg = par5[par5.size() - 1];
            }
            else
            {
                QD_label = 0;  //独立したイベント
            }
        }
        
        diff_peak_seg = FindPeakValue(diff_wf, seg[i]); //微分波形のピーク位置を探す
        
        diff_peak = diff_wf[diff_peak_seg];   //微分波形のピーク値を求める
        
        ph_peak_seg = FindPeakValue(wform, diff_peak_seg);  //生波形のピーク位置を探す
        ph_peak = wform[ph_peak_seg]; //生波形のピーク値を求める
        if(ph_peak > PH_THRES)  //櫻井法の閾値に基づきノイズでないかどうか判別する
        {
            
            //閾値より大きかった場合、生波形の折れ曲がりについて調べる
            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める
            
            std::tie(end_AP_event_seg, QD_c) = CheckConv3(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1, false); //APのイベントの収束位置を探す
            if(QD_c == false)   //QuickDoubleが発生していないか確認
            {
                if (event[i] ==  event[i-1])
                {
                    if(seg[i] >= par2[par2.size() - 1] && seg[i] <  par5[par5.size() - 1] )
                    {
                        Event_label = 3;
                    }
                    else
                    {
                        Event_label = 1;
                    }
                }
                else
                {
                //QuickDoubleなし、通常のAPイベント
                Event_label = 1;
                }
            }
            else
            {
                //QuickDoubleあり
                Event_label = 2;
            }
            //APの開始位置を求める
            //start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);
        }
        else
        {
            //櫻井法の波高値より低い場合は、ノイズとみなす
            Event_label = 0;

            //一応、AP_eventの開始位置と終了位置を求める
            //start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);

            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める

            end_AP_event_seg = CheckConv2(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1, QD_c);
            //end_AP_event_seg = CheckConv(wform, diff_wf, ph_peak_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1);
            //ankleは求めていないので、0を代入する
            //diff_ankle_seg = 0;
            //wf_ankle = 0;
        }

        //これまでの結果を返す
            par.push_back(Event_label);
            par9.push_back(QD_label);
            par2.push_back(start_AP_event_seg);
            par3.push_back(ph_peak_seg);
            par4.push_back(diff_ankle_seg);
            par5.push_back(end_AP_event_seg);
            par6.push_back(diff_peak);
            par7.push_back(ph_peak);
            par8.push_back(wf_ankle);
    }

    f_s->Close();
    f_d->Close();

}


void Compare_SvsK2(std::vector<int> &e_S, std::vector<int> &s_S, 
                std::vector<int> &e_K, std::vector<int> &s_K,
                std::vector<int> &par)
{
    std::size_t size = e_S.size();
    int label;

    for(int i=0;i<size;i++)
    {
        int key_e = e_S[i];
        int key_s = s_S[i];

        if (std::find(e_K.begin(), e_K.end(), key_e) != e_K.end() && std::find(s_K.begin(), s_K.end(), key_s) != s_K.end())
        {
            label = 1;
        }
        else
        {
            label = 0;
        }
        
        par.push_back(label);

    }
}

//ver 4(多分これが最新版)
void JudgeAPevent4(TString filesrc_s, TString treename_s, TString filesrc_d, TString treename_d, 
    int slice, float DIF_THRES, float PH_THRES, float PH_CONV, float DIFF_CONV, 
    std::vector<int> &event, std::vector<int> &seg,
    std::vector<int> &par, std::vector<int> &par2, std::vector<int> &par3, std::vector<int> &par4,
    std::vector<int> &par5, std::vector<float> &par6, std::vector<float> &par7,std::vector<float> &par8,
    std::vector<int> &par9,
    int limit_d = 5, int limit_u = 10, TString Branch_diff = "diffwf", TString Branch_off = "wform")
{
    //ROOTファイルの読み込み
    TFile* f_s = TFile::Open(filesrc_s);
    TTree* tr_s = (TTree*)f_s->Get(treename_s);
    TFile* f_d = TFile::Open(filesrc_d);
    TTree* tr_d = (TTree*)f_d->Get(treename_d);

    //配列を用意
    float time_s[slice];
    float diff_wf[slice];
    float time_d[slice];
    float wform[slice];
    tr_s->SetBranchAddress("time",time_s);
    tr_s->SetBranchAddress(Branch_diff,diff_wf);
    tr_d->SetBranchAddress("time",time_d);
    tr_d->SetBranchAddress(Branch_off,wform);

    //永吉法で見つかったAP_eventの長さ
    std::size_t size = event.size();


    //イベントラベル(0:ノイズ, 1:APevent, 2:QuickDoubleの可能性あり, 3:重複)
    int Event_label = 1;
    //QDラベル(1:QuickDouble有り, 0:QuickDoubleなし)
    int QD_label =  0;

    int start_AP_event_seg;
    int diff_peak_seg;
    int ph_peak_seg;
    int diff_ankle_seg;
    int end_AP_event_seg;

    float diff_peak;
    float ph_peak;
    float wf_ankle;
    bool QD_c;


    for(int i=0;i<size;i++)
    {
        //永吉法で見つかったAPのイベントを読み込む
        tr_s->GetEntry(event[i]);
        tr_d->GetEntry(event[i]);
        
        //APの開始位置を求める
        start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);

        //QuickDoubleイベントかどうか判別
        if(i != 0)
        {
            //一つ前に処理したイベントから連続しているかどうか
            if(event[i] ==  event[i-1] && seg[i] == par5[par5.size() - 1])
            {
                QD_label = 1;  //QuickDoubleの続き
                start_AP_event_seg = par5[par5.size() - 1];
            }
            else
            {
                QD_label = 0;  //独立したイベント
            }
        }
        
        diff_peak_seg = FindPeakValue(diff_wf, seg[i]); //微分波形のピーク位置を探す
        
        diff_peak = diff_wf[diff_peak_seg];   //微分波形のピーク値を求める
        
        ph_peak_seg = FindPeakValue(wform, diff_peak_seg);  //生波形のピーク位置を探す
        ph_peak = wform[ph_peak_seg]; //生波形のピーク値を求める
        if(ph_peak > PH_THRES)  //櫻井法の閾値に基づきノイズでないかどうか判別する
        {
            //前イベントと被っていないか確認
            if(i != 0 && event[i] == event[i-1])
            {
                //前イベントと同じ、イベント番号かつ、segが前イベントのrangeに入ってたら3、それ以外は全て1(暫定)
                if(seg[i] >= par2[par2.size() - 1] && seg[i] <  par5[par5.size() - 1])
                {
                    Event_label = 3;
                }
                else
                {
                    Event_label = 1;
                }
            }
            else
            {
                Event_label = 1;
            }
            
            //閾値より大きかった場合、生波形の折れ曲がりについて調べる
            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める
            
            std::tie(end_AP_event_seg, QD_c) = CheckConv3(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1, false); //APのイベントの収束位置を探す
            if(Event_label != 3)
            {
                if(QD_c == false)   //QuickDoubleが発生していないか確認
                {
                    //QuickDoubleなし、通常のAPイベント
                    Event_label = 1;
                }
                else
                {
                    //QuickDoubleあり
                    Event_label = 2;
                }
            }
        }
        else
        {
            //櫻井法の波高値より低い場合は、ノイズとみなす
            Event_label = 0;

            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める

            end_AP_event_seg = CheckConv2(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1, QD_c);
        }

        //これまでの結果を返す
            par.push_back(Event_label);
            par9.push_back(QD_label);
            par2.push_back(start_AP_event_seg);
            par3.push_back(ph_peak_seg);
            par4.push_back(diff_ankle_seg);
            par5.push_back(end_AP_event_seg);
            par6.push_back(diff_peak);
            par7.push_back(ph_peak);
            par8.push_back(wf_ankle);
    }

    f_s->Close();
    f_d->Close();

}


//ver 4(多分これが最新版)
void JudgeAPevent5(TString filesrc_s, TString treename_s, TString filesrc_d, TString treename_d, 
    int slice, float DIF_THRES, float PH_THRES, float PH_CONV, float DIFF_CONV, 
    std::vector<int> &event, std::vector<int> &seg,
    std::vector<int> &par, std::vector<int> &par2, std::vector<int> &par3, std::vector<int> &par4,
    std::vector<int> &par5, std::vector<float> &par6, std::vector<float> &par7,std::vector<float> &par8,
    std::vector<int> &par9,
    int limit_d = 5, int limit_u = 10)
{
    //ROOTファイルの読み込み
    TFile* f_s = TFile::Open(filesrc_s);
    TTree* tr_s = (TTree*)f_s->Get(treename_s);
    TFile* f_d = TFile::Open(filesrc_d);
    TTree* tr_d = (TTree*)f_d->Get(treename_d);

    //配列を用意
    float time_s[slice];
    float diff_wf[slice];
    float time_d[slice];
    float wform[slice];
    tr_s->SetBranchAddress("time",time_s);
    tr_s->SetBranchAddress("diffwf",diff_wf);
    tr_d->SetBranchAddress("time",time_d);
    tr_d->SetBranchAddress("wform",wform);

    //永吉法で見つかったAP_eventの長さ
    std::size_t size = event.size();


    //イベントラベル(0:ノイズ, 1:APevent, 2:QuickDoubleの可能性あり, 3:重複)
    int Event_label = 1;
    //QDラベル(1:QuickDouble有り, 0:QuickDoubleなし)
    int QD_label =  0;

    int start_AP_event_seg;
    int diff_peak_seg;
    int ph_peak_seg;
    int diff_ankle_seg;
    int end_AP_event_seg;

    float diff_peak;
    float ph_peak;
    float wf_ankle;
    bool QD_c;


    for(int i=0;i<size;i++)
    {
        //永吉法で見つかったAPのイベントを読み込む
        tr_s->GetEntry(event[i]);
        tr_d->GetEntry(event[i]);
        
        
        //APの開始位置を求める
        start_AP_event_seg = CheckConv(wform, diff_wf, seg[i], limit_d, DIF_THRES, PH_CONV, DIFF_CONV, 0);


        /*
        //QuickDoubleイベントかどうか判別
        if(i != 0)
        {
            //一つ前に処理したイベントから連続しているかどうか
            if(event[i] ==  event[i-1] && seg[i] == par5[par5.size() - 1])
            {
                QD_label = 1;  //QuickDoubleの続き
                start_AP_event_seg = par5[par5.size() - 1];
            }
            else
            {
                QD_label = 0;  //独立したイベント
            }
        }
        */
        
        diff_peak_seg = FindPeakValue(diff_wf, seg[i]); //微分波形のピーク位置を探す
        
        diff_peak = diff_wf[diff_peak_seg];   //微分波形のピーク値を求める
        
        ph_peak_seg = FindPeakValue(wform, diff_peak_seg);  //生波形のピーク位置を探す
        ph_peak = wform[ph_peak_seg]; //生波形のピーク値を求める
        if(ph_peak > PH_THRES)  //櫻井法の閾値に基づきノイズでないかどうか判別する
        {
            int a = 1;
            //std::cout << "over"  << std::endl;
            
            //前イベントと被っていないか確認
            if(event[i] == event[i-1])
            {
                //前イベントと同じ、イベント番号かつ、segが前イベントのrangeに入ってたら3、それ以外は全て1(暫定)
                if(seg[i] >= par2[par2.size() - 1] && seg[i] <  par5[par5.size() - 1])
                {
                    Event_label = 3;
                }
                else
                {
                    Event_label = 1;
                }
            }
            else
            {
                Event_label = 1;
            }
            /*
            //閾値より大きかった場合、生波形の折れ曲がりについて調べる
            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める
            
            std::tie(end_AP_event_seg, QD_c) = CheckConv3(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1, false); //APのイベントの収束位置を探す
            if(Event_label != 3)
            {
                if(QD_c == false)   //QuickDoubleが発生していないか確認
                {
                    //QuickDoubleなし、通常のAPイベント
                    Event_label = 1;
                }
                else
                {
                    //QuickDoubleあり
                    Event_label = 2;
                }
            }
            */
        }
        else
        {
            int b = 1;
            //std::cout << "not over"  << std::endl;
            
            //櫻井法の波高値より低い場合は、ノイズとみなす
            Event_label = 0;
            /*
            diff_ankle_seg = FindPeakValue(diff_wf, ph_peak_seg, 0);   //生波形のankle位置を探す(微分波形の極小点)
            wf_ankle = wform[diff_ankle_seg]; //生波形のankle値を求める

            end_AP_event_seg = CheckConv2(wform, diff_wf, diff_ankle_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1, QD_c);
            */
        }

        
        //これまでの結果を返す
        par.push_back(Event_label);
        /*
        par9.push_back(QD_label);
        */
        par2.push_back(start_AP_event_seg);
        /*
        par3.push_back(ph_peak_seg);
        par4.push_back(diff_ankle_seg);
        par5.push_back(end_AP_event_seg);
        par6.push_back(diff_peak);
        par7.push_back(ph_peak);
        par8.push_back(wf_ankle);
        */
    }
    

    f_s->Close();
    f_d->Close();

}