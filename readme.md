# v4
- 1.缓存cache和Timer文件已删除，修用redis缓存多次连接。使用redis连接池进行连接。

- 2.因和前端适应，去掉了服务器连接后主动发送welcome to server语句

- 3.使用client文件测试时，需修改MyTask.cc里内容，将process方法里的发送message改为发送response
同时TcpConnection.cc中需改为调用writeTrain方法

- 4.使用start.sh脚本启动redis和服务器，按ctrl+c结束服务器后，需要使用stop.sh脚本关闭redis。
