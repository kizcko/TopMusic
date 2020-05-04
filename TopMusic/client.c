#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>



#define port 2728
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"
#define COLOR_RED "\x1b[31m"
char server_response[100000];
char send_server[256];
char comanda[256];
char username[256];
char password[256];

int main(){
    int network_socket;
    if((network_socket=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("--!-- SOCKET ERROR --!--\n");
        exit(1);
    }
    
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(port);
    server_address.sin_addr.s_addr=inet_addr("127.0.0.1");
    
    if(connect(network_socket, (struct sockaddr*)& server_address, sizeof(server_address))<0){
        perror("--!-- CONNECT ERROR --!--\n");
        exit(1);
    }
    
    printf(COLOR_GREEN "\n\t\t▀▀█▀▀ █▀▀█ █▀▀█   █▀▄▀█ █░░█ █▀▀ ░▀░ █▀▀ \n\t\t░░█░░ █░░█ █░░█   █░▀░█ █░░█ ▀▀█ ░█░ █░░ \n\t\t░░▀░░ ▀▀▀▀ █▀▀▀   ▀░░░▀ ▀▀▀▀ ▀▀▀ ░▀░ ▀▀▀ \n\n" COLOR_RESET);
    printf(COLOR_GREEN "\n\n--------------------------ᴄᴏᴍᴍᴀɴᴅs--------------------------\n\n\t\t\t    login \n\t\t\t   register \n\t\t             vote\n\t\t\t   seemusic\n\t\t\t    seetop\n\t\t\tseetop by genre\n\t\t\t  see comments\n\t\t\t insert comment\n\t\t\t delete comment\n\t\t\t     quit\n\n\n ");
    printf(COLOR_RESET);
    printf(COLOR_RED "\n\n--------------------------ᴄᴏᴍᴍᴀɴᴅs ᴀᴅᴍɪɴ--------------------------\n\n\t\t\t  restrict user \n\t\t\t unrestrict user \n\t\t\t   insert music\n\t\t\t   delete music\n\t\t\t     see users \n\t\t\t   delete user \n\t\t\t  delete comment\n\t\t\t       quit\n\n\n ");
    printf(COLOR_RESET);
    while(1){
        printf("\nᴄᴏᴍᴍᴀɴᴅ: ");
        fflush(stdout);
        
        memset(comanda, 0, sizeof(comanda));
        read(0,comanda,sizeof(comanda));
        comanda[strlen(comanda)-1]=0;
        
        if (strcmp(comanda,"quit")==0) {
            send(network_socket, &comanda, sizeof(comanda), 0);
            close(network_socket);
            printf(COLOR_RED "\n DISCONNECTED \n" COLOR_RESET);
            exit(1);
        }
        
        if(strcmp(comanda,"login")==0){
            //trimite comanda
            send(network_socket, &comanda, sizeof(comanda), 0);
            //trimite username
            recv(network_socket, &server_response, sizeof(server_response), 0);
            printf(COLOR_GREEN "SERVER: %s\n",server_response);
            printf(COLOR_RESET);
            scanf("%s",username);
            send(network_socket, &username, sizeof(username), 0);
            //trimite password
            recv(network_socket, &server_response, sizeof(server_response), 0);
            if (strcmp(server_response,"\n USERNAME DOESN'T EXIST \n")==0) {
                printf(COLOR_RED "%s\n",server_response);
                printf(COLOR_RESET);
            }
            else{
            
            printf(COLOR_GREEN "%s\n",server_response);
            printf(COLOR_RESET);
            
            scanf("%s",password);
            send(network_socket, &password, sizeof(username), 0);
            //primeste raspuns
            recv(network_socket, &server_response, sizeof(server_response), 0);
            printf(COLOR_GREEN "%s\n",server_response);
            printf(COLOR_RESET);
            }
            
        }
        if (strcmp(comanda,"register")==0){
            //trimite comanda
            send(network_socket, &comanda, sizeof(comanda), 0);
            //send username
            recv(network_socket, &server_response, sizeof(server_response), 0);
            printf(COLOR_GREEN "SERVER: %s\n",server_response);
            printf(COLOR_RESET);
            scanf("%s",username);
            send(network_socket, &username, sizeof(username), 0);
            //Username exist or not
            recv(network_socket, &server_response, sizeof(server_response), 0);
            if (strcmp(server_response,"\n USERNAME EXIST \n")==0) {
                printf(COLOR_RED "%s\n",server_response);
                printf(COLOR_RESET);
            }
            else{
                //send password
                printf(COLOR_GREEN "%s\n",server_response);
                printf(COLOR_RESET);
                scanf("%s",send_server);
                send(network_socket, &send_server, sizeof(send_server), 0);
                //send type
                recv(network_socket, &server_response, sizeof(server_response), 0);
                printf(COLOR_GREEN "SERVER: %s\n",server_response);
                printf(COLOR_RESET);
                scanf("%s",send_server);
                send(network_socket, &send_server, sizeof(send_server), 0);
                if(strcmp(send_server, "admin")==0){
                    recv(network_socket, &server_response, sizeof(server_response), 0);
                    printf(COLOR_GREEN "SERVER: %s\n",server_response);
                    printf(COLOR_RESET);
                    scanf("%s",send_server);
                    send(network_socket, &send_server, sizeof(send_server), 0);
                    
                    recv(network_socket, &server_response, sizeof(server_response), 0); //primeste confrimarea sau nu
                    if(strcmp(server_response, "\n CHEIE NECONFIRMATA \n")==0){ //cheie confirmata
                        printf(COLOR_GREEN "SERVER: %s\n",server_response);
                        printf(COLOR_RESET);
                    }
                    else{
                        printf(COLOR_GREEN "SERVER: %s\n",server_response);
                        printf(COLOR_RESET);
                        
                    }
                }
                
                if(strcmp(send_server, "normal_user")==0){
                    recv(network_socket, &server_response, sizeof(server_response), 0);
                    printf(COLOR_GREEN "SERVER: %s\n",server_response);
                    printf(COLOR_RESET);
                }
            }
        }
        if (strcmp(comanda,"seemusic")==0){
            write(network_socket, comanda, sizeof(comanda));
            read(network_socket, server_response, sizeof(server_response));
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT \n")==0) {
                printf(COLOR_RED "%s\n",server_response);
                printf(COLOR_RESET);
                }
            else{
                printf(COLOR_GREEN "%s\n",server_response);
                printf(COLOR_RESET);
            }
        }
        
        
        if (strcmp(comanda,"vote")==0){
            send(network_socket, &comanda, sizeof(comanda), 0);
            recv(network_socket, &server_response, sizeof(server_response), 0);
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT \n")==0) {
                printf(COLOR_RED "%s\n",server_response);
                printf(COLOR_RESET);
                }
            else{
                if (strcmp(server_response,"\n ESTI RESTRICTIONAT LA FUNCTIA DE A VOTA \n")==0) {
                printf(COLOR_RED "%s\n",server_response);
                printf(COLOR_RESET);
                }
                else{
                    printf(COLOR_GREEN "%s\n",server_response); //titlul piesei
                    printf(COLOR_RESET);
                    scanf("%s",send_server);
                    send(network_socket, &send_server, sizeof(send_server), 0);
                    recv(network_socket, &server_response, sizeof(server_response), 0);
                    if(strcmp(server_response,"\n MELODIA NU EXISTA \n")==0){
                        printf(COLOR_RED "%s\n",server_response);
                        printf(COLOR_RESET);
                    }
                    if(strcmp(server_response,"\n Ai votat deja melodia \n")==0){
                        printf(COLOR_RED "%s\n",server_response);
                        printf(COLOR_RESET);
                    }
                    else{
                        printf(COLOR_GREEN "%s\n",server_response);
                        printf(COLOR_RESET);
                    }
                    
                }
                
                
                }
        }
        
        if (strcmp(comanda,"seetop")==0){
            send(network_socket, &comanda, sizeof(comanda), 0);
            recv(network_socket, &server_response, sizeof(server_response), 0);
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT \n")==0) {
                printf(COLOR_RED "%s\n",server_response);
                printf(COLOR_RESET);
                }
            else{
                printf(COLOR_GREEN "%s\n",server_response);
                printf(COLOR_RESET);
            }
        }
        
        if (strcmp(comanda,"seetop_by_genre")==0){
            send(network_socket, &comanda, sizeof(comanda), 0);
            recv(network_socket, &server_response, sizeof(server_response), 0);
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT \n")==0) { //nelogat
                printf(COLOR_RED "%s\n",server_response);
                printf(COLOR_RESET);
                }
            else{
                printf(COLOR_GREEN "%s\n",server_response);
                printf(COLOR_RESET);
                scanf("%s",send_server);  // trimite genul
                send(network_socket, &send_server, sizeof(send_server), 0);
                recv(network_socket, &server_response, sizeof(server_response), 0);
                if(strcmp(server_response,"\n GENUL NU EXISTA \n")==0){
                    printf(COLOR_RED "%s\n",server_response);
                    printf(COLOR_RESET);
                }
                else{
                    printf(COLOR_GREEN "%s\n",server_response);
                    printf(COLOR_RESET);
                }
            }
        }
    
        if(strcmp(comanda,"restrict user")==0){
            send(network_socket, &comanda, sizeof(comanda), 0);
            recv(network_socket, &server_response, sizeof(server_response), 0);
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n")==0) { //nelogat
                printf(COLOR_RED "%s\n",server_response);
                printf(COLOR_RESET);
                }
            else{
                    printf(COLOR_GREEN "%s\n",server_response);
                    printf(COLOR_RESET);
                    scanf("%s",send_server);  // trimite usernameul
                    send(network_socket, &send_server, sizeof(send_server), 0);
                    recv(network_socket, &server_response, sizeof(server_response), 0);
                if(strcmp(server_response,"\n Utilizatorul nu exista \n")==0){
                    printf(COLOR_RED "%s\n",server_response);
                    printf(COLOR_RESET);
                }
                else{
                    printf(COLOR_GREEN "%s\n",server_response);
                    printf(COLOR_RESET);
                }
                
                }
        }
        
        if(strcmp(comanda,"unrestrict user")==0){
                   send(network_socket, &comanda, sizeof(comanda), 0);
                   recv(network_socket, &server_response, sizeof(server_response), 0);
                   if (strcmp(server_response,"\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n")==0) { //nelogat
                       printf(COLOR_RED "%s\n",server_response);
                       printf(COLOR_RESET);
                       }
                   else{
                           printf(COLOR_GREEN "%s\n",server_response);
                           printf(COLOR_RESET);
                           scanf("%s",send_server);  // trimite usernameul
                           send(network_socket, &send_server, sizeof(send_server), 0);
                           recv(network_socket, &server_response, sizeof(server_response), 0);
                       if(strcmp(server_response,"\n Utilizatorul nu exista \n")==0){
                           printf(COLOR_RED "%s\n",server_response);
                           printf(COLOR_RESET);
                       }
                       else{
                           printf(COLOR_GREEN "%s\n",server_response);
                           printf(COLOR_RESET);
                       }
                       
                       }
               }
        if(strcmp(comanda,"insert music")==0){
            write(network_socket, comanda, sizeof(comanda));
            read(network_socket, server_response, sizeof(server_response));
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n")==0) { //nelogat
                                  printf(COLOR_RED "%s\n",server_response);
                                  printf(COLOR_RESET);
                                  fflush(stdout);
                                  }
            else{
                printf(COLOR_GREEN "%s\n",server_response);
                fflush(stdout);
                printf(COLOR_RESET);
                fflush(stdout);
                    //numele melodie
                memset(send_server, 0, sizeof(send_server));
                read(0, send_server, sizeof(send_server));
                send_server[strlen(send_server)-1]=0;
                
                write(network_socket, send_server, sizeof(send_server));
                read(network_socket, server_response, sizeof(server_response));
                if(strcmp(server_response, "\n Melodia exista \n")==0)
                {
                    printf(COLOR_RED "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                    
                }
                else{
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                    memset(send_server, 0, sizeof(send_server));
                    read(0, send_server, sizeof(send_server));
                    send_server[strlen(send_server)-1]=0;
                    write(network_socket, send_server, sizeof(send_server)); // artistul
                    
                    read(network_socket, server_response, sizeof(server_response));
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                    memset(send_server, 0, sizeof(send_server));
                    read(0, send_server, sizeof(send_server));
                    send_server[strlen(send_server)-1]=0;
                    write(network_socket, send_server, sizeof(send_server)); //genul
                    
                    read(network_socket, server_response, sizeof(server_response));
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                    memset(send_server, 0, sizeof(send_server));
                    read(0, send_server, sizeof(send_server));
                    send_server[strlen(send_server)-1]=0;
                    write(network_socket, send_server, sizeof(send_server)); //link
                    
                    read(network_socket, server_response, sizeof(server_response));
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
                
                
                
            }
        }
        
        if(strcmp(comanda, "delete user")==0){
            write(network_socket, comanda, sizeof(comanda));
            read(network_socket, server_response, sizeof(server_response));
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n")==0) { //nelogat
            printf(COLOR_RED "%s\n",server_response);
            printf(COLOR_RESET);
            fflush(stdout);
            }
            else{
                printf(COLOR_GREEN "%s\n",server_response);
                fflush(stdout);
                printf(COLOR_RESET);
                fflush(stdout);
                //numele utilizatorului
                memset(send_server, 0, sizeof(send_server));
                read(0, send_server, sizeof(send_server));
                send_server[strlen(send_server)-1]=0;
                write(network_socket, send_server, sizeof(send_server));
                read(network_socket, server_response, sizeof(server_response));
                if(strcmp(server_response, "\n Utilizatorul nu exista \n")==0)
                {
                    printf(COLOR_RED "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                    
                }
                else{
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
            }
        }
        
        if(strcmp(comanda, "see users")==0){
            write(network_socket, comanda, sizeof(comanda));
            read(network_socket, server_response, sizeof(server_response));
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n")==0) { //nelogat
            printf(COLOR_RED "%s\n",server_response);
            fflush(stdout);
            printf(COLOR_RESET);
            fflush(stdout);
            }
            else{
               printf(COLOR_GREEN "%s\n",server_response);
                fflush(stdout);
                printf(COLOR_RESET);
                fflush(stdout);
            }
        }
        
        if(strcmp(comanda, "delete music")==0)
        {
            write(network_socket, comanda, sizeof(comanda));
            read(network_socket, server_response, sizeof(server_response));
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n")==0) { //nelogat
            printf(COLOR_RED "%s\n",server_response);
                fflush(stdout);
            printf(COLOR_RESET);
            fflush(stdout);
            }
            else{
               printf(COLOR_GREEN "%s\n",server_response);
               fflush(stdout);
               printf(COLOR_RESET);
               fflush(stdout);
               //id-ul piesei
               memset(send_server, 0, sizeof(send_server));
               read(0, send_server, sizeof(send_server));
               send_server[strlen(send_server)-1]=0;
               write(network_socket, send_server, sizeof(send_server));
               read(network_socket, server_response, sizeof(server_response));
                if(strcmp(server_response, "\n Melodia nu exista \n")==0){
                    printf(COLOR_RED "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
                else{
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
            }
        }
        
        if(strcmp(comanda, "see comments")==0){
            write(network_socket, comanda, sizeof(comanda));
            read(network_socket, server_response, sizeof(server_response));
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT \n")==0) { //nelogat
            printf(COLOR_RED "%s\n",server_response);
                fflush(stdout);
            printf(COLOR_RESET);
            fflush(stdout);
            }
            else{
                printf(COLOR_GREEN "%s\n",server_response);
                fflush(stdout);
                printf(COLOR_RESET);
                fflush(stdout);
                //id-ul piesei
                memset(send_server, 0, sizeof(send_server));
                read(0, send_server, sizeof(send_server));
                send_server[strlen(send_server)-1]=0;
                write(network_socket, send_server, sizeof(send_server));
                read(network_socket, server_response, sizeof(server_response));
                if(strcmp(server_response, "\n Melodia nu exista \n")==0){
                    printf(COLOR_RED "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
                else{
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
                
            }
        }
        
        
        if(strcmp(comanda, "insert comment")==0)
        {
            write(network_socket, comanda, sizeof(comanda));
            read(network_socket, server_response, sizeof(server_response));
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT \n")==0) { //nelogat
            printf(COLOR_RED "%s\n",server_response);
                fflush(stdout);
            printf(COLOR_RESET);
            fflush(stdout);
            }
            else{
                printf(COLOR_GREEN "%s\n",server_response);
                fflush(stdout);
                printf(COLOR_RESET);
                fflush(stdout);
                //id-ul piesei
                memset(send_server, 0, sizeof(send_server));
                read(0, send_server, sizeof(send_server));
                send_server[strlen(send_server)-1]=0;
                write(network_socket, send_server, sizeof(send_server));
                read(network_socket, server_response, sizeof(server_response));
                if(strcmp(server_response, "\n Melodia nu exista \n")==0){
                    printf(COLOR_RED "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
                else{
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                    //comentariul
                    memset(send_server, 0, sizeof(send_server));
                    read(0, send_server, sizeof(send_server));
                    send_server[strlen(send_server)-1]=0;
                    write(network_socket, send_server, sizeof(send_server));
                    read(network_socket, server_response, sizeof(server_response));
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
                
            }
        }
        
        if(strcmp(comanda, "delete comment")==0){
            write(network_socket, comanda, sizeof(comanda));
            read(network_socket, server_response, sizeof(server_response));
            if (strcmp(server_response,"\n TREBUIE SA FII LOGAT \n")==0) { //nelogat
            printf(COLOR_RED "%s\n",server_response);
                fflush(stdout);
            printf(COLOR_RESET);
            fflush(stdout);
            }
            else{
                printf(COLOR_GREEN "%s\n",server_response);
                fflush(stdout);
                printf(COLOR_RESET);
                fflush(stdout);
                if(strcmp(server_response, "\n Id-ul melodiei: \n")==0){
                    memset(send_server, 0, sizeof(send_server));
                    read(0, send_server, sizeof(send_server));
                    send_server[strlen(send_server)-1]=0;
                    
                    write(network_socket, send_server, sizeof(send_server));
                    read(network_socket, server_response, sizeof(server_response));
                    if (strcmp(server_response,"\n Melodia nu exista \n")==0) { //nelogat
                    printf(COLOR_RED "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                    }
                    else{
                        printf(COLOR_GREEN "%s\n",server_response);
                        fflush(stdout);
                        printf(COLOR_RESET);
                        fflush(stdout);
                        memset(send_server, 0, sizeof(send_server));
                        read(0, send_server, sizeof(send_server));
                        send_server[strlen(send_server)-1]=0;
                        write(network_socket, send_server, sizeof(send_server));
                        read(network_socket, server_response, sizeof(server_response));
                        if (strcmp(server_response,"\n COMENTARIUL NU EXISTA \n")==0) { //nelogat
                        printf(COLOR_RED "%s\n",server_response);
                            fflush(stdout);
                        printf(COLOR_RESET);
                        fflush(stdout);
                        }
                        else{
                            printf(COLOR_GREEN "%s\n",server_response);
                            fflush(stdout);
                            printf(COLOR_RESET);
                            fflush(stdout);
                        }
                        
                        
                    }
                    
                    
                    
                }
                else{
                //select comment
                memset(send_server, 0, sizeof(send_server));
                read(0, send_server, sizeof(send_server));
                send_server[strlen(send_server)-1]=0;
                
                write(network_socket, send_server, sizeof(send_server));
                read(network_socket, server_response, sizeof(server_response));
                if (strcmp(server_response,"\n COMENTARIUL NU EXISTA \n")==0) { //nelogat
                printf(COLOR_RED "%s\n",server_response);
                    fflush(stdout);
                printf(COLOR_RESET);
                fflush(stdout);
                }
                else{
                    printf(COLOR_GREEN "%s\n",server_response);
                    fflush(stdout);
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
            }
            }
        }
        
    }
    return 0;
}
