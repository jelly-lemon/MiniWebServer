# JerryMouse

C++ 开发，windows 平台下迷你 web/http 后台。



# 功能描述
能够响应客户端 HTTP GET 静态资源请求，比如 *.html，图片等。



# 使用方法





# 性能测试

## 本机配置

电脑配置

操作系统：Windows 10 

CPU：i7-7700 3.60GHz

内存：8.00 GB (2667 MHz)

主硬盘：2000 GB (希捷 ST2000DM006)



虚拟机配置

操作系统：Centos7 

CPU：

内存：



测压软件

Webbench 1.5



## 对比对象

使用 Apache/httpd.exe 作为对比的对象。














# 开发环境
IDE：CLion

编译器：mingw64-x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0

开发平台：Windows 10



# 技术特点
- 异步日志：
- 线程池：
- select/poll/epoll：IO 多路复用
- IOCP：异步 IO
- 并发模型：



# TODO

- [ ] URL base64 解析



# 遇到的问题

Q: TCP 三次握手完成后，客户端迟迟不发送请求，那服务端保持连接多久呢？如果立即释放连接，说不定客户端请求已经在路上了。如果不释放，服务端又一直干等着，就会浪费内存资源。

A:



Q: 类文件相互 include?

A: 这种类依赖关系是不好的，应避免这种设计。正确的类依赖关系应该是像一颗树那样，具有层次关系。



Q: 多线程环境下，控制台打印混乱，输出被截断？

A: 加一个互斥锁。但是，可能先 printf 和后 printf 同时阻塞，但后 print 的得到锁，然后输出，导致次序混乱。

可以开一个线程，把打印内容放到队列里面。但是，遇到异常时最后一条内容还没打印程序就退出了。

自己再写一个安全退出函数，判断队列是否为空，为空才退出，否则等待。



Q: 经常忘记 delete，导致内存泄露？

A: 



Q: exit 导致最后一条日志没有输出？

A: exit 太快，要等待日志写入完成。可以判断日志队列是否为空，为空才退出，否则等待日志线程写完。



Q: 多线程环境很难调试？

A:



Q：如何获取实际 backlog 的大小？

A：无法获取



Q: 队列日志，读一条消息，就打开、关闭文件，要是有大量日志，会不会造成写入缓慢？

A: 



Q: 类 B 回调类 A 的成员函数？

A:



Q: 在大量短连接且建立连接就立即发送数据的情况下，IO 多路复用对并发处理能力有帮助吗？

A: 



Q: 当服务端空闲时，把线程挂起还是销毁？

A: 