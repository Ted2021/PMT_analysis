#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"
#include "cmath"

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
    tr_d->SetBranchAddress("wform",wform);

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
            end_AP_event_seg = CheckConv(wform, diff_wf, ph_peak_seg, limit_u, DIF_THRES, PH_CONV, DIFF_CONV, 1);
            //ankleは求めていないので、0を代入する
            diff_ankle_seg = 0;
            wf_ankle = 0;
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
