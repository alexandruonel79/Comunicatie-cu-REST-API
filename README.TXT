    ONEL ALEXANDRU 322CB
    
    Am lasat si checkerul folosit in arhiva, in caz ca mai primeste
actualizari si nu apuc sa il actualizez. 
    Am inceput tema prin intelegerea profunda a laboratorului. Primul pas
din rezolvare a fost sa inteleg interactiunea cu serverul. Astfel
am trimi manual comenzi catre server si am vizualizat raspunsurile acestuia.
M am folosit de acest website: https://reqbin.com/ Este la fel ca postman
doar ca e mai usor de folosit. 
    Primul pas din tema a fost parsarea comenzilor. Pentru aceasta m am folosit
de fgets si am setat o limita LINELEN la valoarea 1000. Daca aceasta limita este 
depasita va continua cu urmatorul fgets, dar un utilizator normal nu va ajunge sa
dea input 1000 de caractere pentru un username de ex. Am setat o limita pentru orice
input primit, prin MAXINPUTLEN cu valoarea 100. Deci aplicatia accepta username si 
parola de ex pana la limita de 100 de caractere. Am folosit strcmp pentru fiecare 
comanda si daca era acel caz, atunci intra pe functia care se ocupa cu preluarea 
specifica a acelui input. Am creat un fisier auxiliar denumit input.c
unde se afla toate functiile de parsare a inputului. Totodata acolo se afla
doua functii de validare: ID si nume. Pentru ID am mers pe logica ca trebuie
sa fie compus numai din cifre(fara spatii), iar pentru nume acesta nu are voie
sa contina numere(am permis spatii). ID ul m am gandit la cazul cand incepe cu 
0 de ex 0123, si nu l am tratat in sensul sa dea eroare, dar trimite cererea
catre server cu acel ID si este tratata. Nu am fost sigur cum implementeaza ID urile
serverul in spate.
    Dupa ce se realizeaza citirea si afisarea mesajelor de atentionare in caz de input 
invalid, incepe trimiterea comenzilor spre procesare. Mi am creat un nou fisier denumit 
processCommands.c unde se afla toata logica. De exemplu, pentru functia de login
imi generez mesajul pe care il voi trimite la server folosind functia generateLRMessage()
(Aceasta este o functie auxiliara care genereaza mesaj si pentru login si pentru register
deoarece contentul lor este acelasi: JSON ul). Pe urma deschid o noua conexiune, pe care
o inchid imediat cand termin de trimis si de primit raspuns de la server. Cam asa sunt
toate functiile mele ca si schelet, ceea ce este diferit la acestea este modul de creare
a mesajului si interpretarea erorii primite de la server. In continuare pentru functia 
processLogin() daca serverul imi intoarce un mesaj ce nu contine eroare inseamna ca am
primit cookie ul pe care trebuie sa l extrag din http. Daca am primit eroare atunci 
o extrag si o afisez la utilizator. Pentru register foarte asemantator, daca nu primesc
eroare inseamna ca utilizatorul este inregistrat cu succes. Daca primesc eroare o afisez.
Pentru processLogout() este asemantator, singura eroare posibila este sa nu fiu logat si 
sa nu prezint cookie ul cand dau logout. Codul meu nu te lasa sa trimiti cererea de logout 
deci nu ajunge in aceasta functie vreodata pe cazul de eroare decat daca serverul m a limitat
in privinta numarului de cereri si afisez "Unexpected server error!". Pentru cazul cu 
enter_library folosesc acelasi schelet si daca extrag cookieJwt il returnez sau intoarce
NULL. Pentru getBooks() am modificat basic_extract_json si am creat alta functie
books_extract_json, un strstr diferit pentru a imi extrage conform. Daca exista
aloc un char* de dimensiunea raspunsului serverului si ma folosesc de parson.c 
pentru a imi crea afisarea in forma de json. Pentru getBookById() folosesc aceasi 
idee, dar creez doar un obiect json. Pentru addBook() creez json ul cu parametrii 
primiti si validati de input.c si il trimit. Pentru cererile unde am avut nevoie
de Bearer token am adaugat totodata si cookie ul, desi am testat si nu este necesar,
aceasta a fost ideea mea de a avea o dubla securitate, dar serverul ignora ceea 
ce este in plus deci functioneaza in ambele moduri.
    Variabilele de stare le am lasat in client.c, unde am loggedIn, inLibrary,
cookie si cookieJwt care stiu mereu starea utilizatorului si pot generea 
erori din aceasta.
    Am folosit parson.c deoarece era recomandata in pdf si nu stiam alta librarie
pentru JSON in C. Este okay libraria, dar in java mi se pare mai usor de utilizat 
JSON.
    Am folosit si memorie alocata dinamic pe care am reusit sa o eliberez total
folosind valgrind.
    Am lasat si comentarii in cod pentru o intelegere mai usoara. A fost o tema
mai usoara decat precedentele, dar interesanta.