## HTTP 服务
`
HTTP超文本传输协议(应用层协议), HTML 超文本标记语言（a.html）
- HTTP 1.0          TCP(传输层)
- HTTP 2.0          TCP(传输层)
- HTTP 3.0          UDP(传输层)

1990 HTTP, HTML(Web)

超文本
- 超链接：graph 网
- 图片
- 音频
- 视频

PageRank算法：
- 超链接，投一票
- 每个页面权重不同

### HTTP 服务器
- apache: a pac
- nginx

### nginx

sudo apt install nginx

ps ef | grep nginx

service nginx status

service nginx stop/start

sudo netstat -tpnl | grep nginx

http://127.0.0.1:80

### HTTP 协议
- 浏览器（其他程序）：用户代理 request
- 服务器 执行响应 response

nginx 去 /var/www/html 查找文件

sudo chmod 777 a.txt

127.0.0.1/