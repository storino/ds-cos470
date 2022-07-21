#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>

using namespace std;

#define PORT 4242

// strftime format
#define LOGGER_PRETTY_TIME_FORMAT "%Y-%m-%d %H:%M:%S"

// printf format
#define LOGGER_PRETTY_MS_FORMAT ".%03d"

// convert current time to milliseconds since unix epoch
template <typename T>
static int to_ms(const std::chrono::time_point<T>& tp)
{
    using namespace std::chrono;

    auto dur = tp.time_since_epoch();
    return static_cast<int>(duration_cast<milliseconds>(dur).count());
}


// format it in two parts: main part with date and time and part with milliseconds
static std::string pretty_time()
{
    auto tp = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(tp);

    // this function use static global pointer. so it is not thread safe solution
    std::tm* time_info = std::localtime(&current_time);

    char buffer[128];

    int string_size = strftime(
        buffer, sizeof(buffer),
        LOGGER_PRETTY_TIME_FORMAT,
        time_info
    );

    int ms = to_ms(tp) % 1000;

    string_size += std::snprintf(
        buffer + string_size, sizeof(buffer) - string_size,
        LOGGER_PRETTY_MS_FORMAT, ms
    );

    return std::string(buffer, buffer + string_size);
}

int main(int argc , char *argv[])
{
    int n = atoi(argv[1]);
    int r = atoi(argv[2]);
    int k = atoi(argv[3]);

    ofstream myfile;

    int sock = 0;
    char buffer[10] = {0};
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr,
                sizeof(serv_addr))
        < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    for(int i = 0; i < n; i++) 
    {   
        if (fork() == 0)
        {
            int pid = getpid();
            char str_pid[32];
            sprintf(str_pid, "%d", pid);
            
            for(int j = 0; j < r; j++)
            {       
                //char str_j[32];
                //sprintf(str_j, "%d", j);
                //char *req_message = strcat(str_pid,"|");
                //req_message = strcat(req_message,str_j);

                send(sock, str_pid, strlen(str_pid), 0);

                read(sock, buffer, 1024);

                string time = pretty_time();

                myfile.open ("resultados.txt", ios_base::app);
                myfile << time << "\n";
                myfile.close();

                sleep(k);
                char *message = "realese";
                send(sock, message, strlen(message), 0);
            }
            exit(0);
        }
    }



    return 0;
}