#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>

void CreateGraphFromBranch(TString filesrc, TString treename) {
    TFile *file = new TFile(filesrc, "READ");
    if (!file || file->IsZombie()) {
        cout << "Error: Could not open ROOT file." << endl;
        return;
    }

    TTree *tree = (TTree*)file->Get(treename);
    if (!tree) {
        cout << "Error: Could not find the specified tree." << endl;
        return;
    }

    float time[1024] = {0}; // x軸のデータを格納する変数
    float wfp[1024] = {0};
    float wfn[1024] = {0}; // y軸のデータを格納する変数

    tree->SetBranchAddress("time", time, "time[1024]/F"); // x軸のブランチ名を指定して変数にデータを読み取る
    tree->SetBranchAddress("wform3", wfp, "wform3[1024]/F"); // y軸のブランチ名を指定して変数にデータを読み取る
    tree->SetBranchAddress("wform2", wfn, "wform2[1024]/F"); // y軸のブランチ名を指定して変数にデータを読み取る

    Int_t nentries = tree->GetEntries();

    TGraph *graph = new TGraph(nentries); // TGraphオブジェクトを作成

    for (Int_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        graph->SetPoint(i, time,  wfp-wfn); // グラフにデータポイントを追加
    }

    TCanvas *canvas = new TCanvas("canvas", "Graph from Branch Data", 800, 600);
    graph->SetTitle("Graph from Branch Data");
    graph->GetXaxis()->SetTitle("X-axis Label");
    graph->GetYaxis()->SetTitle("Y-axis Label");
    graph->Draw("APL"); // グラフを描画

    canvas->SaveAs("branch_data_graph.png"); // グラフを画像ファイルに保存
}
