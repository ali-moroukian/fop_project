#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_type_item(char *str)
{
    char *type;
    type = strtok(str, "\"");
    for(int i = 0; i < 3; i++)
        type = strtok(NULL, "\"");
    return type;
}

char *get_content_item(char *str)
{
    char *content;
    content = strtok(str, "\"");
    for(int i = 0; i < 7; i++)
        content = strtok(NULL, "\"");
    return content;
}

char *get_sender_content(char *str, int k)
{
    char *sender;
    sender = strtok(str, "\"");
    for(int i = 0; i < 8*k + 1; i++)
        sender = strtok(NULL, "\"");
    return sender;
}

char *get_massage_content(char *str, int k)
{
    char *massage;
    massage = strtok(str, "\"");
    for(int i = 0; i < 8*k + 5; i++)
        massage = strtok(NULL, "\"");
    return massage;
}

int get_num_of_array(char *str)
{
    int i = 0, counter = 0;
    while(str[i] != '\0')
    {
        if(str[i] == '{' || str[i] == '}')
            counter++;
        i++;
    }
    return (counter/2 - 1);
}

char *get_content_array_str(char *str, int k)
{
    char *array;
    array = strtok(str, "\"");
    for(int i = 1; i < 2*k + 6; i++)
    {
        array = strtok(NULL, "\"");
    }
    return array;
}

int get_num_of_array_str(char *str)
{
    int i = 0, counter = 0;
    while(str[i] != '\0')
    {
        if(str[i] == '\"')
            counter++;
        i++;
    }
    return (counter/2 - 3);
}
