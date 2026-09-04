#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

char *type(char *p) {
    if (strcmp(p, ".html") == 0) return "text/html";
    if (strcmp(p, ".css") == 0) return "text/css";
    if (strcmp(p, ".js") == 0 || strcmp(p, ".mjs") == 0) return "text/javascript";
    if (strcmp(p, ".txt") == 0 || strcmp(p, ".text") == 0) return "text/plain";
    if (strcmp(p, ".csv") == 0) return "text/csv";
    if (strcmp(p, ".doc") == 0) return "application/msword";
    if (strcmp(p, ".docx") == 0) return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    if (strcmp(p, ".xls") == 0) return "application/vnd.ms-excel";
    if (strcmp(p, ".xlsx") == 0) return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    if (strcmp(p, ".ppt") == 0) return "application/vnd.ms-powerpoint";
    if (strcmp(p, ".pptx") == 0) return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    if (strcmp(p, ".pdf") == 0) return "application/pdf";
    if (strcmp(p, ".rtf") == 0) return "application/rtf";
    if (strcmp(p, ".gif") == 0) return "image/gif";
    if (strcmp(p, ".ico") == 0) return "image/x-icon";
    if (strcmp(p, ".jpeg") == 0 || strcmp(p, ".jpg") == 0) return "image/jpeg";
    if (strcmp(p, ".png") == 0) return "image/png";
    if (strcmp(p, ".bmp") == 0) return "image/bmp";
    if (strcmp(p, ".avif") == 0) return "image/avif";
    if (strcmp(p, ".svg") == 0) return "image/svg+xml";
    if (strcmp(p, ".tif") == 0 || strcmp(p, ".tiff") == 0) return "image/tiff";
    if (strcmp(p, ".webp") == 0) return "image/webp";
    if (strcmp(p, ".mp3") == 0) return "audio/mpeg";
    if (strcmp(p, ".wav") == 0) return "audio/x-wav";
    if (strcmp(p, ".mp4") == 0) return "video/mp4";
    if (strcmp(p, ".mpeg") == 0 || strcmp(p, ".mpg") == 0) return "video/mpeg";
    if (strcmp(p, ".avi") == 0) return "video/x-msvideo";
    if (strcmp(p, ".mov") == 0) return "video/quicktime";
    if (strcmp(p, ".ogg") == 0) return "audio/ogg";
    if (strcmp(p, ".ogv") == 0) return "video/ogg";
    return "application/octet-stream";
}

int main(int argc, char const *argv[]) {
    signal(SIGCHLD, SIG_IGN);  // 防止子进程变成僵尸

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(fd, 9);
    printf("✅ C 服务器启动，监听 9000 端口，PID: %d\n", getpid());

    while (1) {
        struct sockaddr_in c_addr;
        int len = sizeof(c_addr);
        int c_fd = accept(fd, (struct sockaddr *)&c_addr, &len);
        if (c_fd < 0) continue;

        pid_t pid = fork();
        if (pid == 0) {  // 子进程
            close(fd);

            char buf[4096] = {0};
            recv(c_fd, buf, sizeof(buf) - 1, 0);
            printf("📩 请求:\n%s\n", buf);

            char fangfa[20], lujin[1024], banben[20];
            sscanf(buf, "%s %s %s", fangfa, lujin, banben);

            // 根目录补全 index.html
            if (strcmp(lujin, "/") == 0 || strlen(lujin) == 0)
                strcpy(lujin, "/index.html");

            // 安全过滤，禁止目录遍历
            if (strstr(lujin, "..") != NULL) {
                char msg[] = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\n\r\n<h1>403 Forbidden</h1>";
                send(c_fd, msg, strlen(msg), 0);
                close(c_fd);
                exit(0);
            }

            // 获取 MIME 类型
            char *dot = strrchr(lujin, '.');
            char *mime = "application/octet-stream";
            if (dot) mime = type(dot);

            // 关键：静态文件根目录指向 html 子目录
            char ch1[2048];
            snprintf(ch1, sizeof(ch1), "./html%s", lujin);

            int file_fd = open(ch1, O_RDONLY);
            if (file_fd == -1) {
                char msg[1024];
                snprintf(msg, sizeof(msg), "%s 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>", banben);
                send(c_fd, msg, strlen(msg), 0);
            } else {
                char header[512];
                snprintf(header, sizeof(header), "%s 200 OK\r\nContent-Type: %s\r\n\r\n", banben, mime);
                send(c_fd, header, strlen(header), 0);
                int n;
                char body[4096];
                while ((n = read(file_fd, body, sizeof(body))) > 0)
                    send(c_fd, body, n, 0);
                close(file_fd);
            }
            close(c_fd);
            exit(0);
        } else {
            close(c_fd);  // 父进程立即释放连接
        }
    }
    close(fd);
    return 0;
}
