#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/ethernet.h>
#include <string.h>
#include <sqlite3.h>

#define port 2728
char *err_msg = 0;
static int username_callback(void *str, int count, char **data, char **columns)
{
    char* str2 = (char*) str;
    strcpy (str2,data[1]);
    return 0;
}
static int password_callback(void *str, int count, char **data, char **columns)
{
    char* str2 = (char*) str;
    strcpy (str2,data[2]);
    return 0;
}
static int type_callback(void *str, int count, char **data, char **columns)
{
    char* str2 = (char*) str;
    strcpy (str2,data[4]);
    return 0;
}
static int id_callback(void *str, int count, char **data, char **columns)
{
    char* str2 = (char*) str;
    strcpy (str2,data[0]);
    return 0;
}

static int restrict_vote_callback(void *str, int count, char **data, char **columns)
{
    char* str2 = (char*) str;
    strcpy (str2,data[3]);
    return 0;
}

static int genre_callback(void *str, int count, char **data, char **columns)
{
    char* str2 = (char*) str;
    strcpy (str2,data[3]);
    return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}



static int seemusic_callback(void *str, int count, char **data2, char **columns){
    char* str2 = (char*) str;
    for (int i=0;i<count;i++){
        //strcat(str2,columns[i]);
        //strcat(str2,":");
        if(data2[i])
            strcat(str2,data2[i]);
        else
            strcat(str2,NULL);
        strcat(str2,"\t\t");
    }
    strcat(str2,"\n");
    return 0;
}

static int seeuser_callback(void *str, int count, char **data2, char **columns){
    char* str2 = (char*) str;
    for (int i=0;i<2;i++){
        //strcat(str2,columns[i]);
        //strcat(str2,":");
        if(data2[i])
            strcat(str2,data2[i]);
        else
            strcat(str2,NULL);
        strcat(str2,"\t\t");
    }
    strcat(str2,"\n");
    return 0;
}

int main()
{
    char recv_message[256];
    int server_socket;
    char send_message[256];
    char username[256];
    char password[256];
    char sql[10000];
    char send_username[256]=" \n Introduceti username \n";
    char data[100000];char data2[100000];char data3[100000];
    char type[100];
    char admin_apprv[100]="112";
    int login_var=0;
    char title_music[100];
    char genre_music[100];
    char restricted_username[100];
    char insert_music[100];
    char insert_artist[100];
    char insert_genre[100];
    char insert_link[100];
    char id_user[100];
    char id_music[100];
    char insert_comment[10000];
    char comm[100];
    
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t addr_size;
    if((server_socket=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("--!-- SOCKET ERROR --!--\n");
        exit(1);
    }
    else{
        printf("Server socket created\n");
    }
    
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(port);  //***fiecare tip
    server_address.sin_addr.s_addr=inet_addr("127.0.0.1");
    
    if(bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address))<0){ //forward any addr to socket
        perror("--!-- BIND ERROR --!--\n");
        exit(1);
    }
    else{
        printf("Bind to port %d\n", 2728);
    }
    if(listen(server_socket, 10)<0){
        perror("--!-- LISTEN ERROR --!--\n");
        exit(1);
    }
    else{
        printf("Asteptam conexiuni");
    }
    
    
    
    
    while (1) {
        client_socket=accept(server_socket,(struct sockaddr*)&client_address,&addr_size);
        if(client_socket<0){
            perror("--!-- ACCEPT ERROR --!--\n");
            exit(1);
        }
        printf("\nCONNECTION ACCEPTED FROM %s:%d \n", inet_ntoa(client_address.sin_addr),htons(client_address.sin_port));
        
        
        pid_t childpid;
        childpid=fork();
        if(childpid==0){
            while (1) {
                sqlite3* db;
                int rc = sqlite3_open("topmusic.db", &db);
                if(rc!= SQLITE_OK){
                    perror("--!-- DATABASE ERROR --!--\n");
                    exit(1);
                }
                else
                {
                    printf("\n DATABASE ACCEPTED \n");
                }
                
                recv(client_socket,recv_message , 256, 0);

                if (strcmp(recv_message,"quit")==0){
                    printf("DISCONNECTED FROM %s:%d \n", inet_ntoa(client_address.sin_addr),htons(client_address.sin_port));
                    break;}
                if (strcmp(recv_message,"login")==0){  //Login
                    send(client_socket,send_username,256,0);
                    recv(client_socket,recv_message , 256, 0);
                    strcpy(username, recv_message);
                    sprintf(sql, "select * from user where username ='%s'",username);
                    rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                    if( rc != SQLITE_OK ) {
                       fprintf(stderr, "SQL error \n");
                    }
                    else {
                        if(strcmp(data, username)==0){
                            strcpy(send_message, "\n Introducecti parola \n");
                            send(client_socket,send_message,256,0);
                            recv(client_socket,recv_message , 256, 0);
                            
                            rc = sqlite3_exec(db,sql , password_callback, data, NULL);
                            
                            if (rc !=SQLITE_OK){
                                fprintf(stderr, "SQL error \n");
                                strcpy(send_message, "\n WRONG PASSWORD \n");
                                send(client_socket,send_message,256,0);
                            }
                            else{
                                if(strcmp(data,recv_message)==0){
                                    
                                    rc = sqlite3_exec(db,sql , type_callback, data, NULL);
                                    if( rc != SQLITE_OK ) {
                                       fprintf(stderr, "SQL error \n");
                                    }
                                    else{
                                        type[0]=0;
                                        strcpy(type, data);
                                        login_var=1;
                                    }
                                    sprintf(send_message, "WELCOME %s, you are %s",username,type);
                                send(client_socket,send_message,256,0);
                                    
                                }
                                else{
                                    strcpy(send_message, "\n PASSWORD INCCORECT \n ");
                                    send(client_socket,send_message,256,0);
                                }
                            }
                            
                            
                        }
                        else{
                            strcpy(send_message, "\n USERNAME DOESN'T EXIST \n");
                            send(client_socket,send_message,256,0);
                        }
                        
                    }
                    
                    
                }
                
                if (strcmp(recv_message,"register")==0){
                    send(client_socket,send_username,256,0);
                    recv(client_socket,recv_message , 256, 0);
                    strcpy(username, recv_message);
                    
                    sprintf(sql, "select * from user where username ='%s'",username);
                    rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                    
                    if( rc != SQLITE_OK ) {
                       fprintf(stderr, "SQL error \n");
                    }
                    else {
                        if(strcmp(data, username)==0){
                            strcpy(send_message, "\n USERNAME EXIST \n");
                            send(client_socket,send_message,256,0);
                        }
                        else{
                            strcpy(send_message, "\n Introduceti password \n");
                            send(client_socket,send_message,256,0);
                            recv(client_socket,recv_message , 256, 0);
                            strcpy(password, recv_message);
                            strcpy(send_message, "\n Account type: normal_user or admin \n");
                            send(client_socket,send_message,256,0);
                            recv(client_socket,recv_message , 256, 0);
                            strcpy(type, recv_message);
                            if(strcmp(type,"admin")==0){ //daca cere drepturi de admin la inregistrare
                                strcpy(send_message, "\n Introduceti cheia de admin \n");
                                send(client_socket,send_message,256,0);
                                recv(client_socket,recv_message , 256, 0);
                                if(strcmp(recv_message,admin_apprv)==0){ //verificare cheie admin
                                    strcpy(send_message, "\n CHEIE CONFIRMATA, user created \n");
                                    send(client_socket,send_message,256,0);
                                    sprintf(sql, "insert into user(username,password,type) values ('%s','%s','%s')",username,password,type);
                                    rc = sqlite3_exec(db, sql, callback, 0, NULL);
                                    
                                    if( rc != SQLITE_OK ){
                                       fprintf(stderr, "SQL error \n");
                                    } else {
                                       fprintf(stdout, "Records created successfully\n");
                                    }
                                }
                                else{
                                    strcpy(send_message, "\n CHEIE NECONFIRMATA \n");
                                    send(client_socket,send_message,256,0);
                                }
                                
                            }
                            
                           if(strcmp(type,"normal_user")==0){ //daca cere drepturi de normal_user la inregistrare
                           sprintf(sql, "insert into user(username,password,type) values ('%s','%s','%s')",username,password,type);
                           rc = sqlite3_exec(db, sql, callback, 0, NULL);
                           
                           if( rc != SQLITE_OK ){
                              fprintf(stderr, "SQL error \n");
                           } else {
                              fprintf(stdout, "Records created successfully\n");
                           }
                               strcpy(send_message, "\n USER CREATED \n");
                               send(client_socket,send_message,256,0);
                           }
                        }
                    
                    }
                }
                
                
                if(strcmp(recv_message,"seemusic")==0){
                    if((strcmp(type,"admin")==0 || strcmp(type,"normal_user")==0)){ //user logat
                    memset(data2, 0, sizeof(data2));
                    sprintf(sql, "select * from music");
                    strcpy(data2, "\n ----------MUSIC---------- \n\n");
                    rc = sqlite3_exec(db,sql , seemusic_callback, data2, NULL);
                    if( rc != SQLITE_OK ) {
                       fprintf(stderr, "SQL error \n");
                    }
                    else {
                        printf("SQL FACUT \n");
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, data2);
                        write(client_socket, send_message, sizeof(send_message));
                    }
                    }
                    else{
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT \n"); //user nelogat
                        write(client_socket, send_message, sizeof(send_message));
                    }

                }
                
                
                
                //VOTE
                if(strcmp(recv_message,"vote")==0){
                    if((strcmp(type,"admin")==0 || strcmp(type,"normal_user")==0)){
                    sprintf(sql, "select * from user where username ='%s'",username);
                    rc = sqlite3_exec(db,sql , restrict_vote_callback, data, NULL);
                    if( rc != SQLITE_OK ) {
                       fprintf(stderr, "SQL error \n");
                    }
                    else{
                        if(strcmp(data,"1")==0){
                            strcpy(send_message, "\n ESTI RESTRICTIONAT LA FUNCTIA DE A VOTA \n"); //user nelogat
                            send(client_socket,send_message,256,0);
                        }
                        else{
                            strcpy(send_message, "\n Introduceti ID-ul melodiei prin comanda seemusic \n"); //user logat
                            send(client_socket,send_message,256,0);
                            recv(client_socket,recv_message , 256, 0);
                            strcpy(title_music, recv_message);//titlul piesei sa fie votata
                            sprintf(sql, "select * from user where username ='%s'",username);
                            rc = sqlite3_exec(db,sql , id_callback, data3, NULL);
                            if( rc != SQLITE_OK ) {
                               fprintf(stderr, "SQL error \n");
                            }
                            else{
                            strcpy(id_user, data3);
                            sprintf(sql,"select * from vote where id_user='%s' and id_song='%s'",id_user,title_music);
                            rc = sqlite3_exec(db,sql , password_callback, data, NULL);
                            if( rc != SQLITE_OK ) {
                                fprintf(stderr, "SQL error \n");
                                                  }
                            else{
                                
                                if(strcmp(data, "1")==0){
                                    strcpy(send_message, "\n Ai votat deja melodia \n"); //user nelogat
                                    send(client_socket,send_message,256,0);
                                }
                                else{
                            sprintf(sql, "select * from music where id_song='%s'",title_music);
                            rc = sqlite3_exec(db,sql , id_callback, data, NULL);
                            if( rc != SQLITE_OK ) {
                               fprintf(stderr, "SQL error \n");
                            }
                            else{
                                if(strcmp(data,title_music)!=0){
                                    strcpy(send_message, "\n MELODIA NU EXISTA \n"); //user nelogat
                                    send(client_socket,send_message,256,0);
                                }
                                else{
                                    sprintf(sql, "UPDATE music set votes = votes+1 where id_song='%s';",title_music);
                                    rc = sqlite3_exec(db, sql, callback, (void*)data, NULL);
                                    
                                    if( rc != SQLITE_OK ) {
                                       fprintf(stderr, "SQL error:\n");
                                    } else {
                                        sprintf(sql, "INSERT INTO vote (id_song,id_user,vote) VALUES('%s','%s','1')",title_music,id_user);
                                        rc = sqlite3_exec(db, sql, callback, (void*)data, NULL);
                                        
                                        if( rc != SQLITE_OK ) {
                                            fprintf(stderr, "SQL error:\n");
                                        }
                                        else{
                                       strcpy(send_message, "\n VOTED \n"); //user nelogat
                                        send(client_socket,send_message,256,0);}
                                    }
                                }
                            }
                            }
                            }
                        }//else not restrict
                        }
                    }
                    }
                    
                    
                    else{
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT \n"); //user nelogat
                        send(client_socket,send_message,256,0);
                    }
                }
                
                if(strcmp(recv_message,"seetop")==0){
                    if((strcmp(type,"admin")==0 || strcmp(type,"normal_user")==0)){
                        sql[0] = 0;
                        data2[0]=0;
                        sprintf(sql, "select * from music where votes != 0 order by votes desc");
                        strcpy(data2, "\n ----------MUSIC---------- \n\n");
                        rc = sqlite3_exec(db,sql , seemusic_callback, data2, NULL);
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error \n");
                        }
                        else {
                            strcpy(send_message, data2);
                            send(client_socket,send_message,256,0);
                        }
                    }
                    else{
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT \n"); //user nelogat
                        send(client_socket,send_message,256,0);
                    }
                }
                
                
                
                if(strcmp(recv_message,"seetop_by_genre")==0){  //top_by_genre
                    if((strcmp(type,"admin")==0 || strcmp(type,"normal_user")==0)){
                        strcpy(send_message, "\n Genul de muzica : \n"); //user nelogat
                        send(client_socket,send_message,256,0);
                        recv(client_socket,recv_message , 256, 0);
                        strcpy(genre_music, recv_message);
                       
                        sql[0] = 0;
                        data2[0]=0;
                        sprintf(sql, "select * from music where genre='%s'",genre_music);
                        strcpy(data2, "\n ----------MUSIC---------- \n\n");
                        rc = sqlite3_exec(db,sql , seemusic_callback, data2, NULL);
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error \n");
                        }
                        else {
                            strcpy(send_message, data2);
                            send(client_socket,send_message,256,0);
                        }
                        
                        /*
                        sprintf(sql, "select distinct genre from music where genre='%s';",genre_music);
                        rc = sqlite3_exec(db,sql , genre_callback, data, NULL);
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error \n");
                        }
                        else{
                            if(strcmp(data,genre_music)!=0){
                                strcpy(send_message, "\n GENUL NU EXISTA \n"); //user nelogat
                                    send(client_socket,send_message,256,0);
                                
                            }
                            else{ //genre doesn't exist
                                
                            
                                sql[0] = 0;
                                data2[0]=0;
                                sprintf(sql, "select * from music where genre='%s'",genre_music);
                                strcpy(data2, "\n ----------MUSIC---------- \n\n");
                                rc = sqlite3_exec(db,sql , seemusic_callback, data2, NULL);
                                if( rc != SQLITE_OK ) {
                                   fprintf(stderr, "SQL error \n");
                                }
                                else {
                                    strcpy(send_message, data2);
                                    send(client_socket,send_message,256,0);
                                }
                            
                            }
                        }  */
                        
                        
                    }
                    else{
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT \n"); //user nelogat
                        send(client_socket,send_message,256,0);
                        
                    }
                }
                
                if(strcmp(recv_message,"restrict user")==0){
                    if(strcmp(type,"admin")==0){
                        strcpy(send_message, "\n Numele utilizatorului: \n"); //user nelogat
                        send(client_socket,send_message,256,0);
                        recv(client_socket,recv_message , 256, 0);
                        strcpy(restricted_username, recv_message);
                        sprintf(sql, "select * from user where username='%s'",restricted_username);
                        rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error \n");
                        }
                        else{
                            if(strcmp(data, restricted_username)==0){ //utilizatorul exista
                            sprintf(sql, "update user set vote_restriction=1 where username='%s';",restricted_username);
                            rc = sqlite3_exec(db, sql, callback, (void*)data, NULL);
                            
                            if( rc != SQLITE_OK ) {
                               fprintf(stderr, "SQL error:\n");
                            } else {
                                sprintf(send_message, "\n User-ul %s a fost restrictionat \n",restricted_username);
                               send(client_socket,send_message,256,0);
                            }
                        }
                            else{
                                strcpy(send_message, "\n Utilizatorul nu exista \n"); //user nelogat
                                send(client_socket,send_message,256,0);
                            }
                        }
                        
                    }
                    else{
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n"); //user nelogat
                        send(client_socket,send_message,256,0);
                        }
                    
                }
                
                if(strcmp(recv_message,"unrestrict user")==0){
                    if(strcmp(type,"admin")==0){
                        strcpy(send_message, "\n Numele utilizatorului: \n"); //user nelogat
                        send(client_socket,send_message,256,0);
                        recv(client_socket,recv_message , 256, 0);
                        strcpy(restricted_username, recv_message);
                        sprintf(sql, "select * from user where username='%s'",restricted_username);
                        rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error \n");
                        }
                        else{
                            if(strcmp(data, restricted_username)==0){ //utilizatorul exista
                            sprintf(sql, "update user set vote_restriction=0 where username='%s';",restricted_username);
                            rc = sqlite3_exec(db, sql, callback, (void*)data, NULL);
                            
                            if( rc != SQLITE_OK ) {
                               fprintf(stderr, "SQL error:\n");
                            } else {
                                sprintf(send_message, "\n User-ului %s i-a fost scoasa restrictia  \n",restricted_username);
                               send(client_socket,send_message,256,0);
                            }
                        }
                            else{
                                strcpy(send_message, "\n Utilizatorul nu exista \n"); //user nelogat
                                send(client_socket,send_message,256,0);
                            }
                        }
                        
                    }
                    else{
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n"); //user nelogat
                        send(client_socket,send_message,256,0);
                        }
                    
                }
                
                if(strcmp(recv_message,"insert music")==0){
                    if(strcmp(type,"admin")==0){
                        strcpy(send_message, "\n Numele melodiei \n"); //user logat
                        write(client_socket, send_message, 256);
                        read(client_socket, recv_message, 256);
                        memset(insert_music, 0, sizeof(insert_music));
                        strcpy(insert_music, recv_message);
                        sprintf(sql, "select * from music where title='%s';",insert_music);
                        rc = sqlite3_exec(db, sql, password_callback, (void*)data, NULL);
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error:\n");
                        }
                        else{
                            if(strcmp(data,insert_music)==0){
                                strcpy(send_message, "\n Melodia exista \n");
                                write(client_socket, send_message, 256);
                            }
                            else{
                               strcpy(send_message, "\n Artistul: \n");
                                write(client_socket, send_message, 256);
                                read(client_socket, recv_message, 256);
                                memset(insert_artist, 0, sizeof(insert_artist));
                                strcpy(insert_artist, recv_message);
                                
                                strcpy(send_message, "\n Genul musical: \n");
                                write(client_socket, send_message, 256);
                                read(client_socket, recv_message, 256);
                                memset(insert_genre, 0, sizeof(insert_genre));
                                strcpy(insert_genre, recv_message);
                                
                                strcpy(send_message, "\n Link: \n");
                                write(client_socket, send_message, 256);
                                read(client_socket, recv_message, 256);
                                memset(insert_link, 0, sizeof(insert_genre));
                                strcpy(insert_link, recv_message);
                                sql[0] = 0;
                                data[0]=0;
                                sprintf(sql, "INSERT INTO music (artist,title,genre,votes,link) VALUES ('%s','%s','%s',0,'%s');",insert_artist,insert_music,insert_genre,insert_link);
                                rc = sqlite3_exec(db, sql, callback, (void*)data, NULL);
                                
                                if( rc != SQLITE_OK ) {
                                   fprintf(stderr, "SQL error:\n");
                                } else {
                                    memset(sql, 0, sizeof(sql));
                                    memset(data, 0, sizeof(data));
                                    strcpy(send_message, "\n Melodia a fost adaugata \n");
                                   write(client_socket, send_message, 256);
                                }
                                
                            }
                                
                        }
                        
                    }
                    
                    
                    else{
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n"); //user nelogat
                        write(client_socket, send_message, 256);
                        }
                }
                
                if(strcmp(recv_message,"delete user")==0){
                    if(strcmp(type,"admin")==0){
                        strcpy(send_message, "\n Numele utilizatorului : \n");
                        write(client_socket, send_message, 256);
                        read(client_socket, recv_message, 256);
                        sql[0] = 0;
                        data[0]=0;
                        sprintf(sql, "select * from user where username ='%s'",recv_message);
                        rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                        
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error \n");
                        }
                        else {
                            if(strcmp(data, username)==0){
                                strcpy(send_message, "\n Utilizatorul nu exista \n");
                                send(client_socket,send_message,256,0);
                            }
                            else{
                                    sprintf(sql, "select * from user where username ='%s'",recv_message);
                                    rc = sqlite3_exec(db,sql , id_callback, data3, NULL);
                                    if( rc != SQLITE_OK ) {
                                       fprintf(stderr, "SQL error \n");
                                    }
                                    else{
                                    strcpy(id_user, data3);
                                        sprintf(sql, "delete from vote where id_user='%s';",id_user);
                                        rc = sqlite3_exec(db,sql , username_callback, NULL, NULL);
                                        if( rc != SQLITE_OK ) {
                                           fprintf(stderr, "SQL error \n");
                                        }
                                        else{
                                            sprintf(sql, "delete from comments where id_user='%s';",id_user);
                                            rc = sqlite3_exec(db,sql , username_callback, NULL, NULL);
                                            if( rc != SQLITE_OK ) {
                                               fprintf(stderr, "SQL error \n");
                                            }
                                            else{
                                                sprintf(sql, "delete from user where username ='%s'",recv_message);
                                                rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                                                if( rc != SQLITE_OK ) {
                                                   fprintf(stderr, "SQL error \n");
                                                }
                                                else{
                                                    strcpy(send_message, "\n Utilizatorul a fost sters \n");
                                                    send(client_socket,send_message,256,0);
                                                }
                                            }
                                        }
                                        
                                    }
                                
                            }
                        
                        }
                    }
                    else{
                    strcpy(send_message, "\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n"); //user nelogat
                    write(client_socket, send_message, 256);
                    }
                }
                
                if(strcmp(recv_message,"see users")==0){
                    if(strcmp(type,"admin")==0){ //user logat
                    memset(data2, 0, sizeof(data2));
                    sprintf(sql, "select * from user");
                    strcpy(data2, "\n ----------USERS---------- \n\n");
                    rc = sqlite3_exec(db,sql , seeuser_callback, data2, NULL);
                    if( rc != SQLITE_OK ) {
                       fprintf(stderr, "SQL error \n");
                    }
                    else {
                        printf("SQL FACUT \n");
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, data2);
                        write(client_socket, send_message, sizeof(send_message));
                    }
                    }
                    else{
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n"); //user nelogat
                        write(client_socket, send_message, sizeof(send_message));
                        
                    }
                }
                
                if (strcmp(recv_message, "delete music")==0){
                    if(strcmp(type,"admin")==0){
                        strcpy(send_message, "\n Id-ul melodiei: \n");
                        write(client_socket, send_message, sizeof(send_message));
                        read(client_socket, recv_message, sizeof(recv_message));
                        sprintf(sql, "select * from music where id_song='%s';",recv_message);
                        rc = sqlite3_exec(db,sql , id_callback, data, NULL);
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error \n");
                        }
                        else {
                            if(strcmp(data, recv_message)!=0){
                                strcpy(send_message, "\n Melodia nu exista \n");
                                send(client_socket,send_message,256,0);
                            }
                            else{
                                sprintf(sql, "delete from vote where id_song='%s';",recv_message);
                                rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                                if( rc != SQLITE_OK ) {
                                   fprintf(stderr, "SQL error \n");
                                }
                                else{
                                    sprintf(sql, "delete from comments where id_song='%s';",recv_message);
                                    rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                                    if( rc != SQLITE_OK ) {
                                       fprintf(stderr, "SQL error \n");
                                    }
                                    else{
                                    sprintf(sql, "delete from music where id_song='%s';",recv_message);
                                    rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                                    if( rc != SQLITE_OK ) {
                                       fprintf(stderr, "SQL error \n");
                                    }
                                    
                                    strcpy(send_message, "\n Melodia a fost stearsa \n");
                                    send(client_socket,send_message,256,0);
                                    }
                                }
                            }
                        }
                    }
                    else{
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT CA UTILIZATOR ADMIN \n"); //user nelogat
                        write(client_socket, send_message, sizeof(send_message));
                        
                    }
                }
                
                if (strcmp(recv_message, "see comments")==0){
                    if((strcmp(type,"admin")==0 || strcmp(type,"normal_user")==0)){
                        strcpy(send_message, "\n Id-ul melodiei: \n");
                        write(client_socket, send_message, sizeof(send_message));
                        read(client_socket, recv_message, sizeof(recv_message));
                        sprintf(sql, "select * from music where id_song='%s';",recv_message);
                        rc = sqlite3_exec(db,sql , id_callback, data, NULL);
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error \n");
                        }
                        else {
                            if(strcmp(data, recv_message)!= 0){
                                strcpy(send_message, "\n Melodia nu exista \n");
                                send(client_socket,send_message,256,0);
                            }
                            else{
                                strcpy(data2, "\n ----------COMMENTS---------- \n\n");
                                sprintf(sql, "select  distinct u.username,c.comm  from comments c, user u,music m where c.id_user=u.id_user and  c.id_song='%s'; ",recv_message);
                                rc = sqlite3_exec(db,sql , seemusic_callback, data2, NULL);
                                if( rc != SQLITE_OK ) {
                                   fprintf(stderr, "SQL error \n");
                                }
                                else{
                                    strcpy(send_message, data2);
                                    send(client_socket,send_message,256,0);
                                }
                            }
                        }
                        
                        
                    }
                    else{
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT \n"); //user nelogat
                        write(client_socket, send_message, sizeof(send_message));
                        
                    }
                }
                
                if (strcmp(recv_message, "insert comment")==0){
                    if((strcmp(type,"admin")==0 || strcmp(type,"normal_user")==0)){
                        strcpy(send_message, "\n Id-ul melodiei: \n");
                        write(client_socket, send_message, sizeof(send_message));
                        read(client_socket, recv_message, sizeof(recv_message));
                        sprintf(sql, "select * from music where id_song='%s';",recv_message);
                        strcpy(id_music, recv_message);
                        rc = sqlite3_exec(db,sql , id_callback, data, NULL);
                        if( rc != SQLITE_OK ) {
                           fprintf(stderr, "SQL error \n");
                        }
                        else {
                            if(strcmp(data, recv_message)!= 0){
                                strcpy(send_message, "\n Melodia nu exista \n");
                                send(client_socket,send_message,256,0);
                            }
                            else{
                                strcpy(send_message, "\n Comentariul: \n");
                                write(client_socket, send_message, sizeof(send_message));
                                read(client_socket, recv_message, sizeof(recv_message));
                                strcpy(insert_comment, recv_message);
                                sprintf(sql, "select * from user where username ='%s'",username);
                                rc = sqlite3_exec(db,sql , id_callback, data3, NULL);
                                if( rc != SQLITE_OK ) {
                                   fprintf(stderr, "SQL error \n");
                                }
                                else{
                                strcpy(id_user, data3);
                                    sprintf(sql, "insert into comments(id_song,id_user,comm) values ('%s','%s','%s')",id_music,id_user,insert_comment);
                                    rc = sqlite3_exec(db, sql, callback, NULL, NULL);
                                    
                                    if( rc != SQLITE_OK ) {
                                       fprintf(stderr, "SQL error:\n");
                                    } else {
                                        memset(sql, 0, sizeof(sql));
                                        strcpy(send_message, "\n Comentariul a fost adaugata \n");
                                       write(client_socket, send_message, 256);
                                    }
                                }
                            }
                        }
                    }
                    else{
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT \n"); //user nelogat
                        write(client_socket, send_message, sizeof(send_message));
                    }
                }
                
                if (strcmp(recv_message, "commands admin")==0){
                    if(strcmp(type,"normal_user")==0){
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT CA ADMIN \n"); //user nelogat
                        write(client_socket, send_message, sizeof(send_message));
                    }
                    else{
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, "\n OK \n"); //user nelogat
                        write(client_socket, send_message, sizeof(send_message));
                    }
                }
                
                if (strcmp(recv_message, "delete comment")==0){
                    if(strcmp(type,"normal_user")==0){
                        sprintf(sql, "select * from user where username ='%s'",username);
                        rc = sqlite3_exec(db,sql , id_callback, data3, NULL);
                                    if( rc != SQLITE_OK ) {
                                    fprintf(stderr, "SQL error \n");
                                    }
                                    else{
                                        strcpy(id_user, data3);
                                        sprintf(sql, "select distinct c.id_comm,m.artist,m.title,c.comm from comments c,user u, music m where c.id_user='%s' and m.id_song=c.id_song;",id_user);
                                        strcpy(data2, "\n ----------YOUR COMMENTS---------- \n\n");
                                        rc = sqlite3_exec(db,sql , seemusic_callback, data2, NULL);
                                        if( rc != SQLITE_OK ) {
                                                fprintf(stderr, "SQL error \n");
                                                              }
                                        else {
                                                printf("SQL FACUT \n");
                                                memset(send_message, 0, sizeof(send_message));
                                                strcpy(send_message, data2);
                                            strcat(send_message, "\n\n ALEGETI COMENTARIUL : ");
                                                write(client_socket, send_message, sizeof(send_message));
                                                read(client_socket,recv_message,sizeof(recv_message));
                                            strcpy(comm, recv_message);
                                            sprintf(sql,"select * from comments where id_song='%s' and id_user='%s' ",comm,id_user);
                                            rc = sqlite3_exec(db,sql , id_callback, data, NULL);
                                            if( rc != SQLITE_OK ) {
                                            fprintf(stderr, "SQL error \n");
                                            }
                                                else{
                                                    if(strcmp(data,comm)!=0){
                                                        sprintf(sql, "delete from comments where id_comm='%s';",comm);
                                                        rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                                                        if( rc != SQLITE_OK ) {
                                                           fprintf(stderr, "SQL error \n");
                                                        }
                                                        
                                                        else{
                                                            strcpy(send_message, "\n Comentariul a fost stears \n");
                                                            send(client_socket,send_message,256,0);
                                                        }
                                                        
                                                        
                                                    }
                                                    else{
                                                        strcpy(send_message, "\n COMENTARIUL NU EXISTA \n"); //user nelogat
                                                        write(client_socket, send_message, sizeof(send_message));
                                                     }
                                                    }
                                                
                                            }
                                        }
                    }
                    
                    if(strcmp(type, "admin")==0){
                            
                        strcpy(send_message, "\n Id-ul melodiei: \n");
                                               write(client_socket, send_message, sizeof(send_message));
                                               read(client_socket, recv_message, sizeof(recv_message));
                                               sprintf(sql, "select * from music where id_song='%s';",recv_message);
                                               rc = sqlite3_exec(db,sql , id_callback, data, NULL);
                                               if( rc != SQLITE_OK ) {
                                                  fprintf(stderr, "SQL error \n");
                                               }
                                               else {
                                                   if(strcmp(data, recv_message)!= 0){
                                                       strcpy(send_message, "\n Melodia nu exista \n");
                                                       write(client_socket, send_message, sizeof(send_message));
                                                   }
                                                   else{
                                                       strcpy(data2, "\n ----------COMMENTS---------- \n\n");
                                                       sprintf(sql, "select  distinct c.id_comm, u.username,c.comm  from comments c, user u,music m where c.id_user=u.id_user and  c.id_song='%s'; ",recv_message);
                                                       rc = sqlite3_exec(db,sql , seemusic_callback, data2, NULL);
                                                       if( rc != SQLITE_OK ) {
                                                          fprintf(stderr, "SQL error \n");
                                                       }
                                                       else{
                                                           strcpy(send_message, data2);
                                                           strcat(send_message, "\n Selectati ID-ul : ");
                                                           write(client_socket, send_message, sizeof(send_message));
                                                           read(client_socket, recv_message, sizeof(recv_message));
                                                           
                                                           strcpy(comm, recv_message);
                                                           sprintf(sql,"select * from comments where id_song='%s';",comm);
                                                           rc = sqlite3_exec(db,sql , id_callback, data, NULL);
                                                           if( rc != SQLITE_OK ) {
                                                           fprintf(stderr, "SQL error \n");
                                                           }
                                                               else{
                                                                   if(strcmp(data,comm)!=0){
                                                                       sprintf(sql, "delete from comments where id_comm='%s';",comm);
                                                                       rc = sqlite3_exec(db,sql , username_callback, data, NULL);
                                                                       if( rc != SQLITE_OK ) {
                                                                          fprintf(stderr, "SQL error \n");
                                                                       }
                                                                       
                                                                       else{
                                                                           strcpy(send_message, "\n Comentariul a fost stears \n");
                                                                           send(client_socket,send_message,256,0);
                                                                       }
                                                                       
                                                                       
                                                                   }
                                                                   else{
                                                                       strcpy(send_message, "\n COMENTARIUL NU EXISTA \n"); //user nelogat
                                                                       write(client_socket, send_message, sizeof(send_message));
                                                                    }
                                                                   }
                                                           
                                                       }
                                                   }
                                               }
                                               
                        
                        
                    }
                    else{
                        memset(send_message, 0, sizeof(send_message));
                        strcpy(send_message, "\n TREBUIE SA FII LOGAT \n"); //user nelogat
                        write(client_socket, send_message, sizeof(send_message));
                    }
                }
                
                
                sqlite3_close(db);
            }
        }
    }
    close(client_socket);
    
    return 0;
}

