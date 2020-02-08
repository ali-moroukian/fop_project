#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
#include <dir.h>
#include <process.h>
#include <time.h>
#include <winsock2.h>
#include "cJSON.h"
#define MAX 1024
#define PORT 12345
#define SA struct sockaddr

void connect_client();
void answering(int server_socket, FILE *users, FILE *channels, FILE *AuthToken, FILE *channel_member);
int maximum(int a, int b);
int search_file(FILE *fptr, char *wanted);
int search_file_gets(FILE *fptr, char *wanted);

int main()
{
    while(1)
    {
        connect_client();
    }
}

void connect_client()
{
    static int counter = 0;
    int server_socket, client_socket;
    struct sockaddr_in server, client;

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

    // Create and verify socket
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    else if(counter == 0 && server_socket != INVALID_SOCKET)
        printf("Socket successfully created..\n");

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed...\n");
        exit(0);
    }
    else if(counter == 0 && (bind(server_socket, (SA *)&server, sizeof(server))) != 0)
        printf("Socket successfully bound..\n");

    // Now server is ready to listen and verify
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else if(counter == 0 && listen(server_socket, 5) == 0)
        printf("Server listening..\n");

    // Accept the data packet from client and verify
    int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server acceptance failed...\n");
        exit(0);
    }

    mkdir("Resources");
    mkdir("Resources/channels");
    FILE *users, *channels, *AuthToken, *channel_member;
    users = fopen("./Resources/users.txt", "a+");
    channels = fopen("./Resources/channels.txt", "a+");
    if(counter == 0)
    {
        AuthToken = fopen("./Resources/AuthToken.txt", "w+");
        channel_member = fopen("./Resources/channel member.txt", "w+");
    }
    else
    {
        AuthToken = fopen("./Resources/AuthToken.txt", "a+");
        channel_member = fopen("./Resources/channel member.txt", "a+");
    }

    answering(client_socket, users, channels, AuthToken, channel_member);

    fclose(users);
    fclose(channels);
    fclose(AuthToken);
    fclose(channel_member);
    // Close the socket
    closesocket(server_socket);
    counter++;
}

void answering(int server_socket, FILE *users, FILE *channels, FILE *AuthToken, FILE *channel_member)
{
    char order[MAX], temp[1000], username[100], password[100], name[100], token[100], massage[1000], search[100], address[100], channel_name[100];
    memset(order, 0, MAX);
    recv(server_socket, order, MAX, 0);
    printf("CLIENT : %s", order);
    if(!memcmp(order, "register", 8))
    {
        sscanf(order, "%*8c %[^,] %*1c %s", username, password);
        int flag = search_file(users, username);
        if(flag)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"This username in NOT VALID.\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else
        {
            fprintf(users, "%s %s\n", username, password);
            char str[100];
            strcpy(str, "{\"type\":\"Successful\",\"content\":\"\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
    }
    else if(!memcmp(order, "login", 5))
    {
        sscanf(order, "%*5c %[^,] %*1c %s", username, password);
        int flag_username = search_file(users, username);
        strcpy(search, username);
        strcat(search, " ");
        strcat(search, password);
        strcat(search, "\n");
        int flag_password = search_file_gets(users, search);
        if(flag_username == 0)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"You should create account first. Your account is not found\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else if(flag_password == 1)
        {
            srand(time(NULL));
            int random;
            char equal_random, authtoken[16];
            for(int i = 0; i < 15; i++)
            {
                random = rand() % 26 + 65;
                char equal_random = random;
                authtoken[i] = equal_random;
            }
            authtoken[15] = '\0';
            fprintf(AuthToken, "%s %s\n", authtoken, username);
            char str[100];
            strcpy(str, "{\"type\":\"AuthToken\",\"content\":\"");
            strcat(str, authtoken);
            strcat(str, "\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else
        {
            cJSON *answer = cJSON_CreateObject();
            cJSON_AddStringToObject(answer, "type", "Error");
            cJSON_AddStringToObject(answer, "content", "Your PASSWORD is wrong");
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"Your PASSWORD is wrong\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
    }
    else if(!memcmp(order, "create channel", 14))
    {
        sscanf(order, "%*14c %[^,] %*1c %s", name, token);
        int flag_name = search_file(channels, name);
        int flag_token = search_file(AuthToken, token);
        if(flag_token == 0)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"Your AUTHTOKEN is not valid\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else if(flag_name)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"This name in NOT VALID.\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else if(flag_name == 0 && flag_token == 1)
        {
            rewind(AuthToken);
            while(1)
            {
                if(feof(AuthToken))
                    break;
                fscanf(AuthToken, "%s", search);
                fscanf(AuthToken, "%s", username);
                if( !memcmp(token, search, maximum(strlen(token), strlen(search)) ) )
                    break;
            }
            strcpy(massage, username);
            strcat(massage, " created ");
            strcat(massage, name);
            strcat(massage, ".");
            FILE *fptr;
            strcpy(address, "./Resources/channels/");
            strcat(address, username);
            strcat(address, ".txt");
            fptr = fopen(address, "w+");
            fprintf(fptr, "%s\n", name);
            fclose(fptr);
            strcpy(address, "./Resources/channels/");
            strcat(address, name);
            strcat(address, ".txt");
            fptr = fopen(address, "a+");
            fprintf(fptr, "%s %s\n", "server", massage);
            fclose(fptr);
            fprintf(channels, "%s\n", name);
            fprintf(channel_member, "%s %s yes\n", name, username);
            char str[100];
            strcpy(str, "{\"type\":\"Successful\",\"content\":\"\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
    }
    else if(!memcmp(order, "join channel", 12))
    {
        sscanf(order, "%*12c %[^,] %*1c %s", name, token);
        int flag_name = search_file(channels, name);
        int flag_token = search_file(AuthToken, token);
        if(flag_token == 0)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"Your AUTHTOKEN is not valid\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else if(flag_name == 0)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"This channel doesn't exist.\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else if(flag_name == 1 && flag_token == 1)
        {
            rewind(AuthToken);
            while(1)
            {
                if(feof(AuthToken))
                    break;
                fscanf(AuthToken, "%s", search);
                fscanf(AuthToken, "%s", username);
                if( !memcmp(token, search, maximum(strlen(token), strlen(search)) ) )
                    break;
            }
            strcpy(massage, username);
            strcat(massage, " joined.");
            FILE *fptr;
            strcpy(address, "./Resources/channels/");
            strcat(address, username);
            strcat(address, ".txt");
            fptr = fopen(address, "w+");
            fprintf(fptr, "%s\n", name);
            fclose(fptr);
            strcpy(address, "./Resources/channels/");
            strcat(address, name);
            strcat(address, ".txt");
            fptr = fopen(address, "a+");
            char server[100];
            strcpy(server, "server");
            fprintf(fptr, "%s %s\n", server, massage);
            fclose(fptr);
            fprintf(channels, "%s\n", name);
            fprintf(channel_member, "%s %s yes\n", name, username);
            char str[100];
            strcpy(str, "{\"type\":\"Successful\",\"content\":\"\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
    }
    else if(!memcmp(order, "send", 4))
    {
        sscanf(order, "%*4c %[^,] %*1c %s", massage, token);
        int flag = search_file(AuthToken, token);
        if(flag == 0)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"Your AUTHTOKEN is not valid\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else
        {
            rewind(AuthToken);
            while(1)
            {
                if(feof(AuthToken))
                    break;
                fscanf(AuthToken, "%s", search);
                fscanf(AuthToken, "%s", username);
                if( !memcmp(token, search, maximum(strlen(token), strlen(search)) ) )
                    break;
            }
            FILE *fptr;
            strcpy(address, "./Resources/channels/");
            strcat(address, username);
            strcat(address, ".txt");
            printf("%s\n", address);
            fptr = fopen(address, "r+");
            fgets(channel_name, MAX, fptr);
            channel_name[strlen(channel_name) - 1] = '\0';
            fclose(fptr);
            strcpy(address, "./Resources/channels/");
            strcat(address, channel_name);
            strcat(address, ".txt");
            printf("%s\n", address);
            fptr = fopen(address, "a+");
            fprintf(fptr, "%s %s\n", username, massage);
            fclose(fptr);
            char str[100];
            strcpy(str, "{\"type\":\"Successful\",\"content\":\"\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
    }
    else if(!memcmp(order, "refresh", 7))
    {
        sscanf(order, "%*7c %s", token);
        int flag = search_file(AuthToken, token);
        if(flag == 0)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"Your AUTHTOKEN is not valid\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else
        {
            rewind(AuthToken);
            while(1)
            {
                if(feof(AuthToken))
                    break;
                fscanf(AuthToken, "%s", search);
                fscanf(AuthToken, "%s", username);
                if( !memcmp(token, search, maximum(strlen(token), strlen(search)) ) )
                    break;
            }
            FILE *fptr;
            strcpy(address, "./Resources/channels/");
            strcat(address, username);
            strcat(address, ".txt");
            fptr = fopen(address, "r+");
            fgets(channel_name, MAX, fptr);
            channel_name[strlen(channel_name) - 1] = '\0';
            fclose(fptr);
            strcpy(address, "./Resources/channels/");
            strcat(address, channel_name);
            strcat(address, ".txt");
            fptr = fopen(address, "a+");
            char str[1000], sender[100], next_temp[1000];
            strcpy(str, "{\"type\":\"list\",\"content\":[");
            int counter = 0;
            rewind(fptr);
            while(1)
            {
                if(feof(fptr))
                    break;
                if(counter)
                {
                    strcat(str, ",");
                    strcpy(temp, next_temp);
                    fgets(next_temp, MAX, fptr);
                }
                else
                {
                    fgets(temp, MAX, fptr);
                    fgets(next_temp, MAX, fptr);
                }
                sscanf(temp, "%s %[^\n]", sender, massage);
                strcat(str, "{\"sender\":\"");
                strcat(str, sender);
                strcat(str, "\",\"content\":\"");
                strcat(str, massage);
                strcat(str, "\"}");
                counter++;
            }
            fclose(fptr);
            strcat(str, "]}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
    }
    else if(!memcmp(order, "channel members", 15))
    {
        sscanf(order, "%*15c %s", token);
        int flag = search_file(AuthToken, token);
        if(flag == 0)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"Your AUTHTOKEN is not valid\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else
        {
            rewind(AuthToken);
            while(1)
            {
                if(feof(AuthToken))
                    break;
                fscanf(AuthToken, "%s", search);
                fscanf(AuthToken, "%s", username);
                if( !memcmp(token, search, maximum(strlen(token), strlen(search)) ) )
                    break;
            }
            FILE *fptr;
            strcpy(address, "./Resources/channels/");
            strcat(address, username);
            strcat(address, ".txt");
            fptr = fopen(address, "r+");
            fgets(channel_name, MAX, fptr);
            channel_name[strlen(channel_name) - 1] = '\0';
            fclose(fptr);
            char str[1000], boolean[100], member[1000][100];
            int counter = 0, i = 0;
            strcpy(str, "{\"type\":\"list\",\"content\":[");
            while(1)
            {
                int flag = 1;
                if(feof(channel_member))
                    break;
                fgets(temp, MAX, channel_member);
                sscanf(temp, "%s %s %s", name, username, boolean);
                if(!strcmp(name, channel_name))
                {
                    for(int j = 0; j < i; j++)
                    {
                        if(!strcmp(username, member[j]))
                            flag = 0;
                    }
                    if(!strcmp(boolean, "yes") && counter != 0 && flag == 1)
                    {
                        strcpy(member[i++], username);
                        strcat(str, ",\"");
                        strcat(str, username);
                        strcat(str, "\"");
                        counter++;
                    }
                    else if(!strcmp(boolean, "yes") && flag == 1)
                    {
                        strcpy(member[i++], username);
                        strcat(str, "\"");
                        strcat(str, username);
                        strcat(str, "\"");
                        counter++;
                    }
                }
            }
            strcat(str, "]}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
    }
    else if(!memcmp(order, "leave", 5))
    {
        sscanf(order, "%*5c %s", token);
        int flag = search_file(AuthToken, token);
        if(flag == 0)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"Your AUTHTOKEN is not valid\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else
        {
            rewind(AuthToken);
            while(1)
            {
                if(feof(AuthToken))
                    break;
                fscanf(AuthToken, "%s", search);
                fscanf(AuthToken, "%s", username);
                if( !memcmp(token, search, maximum(strlen(token), strlen(search)) ) )
                    break;
            }
            FILE *fptr;
            strcpy(address, "./Resources/channels/");
            strcat(address, username);
            strcat(address, ".txt");
            fptr = fopen(address, "r+");
            fgets(channel_name, MAX, fptr);
            channel_name[strlen(channel_name) - 1] = '\0';
            fclose(fptr);
            fptr = fopen(address, "w+");
            fclose(fptr);
            strcpy(address, "./Resources/channels/");
            strcat(address, channel_name);
            strcat(address, ".txt");
            fptr = fopen(address, "a+");
            strcpy(massage, username);
            strcat(massage, " leaved the channel.\n");
            char server[100];
            strcpy(server, "server");
            fprintf(fptr, "%s %s", server, massage);
            fclose(fptr);

            int counter = 0;
            char all_member[1000], next_temp[100], shit[100];
            strcpy(all_member, "");
            rewind(channel_member);
            while(1)
            {
                if(feof(channel_member))
                    break;
                if(counter)
                {
                    strcpy(temp, next_temp);
                    fgets(next_temp, MAX, channel_member);
                    sscanf(temp, "%s %s", shit, search);
                    if( !memcmp(username, search, maximum(strlen(username), strlen(search)) ) )
                        continue;
                    strcat(all_member, temp);
                    counter++;
                }
                else
                {
                    fgets(temp, MAX, channel_member);
                    fgets(next_temp, MAX, channel_member);
                    sscanf(temp, "%s %s", shit, search);
                    counter++;
                    if( !memcmp(username, search, maximum(strlen(username), strlen(search)) ) )
                        continue;
                    strcat(all_member, temp);
                }
            }
            strcat(all_member, channel_name);
            strcat(all_member, " ");
            strcat(all_member, username);
            strcat(all_member, " ");
            strcat(all_member, "no!\n");
            fclose(channel_member);
            channel_member = fopen("./Resources/channel member.txt", "w+");
            fprintf(channel_member, "%s", all_member);
            char str[100];
            strcpy(str, "{\"type\":\"Successful\",\"content\":\"\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
    }
    else if(!memcmp(order, "logout", 6))
    {
        sscanf(order, "%*6c %s", token);
        int flag = search_file(AuthToken, token);
        if(flag == 0)
        {
            char str[100];
            strcpy(str, "{\"type\":\"Error\",\"content\":\"Your AUTHTOKEN is not valid\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
        else
        {
            int counter = 0;
            char all_tokens[1000], next_temp[100];
            strcpy(all_tokens, "");
            rewind(AuthToken);
            while(1)
            {
                if(feof(AuthToken))
                    break;
                if(counter)
                {
                    strcpy(temp, next_temp);
                    fgets(next_temp, MAX, AuthToken);
                    sscanf(temp, "%s", search);
                    if( !memcmp(token, search, maximum(strlen(token), strlen(search)) ) )
                        continue;
                    strcat(all_tokens, temp);
                    counter++;
                }
                else
                {
                    fgets(temp, MAX, AuthToken);
                    fgets(next_temp, MAX, AuthToken);
                    sscanf(temp, "%s", search);
                    counter++;
                    if( !memcmp(token, search, maximum(strlen(token), strlen(search)) ) )
                        continue;
                    strcat(all_tokens, temp);
                }
            }
            fclose(AuthToken);
            AuthToken = fopen("./Resources/AuthToken.txt", "w+");
            fprintf(AuthToken, "%s", all_tokens);
            char str[100];
            strcpy(str, "{\"type\":\"Successful\",\"content\":\"\"}");
            printf("%s\n", str);
            send(server_socket, str, sizeof(str), 0);
        }
    }
}

int maximum(int a, int b)
{
    if(a > b)
        return a;
    else
        return b;
}

int search_file(FILE *fptr, char *wanted)
{
    int flag = 0;
    char *search;
    rewind(fptr);
    while(1)
    {
        if(feof(fptr))
            break;
        fscanf(fptr, "%s", search);
        if( !memcmp(wanted, search, maximum(strlen(wanted), strlen(search)) ) )
        {
            flag = 1;
            break;
        }
    }
    return flag;
}

int search_file_gets(FILE *fptr, char *wanted)
{
    int flag = 0;
    char *search;
    rewind(fptr);
    while(1)
    {
        if(feof(fptr))
            break;
        fgets(search, MAX, fptr);
        if( !memcmp(wanted, search, maximum(strlen(wanted), strlen(search)) ) )
        {
            flag = 1;
            break;
        }
    }
    return flag;
}
