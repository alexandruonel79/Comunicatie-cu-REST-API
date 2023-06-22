#include "processCommands.h"

#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */

#include "helpers.h"
#include "input.h"
#include "parson.h"
#include "requests.h"

char ipServer[16] = "34.254.242.81";
int port = 8080;

// formateaza toate detaliile despre carte
char *formatAllJson(char *string) {
  JSON_Value *root_value = json_parse_string(string);
  JSON_Object *root_object = json_value_get_object(root_value);

  int id = json_object_get_number(root_object, "id");
  const char *title = json_object_get_string(root_object, "title");
  const char *author = json_object_get_string(root_object, "author");
  const char *genre = json_object_get_string(root_object, "genre");
  const char *publisher = json_object_get_string(root_object, "publisher");
  int page_count = json_object_get_number(root_object, "page_count");
  char *formattedString = calloc(strlen(string), 1);
  sprintf(formattedString,
          "id=%d\ntitle=%s\nauthor=%s\ngenre=%s\npublisher=%s\npage_count=%d",
          id, title, author, genre, publisher, page_count);
  free(string);
  json_value_free(root_value);
  return formattedString;
}
// formats doar titlul si id ul cartii
char *formatPartialJson(char *string) {
  JSON_Value *root_value = json_parse_string(string);
  JSON_Object *root_object = json_value_get_object(root_value);

  const char *title = json_object_get_string(root_object, "title");
  int id = json_object_get_number(root_object, "id");

  char *formattedString = calloc(strlen(string), 1);
  sprintf(formattedString, "title=%s\nid=%d\n", title, id);
  free(string);
  json_value_free(root_value);
  return formattedString;
}
// formateaza mesajul pentru login si register
// continutul este acelasi
char *generateLRMessage(char *username, char *password, char *url) {
  char content_type[50] = "application/json";
  char **body_data = calloc(1, sizeof(char *));
  body_data[0] = calloc(MAXJSONLEN, sizeof(char));

  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);

  json_object_set_string(root_object, "username", username);
  json_object_set_string(root_object, "password", password);

  char *json_str = json_serialize_to_string(root_value);
  memcpy(body_data[0], json_str, strlen(json_str));

  json_value_free(root_value);
  json_free_serialized_string(json_str);

  return compute_post_request(ipServer, url, content_type, body_data, 1, NULL,
                              0);
}
char *processLogin(char *username, char *password) {
  int sockFd;
  char url[50] = "/api/v1/tema/auth/login";
  char *message = generateLRMessage(username, password, url);
  /// deschid o conexiune noua
  sockFd = open_connection(ipServer, port, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockFd, message);
  free(message);
  char *response = receive_from_server(sockFd);
  char *errorMessage = basic_extract_json_response(response);
  close(sockFd);
  /// daca nu am error message inseamna ca am primit cookie ul
  if (errorMessage == NULL) {
    // cazul cookie
    char *extractCookie = strstr(response, "connect.sid");
    char *cookie = strtok(extractCookie, " ");
    /// creez o copie pentru a lucra curat cu memoria
    /// strndup adauga si imi seteaza \n la final
    size_t len = strlen(cookie);
    char *cookie_copy = strndup(cookie, len);
    free(response);
    return cookie_copy;
  } else {
    /// cazul eroare!
    JSON_Value *json_value = json_parse_string(errorMessage);
    JSON_Object *json_object = json_value_get_object(json_value);
    const char *name = json_object_get_string(json_object, "error");
    printf("Error: %s\n", name);
    json_value_free(json_value);
    free(response);
    return NULL;
  }

  return NULL;
}
void processRegister(char *username, char *password) {
  int sockFd;
  char url[50] = "/api/v1/tema/auth/register";
  char *message = generateLRMessage(username, password, url);
  // deschid conexiunea
  sockFd = open_connection(ipServer, port, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockFd, message);
  free(message);
  char *response = receive_from_server(sockFd);
  close(sockFd);
  char *errorMessage = basic_extract_json_response(response);
  /// daca nu e niciun error message ins ca e succes
  if (errorMessage == NULL) {
    /// cazul cu succes
    printf("User succesfully registered!\n");
    free(response);
  } else {
    /// cazul eroare
    /// extragem eroarea data de server si o afisam
    JSON_Value *json_value = json_parse_string(errorMessage);
    JSON_Object *json_object = json_value_get_object(json_value);
    const char *name = json_object_get_string(json_object, "error");
    printf("Error: %s\n", name);
    json_value_free(json_value);
    free(response);
  }
}
void processLogout(char *cookie) {
  int sockFd;
  char url[50] = "/api/v1/tema/auth/logout";
  char **cookies = calloc(1, sizeof(char *));
  cookies[0] = calloc(MAXCOOKIELEN, sizeof(char));

  int dimCookie = strlen(cookie);

  memcpy(cookies[0], cookie, dimCookie);

  char *message = compute_get_request(ipServer, url, NULL, cookies, 1);
  // deschid conexiunea
  sockFd = open_connection(ipServer, port, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockFd, message);
  free(cookies[0]);
  free(cookies);
  free(message);
  char *response = receive_from_server(sockFd);
  close(sockFd);
  // extrag mesajul de eroare
  char *errorMessage = basic_extract_json_response(response);
  free(response);
  if (errorMessage == NULL) {
    /// delogare cu succes
    printf("Succesfully logged out!\n");
    return;
  }

  printf("Unexpected server error!\n");
}
char *enterLibrary(char *cookie) {
  int sockFd;
  char url[50] = "/api/v1/tema/library/access";
  /// creez matricea de cookies si adaug doar un cookie
  /// cel primit de la login
  char **cookies = calloc(1, sizeof(char *));
  cookies[0] = calloc(MAXCOOKIELEN, sizeof(char));
  memcpy(cookies[0], cookie, strlen(cookie));

  char *message = compute_get_request(ipServer, url, NULL, cookies, 1);
  sockFd = open_connection(ipServer, port, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockFd, message);
  /// eliberez memoria
  free(cookies[0]);
  free(cookies);
  free(message);

  char *response = receive_from_server(sockFd);
  char *cookieJwt = basic_extract_json_response(response);
  close(sockFd);
  /// daca am extras ceva inseamna ca e cookie ul
  if (cookieJwt != NULL) {
    /// extrag cookie ul cu ajutorul strtok
    char *extractJwt = strtok(cookieJwt, ":");
    extractJwt = strtok(NULL, ":");
    memcpy(extractJwt, extractJwt + 1, strlen(extractJwt) - 1);
    extractJwt[strlen(extractJwt) - 3] = '\0';

    /// copiez in alta variabila pentru a putea elibera memoria salvata
    ///  in response
    char *finalCookieJwt = calloc(MAXCOOKIELEN, 1);
    memcpy(finalCookieJwt, extractJwt, strlen(extractJwt));
    free(response);

    return finalCookieJwt;
  } else {
    /// cazul cand nu esti autentificat
    /// este tratat deja in main
    return NULL;
  }
}

char *getBooks(char *cookie, char *cookieJwt) {
  int sockFd;
  char url[50] = "/api/v1/tema/library/books";
  /// aloc matricea de cookies
  /// adaug cookie ul de la login si tokenul de autentificare
  char **cookies = calloc(2, sizeof(char *));
  cookies[0] = calloc(MAXCOOKIELEN, sizeof(char));
  cookies[1] = calloc(MAXCOOKIELEN, sizeof(char));
  memcpy(cookies[0], cookie, strlen(cookie));
  memcpy(cookies[1], cookieJwt, strlen(cookieJwt));

  char *message = compute_get_request(ipServer, url, NULL, cookies, 2);
  /// deschid o noua conexiune pentru a transmite mesaje cu serverul
  sockFd = open_connection(ipServer, port, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockFd, message);
  /// eliberez memoria
  free(cookies[0]);
  free(cookies[1]);
  free(cookies);
  free(message);
  /// primesc raspunsul de la server
  char *response = receive_from_server(sockFd);
  int responseLength = strlen(response);
  /// extrag lista de carti primita
  char *booksJson = books_extract_json_response(response);
  close(sockFd);
  /// daca nu e NULL inseamna ca avem carti
  if (booksJson != NULL) {
    /// creez o copie pentru a putea elibera memoria
    char *copyBooksJson = calloc(strlen(booksJson), 1);
    memcpy(copyBooksJson, booksJson, strlen(booksJson));
    free(response);

    if (copyBooksJson != NULL) {
      char *allStrings = calloc(responseLength, 1);
      JSON_Value *root_value = json_parse_string(copyBooksJson);
      free(copyBooksJson);
      JSON_Array *array = json_value_get_array(root_value);
      for (size_t i = 0; i < json_array_get_count(array); i++) {
        JSON_Object *object = json_array_get_object(array, i);
        char *string =
            json_serialize_to_string(json_object_get_wrapping_value(object));

        strcat(allStrings, "------------------------------\n");
        char *formattedString = formatPartialJson(string);

        strcat(allStrings, formattedString);
        free(formattedString);
      }
      strcat(allStrings, "------------------------------");
      json_value_free(root_value);

      return allStrings;
    } else {
      printf("Eroare!\n");
      return NULL;
    }
  } else {
    free(response);
    return NULL;
  }
}
char *getBookById(char *idBook, char *cookie, char *cookieJwt) {
  int sockFd;
  char url[50] = "/api/v1/tema/library/books/";
  strcat(url, idBook);
  char **cookies = calloc(2, sizeof(char *));
  cookies[0] = calloc(MAXCOOKIELEN, sizeof(char));
  cookies[1] = calloc(MAXCOOKIELEN, sizeof(char));
  memcpy(cookies[0], cookie, strlen(cookie));
  memcpy(cookies[1], cookieJwt, strlen(cookieJwt));

  char *message = compute_get_request(ipServer, url, NULL, cookies, 2);
  /// deschid conexiunea
  sockFd = open_connection(ipServer, port, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockFd, message);
  free(cookies[0]);
  free(cookies[1]);
  free(cookies);
  free(message);
  char *response = receive_from_server(sockFd);
  char *bookJson = basic_extract_json_response(response);
  close(sockFd);
  int responseLength = strlen(response);
  if (bookJson != NULL) {
    char *copyBookJson = calloc(responseLength, 1);
    memcpy(copyBookJson, bookJson, strlen(bookJson));

    JSON_Value *json_value = json_parse_string(copyBookJson);
    JSON_Object *json_object = json_value_get_object(json_value);
    const char *errorValue = json_object_get_string(json_object, "error");
    // daca nu contine eroare inseamna ca am primit carte
    if (errorValue == NULL) {
      copyBookJson = formatAllJson(copyBookJson);

      free(response);
      json_value_free(json_value);
      return copyBookJson;
    } else {
      free(copyBookJson);
      // Something bad happened -> id too big
      // deci daca nu avem Something.. ins ca returnam eroarea
      char *result = strstr(errorValue, "Something");
      if (result == NULL) {
        /// cazul cu No book was found
        // json_value_free(json_value);
        char *errorValueSend = calloc(responseLength, 1);
        memcpy(errorValueSend, errorValue, strlen(errorValue));
        json_value_free(json_value);
        free(response);
        return errorValueSend;
      } else {
        // eroare interna server pentru ID prea mare
        json_value_free(json_value);
        free(response);
        return NULL;
      }
    }
  } else {
    printf("Unexpected server error!\n");
    return NULL;
  }
}

int addBook(char *cookie, char *cookieJwt, char *title, char *author,
            char *genre, char *publisher, char *page_count) {
  char content_type[50] = "application/json";
  char **body_data = calloc(1, sizeof(char *));

  /// creez json ul cu toate campurile setate
  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);

  json_object_set_string(root_object, "title", title);
  json_object_set_string(root_object, "author", author);
  json_object_set_string(root_object, "genre", genre);
  json_object_set_string(root_object, "page_count", page_count);
  json_object_set_string(root_object, "publisher", publisher);

  char *json_str = json_serialize_to_string(root_value);
  body_data[0] = calloc(MAXJSONLEN, sizeof(char));
  memcpy(body_data[0], json_str, strlen(json_str));
  /// adaug cookie urile
  char **cookies = calloc(2, sizeof(char *));
  cookies[0] = calloc(MAXCOOKIELEN, sizeof(char));
  cookies[1] = calloc(MAXCOOKIELEN, sizeof(char));
  memcpy(cookies[0], cookie, strlen(cookie));
  memcpy(cookies[1], cookieJwt, strlen(cookieJwt));

  char url[50] = "/api/v1/tema/library/books/";

  json_value_free(root_value);
  json_free_serialized_string(json_str);
  /// creez mesajul
  char *message = compute_post_request(ipServer, url, content_type, body_data,
                                       1, cookies, 2);
  int sockFd = open_connection(ipServer, port, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockFd, message);
  free(message);
  free(cookies[0]);
  free(cookies[1]);
  free(cookies);
  char *response = receive_from_server(sockFd);
  char *errorMessage = basic_extract_json_response(response);
  close(sockFd);
  /// daca avem eroare intoarce 0
  if (errorMessage != NULL) {
    free(response);
    return 0;
  } else {
    /// cartea este adaugata cu succes
    free(response);
    return 1;
  }
}
int deleteBookById(char *idBook, char *cookie, char *cookieJwt) {
  int sockFd;
  char url[50] = "/api/v1/tema/library/books/";
  strcat(url, idBook);
  /// adaug cookie urile
  char **cookies = calloc(2, sizeof(char *));
  cookies[0] = calloc(MAXCOOKIELEN, sizeof(char));
  cookies[1] = calloc(MAXCOOKIELEN, sizeof(char));
  memcpy(cookies[0], cookie, strlen(cookie));
  memcpy(cookies[1], cookieJwt, strlen(cookieJwt));
  /// creez mesajul
  char *message = compute_delete_request(ipServer, url, NULL, cookies, 2);
  /// deschid conexiunea
  sockFd = open_connection(ipServer, port, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockFd, message);
  free(cookies[0]);
  free(cookies[1]);
  free(cookies);
  free(message);
  char *response = receive_from_server(sockFd);
  char *error = basic_extract_json_response(response);
  close(sockFd);
  /// daca am eroare returnez 0
  if (error != NULL) {
    free(response);
    return 0;
  } else {
    /// cazul cu succes cand am sters cartea
    free(response);
    return 1;
  }
}