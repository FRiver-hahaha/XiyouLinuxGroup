客户端错误日志

已连接到 FTP 服务器 127.0.0.1:2100
FTP 服务器就绪
登录成功
登录成功

进入 FTP 交互模式

FTP 支持命令:
  list              - 列出远程文件
  get <filename>    - 下载文件
  put <filename>    - 上传文件
  pasv              - 进入被动模式
  quit              - 退出

ftp> help
命令未实现：HELP
ftp> HELP
命令未实现：HELP
ftp> HELP
命令未实现：HELP
ftp> LIST
准备发送目录列表
无法打开数据连接
ftp> LIST
准备发送目录列表
无法打开数据连接
ftp> PASV
227 entering passive mode (10,30,0,158,19,143)
ftp> LIST
准备发送目录列表
ftp> LIST
准备发送目录列表
ftp> 


服务端错误日志
[22:21:21] (submit)任务已加入队列
[22:21:21] 接收到 FTP 命令：PASV
[22:21:21] PASV 响应：227 entering passive mode (10,30,0,158,19,140)

[22:21:25] (submit)任务已加入队列
[22:21:25] 接收到 FTP 命令：PASV
[22:21:25] PASV 响应：227 entering passive mode (10,30,0,158,19,141)

[22:21:30] (submit)任务已加入队列
[22:21:30] 接收到 FTP 命令：ls
[22:21:43] (submit)任务已加入队列
[22:21:43] 接收到 FTP 命令：pwd
[22:21:44] (submit)任务已加入队列
[22:21:44] 接收到 FTP 命令：pwd
[22:22:07] (submit)任务已加入队列
[22:22:07] 接收到 FTP 命令：PASV
[22:22:07] PASV 响应：227 entering passive mode (10,30,0,158,19,142)

[22:24:34] 新客户端连接：127.0.0.1:39216
[22:24:34] (submit)任务已加入队列
[22:24:34] 接收到 FTP 命令：USER anonymous
[22:24:34] (submit)任务已加入队列
[22:24:34] 接收到 FTP 命令：PASS anonymous@
[22:24:38] (submit)任务已加入队列
[22:24:38] 接收到 FTP 命令：help
[22:24:39] (submit)任务已加入队列
[22:24:39] 接收到 FTP 命令：HELP
[22:24:40] (submit)任务已加入队列
[22:24:40] 接收到 FTP 命令：HELP
[22:24:48] (submit)任务已加入队列
[22:24:48] 接收到 FTP 命令：LIST
[22:24:51] (submit)任务已加入队列
[22:24:51] 接收到 FTP 命令：LIST
[22:24:52] (submit)任务已加入队列
[22:24:52] 接收到 FTP 命令：PASV
[22:24:52] PASV 响应：227 entering passive mode (10,30,0,158,19,143)

[22:24:55] (submit)任务已加入队列
[22:24:55] 接收到 FTP 命令：LIST
[22:24:58] (submit)任务已加入队列
[22:24:58] 接收到 FTP 命令：LIST
[22:25:52] (submit)任务已加入队列
[22:25:52] 接收到 FTP 命令：LIST



告诉我为什么无法list，而且我这个无法切换目录，同样我也不知道如何上传下载文件，给我一个符合人性化的操作，并且修改我的代码，按照之前的规则，需要被删除的每行做好注释，修改的部分做好批注