#!/bin/python3
# -*- coding: UTF-8
import matplotlib.pyplot as plt
import os
import sys
import pandas as pd
import seaborn as sns

sns.set_context("paper", font_scale=1.5)
plt.rcParams['xtick.direction'] = 'in'#将x的刻度线方向设置向内
plt.rcParams['ytick.direction'] = 'in'#将y的刻度方向设置向内
plt.rcParams['font.family']=['MiSans VF'] #用来正常显示中文标签

# 设置文件目录
filepath = sys.argv[1]
