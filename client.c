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
#include "processCommands.h"
#include "requests.h"

int main(int argc, char *argv[]) {
  /// variabilele de stare
  /// in functie de ele stiu daca utilizatorul
  /// este logat sau/si in librarie
  char *cookie = NULL, *cookieJwt = NULL;
  int loggedIn = 0, inLibrary = 0;

  char *line = calloc(LINELEN, sizeof(char));
  while (1) {
    /// citesc in line si folosesc strcmp pentru a
    /// determina ce comanda este de procesat
    fgets(line, LINELEN, stdin);

    if (strlen(line) > MAXINPUTLEN) {
      printf("Commands are maximum 13 characters!\n");
      memset(line, '\0', LINELEN);
      continue;
    }
    line[strlen(line) - 1] = '\0';

    if (strcmp(line, "login") == 0) {
      if (loggedIn == 1) {
        printf("Already logged in!\n");
        printf("You can logout and login with another account.\n");
      } else {
        // inputul e limitat la 100 de caractere
        char username[LINELEN], password[LINELEN];
        int readInput;
        /// verficam daca inputul respecta restrictiile
        readInput = getInputLR(username, password);
        /// daca le respecta si nu suntem deja logati
        /// incepem procesul de verificare login
        if (readInput == 1) {
          cookie = processLogin(username, password);

          if (cookie != NULL) {
            loggedIn = 1;
            printf("Succesfully logged in!\n");
          }
        }
      }
    } else if (strcmp(line, "register") == 0) {
      // inputul e limitat la 100 de caractere
      char username[LINELEN], password[LINELEN];
      int readInput;
      /// verific daca inputul e validat
      readInput = getInputLR(username, password);
      if (readInput == 1) {
        /// functia se ocupa si de afisarile conforme
        processRegister(username, password);
      }
    } else if (strcmp(line, "logout") == 0) {
      if (cookie == NULL || loggedIn == 0) {
        printf("You were not logged in before!\n");
      } else {
        processLogout(cookie);
        /// eliberez memoria si resetez toate variabilele
        /// care au legatura cu starea clientului
        if (cookie != NULL) free(cookie);

        if (cookieJwt != NULL) free(cookieJwt);

        cookie = NULL;
        loggedIn = 0;
        inLibrary = 0;
        cookieJwt = NULL;
      }
    } else if (strcmp(line, "enter_library") == 0) {
      if (inLibrary == 1) {
        printf("Already in library!\n");
      } else {
        if (loggedIn == 0 && cookie == NULL) {
          printf("You must be logged in to acces the library!\n");
        } else {
          /// daca sunt deja logat primesc un cookieJwt
          cookieJwt = enterLibrary(cookie);

          if (cookieJwt != NULL) {
            inLibrary = 1;
            printf("Welcome to the library!\n");
          }
        }
      }
    } else if (strcmp(line, "get_books") == 0) {
      if (loggedIn == 0) {
        printf("You must be logged in!\n");
      } else if (inLibrary == 0) {
        printf("You must be in the library!\n");
      } else {
        /// daca sunt logat si in librarie
        /// atunci pot lua cartile
        char *books = getBooks(cookie, cookieJwt);

        if (books != NULL) {
          printf("%s\n", books);
          free(books);
        } else {
          printf("There are no books!\n");
        }
      }
    } else if (strcmp(line, "get_book") == 0) {
      if (loggedIn == 0) {
        printf("You must be logged in!\n");
      } else if (inLibrary == 0) {
        printf("You must be in the library!\n");
      } else {
        char idBook[LINELEN];
        int input = getInputGetBook(idBook);
        if (input == 1) {
          char *book = getBookById(idBook, cookie, cookieJwt);
          if (book != NULL) {
            printf("%s\n", book);
            free(book);
          } else {
            // este interpretat si din raspunsul serverului
            printf("The Id is too big, no book has that Id!\n");
          }
        }
      }
    } else if (strcmp(line, "add_book") == 0) {
      char title[LINELEN], author[LINELEN], genre[LINELEN], publisher[LINELEN],
          page_count[LINELEN];
      if (loggedIn == 0) {
        printf("You must be logged in!\n");
      } else if (inLibrary == 0) {
        printf("You must be in the library!\n");
      } else {
        int correctInput;
        correctInput =
            getInputAddBook(title, author, genre, publisher, page_count);

        if (correctInput == 1) {
          int addBookResponse;
          addBookResponse = addBook(cookie, cookieJwt, title, author, genre,
                                    publisher, page_count);
          if (addBookResponse == 1) {
            printf("Book added!\n");
          } else {
            printf("Page number too big!\n");
          }
        }
      }
    } else if (strcmp(line, "delete_book") == 0) {
      char id[LINELEN];
      if (loggedIn == 0) {
        printf("You must be logged in!\n");
      } else if (inLibrary == 0) {
        printf("You must be in the library!\n");
      } else {
        int correctInput = getInputDeleteBook(id);

        if (correctInput == 1) {
          int deleteResponse = deleteBookById(id, cookie, cookieJwt);

          if (deleteResponse == 0) {
            printf("The book with the given Id does not exist!\n");
          } else {
            printf("Book deleted succesfully!\n");
          }
        }
      }
    } else if (strcmp(line, "exit") == 0) {
      /// eliberez memoria ocupata
      free(line);
      if (cookie != NULL) free(cookie);
      if (cookieJwt != NULL) free(cookieJwt);
      return 0;
    } else {
      printf("Wrong command! Try again!\n");
    }
    memset(line, '\0', LINELEN);
  }
  return 0;
}
