#!/bin/zsh
python3 stage.py -d ../dco_re_100 -o output/100.txt
python3 stage.py -d ../dco_re_200 -o output/200.txt
python3 stage.py -d ../dco_re_300 -o output/300.txt
python3 stage.py -d ../dco_re_400 -o output/400.txt
python3 plot_stage.py
