#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <winsock2.h>
#include "cJSON.h"
#define MAX 1024
#define PORT 12345
#define SA struct sockaddr

void connect_disconnect(char* str);
void user_menu(char* str);
void main_menu(char* str, char* token);
void channel_menu(char* str, char* token);

int main()
{
    char* const str = (char*)malloc(MAX * sizeof(char));
    memset(str, 0, MAX);
    user_menu(str);
}

void connect_disconnect(char* str)
{
    int client_socket;
	struct sockaddr_in servaddr;

	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        exit(0);
    }

	// Create and verify socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}

	// Assign IP and port
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// Connect the client socket to server socket
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed...\n");
		exit(0);
	}
	send(client_socket, str, strlen(str), 0);
	memset(str, 0, MAX);
	recv(client_socket, str, MAX, 0);
	closesocket(client_socket);
}

void user_menu(char* str)
{
    static int counter = 0;
    int num;
    char username[50], password[50];
    if(counter == 0)
        printf("\t\t\t\t\t**** WELCOME ****\n\n");
    counter++;
    printf("\t\t\t\t\tUser Menu\n\t\t\t\t\t1. Create Account\n\t\t\t\t\t2. Login\n\t\t\t\t\t3. Exit\n");
    fflush(stdin);
    printf("\nEnter the Number : ");
    scanf("%d", &num);
    if(num == 1)
    {
        printf("\nUserName : ");
        scanf(" %[^\n]", username);
        printf("PassWord : ");
        scanf(" %[^\n]", password);
        memset(str, 0, MAX);
        strcpy(str, "register ");
        strcat(str, username);
        strcat(str, ", ");
        strcat(str, password);
        strcat(str, "\n");
        connect_disconnect(str);
        char str_copy[100];
        strcpy(str_copy, str);
        if(memcmp(get_type_item(str_copy), "Error", 5))
            printf("\n\t\t\t\t\tYou have successfully registered.\n\t\t\t\t\tLog in to chat\n\n");
        else
        {
            strcpy(str_copy, str);
            printf("\n\t\t\t\t\tWARNING : %s\n\t\t\t\t\tPlease try again!\n\n", get_content_item(str_copy));
        }
        user_menu(str);
    }
    else if(num == 2)
    {
        printf("\nUserName : ");
        scanf(" %[^\n]", username);
        printf("PassWord : ");
        scanf(" %[^\n]", password);
        memset(str, 0, MAX);
        strcpy(str, "login ");
        strcat(str, username);
        strcat(str, ", ");
        strcat(str, password);
        strcat(str, "\n");
        connect_disconnect(str);
        char str_copy[100];
        strcpy(str_copy, str);
        if(memcmp(get_type_item(str_copy), "Error", 5))
        {
            strcpy(str_copy, str);
            printf("\n\t\t\t\t\tYou have successfully log in.\n\n");
            main_menu(str, get_content_item(str_copy));
        }
        else
        {
            strcpy(str_copy, str);
            printf("\n\t\t\t\t\tWARNING : %s\n\t\t\t\t\tPlease try again!\n\n", get_content_item(str_copy));
            user_menu(str);
        }
    }
    else if(num == 3)
        exit(1);
    else
    {
        printf("\t\t\t\t\tNot Valid\n\n");
        user_menu(str);
    }
}

void main_menu(char* str, char* token)
{
    int num;
    printf("\t\t\t\t\tWhat do you want to do?\n\t\t\t\t\t1. New Channel\n\t\t\t\t\t2. Join Channel\n\t\t\t\t\t3. logout\n");
    fflush(stdin);
    printf("\nEnter the Number : ");
    scanf("%d", &num);
    if(num == 1)
    {
        char name[50];
        printf("\nChannel Name : ");
        scanf(" %[^\n]", name);
        memset(str, 0, MAX);
        strcpy(str, "create channel ");
        strcat(str, name);
        strcat(str, ", ");
        strcat(str, token);
        strcat(str, "\n");
        connect_disconnect(str);
        char str_copy[100];
        strcpy(str_copy, str);
        if(memcmp(get_type_item(str_copy), "Error", 5))
        {
            printf("\t\t\t\t\t%s channel have successfully created\n", name);
            channel_menu(str, token);
        }
        else
        {
            strcpy(str_copy, str);
            printf("\t\t\t\t\tWARNING : %s\n", get_content_item(str_copy));
            main_menu(str, token);
        }
    }
    if(num == 2)
    {
        char name[50];
        printf("Channel Name : ");
        scanf(" %[^\n]", name);
        memset(str, 0, MAX);
        strcpy(str, "join channel ");
        strcat(str, name);
        strcat(str, ", ");
        strcat(str, token);
        strcat(str, "\n");
        connect_disconnect(str);
        char str_copy[100];
        strcpy(str_copy, str);
        if(memcmp(get_type_item(str_copy), "Error", 5))
        {
            printf("\t\t\t\t\tYou have successfully joined to %s channel\n", name);
            channel_menu(str, token);
        }
        else
        {
            strcpy(str_copy, str);
            printf("\t\t\t\t\tWARNING : %s\n", get_content_item(str_copy));
            main_menu(str, token);
        }
    }
    if(num == 3)
    {
        memset(str, 0, MAX);
        strcpy(str, "logout ");
        strcat(str, token);
        strcat(str, "\n");
        connect_disconnect(str);
        char str_copy[100];
        strcpy(str_copy, str);
        if(memcmp(get_type_item(str_copy), "Error", 5))
        {
            printf("\t\t\t\t\tYou have successfully logout\n");
            user_menu(str);
        }
        else
        {
            strcpy(str_copy, str);
            printf("\t\t\t\t\tWARNING : %s\n", get_content_item(str_copy));
            user_menu(str);
        }
    }
    else
    {
        printf("\t\t\t\t\tNot Valid\n\n");
        main_menu(str, token);
    }
}

void channel_menu(char* str, char* token)
{
    int num;
    printf("\n\t\t\t\t\t1. Send Massage\n\t\t\t\t\t2. Refresh\n\t\t\t\t\t3. Channel Members\n\t\t\t\t\t4. Leave Channel\n");
    fflush(stdin);
    printf("\nEnter the Number : ");
    scanf("%d", &num);
    if(num == 1)
    {
        char massage[200];
        printf("\nEnter your Massage : ");
        scanf(" %[^\n]", massage);
        memset(str, 0, MAX);
        strcpy(str, "send ");
        strcat(str, massage);
        strcat(str, ", ");
        strcat(str, token);
        strcat(str, "\n");
        connect_disconnect(str);
        char str_copy[100];
        strcpy(str_copy, str);
        if(memcmp(get_type_item(str_copy), "Error", 5))
        {
            channel_menu(str, token);
        }
        else
        {
            strcpy(str_copy, str);
            printf("\t\t\t\t\tWARNING : %s\n", get_content_item(str_copy));
            channel_menu(str, token);
        }
    }
        else if(num == 2)
    {
        memset(str, 0, MAX);
        strcpy(str, "refresh ");
        strcat(str, token);
        strcat(str, "\n");
        connect_disconnect(str);
        char str_copy[10000];
        strcpy(str_copy, str);
        if(memcmp(get_type_item(str_copy), "Error", 5))
        {
            int counter = get_num_of_array(str);
			for(int i = 1; i <= counter; i++)
			{
			    strcpy(str_copy, str);
				printf("\t\t\t\t\t");
				if(memcmp(get_sender_content(str_copy, i), "server", 6))
                {
                    strcpy(str_copy, str);
                    printf("%s :  ", get_sender_content(str_copy, i));
                }
                strcpy(str_copy, str);
				printf("%s\n", get_massage_content(str_copy, i));
			}
        }
        else
        {
            strcpy(str_copy, str);
            printf("\t\t\t\t\tWARNING : %s\n", get_content_item(str_copy));
        }
        channel_menu(str, token);
    }
    else if(num == 3)
    {
        memset(str, 0, MAX);
        strcpy(str, "channel members ");
        strcat(str, token);
        strcat(str, "\n");
        connect_disconnect(str);
        char str_copy[100];
        strcpy(str_copy, str);
        if(memcmp(get_type_item(str_copy), "Error", 5))
        {
            int counter = get_num_of_array_str(str);
			printf("Channel members :\n");
			for(int i = 1; i <= counter; i++)
			{
			    strcpy(str_copy, str);
			    printf("\t\t\t\t\t%s\n", get_content_array_str(str_copy, i));
			}
        }
        else
        {
            strcpy(str_copy, str);
            printf("\t\t\t\t\tWARNING : %s\n", get_content_item(str_copy));
        }
        channel_menu(str, token);
    }
    else if(num == 4)
    {
        memset(str, 0, MAX);
        strcpy(str, "leave ");
        strcat(str, token);
        strcat(str, "\n");
        connect_disconnect(str);
        char str_copy[100];
        strcpy(str_copy, str);
        if(memcmp(get_type_item(str_copy), "Error", 5))
        {
            printf("\t\t\t\t\tYou Leave the Channel\n");
            main_menu(str, token);
        }
        else
        {
            strcpy(str_copy, str);
            printf("\t\t\t\t\tWARNING : %s\n", get_content_item(str_copy));
            channel_menu(str, token);
        }
    }
    else
    {
        printf("\t\t\t\t\tNot Valid\n\n");
        channel_menu(str, token);
    }
}
