#!/bin/zsh

#########USAGE#########


echo "#####Convert ROOT FILE#####"
echo "YOUR INPUT FILE :" $1

#root 'Convert_ROOT_file.cxx("/Users/kiyomoto/reaserch/PMT_data/230627/AC1949/20230627_AC1949_AP0000.root", "Treesingle_0", "./hoge.root", "Treewform_0")'
root '/Users/kiyomoto/git/Script/C_macro/Convert_ROOT_file.cxx("'$1'", "'$2'", "hoge1.root", "'$2'")'
root '/Users/kiyomoto/git/Script/C_macro/Convert_ROOT_file.cxx("'$1'", "'$3'", "hoge1.root", "'$3'")'
hadd "./generate.root" "hoge1.root" "hoge2.root"
rm "hoge1.root"
rm "hoge2.root"

#ls -a