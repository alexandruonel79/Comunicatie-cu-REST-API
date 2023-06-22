#include <stdlib.h> /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "processCommands.h"

// functiile din laborator, usor adaptate
char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (query_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    char add[100] = "Host: ";
    strcat(add, host);
    compute_message(message, add);
    /// in functie de nr de cookies primit parametru
    /// adaug si cookie ul de login si token ul de
    /// autentificare
    if (cookies_count != 0)
    {
        if (cookies_count >= 1)
        {
            char addCookie[MAXCOOKIELEN] = "Cookie: ";
            strcat(addCookie, cookies[0]);
            compute_message(message, addCookie);
        }
        if (cookies_count == 2)
        {
            char addCookieJwt[MAXCOOKIELEN] = "Authorization: Bearer ";
            strcat(addCookieJwt, cookies[1]);
            compute_message(message, addCookieJwt);
        }
    }
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, char **body_data,
                           int body_data_fields_count, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    char *hostLine = calloc(LINELEN, sizeof(char));
    memcpy(hostLine, "Host: ", 7);
    strcat(hostLine, host);
    compute_message(message, hostLine);

    char *contentTypeLine = calloc(LINELEN, sizeof(char));
    memcpy(contentTypeLine, "Content-Type: ", 14);
    strcat(contentTypeLine, content_type);
    compute_message(message, contentTypeLine);

    char *contentLengthLine = calloc(LINELEN, sizeof(char));
    memcpy(contentLengthLine, "Content-Length: ", 16);
    int contentLength = 0;
    /// parcurg tot ce se afla in matrice si calculez
    /// total contentLength
    for (int i = 0; i < body_data_fields_count; i++)
    {
        contentLength += strlen(body_data[i]);
    }
    char *contentLengthString = calloc(LINELEN, sizeof(char));
    sprintf(contentLengthString, "%d", contentLength);

    strcat(contentLengthLine, contentLengthString);
    compute_message(message, contentLengthLine);
    /// in functie de argument, adaug si cookie login
    /// si token sau doar cookie login
    if (cookies_count != 0)
    {
        if (cookies_count >= 1)
        {
            char addCookie[MAXCOOKIELEN] = "Cookie: ";
            strcat(addCookie, cookies[0]);
            compute_message(message, addCookie);
        }
        if (cookies_count == 2)
        {
            char addCookieJwt[MAXCOOKIELEN] = "Authorization: Bearer ";
            strcat(addCookieJwt, cookies[1]);
            compute_message(message, addCookieJwt);
        }
    }

    memset(line, 0, LINELEN);
    for (int i = 0; i < body_data_fields_count; i++)
    {
        strcat(body_data_buffer, body_data[i]);
        free(body_data[i]);
    }
    free(body_data);
    strcat(message, "\r\n");
    strcat(message, body_data_buffer);

    free(line);
    free(contentLengthString);
    free(contentLengthLine);
    free(contentTypeLine);
    free(hostLine);
    free(body_data_buffer);

    return message;
}
// foarte similara cu post request dar consider ca ar fi utila
/// o functie separata de delete pentru viitoare imbunatatiri
char *compute_delete_request(char *host, char *url, char *query_params,
                             char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (query_params != NULL)
    {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    char add[100] = "Host: ";
    strcat(add, host);
    compute_message(message, add);

    if (cookies_count != 0)
    {
        if (cookies_count >= 1)
        {
            char addCookie[MAXCOOKIELEN] = "Cookie: ";
            strcat(addCookie, cookies[0]);
            compute_message(message, addCookie);
        }
        if (cookies_count == 2)
        {
            char addCookieJwt[MAXCOOKIELEN] = "Authorization: Bearer ";
            strcat(addCookieJwt, cookies[1]);
            compute_message(message, addCookieJwt);
        }
    }

    compute_message(message, "");

    free(line);
    return message;
}