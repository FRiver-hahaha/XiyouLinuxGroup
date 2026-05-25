# FTP 服务器架构图

## 一、业务逻辑图

### 1.1 系统整体架构

```mermaid
graph TB
    subgraph Client["客户端 (Client)"]
        C1[微壳交互界面]
        C2[命令解析器]
        C3[控制连接]
        C4[数据连接]
    end
    
    subgraph Server["服务器端 (Server)"]
        S1[Epoll 事件循环]
        S2[命令处理器]
        S3[会话管理]
        S4[线程池]
        S5[文件系统]
    end
    
    subgraph Network["网络层"]
        N1[控制端口 2100]
        N2[数据端口 2000]
        N3[PASV 端口 5000-5100]
    end
    
    C1 --> C2
    C2 --> C3
    C3 --> N1
    C4 --> N2
    C4 --> N3
    
    N1 --> S1
    N2 --> S1
    N3 --> S1
    
    S1 --> S2
    S1 --> S4
    S2 --> S3
    S3 --> S5
    
    style Client fill:#e1f5ff
    style Server fill:#fff4e1
    style Network fill:#f0f0f0
```

### 1.2 核心业务流程

```mermaid
sequenceDiagram
    participant User as 用户
    participant Client as FTP 客户端
    participant Control as 控制连接 (2100)
    participant Server as FTP 服务器
    participant Data as 数据连接 (PASV)
    participant FS as 文件系统
    
    User->>Client: 启动客户端
    Client->>Control: 连接服务器
    Server-->>Client: 220 服务器就绪
    Client->>Control: USER anonymous
    Server-->>Client: 230 登录成功
    Client->>Control: PASS anonymous@
    Server-->>Client: 230 登录成功
    
    Note over Client,Server: 文件列表操作
    Client->>Control: PASV
    Server-->>Client: 227 (h1,h2,h3,h4,p1,p2)
    Client->>Data: 连接 PASV 端口
    Client->>Control: LIST
    Server->>FS: 读取目录
    FS-->>Server: 目录内容
    Server->>Data: 发送目录列表
    Data-->>Client: 接收列表
    Server-->>Client: 226 传输完成
    
    Note over Client,Server: 文件下载操作
    Client->>Control: PASV
    Server-->>Client: 227 (端口信息)
    Client->>Data: 连接 PASV 端口
    Client->>Control: RETR filename
    Server->>FS: 读取文件
    FS-->>Server: 文件内容
    Server->>Data: 发送文件
    Data-->>Client: 保存文件
    Server-->>Client: 226 传输完成
    
    Note over Client,Server: 目录切换操作
    Client->>Control: CWD /path
    Server->>FS: 检查目录
    FS-->>Server: 目录存在
    Server->>Server: 更新 workingDir
    Server-->>Client: 250 目录已更改
    
    User->>Client: quit
    Client->>Control: QUIT
    Server-->>Client: 221 再见
```

### 1.3 会话管理流程

```mermaid
stateDiagram-v2
    [*] --> 等待连接
    等待连接 --> 已连接：接受客户端连接
    已连接 --> 已认证：USER/PASS 成功
    已认证 --> 被动模式：PASV 命令
    被动模式 --> 数据传输：LIST/RETR/STOR
    数据传输 --> 已认证：传输完成
    已认证 --> 已切换目录：CWD/CDUP
    已切换目录 --> 已认证：操作完成
    已认证 --> [*] : QUIT
    已连接 --> [*] : 连接断开
    已认证 --> [*] : 超时/错误
```

### 1.4 被动模式 (PASV) 工作流程

```mermaid
flowchart TD
    A[客户端发送 PASV] --> B[服务器分配端口]
    B --> C[保存 pasvPort 到 session]
    C --> D[返回 227 响应<br/>h1,h2,h3,h4,p1,p2]
    D --> E[客户端解析端口<br/>port = p1*256 + p2]
    E --> F[客户端连接数据端口]
    F --> G[服务器绑定相同端口监听]
    G --> H[接受数据连接]
    H --> I[进行数据传输]
    I --> J[关闭数据连接]
    J --> K[释放端口]
    
    style B fill:#ffe4e1
    style C fill:#ffe4e1
    style G fill:#ffe4e1
    style K fill:#e1ffe1
```

---

## 二、代码流程图

### 2.1 服务器启动流程

```mermaid
flowchart TD
    Start([main 函数]) --> CreateServer[创建 Server 对象<br/>controlPort=2100<br/>dataPort=2000]
    CreateServer --> CallStart[调用 server.start()]
    
    CallStart --> CreateControlSock[创建控制套接字]
    CreateControlSock --> BindControl[绑定控制端口 2100]
    BindControl --> ListenControl[监听控制端口]
    
    ListenControl --> CreateDataSock[创建数据套接字]
    CreateDataSock --> BindData[绑定数据端口 2000]
    BindData --> ListenData[监听数据端口]
    
    ListenData --> CreateEpoll[创建 epoll 实例]
    CreateEpoll --> AddControlToEpoll[添加控制套接字到 epoll]
    AddControlToEpoll --> PrintInfo[打印服务器信息]
    PrintInfo --> ReturnTrue[返回 true]
    
    ReturnTrue --> CallRun[调用 server.run()]
    CallRun --> EpollWait[epoll_wait 循环]
    
    style CreateServer fill:#e1f5ff
    style CallStart fill:#e1f5ff
    style CallRun fill:#e1f5ff
    style EpollWait fill:#fff4e1
```

### 2.2 Epoll 事件处理流程

```mermaid
flowchart TD
    EpollWait[epoll_wait 等待事件] --> HasEvent{有事件？}
    HasEvent -->|否 | CheckRunning{running?}
    CheckRunning -->|是 | EpollWait
    CheckRunning -->|否 | Cleanup[清理资源]
    
    HasEvent -->|是 | CheckFd{fd 类型？}
    
    CheckFd -->|controlServerSocket| AcceptConn[accept 新连接]
    AcceptConn --> CreateSession[创建 FtpSession]
    CreateSession --> SaveSession[保存到 sessions 映射]
    SaveSession --> SendWelcome[发送欢迎消息 220]
    SendWelcome --> AddToEpoll[添加到 epoll 监听]
    AddToEpoll --> EpollWait
    
    CheckFd -->|clientSocket| SubmitTask[提交到线程池]
    SubmitTask --> HandleData[handleClientData]
    HandleData --> EpollWait
    
    Cleanup --> CloseEpoll[关闭 epoll]
    CloseEpoll --> CloseSockets[关闭套接字]
    CloseSockets --> End([结束])
    
    style AcceptConn fill:#e1f5ff
    style CreateSession fill:#e1f5ff
    style SubmitTask fill:#fff4e1
    style HandleData fill:#fff4e1
```

### 2.3 命令处理流程

```mermaid
flowchart TD
    RecvData[recv 接收数据] --> ParseCmd[解析命令和参数]
    ParseCmd --> ToUpper[转换为大写]
    ToUpper --> CheckCmd{命令类型？}
    
    CheckCmd -->|PASV| HandlePasv[handlePasvCommand]
    CheckCmd -->|LIST| HandleList[handleListCommand]
    CheckCmd -->|RETR| HandleRetr[handleRetrCommand]
    CheckCmd -->|STOR| HandleStor[handleStorCommand]
    CheckCmd -->|PWD| HandlePwd[返回 workingDir]
    CheckCmd -->|CWD| HandleCwd[handleCwdCommand]
    CheckCmd -->|CDUP| HandleCdup[handleCdupCommand]
    CheckCmd -->|QUIT| HandleQuit[移除会话]
    CheckCmd -->|USER/PASS| HandleAuth[返回 230]
    CheckCmd -->|HELP| HandleHelp[返回帮助信息]
    CheckCmd -->|其他| HandleUnknown[返回 502 错误]
    
    HandlePasv --> SendResp[发送响应]
    HandleList --> SendResp
    HandleRetr --> SendResp
    HandleStor --> SendResp
    HandlePwd --> SendResp
    HandleCwd --> SendResp
    HandleCdup --> SendResp
    HandleQuit --> SendResp
    HandleAuth --> SendResp
    HandleHelp --> SendResp
    HandleUnknown --> SendResp
    
    SendResp --> End([结束])
    
    style CheckCmd fill:#ffe4e1
    style HandlePasv fill:#e1f5ff
    style HandleList fill:#e1f5ff
    style HandleRetr fill:#e1f5ff
    style HandleStor fill:#e1f5ff
    style HandleCwd fill:#e1f5ff
```

### 2.4 LIST 命令详细流程

```mermaid
sequenceDiagram
    participant C as 客户端
    participant S as 服务器
    participant Session as Session 对象
    participant FS as 文件系统
    
    C->>S: PASV
    S->>Session: 保存 pasvPort
    S-->>C: 227 (10,30,0,158,19,143)
    
    C->>S: 连接到 PASV 端口 5007
    
    C->>S: LIST
    S->>Session: 读取 pasvPort
    S->>S: 创建监听套接字<br/>绑定 pasvPort
    S->>S: accept 数据连接
    S-->>C: 150 准备发送目录列表
    
    S->>FS: opendir + readdir
    FS-->>S: 目录条目
    
    S->>S: 格式化目录列表
    S->>数据连接：发送目录内容
    S->>S: 关闭数据连接
    
    S-->>C: 226 传输完成
    
    Note over S: 关键修复：使用 session<br/>保存的 pasvPort，<br/>而不是重新分配
```

### 2.5 CWD 命令详细流程

```mermaid
flowchart TD
    Start[CWD 命令] --> CheckEmpty{路径为空？}
    CheckEmpty -->|是 | Err501[返回 501 错误]
    CheckEmpty -->|否 | GetSession[获取 Session]
    
    GetSession --> CheckPath{路径类型？}
    CheckPath -->|绝对路径 | SetNewDir[newDir = path]
    CheckPath -->|相对路径 | CombineDir[newDir = workingDir + path]
    
    SetNewDir --> Normalize[规范化路径]
    CombineDir --> Normalize
    
    Normalize --> ParseParts[按/分割路径]
    ParseParts --> ProcessParts{处理每个部分}
    
    ProcessParts -->|空或.| 跳过
    ProcessParts -->|..| PopBack[弹出最后一个部分]
    ProcessParts -->|正常目录 | PushBack[添加到 parts]
    
    PopBack --> Rebuild[重建路径]
    PushBack --> Rebuild
    跳过 --> Rebuild
    
    Rebuild --> CheckExist{opendir 检查}
    CheckExist -->|不存在 | Err550[返回 550 错误]
    CheckExist -->|存在 | UpdateSession[更新 session.workingDir]
    
    UpdateSession --> Success[返回 250 成功]
    Err501 --> End([结束])
    Err550 --> End
    Success --> End
    
    style CheckEmpty fill:#ffe4e1
    style CheckPath fill:#ffe4e1
    style CheckExist fill:#ffe4e1
    style UpdateSession fill:#e1ffe1
    style Success fill:#e1ffe1
```

### 2.6 文件下载 (RETR) 流程

```mermaid
flowchart TD
    Start[RETR filename] --> CheckEmpty{文件名空？}
    CheckEmpty -->|是 | Err501[501 错误]
    CheckEmpty -->|否 | GetSession[获取 Session]
    
    GetSession --> BuildPath[构建文件路径<br/>workingDir + filename]
    BuildPath --> ReadFile[readFile 读取文件]
    ReadFile --> CheckContent{文件存在？}
    
    CheckContent -->|否 | Err550[550 文件不存在]
    CheckContent -->|是 | CreateDataSock[创建数据连接]
    
    CreateDataSock --> UsePasvPort[使用 session.pasvPort]
    UsePasvPort --> BindListen[绑定并监听]
    BindListen --> Accept[accept 客户端连接]
    
    Accept --> Send150[发送 150 响应<br/>带文件大小]
    Send150 --> SendData[sendDataToClient]
    SendData --> CloseData[关闭数据连接]
    CloseData --> Send226[发送 226 完成]
    
    Err501 --> End([结束])
    Err550 --> End
    Send226 --> End
    
    style CheckEmpty fill:#ffe4e1
    style CheckContent fill:#ffe4e1
    style UsePasvPort fill:#e1f5ff
    style Send150 fill:#e1ffe1
    style Send226 fill:#e1ffe1
```

### 2.7 客户端交互流程

```mermaid
flowchart TD
    Start([启动客户端]) --> Connect[connectFtpServer]
    Connect --> RecvWelcome[接收欢迎消息]
    RecvWelcome --> SendUser[发送 USER anonymous]
    SendUser --> SendPass[发送 PASS anonymous@]
    
    SendPass --> ShowMenu[显示命令菜单]
    ShowMenu --> InputCmd[输入命令]
    
    InputCmd --> ParseCmd{解析命令}
    
    ParseCmd -->|quit/exit| SendQuit[发送 QUIT]
    ParseCmd -->|list/ls| CallList[调用 listFiles]
    ParseCmd -->|get | CallDownload[调用 downloadFile]
    ParseCmd -->|put | CallUpload[调用 uploadFile]
    ParseCmd -->|cd | SendCwd[发送 CWD]
    ParseCmd -->|cdup| SendCdup[发送 CDUP]
    ParseCmd -->|pwd| SendPwd[发送 PWD]
    ParseCmd -->|help| SendHelp[发送 HELP]
    ParseCmd -->|pasv| CallPasv[调用 pasvMode]
    ParseCmd -->|其他| SendRaw[直接发送 FTP 命令]
    
    SendQuit --> End([断开连接])
    CallList --> ShowResult[显示结果]
    CallDownload --> ShowResult
    CallUpload --> ShowResult
    SendCwd --> ShowResponse[显示响应]
    SendCdup --> ShowResponse
    SendPwd --> ShowResponse
    SendHelp --> ShowResponse
    CallPasv --> ShowResponse
    SendRaw --> ShowResponse
    
    ShowResult --> InputCmd
    ShowResponse --> InputCmd
    
    style Connect fill:#e1f5ff
    style CallList fill:#e1f5ff
    style CallDownload fill:#e1f5ff
    style CallUpload fill:#e1f5ff
```

---

## 三、数据结构图

### 3.1 FtpSession 结构

```mermaid
classDiagram
    class FtpSession {
        +int controlSock
        +int dataSock
        +int pasvPort
        +string clientIp
        +string workingDir
        +bool pasvMode
    }
    
    note for FtpSession "会话状态对象\n保存每个客户端的连接状态"
```

### 3.2 Server 类结构

```mermaid
classDiagram
    class Server {
        -int controlServerSocket
        -int dataServerSocket
        -int epollFd
        -int controlPort
        -int dataPort
        -atomic~bool~ running
        -unique_ptr~ThreadPool~ threadPool
        -mutex sessionMutex
        -mutex dataPortMutex
        -unordered_map~int, FtpSession*~ sessions
        -string serverIp
        -int nextPasvPort
        
        +Server(int, int)
        +bool start()
        +void run()
        +void stop()
        +void handleClientData(int)
        +void handleFtpCommand(int, string)
        +void handlePasvCommand(int)
        +void handleListCommand(int)
        +void handleRetrCommand(int, string)
        +void handleStorCommand(int, string)
        +void handleCwdCommand(int, string)
        +void handleCdupCommand(int)
        +void sendDataToClient(int, string)
        +string receiveDataFromClient(int, size_t)
        +string listDirectory(string)
        +bool saveFile(string, string)
        +string readFile(string)
        +void removeSession(int)
        +string getServerIp()
        +string getWorkingDir(int)
        +int allocatePasvPort()
        +void releasePasvPort(int)
        +void printServerInfo()
        +int getSessionCount()
    }
    
    class ThreadPool {
        +ThreadPool(int)
        +void submit(task, callback)
    }
    
    Server --> ThreadPool : 使用
    Server --> FtpSession : 管理多个
```

### 3.3 Client 类结构

```mermaid
classDiagram
    class Client {
        -int clientSocket
        -int serverPort
        -string serverAddress
        -int dataSocket
        -int pasvPort
        -string pasvHost
        
        +Client(string, int)
        +bool connectServer()
        +bool connectFtpServer()
        +void receiveResponse()
        +string sendCommand(string)
        +string sendFtpCommand(string)
        +bool pasvMode()
        +string listFiles()
        +bool downloadFile(string)
        +bool uploadFile(string)
        +void closeDataConnection()
        +void disconnect()
        +void microShell()
    }
```

---

## 四、端口使用图

```mermaid
graph LR
    subgraph Ports["端口分配"]
        P1[控制端口 2100<br/>TCP 监听]
        P2[主动数据端口 2000<br/>TCP 监听]
        P3[PASV 端口池 5000-5100<br/>动态分配]
    end
    
    subgraph Usage["使用场景"]
        U1[FTP 命令传输]
        U2[主动模式数据]
        U3[被动模式数据]
    end
    
    P1 --> U1
    P2 --> U2
    P3 --> U3
    
    style P1 fill:#e1f5ff
    style P2 fill:#fff4e1
    style P3 fill:#e1ffe1
```

---

## 五、关键修复对比

### 5.1 LIST 命令修复前后对比

```mermaid
flowchart TD
    subgraph Before["修复前 ❌"]
        B1[客户端 PASV] --> B2[服务器分配端口 5001]
        B2 --> B3[客户端 LIST]
        B3 --> B4[服务器重新分配 5002]
        B4 --> B5[客户端连接 5001]
        B5 --> B6[服务器监听 5002]
        B6 --> B7[连接失败！]
    end
    
    subgraph After["修复后 ✅"]
        A1[客户端 PASV] --> A2[服务器分配 5001<br/>保存到 session]
        A2 --> A3[客户端 LIST]
        A3 --> A4[服务器使用 session.pasvPort 5001]
        A4 --> A5[客户端连接 5001]
        A5 --> A6[服务器监听 5001]
        A6 --> A7[连接成功！]
    end
    
    style B7 fill:#ffe4e1
    style A7 fill:#e1ffe1
```

### 5.2 CWD 命令修复前后对比

```mermaid
flowchart TD
    subgraph Before["修复前 ❌"]
        B1[客户端 CWD /bin] --> B2[服务器返回<br/>"目录已更改"]
        B2 --> B3[workingDir 未更新]
        B3 --> B4[后续操作仍在根目录]
    end
    
    subgraph After["修复后 ✅"]
        A1[客户端 CWD /bin] --> A2[解析路径]
        A2 --> A3[规范化路径]
        A3 --> A4[opendir 检查]
        A4 --> A5[更新 workingDir]
        A5 --> A6[返回 250 成功]
        A6 --> A7[后续操作在/bin 目录]
    end
    
    style B4 fill:#ffe4e1
    style A7 fill:#e1ffe1
```

---

## 六、响应码对照表

| 响应码 | 含义 | 使用场景 |
|--------|------|----------|
| 150 | 准备发送/接收 | LIST/RETR/STOR 开始前 |
| 200 | 命令成功 | TYPE 命令 |
| 211 | 系统状态 | FEAT 命令 |
| 214 | 帮助信息 | HELP 命令 |
| 215 | 系统类型 | SYST 命令 |
| 220 | 服务就绪 | 连接建立时 |
| 221 | 服务关闭 | QUIT 响应 |
| 226 | 传输完成 | LIST/RETR/STOR 完成 |
| 227 | 进入被动模式 | PASV 响应 |
| 230 | 登录成功 | USER/PASS 响应 |
| 250 | 请求操作成功 | CWD/CDUP 响应 |
| 257 | 路径名创建 | PWD 响应 |
| 421 | 服务不可用 | 服务器关闭 |
| 425 | 无法打开数据连接 | 数据连接失败 |
| 500 | 语法错误 | 空命令 |
| 501 | 参数错误 | 缺少参数 |
| 502 | 命令未实现 | 未知命令 |
| 550 | 文件/目录不可用 | 文件不存在 |
