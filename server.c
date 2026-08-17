// server.c
// 对浏览器提供响应
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

char *type(char *p)
{
    // 文本文档
    if (strcmp(p, ".html") == 0)
        return "text/html";
    if (strcmp(p, ".css") == 0)
        return "text/css";
    if (strcmp(p, ".js") == 0 || strcmp(p, ".mjs") == 0)
        return "text/javascript";
    if (strcmp(p, ".txt") == 0 || strcmp(p, ".text") == 0)
        return "text/plain";
    if (strcmp(p, ".csv") == 0)
        return "text/csv";
    if (strcmp(p, ".doc") == 0)
        return "application/msword";
    if (strcmp(p, ".docx") == 0)
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    if (strcmp(p, ".xls") == 0)
        return "application/vnd.ms-excel";
    if (strcmp(p, ".xlsx") == 0)
        return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    if (strcmp(p, ".ppt") == 0)
        return "application/vnd.ms-powerpoint";
    if (strcmp(p, ".pptx") == 0)
        return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    if (strcmp(p, ".pdf") == 0)
        return "application.pdf";
    if (strcmp(p, ".rtf") == 0)
        return "application.rtf";
    // 图片与图形
    if (strcmp(p, ".gif") == 0)
        return "image/gif";
    if (strcmp(p, ".ico") == 0)
        return "image.vnd.microsoft.icon";
    if (strcmp(p, ".jpeg") == 0 || strcmp(p, ".jpg") == 0)
        return "image/jpeg";
    if (strcmp(p, ".png") == 0)
        return "image/png";
    if (strcmp(p, ".bmp") == 0)
        return "image/bmp";
    if (strcmp(p, ".avif") == 0)
        return "image/avif";
    if (strcmp(p, ".svg") == 0)
        return "image/svg+xml";
    if (strcmp(p, ".tif") == 0 || strcmp(p, ".tiff") == 0)
        return "image/tiff";
    if (strcmp(p, ".webp") == 0)
        return "image/webp";
    // 音频与视频
    if (strcmp(p, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(p, ".wav") == 0)
        return "audio/x-wav";
    if (strcmp(p, ".mp4") == 0)
        return "video/mp4";
    if (strcmp(p, ".mpeg") == 0 || strcmp(p, ".mpg") == 0)
        return "video/mpeg";
    if (strcmp(p, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(p, ".mov") == 0)
        return "video/quicktime";
    if (strcmp(p, ".ogg") == 0)
        return "audio/ogg";
    if (strcmp(p, ".ogv") == 0)
        return "video/ogg";
    return "application/octet-stream";
}

int main(int argc, char const *argv[])
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    // 端口复用
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    listen(fd, 9);
    printf("服务器启动!\n");

    while (1)
    {
        struct sockaddr_in c_addr;
        int len = sizeof(c_addr);
        int c_fd = accept(fd, (struct sockaddr *)&c_addr, &len);

        char buf[2048] = {0};
        recv(c_fd, buf, sizeof(buf), 0);
        printf("%s", buf);

        char fangfa[20] = {0};
        char lujin[50] = {0};
        char banben[20] = {0};

        sscanf(buf, "%s %s %s", fangfa, lujin, banben);

        char ch1[1024] = {0};
        char *ch2 = strchr(lujin, '.');
        ch2 = type(ch2);
        char msg[2048] = {0};

        if (strcmp(ch2, "application/octet-stream") == 0)
        {
            char msg1[] = "HTTP/1.1 404 NOT Found\r\n"
                    "Content-Type: text/html\r\n"
                    "\r\n"
                    "<h1>404 NOT Found</h1>";
            send(c_fd, msg1, strlen(msg1), 0);
        }

        sprintf(ch1, "/var/www/html%s",lujin);
        if (strstr(ch1, "..") != NULL)
        {
            snprintf(msg, sizeof(msg), 
                "%s 403 Forbidden\r\n"
                "Content-Type: %s \r\n\r\n"
                "<h1>403 Forbidden</h1>", banben, ch2);
            printf("%s", msg);
            send(c_fd, msg, strlen(msg), 0);
        }

        int file_fd = open(ch1, O_RDONLY);
        if (file_fd == -1)
        {
            snprintf(msg, sizeof(msg), 
                "%s 404 Not Found\r\n"
                "Content-Type: %s \r\n\r\n"
                "<h1>404 Not Found</h1>", banben, ch2);
            send(c_fd, msg, strlen(msg), 0);
        }
        else
        {
            snprintf(msg, sizeof(msg), 
                "%s 200 OK\r\n"
                "Content-Type: %s \r\n\r\n", banben, ch2);
            send(c_fd, msg, strlen(msg), 0);

            int n;
            while ((n = read(file_fd, msg, sizeof(msg))) > 0)
            {
                send(c_fd, msg, n, 0);
            }
        }
        close(file_fd); 
        close(c_fd);
    }
    close(fd);
    
    return 0;
}
