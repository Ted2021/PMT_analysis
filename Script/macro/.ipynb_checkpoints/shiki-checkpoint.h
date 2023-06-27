#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"
#include "iostream"
#include "vector"

void Shki(std::vector<int> &e, std::vector<int> &s, std::vector<float> &w, std::vector<float> &sa, std::vector<int> &par, std::vector<int> &par2, std::vector<float> &par3, std::vector<float> &par4, std::vector<int> &par5, int thre_num)
{
    std::size_t list_size = e.size();
    //int for_num = list_size - 1;
    //std::cout << "list size: " << for_num << std::endl;
    bool Flag = false;
    int Count = 0;
    std::vector<int> event;
    std::vector<int> seg;
    std::vector<float> wave;
    std::vector<float> sabun;

    for(int i=0 ; i<list_size - 1; i++)
    {
        if(e.at(i+1) == e.at(i) && s.at(i+1) - s.at(i) ==  1)
        {
            Flag = true;
            Count += 1;
            //std::cout << "Count + 1! " << std::endl;
            event.push_back(e.at(i));
            seg.push_back(s.at(i));
            wave.push_back(w.at(i));
            sabun.push_back(sa.at(i));

        } else
        {
            Flag = false;
        }

        if(Flag == false)
        {
            if(Count >= thre_num - 1)
            {
                std::vector<float>::iterator iter = std::max_element(wave.begin(), wave.end());
                size_t index = std::distance(wave.begin(), iter);
                par.push_back(event.at(index));
                par2.push_back(seg.at(index));
                par3.push_back(wave.at(index));
                par4.push_back(sabun.at(index));
                par5.push_back(Count);
                //std::cout << "Push out!" << std::endl;
                //std::cout << "max dist: " << dist[index] << " index: " << index << std::endl;
            }
            Count = 0;
            /*
            event.clear();
            seg.clear();
            wave.clear();
            sabun.clear();
            */
            event.erase(event.begin(), event.end());
            seg.erase(seg.begin(), seg.end());
            wave.erase(wave.begin(), wave.end());
            sabun.erase(sabun.begin(), sabun.end());
        }
    }

}