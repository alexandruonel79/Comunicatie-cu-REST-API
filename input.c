#include "input.h"

#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */

#include "helpers.h"
#include "parson.h"
#include "requests.h"

int validateInputLR(char *input) {
  /// daca exista un spatiu returnez 0
  /// in cadrul username si password nu ai voie
  /// caracterul spatiu
  char *ret;
  ret = strchr(input, ' ');

  if (ret == NULL) {
    return 1;
  } else {
    return 0;
  }
}
int validateId(char *idBook) {
  // daca nu este cifra return 0
  for (int i = 0; i < strlen(idBook); i++) {
    if (idBook[i] > '9' || idBook[i] < '0') {
      return 0;
    }
  }
  return 1;
}
int isValidName(char *name) {
  /// verific daca contine cifre
  for (int i = 0; i < strlen(name); i++) {
    if (name[i] <= '9' && name[i] >= '0') {
      return 0;
    }
  }
  return 1;
}
int getInputLR(char *username, char *password) {
  /// functia citeste username ul si parola si
  /// verifica daca respecta restrictia
  /// return 1 in caz de succes, 0 in caz de insucces
  printf("username=");

  fgets(username, LINELEN, stdin);

  printf("password=");

  fgets(password, LINELEN, stdin);

  if (strlen(username) > MAXINPUTLEN) {
    printf("Username is maximum 100 characters!\n");
    return 0;
  }
  if (strlen(username) == 1) {
    printf("Cannot leave username empty!\n");
    return 0;
  }
  if (validateInputLR(username) == 0) {
    printf("The username cannot contain spaces!\n");
    printf("Rewrite a new command!\n");
    return 0;
  }

  if (strlen(password) > MAXINPUTLEN) {
    printf("Password is maximum 100 characters!\n");
    return 0;
  }
  if (strlen(password) == 1) {
    printf("Cannot leave password empty!\n");
    return 0;
  }
  if (validateInputLR(password) == 0) {
    printf("The password cannot contain spaces!\n");
    printf("Rewrite a new command!\n");
    return 0;
  }
  username[strlen(username) - 1] = '\0';
  password[strlen(password) - 1] = '\0';
  return 1;
}
int getInputGetBook(char *idBook) {
  printf("id=");

  fgets(idBook, LINELEN, stdin);

  if (strlen(idBook) > MAXINPUTLEN) {
    printf("The Id is too big, no book has that Id!\n");
    return 0;
  }
  if (strlen(idBook) == 1) {
    printf("Cannot leave id empty!\n");
    return 0;
  }
  idBook[strlen(idBook) - 1] = '\0';

  if (validateId(idBook) == 0) {
    printf("The Id must be a valid number!\n");
    return 0;
  }

  if (strlen(idBook) > 10) {
    printf("The Id is too big, no book has that Id!\n");
    return 0;
  }

  return 1;
}
int getInputAddBook(char *title, char *author, char *genre, char *publisher,
                    char *page_count) {
  printf("title=");
  fgets(title, LINELEN, stdin);

  printf("author=");
  fgets(author, LINELEN, stdin);

  printf("genre=");
  fgets(genre, LINELEN, stdin);

  printf("publisher=");
  fgets(publisher, LINELEN, stdin);

  printf("page_count=");
  fgets(page_count, LINELEN, stdin);

  if (strlen(title) > MAXINPUTLEN) {
    printf("The title is too big!\n");
    return 0;
  }
  if (strlen(title) == 1) {
    printf("Cannot leave title empty!\n");
    return 0;
  }
  if (strlen(author) > MAXINPUTLEN) {
    printf("The author's name is too big!\n");
    return 0;
  }
  if (strlen(author) == 1) {
    printf("Cannot leave author empty!\n");
    return 0;
  }
  if (isValidName(author) == 0) {
    printf("The author's name should not contain numbers\n");
    return 0;
  }
  if (strlen(genre) > MAXINPUTLEN) {
    printf("The genre is too big!\n");
    return 0;
  }
  if (strlen(genre) == 1) {
    printf("Cannot leave genre empty!\n");
    return 0;
  }
  if (strlen(publisher) > MAXINPUTLEN) {
    printf("The publisher is too big!\n");
    return 0;
  }
  if (strlen(publisher) == 1) {
    printf("Cannot leave publisher empty!\n");
    return 0;
  }
  if (strlen(page_count) > MAXINPUTLEN) {
    printf("The page_count is too big!\n");
    return 0;
  }
  if (strlen(page_count) == 1) {
    printf("Cannot leave page_count empty!\n");
    return 0;
  }
  title[strlen(title) - 1] = '\0';
  author[strlen(author) - 1] = '\0';
  genre[strlen(genre) - 1] = '\0';
  publisher[strlen(publisher) - 1] = '\0';
  page_count[strlen(page_count) - 1] = '\0';

  if (validateId(page_count) == 0) {
    printf("Page_count must be a valid number(max 10)!(No spaces allowed)\n");
    return 0;
  }
  if (strlen(page_count) > 10) {
    printf("The page_count is too big, no book has that page_count!\n");
    return 0;
  }

  return 1;
}
int getInputDeleteBook(char *id) {
  printf("id=");
  fgets(id, LINELEN, stdin);

  if (strlen(id) > MAXINPUTLEN) {
    printf("The Id is too big, no book has that Id!\n");
    return 0;
  }
  if (strlen(id) == 1) {
    printf("Cannot leave id empty!\n");
    return 0;
  }
  id[strlen(id) - 1] = '\0';
  if (validateId(id) == 0) {
    printf("Id must be a valid number!\n");
    return 0;
  }
  if (strlen(id) > 10) {
    printf("The Id is too big, no book has that Id!\n");
    return 0;
  }

  return 1;
}