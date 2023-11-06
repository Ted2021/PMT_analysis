#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "iostream"

void Calc_Chrg(TString filesrc,TString filedrk, 
                std::vector<float> &chrg_list, 
                TString treename = "Treesource_0", TString treedark = "Treedark_0", 
                TString branch_wf_s = "wform", TString branch_wf_d = "wform",
                int t_min=320, int t_max=360, int seg = 1024)
{
    TFile* f  = TFile::Open(filesrc);
    TFile* f_drk  = TFile::Open(filedrk);
    TTree* tr = (TTree*)f->Get(treename);
    TTree* tr_drk = (TTree*)f_drk->Get(treedark);
    float time_e[seg];
    float time_d[seg];
    float wf[seg];//Positiveの波形のデータ格納場所
    float wf_d[seg];//Positiveの波形(Dark)のデータ格納場所
    tr->SetBranchAddress("time",time_e);
    tr_drk->SetBranchAddress("time",time_d);
    tr->SetBranchAddress(branch_wf_s,wf);
    tr_drk->SetBranchAddress(branch_wf_d,wf_d);

    int int_wnd[2] = {t_min,t_max};//64-74 ns
    int ped_region[2] = {50,150};
    float chrg = 0;

    int nEve = tr->GetEntries();
    int nEve_ped = tr_drk->GetEntries();
    float ped = 0;
    float ped_drk = 0;
    float ave_dark[seg];

    //Darkの平均波形の作成
    for(int l=0;l<nEve_ped;l++)
    {
        ped_drk = 0;
        tr_drk->GetEntry(l);
        //Darkの平均的なPedestalを作成する
        for(int m=0;m<int_wnd[1];m++)
        {
            if (m>=ped_region[0]&&m<ped_region[1])
            {
                ped_drk += (wf_d[m])/(ped_region[1]-ped_region[0]);
            }
        }
        for(int m=0;m<int_wnd[1];m++)
        {
            if(l == 0)
            {
                ave_dark[m] = 0;
            }
            ave_dark[m] += (wf_d[m] - ped_drk)/nEve_ped;
        }
    }
    for(int i=0;i<nEve;i++)//イベント回数で回す
    {
      tr->GetEntry(i);
      //値の初期化
      chrg = 0;  
      ped = 0;
      //Pedestalの作成
      for(int j=0;j<int_wnd[1];j++)
        {
          if (j>=ped_region[0]&&j<ped_region[1])
          {
            ped += (wf[j])/(ped_region[1]-ped_region[0]);
          }
        }
      for(int j=int_wnd[0];j<int_wnd[1];j++)//積分時間
      {
        chrg += ((wf[j]- ave_dark[j] - ped)*(time_e[j]-time_e[j-1]));
      }
      chrg_list.push_back(chrg);
      //hst->Fill(chrg);
    }
}

void Calc_Chrg_Nodark(TString filesrc, 
                std::vector<float> &chrg_list, 
                TString treename = "Treesource_0", 
                TString branch_wf_s = "wform", 
                int t_min=320, int t_max=360, 
                int ped_min = 50, int ped_max = 150,
                int seg = 1024)
{
    TFile* f  = TFile::Open(filesrc);
    TTree* tr = (TTree*)f->Get(treename);
    float time_e[seg];
    float wf[seg];//Positiveの波形のデータ格納場所
    tr->SetBranchAddress("time",time_e);
    tr->SetBranchAddress(branch_wf_s,wf);

    int int_wnd[2] = {t_min,t_max};//64-74 ns
    int ped_region[2] = {ped_min,ped_max};
    float chrg = 0;

    int nEve = tr->GetEntries();
    float ped = 0;
    float ave_dark[seg];

    for(int i=0;i<nEve;i++)//イベント回数で回す
    {
      tr->GetEntry(i);
      //値の初期化
      chrg = 0;  
      ped = 0;
      //Pedestalの作成
      for(int j=0;j<int_wnd[1];j++)
        {
          if (j>=ped_region[0]&&j<ped_region[1])
          {
            ped += (wf[j])/(ped_region[1]-ped_region[0]);
          }
        }
      for(int j=int_wnd[0];j<int_wnd[1];j++)//積分時間
      {
        chrg += ((wf[j] - ped)*(time_e[j]-time_e[j-1]));
      }
      chrg_list.push_back(chrg);
      //hst->Fill(chrg);
    }
}

TH1F* Make_Charge_Hist(std::vector<float> &chrg, 
                        TString hist_Title,
                        int bins = 330, int hist_min = -20, int hist_max = 150)
{
    std::size_t list_size = chrg.size();
    TH1F* hst = new TH1F(hist_Title,hist_Title, bins, hist_min, hist_max);//Histgramの作成

    for(int i=0;i<list_size;i++)
    {
        hst->Fill(chrg[i]);
    }

    return hst;

}