# LanPlayer
局域网里不同电脑同时播放一首歌，产生剧院效果，顺便熟悉一下tcp/ip协议。

## 源起
2014年，大学二年级，和室友happy steve在听音乐，突然我们有个想法。
想把我们的电脑连起来同时播放一首歌。于是，我们做了这个软件。
谨以此纪念大二岁月。

## 要求
1. 能传输mp3文件
2. 误差不超过50ms，这样人耳就分辨不出了
3. 自动校正误差（未实现）

## 用到的资料（VS2012自带）
1. ws2_32.lib用来Socket的Windows编程
2. Windows下API中CreateThread用来线程
3. vfw.lib用来播放mp3

## 使用
1. 下载本项目，不需要其他文件
2. 在VS2012中安装Git Source Control Provider插件
[下载地址](http://gitscc.codeplex.com/)
3. 点开LanPlayer.sln即可。

#### 作者 talengu / happy steve

##### `希望大家喜欢，欢迎提出建议。2016年12月30日`
