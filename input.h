#ifndef _GET_INPUT_
#define _GET_INPUT_
#define MAXINPUTLEN 100
// valideaza un input pentru login si register
// nu are voie sa contina spatii
int validateInputLR(char *input);

// valideaza un id
// nu are voie sa contina orice altceva decat cifre
// fara spatii
int validateId(char *idBook);

// verifica daca un nume e valid, adica
// daca contine litere
// caracterele speciale le am permis
int isValidName(char *name);

// obtine inputul pentru login si register
//  afisand mesajele conforme in caz de eroare
int getInputLR(char *username, char *password);

// obtine inputul pentru getBook si il valideaza
int getInputGetBook(char *idBook);

// obtine inputul pentru addBook si il valideaza
int getInputAddBook(char *title, char *author, char *genre, char *publisher, char *page_count);

// obtine inputul pentru deleteBook si il valideaza
int getInputDeleteBook(char *id);

#endif