# 哈工大操作系统课程及实验完结体会以及经验 , 以及开箱即用的虚拟机实验环境

## 先导篇 : 做实验之前你要看的

课程学习需要 汇编语言 , C 语言  

​	汇编我是现学的 , 前面几个实验要用到

C 语言 , 这个不用说 

​	⚠️ **注意 , 字符串数组要加 '\0'** 

C的库函数要能正确使用 , 在网上找好的一个 C 函数库参考网站

​	❌ 这里点名批评菜鸟教程

linux 下的系统调用函数也要找个好网站查询

​	⚠️ 不要强行使用 man 手册, 和中国人八字不合

linux 基础常用的几个命令, ls , cd , vi , cp , mv ...

​	🉑️ 强烈建议 vscode 代替 vi , 直接打开文件夹 , 搜索功能很好用

​	或者使用其他好用的编辑器, 甚至ide , 有些时候不是因为写不出而出 bug , 远古版本的 gcc 真的🗑

实验指导书一定要认真看 , 没有一句废话 🙁🙁🙁
指导书推荐的<<linux 内核注释>>也很有用

出各种各样的 bug  🐛 🕷 🦟 🐞 非常正常, 要多用搜索引擎🔍

## 🎉🎉🎉大礼包 : 虚拟机实验环境

### 💿 : [百度网盘](https://pan.baidu.com/s/1kCq6bzSC0ckGVBP54sGiyw) 

链接: https://pan.baidu.com/s/1kCq6bzSC0ckGVBP54sGiyw  
密码: 7doh

**解压缩的时候全部放到同一个文件夹 , 用 7-zip 解压 001 压缩文件**

**Ubuntu 系统密码 : kevin**

> 这是我自己用的虚拟机环境, Ubuntu 16.04 32位 , 已经配置好了
>
> VMware player 个人免费 , 用它直接打开虚拟机就可以做实验 , 还有我做实验的代码来对照
>
> 想自己配环境的自然是可以的 , 网上都有, 但是 64 位系统会比较难用 , 要下载 32位的库, 太麻烦, 直接用 32 位就好了 , 不要让 🗑 破坏学习热情
>

### 这是实验指导书和我做的笔记 

📖 : [onenote 笔记pdf](https://github.com/Kevin-Kevin/hit-operatingSystem/blob/master/resource/hit-%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F%E5%AE%9E%E9%AA%8C%E6%8C%87%E5%AF%BC%E4%B9%A6.pdf)  ( 如果打不开就看蓝桥的实验指导书也可以)

CSDN : [操作系统博客专栏](https://blog.csdn.net/weixin_43987915/category_10331305.html?spm=1001.2014.3001.5482)

后续会再复习一遍操作系统 , 做一下课程笔记

## 🔗[仓库](https://github.com/Kevin-Kevin/hit-operatingSystem)使用指南

哈工大操作系统试验（蓝桥）

- 相关资料在 [resource](https://github.com/Kevin-Kevin/hit-operatingSystem/tree/master/resource) 里面
  - 一些脚本文件是用于实验结果的测试
  - hit-oslab-linux-20110823.tar.gz 是 oslab 实验平台压缩包
  - gcc-3.4 用于编译 linux-0.11
  - pdf 是相关书籍
  - 哈工大的操作系统课程有李治军老师自己出的一本书《操作系统原理、实现与实践》，我是淘宝买了这本书方便学习
  - 实验指导书是从蓝桥上面复制的, 后面是我做实验时候的一些思路以及一些问题的答案
- 实验指导书（蓝桥）：https://www.lanqiao.cn/mobile/courses/115/learning?id=569
- 哈工大师兄实验指导书：https://hoverwinter.gitbooks.io/hit-oslab-manual/content/index.html
- MOOC 课程（B站也有）：https://www.icourse163.org/learn/HIT-1002531008?tid=1450346461#/learn/content
