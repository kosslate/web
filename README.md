# web 
## Introduction　<br>
  本项目编写的Web服务器，解析了get、head请求，可处理静态资源，支持HTTP长连接，利用buffer读取数据，并实现了异步日志，记录服务器运行状态。<br>
## Envoirment　<br>
  OS: Ubuntu 14.04 <br>
  Complier: g++ 4.8 <br>
## Usage <br>
  ./server <br>
## Technical points <br>
  改进了输入输出buff，利用muduo中的vector<char>模式　<br>
## 参考
muduo <br>
