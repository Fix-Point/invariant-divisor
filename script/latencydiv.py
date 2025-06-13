#!/bin/python3
# -*- coding: UTF-8
import re
import matplotlib.pyplot as plt
import os
import sys
import glob
import numpy as np
import seaborn as sns

sns.set_context("paper", font_scale=1.5)
plt.rcParams['xtick.direction'] = 'in'#将x的刻度线方向设置向内
plt.rcParams['ytick.direction'] = 'in'#将y的刻度方向设置向内
plt.rcParams['font.family']=['MiSans VF'] #用来正常显示中文标签

# 设置文件目录
filepath = sys.argv[1]

def extract_histogram_data(input_data):
    pattern = r'(\d+)\s\|\s(\d+)\b'
    matches = re.findall(pattern, input_data)
    histogram = []
    for match in matches:
        number = match[0]
        frequency = int(match[1])
        histogram.extend([int(number)] * frequency)

    return histogram

def plot_histograms_from_files(directory):
    all_histograms = []  # 保存所有文件的直方图数据
    all_labels = []  # 保存所有文件名
    file_names = []
    max_value = []
    min_value = []
    mean_value = []
    colors = plt.cm.tab10.colors  # 使用tab10默认颜色循环
    # print (directory + '/*_histogram.log')
    for i, filename in enumerate(sorted(glob.glob(directory + '/*_histogram.log'))):
        with open(filename, 'r') as f:
            input_data = f.read()
        histogram = extract_histogram_data(input_data)
        # print (histogram)
        all_histograms.extend(histogram)
        # 提取纯文件名（不包含路径和后缀名）
        file_name = os.path.splitext(os.path.basename(filename))[0]
        file_name = file_name[:-10]
        file_names.append(file_name)
        all_labels.extend([file_name] * len(histogram))

        # 计算最大值、平均值和最小值
        max_value.append(np.max(histogram))
        min_value.append(np.min(histogram))
        mean_value.append(np.median(histogram))

    fg = sns.catplot(y = all_labels,  x = all_histograms, kind="violin", inner=None, hue=all_labels, cut=0, height=5.27, aspect=11.7/5.27)

    fg.margin_titles=True
    for ax in fg.axes.flatten(): # Loop directly on the flattened axes
        for _, spine in ax.spines.items():
            spine.set_visible(True) # You have to first turn them on
            spine.set_color('black')
            spine.set_linewidth(1)

    # 获取箱线图的所有Axes对象
    for ax in fg.axes.flatten(): # Loop directly on the flattened axes
        for i, _ in enumerate(file_names):
            ax.annotate("平均 " + format(mean_value[i], ".0f"), xy=(mean_value[i],i), ha='right')
            ax.annotate("最小 " + format(min_value[i], ".0f"), xy=(min_value[i],i + 0.25), ha='right')
            ax.annotate("最大 " + format(max_value[i], ".0f"), xy=(max_value[i],i - 0.25), ha='right')

    plt.xticks()  # 旋转 x 轴标签，以免重叠
    plt.xlabel('Latency (us)')
    plt.xlim(-10, 128)
    plt.subplots_adjust(left=0.15, right = 0.995, bottom = 0.13, top = 0.95, wspace = 0.20 )
    plt.tight_layout()
    plt.legend(file_names, frameon=False, loc='upper right')
    plt.savefig("./div_violin.pdf")
    plt.savefig("./div_violin.png")
    plt.show()

    dg = sns.catplot(y = all_labels,  x=all_histograms, hue=all_labels, kind="box", whis=(0, 100), height=5.27, aspect=11.7/5.27, medianprops={"color": "r", "linewidth": 2})

    dg.margin_titles=True
    for ax in dg.axes.flatten(): # Loop directly on the flattened axes
        for _, spine in ax.spines.items():
            spine.set_visible(True) # You have to first turn them on
            spine.set_color('black')
            spine.set_linewidth(1)

    # 获取箱线图的所有Axes对象
    axes = dg.facet_axis(0, 0)
    for ax in dg.axes.flatten(): # Loop directly on the flattened axes
        for i, _ in enumerate(file_names):
            ax.annotate("平均 " + format(mean_value[i], ".0f"), xy=(mean_value[i],i), ha='right')
            ax.annotate("最小 " + format(min_value[i], ".0f"), xy=(min_value[i],i + 0.25), ha='right')
            ax.annotate("最大 " + format(max_value[i], ".0f"), xy=(max_value[i],i - 0.25), ha='right')

    plt.xticks()  # 旋转 x 轴标签，以免重叠
    plt.xlabel('延迟 (微秒)')
    # plt.xscale('log')
    plt.xlim(-10, 128)
    plt.tick_params(axis='y', which='both', labelleft=False, labelright=True)
    plt.subplots_adjust(left=0.15, right = 0.995, bottom = 0.13, top = 0.95, wspace = 0.20 )
    plt.tight_layout()
    plt.legend(file_names, frameon=False, loc='upper right')
    plt.savefig("./div_box.pdf")
    plt.savefig("./div_box.png")
    plt.show()

plot_histograms_from_files(filepath)