#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"

/*
引数一覧
filesrc : ROOTファイル
treename : AP波形の入ったTree名

*/


void scannig_wform(TString filesrc, TString treename, int start, float thres1, float thres2, float thres3, float thres4, std::vector<float> &ave, std::vector<int> &par, std::vector<std::vector<int>> &par2, std::vector<std::vector<int>> &par3, std::vector<std::vector<int>> &par4, std::vector<std::vector<int>> &wms, std::vector<std::vector<float>> &par5, std::vector<std::vector<int>> &dms, std::vector<std::vector<float>> &par6, std::vector<int> &par7, std::vector<int> &par8, std::vector<int> & par9)
{
    //ROOTファイルの読み込みとパラメータの対応づけ
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float wform[4504] = {0};
    tr->SetBranchAddress("wave", wform);
    int nEve = tr->GetEntries();        //全イベント数

    //閾値パラメータ(引数で与えるか、事前に定義しておく)
    float PulseThres = thres1;   //波形の立ち上がりの閾値
    float peakThres = thres2;    //ピーク波高値の閾値(波形平均との差分が0.4p.e.)
    float FallEndThres = -thres1/2; //波形の立ち下がりの閾値(0 or -PulseThres)
    float QuickDoubleThres = thres1; //QuickDubleの閾値
    float derivCoveThres = thres3;   //APの収束を決めるderivateの閾値(平均のsigmaかな..)
    float wformCoveThres = thres4;   //APの収束を決めるwformの閾値(darkのsigma)

    //変動パラメータ    (スライスごとに更新)
    float wformPre;
    float derivPre;
    float derivative;

    //フラグ   (Evnetごとに初期化を必ず行う)
    int state = 0;          //AP_eventを決定するフラグ, 0:ready, 1:armed, 2:fall, 3:peakfind
    bool QuickDouble = false;    //QuickDoubleが生じたかを判定するフラグ, False, True
    int qdf = 0;            //QuickDoubleの判定
    int cont_pulse = 0;     //PulseEventが連続して生じた場合の個数, -1個

    int conver = 0;         //収束閾値より低くなった場合のカウント
    int PFStateCnt = 0;     //peakfind状態に入ってからのカウント(このカウントで強制的にEventから抜ける)

    int pre_conver = 0;        //for Noise
    int pre_PFStateCnt = 0;     //for Noise
    
    //derivativeとwformの最大値 (Eventごとに更新)
    float derivativeMax;
    float wformMax;

    //戻り値
    std::vector<int> armed;     // armed状態に入った時のslice番号を格納する配列
    std::vector<int> fall;      // fall状態に入った時のslice番号を格納する配列
    std::vector<int> peak;      // peak状態に入った時のslice番号を格納する配列
    std::vector<int> wf_s;      //wformMaxのSliceを格納する配列
    std::vector<float> wformMax_list; //wformMaxを格納する配列
    std::vector<int> df_s;      //derivativeMaxのSliceを格納する配列
    std::vector<float> derivativeMax_list; //derivativeMaxを格納する配列
    int wfs;
    int end_event;              //AP_Eventの最終スライス

    //全Eventで回す
    for(int i=0;i<nEve;i++)
    {
        tr->GetEntry(i);    //i番目のEventを読み込む

        state = 0;          //最初のStateは、ready
        int l = start;
        derivPre = (wform[l] -wform[l-1]) * 1e3;
        wformPre = wform[l-1] * 1e3;

        //l>= startから l< 4504まで回す　derivativeの計算から4503までにしないとオーバーフローする
        //while(l < 4504)
        while(l < 4503)
        {
            derivative = (wform[l+1] -wform[l]) * 1e3;

            if(derivative > PulseThres && wform[l]*1e3 - ave[l] > peakThres){
                state = 1;  //armedに入る
                if(wform[l] > 0 && wform[l-1] > 0 && wform[l-1] < wform[l]){
                    armed.push_back(l-1);                                      //パラメータ入れる
                }else{
                    armed.push_back(l);                                            //パラメータ入れる
                }                                            //パラメータ入れる

                //パラメーターの初期化
                bool scan_comp = false;
                //int index = l;

                while(scan_comp == false)           //メイン処理
                {
                    //微分波形を求める
                    derivative = (wform[l+1] - wform[l]) * 1e3;

                    if(state == 1)          //armed
                    {
                        if(derivative < derivPre){
                            derivativeMax = derivPre;
                            wformMax = wform[l] * 1e3;
                            wfs = l;
                            state = 2;
                            df_s.push_back(l-1);
                            derivativeMax_list.push_back(derivativeMax);    //パラメータ入れる
                            /*
                            if(wform[l] <= wform[l+1]){
                                fall.push_back(l+1);                        //パラメータ入れる
                            }else{
                                fall.push_back(l);                        //パラメータ入れる
                            }*/
                            fall.push_back(l);
                        }
                    }
                    else if(state == 2)     //fall
                    {
                        //wformの最大値探索
                        if(wform[l] * 1e3 >= wformMax){
                            wformMax = wform[l] * 1e3;
                            wfs = l;
                        }
                        //derivativeが急速に増加した場合->QuickDouble判定
                        if(derivative > FallEndThres && derivative - derivPre <= QuickDoubleThres){
                            QuickDouble = true;  
                        }

                        //条件分岐
                        if(derivPre <= FallEndThres || derivative <= FallEndThres){      //derivativeが閾値より下がった場合  
                            if((wformMax - ave[l] > peakThres) && (wform[wfs-1]*1e3 - ave[wfs-1] > peakThres) && (wform[wfs+1]*1e3 - ave[wfs+1] > peakThres)){        
                                
                                state = 3;                  //peakfind状態に移行
                                wf_s.push_back(wfs);                            //パラメータ格納
                                wformMax_list.push_back(wformMax);              //パラメータ入れる
                                peak.push_back(l);

                            }else{     //wformMaxとその前後が0.4peの波高よりも小さい場合->ノイズとして処理
                                state = 5;       
                            }
                        }
                    }
                    else if(state == 3)     //peakfind
                    {
                        PFStateCnt += 1;
                        if((-derivCoveThres <= derivative <= derivCoveThres) && (wformCoveThres <= wform[l]*1e3 <= 0.0)){
                            conver += 1;
                            //state = 4;
                        }

                        if(derivative > PulseThres){
                            pre_conver = conver;
                            pre_PFStateCnt = PFStateCnt;
                            PFStateCnt = 0;
                            conver = 0;
                            cont_pulse += 1;
                            state = 1;
                            armed.push_back(l);                                            //パラメータ入れる
                        }
                        
                        if (PFStateCnt >= 10 || conver >= 5){
                            pre_conver = 0;
                            pre_PFStateCnt = 0;
                            PFStateCnt = 0;
                            conver = 0;
                            state = 4;
                            end_event = l;                              //パラメータ入れる
                        }
                    }
                    else if(state == 4)     //終了処理
                    {
                        par.push_back(i);
                        par2.push_back(armed);
                        par3.push_back(fall);
                        par4.push_back(peak);
                        wms.push_back(wf_s);
                        par5.push_back(wformMax_list);
                        dms.push_back(df_s);
                        par6.push_back(derivativeMax_list);
                        par7.push_back(end_event);
                        if(QuickDouble == true){
                            qdf = 1;
                        }else{
                            qdf = 0;
                        }
                        par8.push_back(qdf);
                        par9.push_back(cont_pulse);

                        scan_comp = true;

                        wfs = 0;
                        end_event = 0;
                        cont_pulse = 0;
                        QuickDouble = false;
                        wf_s.clear();
                        df_s.clear();
                        armed.clear();
                        fall.clear();
                        peak.clear();
                        wformMax_list.clear();
                        derivativeMax_list.clear();

                    }
                    else if(state == 5)     //ノイズ判定になった場合
                    {
                        if(cont_pulse >= 1){    //ノイズ波形の前にAP_Pulseがあった場合->ノイズ波形でカウントしてしまったparameterを消して、peakfind状態に移行
                            armed.pop_back();
                            fall.pop_back();
                            derivativeMax_list.pop_back();
                            df_s.pop_back();
                            cont_pulse -= 1;
                            conver = pre_conver;
                            PFStateCnt = pre_PFStateCnt;
                            state = 3;
                        }else{                  //ノイズ波形のみのため、パラメータを初期化して判定から抜ける
                            scan_comp = true;
                            wfs = 0;
                            end_event = 0;
                            cont_pulse = 0;
                            QuickDouble = false;
                            wf_s.clear();
                            df_s.clear();
                            armed.clear();
                            fall.clear();
                            peak.clear();
                            wformMax_list.clear();
                            derivativeMax_list.clear();
                        }
                    }

                    //Preの更新
                    derivPre = derivative;
                    wformPre = wform[l] * 1e3;
                    //1スライス進める
                    //index +=1;
                    l += 1;
                    if(l >= 4503){
                        //APのeventが入ってもCharge推定不可のため、破棄する    
                        scan_comp = true;

                        wfs = 0;
                        end_event = 0;
                        cont_pulse = 0;
                        QuickDouble = false;
                        wf_s.clear();
                        df_s.clear();
                        armed.clear();
                        fall.clear();
                        peak.clear();
                        wformMax_list.clear();
                        derivativeMax_list.clear();
                    }

                }
                //スライスの更新
                //l = index;
                
            }else{
                derivPre = derivative;
                wformPre = wform[l] * 1e3;
                l += 1;
            }

        }

    }
    f->Close();
}


void example(std::vector<float> &wform, int entry, int start, float thres1, float thres2, float thres3, float thres4, std::vector<float> &ave, std::vector<int> &par, std::vector<std::vector<int>> &par2, std::vector<std::vector<int>> &par3, std::vector<std::vector<int>> &par4, std::vector<std::vector<int>> &wms, std::vector<std::vector<float>> &par5, std::vector<std::vector<int>> &dms, std::vector<std::vector<float>> &par6, std::vector<int> &par7, std::vector<int> &par8, std::vector<int> & par9)
{

    //閾値パラメータ(引数で与えるか、事前に定義しておく)
    float PulseThres = thres1;   //波形の立ち上がりの閾値
    float peakThres = thres2;    //ピーク波高値の閾値(波形平均との差分が0.4p.e.)
    float FallEndThres = -thres1/2; //波形の立ち下がりの閾値(0 or -PulseThres)
    float QuickDoubleThres = thres1; //QuickDubleの閾値
    float derivCoveThres = thres3;   //APの収束を決めるderivateの閾値(平均のsigmaかな..)
    float wformCoveThres = thres4;   //APの収束を決めるwformの閾値(darkのsigma)

    //変動パラメータ    (スライスごとに更新)
    float wformPre;
    float derivPre;
    float derivative;

    //フラグ   (Evnetごとに初期化を必ず行う)
    int state = 0;          //AP_eventを決定するフラグ, 0:ready, 1:armed, 2:fall, 3:peakfind
    bool QuickDouble = false;    //QuickDoubleが生じたかを判定するフラグ, False, True
    int qdf = 0;            //QuickDoubleの判定
    int cont_pulse = 0;     //PulseEventが連続して生じた場合の個数, -1個

    int conver = 0;         //収束閾値より低くなった場合のカウント
    int PFStateCnt = 0;     //peakfind状態に入ってからのカウント(このカウントで強制的にEventから抜ける)

    int pre_conver = 0;
    int pre_PFStateCnt = 0;
    
    //derivativeとwformの最大値 (Eventごとに更新)
    float derivativeMax;
    float wformMax;

    //戻り値
    std::vector<int> armed;     // armed状態に入った時のslice番号を格納する配列
    std::vector<int> fall;      // fall状態に入った時のslice番号を格納する配列
    std::vector<int> peak;      // peak状態に入った時のslice番号を格納する配列
    std::vector<int> wf_s;      //wformMaxのSliceを格納する配列
    std::vector<float> wformMax_list; //wformMaxを格納する配列
    std::vector<int> df_s;      //derivativeMaxのSliceを格納する配列
    std::vector<float> derivativeMax_list; //derivativeMaxを格納する配列
    int wfs;
    int end_event;              //AP_Eventの最終スライス

    state = 0;          //最初のStateは、ready
    int l = start;
    derivPre = (wform[l] -wform[l-1]) * 1e3;
    wformPre = wform[l-1] * 1e3;

    //l>= startから l< 4504まで回す
    //while(l < 4504)
    while(l < 4503)
    {
        derivative = (wform[l+1] -wform[l]) * 1e3;

        if(derivative > PulseThres && wform[l]*1e3 - ave[l] > peakThres){
            //std::cout << "HIT!" << l <<std::endl;
            state = 1;  //armedに入る
            if(wform[l] > 0 && wform[l-1] > 0 && wform[l-1] < wform[l]){
                armed.push_back(l-1);                                      //パラメータ入れる
            }else{
                armed.push_back(l);                                            //パラメータ入れる
            }
            

            //パラメーターの初期化
            bool scan_comp = false;
            //int index = l;

            while(scan_comp == false)           //メイン処理
            {
                //std::cout << l <<std::endl;
                //微分波形を求める
                derivative = (wform[l+1] - wform[l]) * 1e3;

                if(state == 1)          //armed
                {
                    if(derivative < derivPre){
                        derivativeMax = derivPre;
                        wformMax = wform[l] * 1e3;
                        wfs = l;
                        state = 2;
                        df_s.push_back(l-1);                              //パラメータ入れる
                        derivativeMax_list.push_back(derivativeMax);    //パラメータ入れる
                        /*
                        if(wform[l] <= wform[l+1]){
                            fall.push_back(l+1);                        //パラメータ入れる
                        }else{
                            fall.push_back(l);                        //パラメータ入れる
                        }
                        */
                       fall.push_back(l);
                        
                    }
                }
                else if(state == 2)     //fall
                {
                    //wformの最大値探索
                    if(wform[l] * 1e3 >= wformMax){
                        wformMax = wform[l] * 1e3;
                        wfs = l;
                    }
                    //derivativeが急速に増加した場合->QuickDouble判定
                    if(derivative > FallEndThres && derivative - derivPre <= QuickDoubleThres){
                        QuickDouble = true;  
                    }

                    //条件分岐
                    if(derivative < FallEndThres){      //derivativeが閾値より下がった場合  
                        if((wformMax - ave[l] > peakThres) && (wform[wfs-1]*1e3 - ave[wfs-1] > peakThres) && (wform[wfs+1]*1e3 - ave[wfs+1] > peakThres)){        
                            
                            state = 3;                  //peakfind状態に移行
                            wf_s.push_back(wfs);                            //パラメータ格納
                            wformMax_list.push_back(wformMax);              //パラメータ入れる
                            peak.push_back(l);

                        }else{     //wformMaxとその前後が0.4peの波高よりも小さい場合->ノイズとして処理
                            state = 5;       
                        }
                    }
                }
                else if(state == 3)     //peakfind
                {
                    PFStateCnt += 1;
                    if((-derivCoveThres < derivative < derivCoveThres) && (wformCoveThres < wform[l] < 0.0)){
                        conver += 1;
                        //state = 4;
                    }

                    if(derivative > PulseThres){
                        pre_conver = conver;
                        pre_PFStateCnt = PFStateCnt;
                        PFStateCnt = 0;
                        conver = 0;
                        cont_pulse += 1;
                        state = 1;
                        armed.push_back(l);                                            //パラメータ入れる
                    }else if (PFStateCnt >= 10 || conver >= 5){
                        pre_conver = 0;
                        pre_PFStateCnt = 0;
                        PFStateCnt = 0;
                        conver = 0;
                        state = 4;
                        end_event = l;                              //パラメータ入れる
                    }
                }
                else if(state == 4)     //終了処理
                {
                    par.push_back(entry);
                    par2.push_back(armed);
                    par3.push_back(fall);
                    par4.push_back(peak);
                    wms.push_back(wf_s);
                    par5.push_back(wformMax_list);
                    dms.push_back(df_s);
                    par6.push_back(derivativeMax_list);
                    par7.push_back(end_event);
                    if(QuickDouble == true){
                        qdf = 1;
                    }else{
                        qdf = 0;
                    }
                    par8.push_back(qdf);
                    par9.push_back(cont_pulse);

                    scan_comp = true;
                    wfs = 0;
                    end_event = 0;
                    cont_pulse = 0;
                    QuickDouble = false;
                    wf_s.clear();
                    df_s.clear();
                    armed.clear();
                    fall.clear();
                    peak.clear();
                    wformMax_list.clear();
                    derivativeMax_list.clear();

                }
                else if(state == 5)     //ノイズ判定になった場合
                {
                    //std::cout << "NOISE!" << l <<std::endl;

                    if(cont_pulse >= 1){    //ノイズ波形の前にAP_Pulseがあった場合->ノイズ波形でカウントしてしまったparameterを消して、peakfind状態に移行
                        armed.pop_back();
                        fall.pop_back();
                        derivativeMax_list.pop_back();
                        df_s.pop_back();
                        cont_pulse -= 1;
                        conver = pre_conver;
                        PFStateCnt = pre_PFStateCnt;
                        state = 3;
                    }else{                  //ノイズ波形のみのため、パラメータを初期化して判定から抜ける
                        scan_comp = true;
                        wfs = 0;
                        end_event = 0;
                        cont_pulse = 0;
                        QuickDouble = false;
                        wf_s.clear();
                        df_s.clear();
                        armed.clear();
                        fall.clear();
                        peak.clear();
                        wformMax_list.clear();
                        derivativeMax_list.clear();
                    }
                }

                //Preの更新
                derivPre = derivative;
                wformPre = wform[l]*1e3;
                //1スライス進める
                l+=1;
                if(l >= 4503){
                    scan_comp = true;

                    wfs = 0;
                    end_event = 0;
                    cont_pulse = 0;
                    QuickDouble = false;
                    wf_s.clear();
                    df_s.clear();
                    armed.clear();
                    fall.clear();
                    peak.clear();
                    wformMax_list.clear();
                    derivativeMax_list.clear();
                }

            }
            
        }else{
            //std::cout << derivative <<std::endl;
            derivPre = derivative;
            wformPre = wform[l]*1e3;
            l += 1;
        }

    }

}

void get_wave(TString filesrc, TString treename, int start, std::vector<float> &par)
{
    //ROOTファイルの読み込みとパラメータの対応づけ
    TFile* f = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float wform[4504] = {0};
    tr->SetBranchAddress("wave", wform);
    int nEve = tr->GetEntries();        //全イベント数

    tr->GetEntry(start);
    for(int l = 0;l<4504;l++){
        par.push_back(wform[l]);
    }
    f->Close();
}