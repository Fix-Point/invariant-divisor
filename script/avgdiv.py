#!/bin/python3
# -*- coding: UTF-8

import sys
import os
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

import re

sns.set_context("paper", font_scale=1.5)
plt.rcParams['xtick.direction'] = 'in'#将x的刻度线方向设置向内
plt.rcParams['ytick.direction'] = 'in'#将y的刻度方向设置向内
plt.rcParams['font.family']=['MiSans VF'] #用来正常显示中文标签

# 设置文件目录
filepath = sys.argv[1]

labels = []
values = []

with open(filepath, 'r') as file:
    for line in file:
        if re.search("cycle/op", line):
            parts = line.split(': ')
            if len(parts) == 2:
                labels.append(parts[0])
                values.append(float(parts[1].split()[0]))

plt.figure(figsize=(9, 5))  # 设置画布大小

# 绘制条形图
for i in range(0, len(labels)):
    plt.bar(i, values[i], color=plt.cm.tab10(i), label = labels[i], alpha=0.618, capsize=5)
    plt.text(i, values[i], f'{values[i]:.3f}', ha='center', va='center')

x_pos = np.arange(len(labels))
plt.xticks(x_pos, labels, rotation=45)
plt.legend(loc='upper center', bbox_to_anchor=(0.46, 1.2), frameon=False, labelspacing=0.1, handletextpad=0.1,  ncol= len(labels))
plt.subplots_adjust(left=0.05, right = 0.995, bottom = 0.25, top = 0.88, wspace = 0.20 )
plt.ylabel('cycle/op')
plt.savefig("avgdiv.pdf")
plt.savefig("avgdiv.png")
# plt.show()