//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h>
#include <string>   //strlen 
#include <stdlib.h>
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <map>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <signal.h>
#include <iterator>
#include <ctime>
#include <chrono>

using namespace std;

#define TRUE  1 
#define FALSE 0 

#define PORT  8000
#define NT    2

const int queue_size = 15;

// strftime format
#define LOGGER_PRETTY_TIME_FORMAT "%Y-%m-%d %H:%M:%S"

// printf format
#define LOGGER_PRETTY_MS_FORMAT ".%03d"

// convert current time to milliseconds since unix epoch
template <typename T>
static int to_ms(const chrono::time_point<T>& tp)
{
    using namespace chrono;

    auto dur = tp.time_since_epoch();
    return static_cast<int>(duration_cast<milliseconds>(dur).count());
}


// format it in two parts: main part with date and time and part with milliseconds
static string pretty_time()
{
    auto tp = chrono::system_clock::now();
    time_t current_time = chrono::system_clock::to_time_t(tp);

    // this function use static global pointer. so it is not thread safe solution
    tm* time_info = localtime(&current_time);

    char buffer[128];

    int string_size = strftime(
        buffer, sizeof(buffer),
        LOGGER_PRETTY_TIME_FORMAT,
        time_info
    );

    int ms = to_ms(tp) % 1000;

    string_size += snprintf(
        buffer + string_size, sizeof(buffer) - string_size,
        LOGGER_PRETTY_MS_FORMAT, ms
    );

    return string(buffer, buffer + string_size);
}

class Queue { // an interface for a queue

    private:
        int n; // number of items in queue
        int f; // front
        int r; // rear
        int Q[queue_size]; // array of items
        int max_size; // maximum size of queue

    public:
        Queue() {
            n = 0; // number of items in queue
            f = 0; // front
            r = 0;
            Q[queue_size]; // array of items
            max_size = sizeof(Q);
        }

        int size()
        { // number of items in queue
            return n;
        } 

        bool empty() 
        { // is queue empty?
            return n == 0;
        }   

        int front() 
        { // return front item
            if (this->empty()) {
                length_error("Queue is empty");
                }
            return Q[f];
        }

        void enqueue (int x) 
        { // add item to queue
            if (n == max_size) 
            {
                length_error("Queue maxsize surpassed");
                return;
            }
        Q[r] = x;
        r = (r + 1) % max_size;
        n++;
        }

        // enqueue element at rear
        void dequeue()  // dequeue element at front
        {
            if (this->empty()) 
            {
                length_error("Queue is empty");
                return;
            }
            f = (f + 1) % max_size;
            n--;
        }

        string full_queue() {
            int i = f;
            string s = "";
            while (i != r) {
                s += to_string(Q[i]) + ",";
                i = (i + 1) % max_size;
            }
            return s;
        };
};

class Dict_Sockets {
    private:
        map <int, int> dic_sockets;

    public:
        Dict_Sockets()
        {
            dic_sockets = map <int, int>();
        }

        void insert_socket_if_nescessary(int process_id, int socket)
        {
            if (dic_sockets[process_id] == 0)
            {
                dic_sockets[process_id] = socket;
            }   
        }

        int get_socket(int process_id)
        {
            return dic_sockets[process_id];
        }

};

class Process_Counter {
    private:
        map <int, int> dic_process;
        map<int, int>::iterator itr;

    public:
        Process_Counter()
        {
            dic_process = map <int, int>();
        }

        void inc_process_count(int process_id)
        {
            if (dic_process[process_id] == 0)
            {
                dic_process[process_id] = 1;
            }
            else
            {
                dic_process[process_id]++;
            }
        }

        int get_process_count(int process_id)
        {
            return dic_process[process_id];
        }

        void clear_counter() 
        {
            dic_process.clear();
        } 

        string get_all_counts()
        {
            string s = "";
            for (itr = dic_process.begin(); itr != dic_process.end(); ++itr) {
                //cout << '\t' << itr->first << '\t' << itr->second
                //    << '\n';
                s +=  "{" + to_string(itr->first) + "," + to_string(itr->second) + "}" + ",";
            }
            return s;
        }

};

class CADME {//Centralised Algorythm for Distributed Mutual Exclusion
    private:
        Queue acess_queue;

    public:
        CADME()
        {
            acess_queue = Queue();
        }

    int request_access(int process_id)
    {

        if (acess_queue.empty())  {
            cout << "Granted to process: " << process_id << endl;
            acess_queue.enqueue(process_id);
            cout << "Queue " << acess_queue.full_queue() << endl;
            return process_id;
        }
            // grant acess to process_id

            acess_queue.enqueue(process_id);
            cout << acess_queue.full_queue() << endl;

        return -1;

    }
    int release_access(int process_id)
    {

        if (process_id != acess_queue.front()) {
            // process_id did not request access
            return -1;
        }
            acess_queue.dequeue();

        if (!acess_queue.empty()) {
            int process_with_acess = acess_queue.front();;
            // grant acess to process_with_acess
            cout << "Granted to process: " << process_with_acess << "\n";
            return process_with_acess;
        } 
        return -1;
    }
        int get_acess()
        {
            return acess_queue.front();
        }
        
        string show_queue()
        {
            string s = acess_queue.full_queue();
            return s;
        }
};

int main(int argc , char *argv[])  
{   
    //Process_Counter 
    Dict_Sockets dic_sockets = Dict_Sockets();
    Process_Counter grant_counter = Process_Counter();

    omp_lock_t coordinator_lock;
    omp_lock_t grant_counter_lock;

    int mypid = getpid();
    cout << "My pid is " << mypid << endl;

    omp_init_lock(&coordinator_lock);
    omp_init_lock(&grant_counter_lock);

    CADME coordenator = CADME();

    string log_message;
    string terminal_message;
    
    // Coordenador de threads
    #pragma omp parallel num_threads(NT)
    {

        int thread_num = omp_get_thread_num();

        int opt = TRUE;  
        int master_socket , addrlen , new_socket , client_socket[128], 
            max_clients = 128, activity, i, valread, sd;  
        int max_sd;  
        struct sockaddr_in address;  
            
        char buffer[16384];
     
        string str_queue;
        //set of socket descriptors 
        fd_set readfds;         

        switch (thread_num)
        {
            case 0:
                while(TRUE)
                {
                    int command;
                    cin >> command;
                    if (command == 1)
                    {   
                        #pragma omp_set_lock(&coordinator_lock)
                        str_queue = coordenator.show_queue();
                        #pragma omp_unset_lock(&coordinator_lock)
                        cout << "\n" << str_queue << " < Queue\n\n";
                    }
                    else if (command == 2)
                    {
                        #pragma omp_set_lock(&grant_counter_lock)
                        str_queue = grant_counter.get_all_counts();
                        #pragma omp_unset_lock(&grant_counter_lock)
                        cout << "\n" << str_queue << " < This is the number of GRANT messages for each process.\n\n";
                    }
                    else if (command == 3)
                    {
                        exit(0);
                    }
                    else
                    {
                        cout << "Invalid command.\n\n";
                    }
                }

            case 1:
                //initialise all client_socket[] to 0 so not checked 
                for (i = 0; i < max_clients; i++)
                {  
                    client_socket[i] = 0;  
                }  
                    
                //create a master socket 
                if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
                {  
                    perror("socket failed");  
                    exit(EXIT_FAILURE);  
                }  
                
                //set master socket to allow multiple connections , 
                //this is just a good habit, it will work without this 
                if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
                    sizeof(opt)) < 0 )  
                {  
                    perror("setsockopt");  
                    exit(EXIT_FAILURE);  
                }  
                
                //type of socket created 
                address.sin_family = AF_INET;  
                address.sin_addr.s_addr = INADDR_ANY;  
                address.sin_port = htons( PORT );  
                    
                //bind the socket to localhost port 8888 
                if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
                {  
                    perror("bind failed");  
                    exit(EXIT_FAILURE);  
                }  
                printf("Listener on port %d \n", PORT);  
            
                //try to specify maximum of 3 pending connections for the master socket 
                if (listen(master_socket, 3) < 0)  
                {  
                    perror("listen");  
                    exit(EXIT_FAILURE);  
                }  
                    
                //accept the incoming connection 
                addrlen = sizeof(address);  
                puts("Waiting for connections ...");  
                    
                while(TRUE)  
                {  
                    //clear the socket set 
                    FD_ZERO(&readfds);  
                
                    //add master socket to set 
                    FD_SET(master_socket, &readfds);  
                    max_sd = master_socket;  
                        
                    //add child sockets to set 
                    for ( i = 0 ; i < max_clients ; i++)  
                    {  
                        //socket descriptor 
                        sd = client_socket[i];  
                            
                        //if valid socket descriptor then add to read list 
                        if(sd > 0)  
                            FD_SET( sd , &readfds);  
                            
                        //highest file descriptor number, need it for the select function 
                        if(sd > max_sd)  
                            max_sd = sd;  
                    }  
                
                    //wait for an activity on one of the sockets , timeout is NULL , 
                    //so wait indefinitely 
                    activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
                
                    if ((activity < 0) && (errno!=EINTR))  
                    {  
                        printf("select error");  
                    }  
                        
                    //If something happened on the master socket , 
                    //then its an incoming connection 
                    if (FD_ISSET(master_socket, &readfds))  
                    {  
                        if ((new_socket = accept(master_socket, 
                                (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)  
                        {  
                            perror("accept");  
                            exit(EXIT_FAILURE);  
                        }  
                        
                        //inform user of socket number - used in send and receive commands 
                        printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
                            (address.sin_port));  

                        //add new socket to array of sockets 
                        for (i = 0; i < max_clients; i++)  
                        {  
                            //if position is empty 
                            if( client_socket[i] == 0 )  
                            {  
                                client_socket[i] = new_socket;  
                                printf("Adding to list of sockets as %d\n" , i);  
                                    
                                break;  
                            }  
                        }  
                    }  
                        
                    //else its some IO operation on some other socket
                    for (i = 0; i < max_clients; i++)  
                    {  
                        sd = client_socket[i];  
                            
                        if (FD_ISSET( sd , &readfds))  
                        {  
                            //Check if it was for closing , and also read the 
                            //incoming message 
                            if ((valread = read(sd, buffer, 16384)) == 0)  
                            {  
                                //Somebody disconnected , get his details and print 
                                getpeername(sd , (struct sockaddr*)&address , \
                                    (socklen_t*)&addrlen);  
                                printf("Host disconnected, ip %s , port %d \n" , 
                                    inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                                    
                                //Close the socket and mark as 0 in list for reuse 
                                close( sd );  
                                client_socket[i] = 0;  
                            }  
                                
                            //Echo back the message that came in 
                            else
                            {   
                                ofstream myfile;
                                myfile.open ("log.txt", ios_base::app);

                                const char* grant_value = "1";
                                int grant_acess;

                                char* received_msg = buffer;
                                int msg_type;
                                int msg_number;
                                int pid;

                                sscanf(received_msg, "%d|%d|%d", &msg_type, &pid, &msg_number);
                                printf("%d|%d|%d\n", msg_type, pid, msg_number);

                                dic_sockets.insert_socket_if_nescessary(pid, sd);
                                
                                if(msg_type == 0) 
                                {
                                    string time = pretty_time();
                                    myfile << time << " | " << pid << " | " << "REQUEST" << endl;

                                    omp_set_lock(&coordinator_lock);
                                    grant_acess = coordenator.request_access(pid);
                                    omp_unset_lock(&coordinator_lock);
                                    
                                    if (grant_acess != -1) //grant given
                                    {
                                        write(sd, grant_value, strlen(grant_value));

                                        omp_set_lock(&grant_counter_lock);
                                        grant_counter.inc_process_count(grant_acess);
                                        omp_unset_lock(&grant_counter_lock);

                                        string time = pretty_time();
                                        myfile << time << " | " << pid << " | " << "GRANT" << endl;
                                    }  
                                }

                                else if(msg_type == 2)
                                {
                                    string time = pretty_time();
                                    myfile << time << " | " << pid << " | " << "RELEASE" << endl;

                                    omp_set_lock(&coordinator_lock);
                                    grant_acess = coordenator.release_access(pid);
                                    omp_unset_lock(&coordinator_lock);

                                    if (grant_acess != -1) //grant given
                                    {
                                        sd = dic_sockets.get_socket(grant_acess);
                                        write(sd, grant_value, strlen(grant_value));

                                        omp_set_lock(&grant_counter_lock);
                                        grant_counter.inc_process_count(grant_acess);
                                        omp_unset_lock(&grant_counter_lock);

                                        string time = pretty_time();
                                        myfile << time << " | " << pid << " | " << "GRANT" << endl;
                                    }  
                                }
                                else
                                {
                                    cout << "Type of message received is not 0 (REQUEST) nor 2 (RELEASE)" <<  endl;
                                }

                                myfile.close();
                            }  
                        }  
                    }  
                }

            default:
                cout << "A thread has been created incorrectly" << endl;
                break;
        }
    }

    return 0;    
}  