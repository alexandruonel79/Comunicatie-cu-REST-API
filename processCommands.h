#ifndef _PROCESSCOMMANDS_
#define _PROCESSCOMMANDS_
#define MAXJSONLEN 1000
#define MAXCOOKIELEN 1000
// formateaza Jsonul cu toate campurile necesare
char *formatAllJson(char *string);
// formateaza Jsonul cu titlul si id ul
char *formatPartialJson(char *string);
// genereaza un mesaj pentru login si register
// ambele fiind la fel
char *generateLRMessage(char *username, char *password, char *url);
// proceseaza Login ul
char *processLogin(char *username, char *password);
// proceseaza Register ul
void processRegister(char *username, char *password);
// proceseaza Logout ul
void processLogout(char *cookie);
// functia se ocupa de accesarea librariei
char *enterLibrary(char *cookie);
// functia se ocupa cu returnarea stringului gata de afisat
char *getBooks(char *cookie, char *cookieJwt);
// functia se ocupa cu returnarea stringului gata de afisat
char *getBookById(char *idBook, char *cookie, char *cookieJwt);
// functia adauga o carte in contul utilizatorului
int addBook(char *cookie, char *cookieJwt, char *title, char *author, char *genre, char *publisher, char *page_count);
// functia sterge o carte in contul utilizatorului
int deleteBookById(char *idBook, char *cookie, char *cookieJwt);
#endif