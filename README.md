<h1> 项目企划</h1>
---
 - 基本功能实现 （9.16 - 9.17）9-19 完成
    - epoll 接口
    - TCP 套接字
    - 线程池 
    - 时间控制器
 - 测试
    - 远程并发测试  人为测试 OK

 - 9-19 发现bug， 9-24 修改完成， （中间存在三天假期）
 
 - HTTP 协议解析（使用开源代码）
    - 解析请求
    - 解析响应

- 9-26 已经上传到github， 版本号 0.2.1

---

注释规范：
 - NOTE： 强烈说明
 - TODO： 需要增加的功能
 - WARN： 可能出现问题
 - WATCH： 重点阅读
 - WAIT: 临时使用的代码，期待更新

```
代码结构：
class SocketServer:  每一个对象是一个监听者
      listen


class Epoll: SocketServer&
      add 增加套接字监控
      mod 修改套接字监控
      wait: 等待事件
            get_new_connection 事件C：得到新的连接
            handle_request  事件H：处理请求（线程+请求对象）
      expired: 过滤超时连接（时间器）

class Request: Timer*
      handle 处理请求
      updateTimer  更新时间器

      
class Timer: Request *
      Queue 时间器队列
      sperate 分离时间器和请求对象
      expired 过滤超时间器


class Pthreadpool
      ThreadPool  线程池
      TaskQueue  任务队列
      PthreadRun  绑定函数
      addTask     增加任务
      destory     销毁
```