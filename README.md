Programowanie rozproszone
Projekt: Kosmiczne fajerwerki

Modelowana sytuacja
Danych jest N procesów, zespołów-inżynierów piromanów, każdy o liczebności In (In
inne dla każdego zespołu). Ubiegają się oni o nierozróżnialne zasoby: najpierw
determinują, co tym razem wysadzą. W tym celu zajmują In z B nierozróżnialnych
biurek. Następnie zajmują jedną z K salek konferencyjnych, gdzie z poważnymi
minami obwieszczają, że tym razem start rakiety zakończy się sukcesem. Później
zajmują jedno pole startowe, odpalają rakietę, cieszą się jak dzieci z BUM i zajmują
tym razem jedno z B biurek (tych samych co poprzednio), gdzie wysmarowują
wiarygodnie brzmiące wyjaśnienie, dlaczego rakieta znowu wybuchła.
Założenia na temat środowiska komunikacyjnego:
- zegar Lamporta jest rosnący, pomijamy czasy przetwarzania lokalnego (poza zmianą
stanu)
- kanały FIFO
- żaden z procesów nie ulegnie awarii
- pojemność kanału - maksymalnie 3N-3 (modelowana sytuacja: proces przechodzi do
stanu w którym nie czyta żadnych wiadomości, wtedy może dostać N-1 RELEASow,
po czym wysłać swój REQUEST, dostać N-1 ACK i N-1 REQUESTÓW)
Złożoność komunikacyjna i czasowa:
z punktu widzenia konkretnego procesu żeby dowiedzieć się czy nasz proces może
wejść do sekcji krytycznej: potrzebuje wysłać jeden REQUEST do wszystkich i odebrać N-1
ACK/RELEASow:
- złożoność komunikacyjna - 2n-2 (jeżeli z RELEASEM to 3n-3)
- złożoność czasowa - 2t (jeżeli z RELEASEM to 3t)
TYPY WIADOMOŚCI:
REQUEST_FOR_DESK - zgłoszenie żądanie o przydziale biurek
* znacznik czasowy
* liczba biurek
REQUEST_FOR_ROOM - zgłoszenie żądanie o przydziale sali
* znacznik czasowy
REQUEST_FOR_STARTING_FIELD- zgłoszenie żądanie o przydziale pola startowego
* znacznik czasowy
RELEASE_DESK - zgłoszenie zwolnienia biurek przez proces.
RELEASE_ROOM - zgłoszenie zwolnienia sali przez proces.
RELEASE_STARTING_FIELD - zgłoszenie zwolnienia pola startowego przez proces.
ACK_DESK - reakcja na REQUEST_FOR_DESK
* znacznik czasowy
* liczba biurek
ACK_ROOM - reakcja na REQUEST_FOR_ROOM
* znacznik czasowy
ACK_STARTING_FIELD - reakcja na REQUEST_FOR_STARTING_FIELD
* znacznik czasowy
POMOCNICZE STRUKTURY DANYCH:
N(int) - liczba zespolow/procesow
ln[] - tabela ktora zawiera inf o wielkosci poszczegolnych zespolow
B(int) - ogolna liczba biurek
K(int) - ogolna liczba sal
lclock(int) - aktualna wartość zegara logicznego procesu
desk_queue- kolejka priorytetowa posortowana wg znacznika czasowego, potem na
podstawie id procesów
room_queue - kolejka priorytetowa posortowana wg znacznika czasowego, potem na
podstawie id procesów
starting_field_queue - kolejka priorytetowa posortowana wg znacznika czasowego, potem
na podstawie id procesów
1. stan INIT
1.1 Proces i-ty wysyła wiadomość REQUEST_FOR_DESK do wszystkich procesów
1.2 Proces dodaje swoją wiadomość do lokalnej kolejki desk_queue
1.3 Inicjujemy free_B=B -ilość żądanych biurek, free_K = K-1
1.4 Przechodzimy do stanu WAITING_TO_DISCUSS
W tym stanie proces nie reaguje na żadne wiadomości.
2. stan WAITING_TO_DISCUSS
2.0 Proces odbiera po 1 wiadomości od każdego z procesów
(REQUEST_FOR_DESK lub ACK_DESK )
2.1 Proces sprawdza czy czy free_B >= 0 (innymi słowami czy dla wszyscy przed
nami + my mogą wejść do sekcji krytycznej )
2.1.1 Jeśli tak - przechodzimy do stanu DISCUSSION
2.2 Jeżeli nie - proces oczekuje na wiadomości typu REQUEST_FOR_DESK i
RELEASE_DESK i ACK_DESK
po otrzymaniu REQUEST_FOR_DESK:
1. wstawia je do lokalnej kolejki desk_queue
2. jeżeli nasz proces znajduje się po wstawionym procesie,to free_B zostaje
pomniejszona o liczbę osób w zespole w tym procesie
3. wysyła potwierdzenie do procesu ACK_DESK
4. wrócić do punktu 2.1
po otrzymaniu RELEASE_DESK:
1. usuwamy żądanie procesu od którego dostaliśmy wiadomość z lokalnej
kolejki desk_queue
2. wartość free_B zostaje powiększona o liczbę osób w zespole w tym
procesie
3. wrócić do punktu 2.1
przy otrzymaniu ACK_DESK:
1. Sprawdzamy czy wartość żądanych liczb krzesel jest = 0 - jezeli tak -
ignorujemy tę wiadomość
2. Jeżeli nie - reagujemy tak jak w przypadku REQUEST_FOR_DESK
W tym stanie proces reaguje na wiadomości
REQUEST_FOR_DESK,REQUEST_FOR_STARTING_FIELD, REQUEST_FOR_ROOM,
ACK_DESK,RELEASE_DESK
3. stan WAITING_FOR_ROOM
3.0 Dostaje po 1 wiadomości od każdego z procesów
(REQUEST_FOR_ROOM lub ACK_ROOM ) od każdego procesu
3.1 Proces sprawdza czy czy free_K >= 0
3.1.1 Jeśli tak - PRZECHODZIMY DO STANU THE_BIG_LIE
3.2 jeżeli nie - oczekuje na wiadomości typu REQUEST_FOR_ROOM i
RELEASE_ROOM i ACK_ROOM
po otrzymaniu REQUEST_FOR_ROOM:
1. wstawia je do lokalnej kolejki room_queue
2. jeżeli nasz proces znajduje się po wstawionym procesie,to free_K
zostaje pomniejszona o 1
3. wysyła potwierdzenie do procesu ACK_ROOM
4. wrócić do punktu 3.1
przy otrzymaniu RELEASE_ROOM:
1. usuwamy żądanie procesu od którego dostaliśmy wiadomość z
lokalnej kolejki
2. wartość free_K zostaje powiększona o 1
3. wrócić do punktu 3.1
przy otrzymaniu ACK_ROOM:
1. Sprawdzamy czy proces żąda sale - jeśli nie - ignorujemy tę
wiadomość
2. Jeżeli tak - reagujemy tak jak w przypadku REQUEST_FOR_ROOM
W tym stanie proces reaguje na wiadomości
REQUEST_FOR_DESK,REQUEST_FOR_STARTING_FIELD, REQUEST_FOR_ROOM,
ACK_ROOM,RELEASE_ROOM
4. stan DISCUSSION
4.1 wysyłamy wiadomość RELEASE_DESKS do wszystkich procesów.
4.2 Usuwamy swoje zgłoszenie z lokalnej kolejki desk_queue *zwalniamy zasoby*
4.3 Proces wysyła wiadomość REQUEST_FOR_ROOM do wszystkich procesów
4.4 Proces dodaje swoją wiadomosc do lokalnej kolejki room_queue
4.5 Przechodzimy do stanu WAITING_FOR_ROOM
W tym stanie proces nie reaguje na żadne wiadomości.
5. stan THE_BIG_LIE
5.1 wysyłamy wiadomość RELEASE_ROOM do wszystkich procesów.
5.2 Usuwamy swoje zgłoszenie z lokalnej kolejki room_queue *zwalniamy zasoby*
5.3 Proces i-ty wysyła wiadomość REQUEST_FOR_STARTING_FIELD do
wszystkich procesów
5.4 Proces dodaje swoją wiadomość do lokalnej kolejki starting_field_queue
5.5 Przechodzimy do stanu WAITING_FOR_STARTING_FIELD
W tym stanie proces nie reaguje na żadne wiadomości.
6. stan WAITING_FOR_STARTING_FIELD
6.0 Dostaje po 1 wiadomości od każdego z procesów
(REQUEST_FOR_STARTING_FIELD lub ACK_STARTING_FIELD) od każdego
procesu
6.1 Proces sprawdza czy jest na szczycie kolejki
6.1.1 Jeśli tak - PRZECHODZIMY DO STANU BIG_BOOM
6.2 jeżeli nie - oczekuje na wiadomości typu REQUEST_FOR_STARTING_FIELD i
RELEASE_STARTING_FIELD i ACK_STARTING_FIELD
po otrzymaniu REQUEST_FOR_STARTING_FIELD
1. wstawia je do lokalnej kolejki starting_field_queue
3. wysyła potwierdzenie do procesu ACK_STARTING_FIELD
4. wrócić do punktu 6.1
przy otrzymaniu RELEASE_STARTING_FIELD
1. usuwamy żądanie procesu od którego dostaliśmy wiadomość z lokalnej
kolejki starting_field_queue
3. wrócić do punktu 6.1
przy otrzymaniu ACK_STARTING_FIELD
1. Sprawdzamy czy proces żąda pole startowe - jeśli nie - ignorujemy
tę wiadomość
2. Jeżeli tak - reagujemy tak jak w przypadku
REQUEST_FOR_STARTING_FIELD
W tym stanie proces reaguje na wiadomości
REQUEST_FOR_DESK,REQUEST_FOR_STARTING_FIELD, REQUEST_FOR_ROOM,
ACK_STARTING_FIELD,RELEASE_STARTING_FIELD
7. stan BIG_BOOM
7.1 wysyłamy wiadomość RELEASE_STARTING_FIELD do wszystkich procesów.
7.2 Usuwamy swoje zgłoszenie z lokalnej kolejki starting_field_queue *zwalniamy
zasoby*
7.3 Proces wysyła wiadomość REQUEST_FOR_DESK do wszystkich procesów
7.4 Proces dodaje swoją wiadomość do lokalnej kolejki starting_field_queue
7.5 Przechodzimy do stanu WAITING_FOR_ONE_DESK
W tym stanie proces nie reaguje na żadne wiadomości.
8. stan WAITING_FOR_ONE_DESK
8.0 Dostaje po 1 wiadomości od każdego z procesów (REQUEST_FOR_DESK lub
ACK_DESK ) od każdego procesu i sprawdzenie czy te wiadomości mają znacznik
czasowy wyższy od tego co ma nasz REQUEST
8.1 Proces sprawdza czy czy free_B >= 0
8.1.1 Jeśli tak - przechodzi do stanu EXPLANATION
8.2 oczekuje na wiadomości typu REQUEST_FOR_DESK i RELEASE_DESK i
ACK_DESK
po otrzymaniu REQUEST_FOR_DESK:
1. wstawia je do lokalnej kolejki desk_queue
2. jeżeli nasz proces znajduje się po wstawionym procesie,to free_B zostaje
pomniejszona o 1
3. wysyła potwierdzenie do procesu ACK_DESK
4. wrócić do punktu 8.1
przy otrzymaniu RELEASE_DESK:
1. usuwamy żądanie procesu od którego dostaliśmy wiadomość z lokalnej
kolejki
2. wartość free_B zostaje powiększona o liczbę osób w zespole w tym
procesie
3. wrócić do punktu 8.1
przy otrzymaniu ACK_DESK:
3. Sprawdzamy czy wartość żądanych liczb krzeseł jest = 0 - jezeli tak -
ignorujemy tę wiadomość
4. Jeżeli nie - reagujemy tak jak w przypadku REQUEST_FOR_DESK
W tym stanie proces reaguje na wiadomości
REQUEST_FOR_DESK,REQUEST_FOR_STARTING_FIELD, REQUEST_FOR_ROOM,
ACK_DESK,RELEASE_DESK
9. stan EXPLANATION
*wykonujemy działania i zwalniamy zasoby*
9.1 wysyłamy wiadomość RELEASE_DESK do wszystkich procesów.
9.2 Usuwamy swoje zgłoszenie z lokalnej kolejki desk_queue
9.3 przechodzimy do stanu INIT
W tym stanie proces nie reaguje na żadne wiadomości.
