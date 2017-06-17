#include <ncurses.h>
#include <panel.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/select.h>

/*
  * Initializarea unor functii ce sunt apelate mai devreme
  * Functionalitatea este explicata la inceputul fiecarei functii
*/

void new_game(PANEL *panel_menu, PANEL *panel_game, WINDOW *win_menu, WINDOW *win_game, int *runde, int (*m)[4], int *score, int *moves);
void resume(PANEL *panel_menu, PANEL *panel_game, WINDOW *win_menu, WINDOW *win_game, int *runde, int (*m)[4], int *score, int *moves);
void castigat(PANEL *panel_menu, PANEL *panel_game, WINDOW *win_menu, WINDOW *win_game, int *runde, int (*m)[4], int *score, int *moves);
void pierdut(PANEL *panel_menu, PANEL *panel_game, WINDOW *win_menu, WINDOW *win_game, int *runde, int (*m)[4], int *score, int *moves);

/*
  * Functia "random_numbers" realizeaza alegerea a trei valori random
  * Valorile reprezinta coordonatele pozitiei din matrice unde va fi generat 2 sau 4 si aceasta valoare
  * Primeste ca paramentru un pointer la matrice, realizand si adaugarea valorii in matrice
*/
void random_numbers(int (*m)[4])
{
	int i, j, val, nr_rand, aux;
	srand(time(NULL));
	
	do{
		i = rand() % 4;
		j = rand() % 4;
		nr_rand = rand() % 2;	
		if (nr_rand == 0) 
			val = 2;
		else 
			val = 4;
		aux = m[i][j];
		if (aux == 0) m[i][j] = val;
	} while (aux != 0);
}

/*
  * Functia "stanga" realizeaza mutarea la stanga
  * Procesul se realizeaza in doi pasi, prima daca se fac alipirile si dupa aceea, mutarile
  * Mutarea consta in ocuparea casuselor libere pana la intalnirea marginei tabelei de joc sau a altei casute ocupate
  * Tot aici este modificat si scorul in functie de alipirile care se realizeaza
*/
void stanga(int (*m)[4], int *score, int *mutari, int *alipiri)
{
	int i, j, k;
	for (i=0; i<4; i++)
		for (j=0; j<3; j++)
			if (m[i][j]==m[i][j+1] && m[i][j]!=0){  /*se verifica daca elementul este diferit de zero
								 pentru a nu face alipiri si intre zero-uri*/
								m[i][j]=2*m[i][j];
								*score+=m[i][j];
								m[i][j+1]=0;
								(*alipiri)++;
			} 
			else if (j==0 && m[i][1]==0 && m[i][j]==m[i][2] && m[i][j]!=0){
									     		m[i][j]=2*m[i][j];
									     		*score+=m[i][j];
								 	     		m[i][2]=0;
									     		(*alipiri)++;
			}
			else if (j==0 && m[i][1]==0 && m[i][2]==0 && m[i][j]==m[i][3] && m[i][j]!=0){
										       		      m[i][j]=2*m[i][j];
										    		      *score+=m[i][j];
								 	     	       		      m[i][3]=0;
										       		      (*alipiri)++;
			}
			else if (j==1 && m[i][2]==0 && m[i][j]==m[i][3] && m[i][j]!=0){
									 		m[i][j]=2*m[i][j];
											*score+=m[i][j];
											m[i][3]=0;
									 		(*alipiri)++;
			}
	for (i=0; i<4; i++){
		k=0;
		for (j=0; j<4; j++)
			if (m[i][j]!=0){
					m[i][k]=m[i][j];
					if(k!=j){
						 m[i][j]=0;
						 (*mutari)++; /*numarul de mutari creste doar daca, casuta elementului
								difera de cea in care s-ar face mutarea*/
					}
					k++;
			}
	}
	if ((*mutari) != 0 || (*alipiri) != 0) random_numbers(m); /*daca s-a realizat cel putin o mutare a elementelor 
								   matricei sau alipire, se genereaza un element random*/
}

/*
  * Functia "dreapta" realizeaza mutarea la dreapta
  * Se foloseste acelasi algoritm, singura diferenta constand in directia de parcurgere 
    a matricei (de la dreapta la stanga)
*/
void dreapta(int (*m)[4], int *score, int *mutari, int *alipiri)
{
	int i, j, k;
	for (i=0; i<4; i++)
		for (j=3; j>0; j--)
			if (m[i][j]==m[i][j-1] && m[i][j]!=0){
							       	m[i][j]=2*m[i][j];
							       	*score+=m[i][j];
								m[i][j-1]=0;
								(*alipiri)++;
			}
			else if (j==3 && m[i][2]==0 && m[i][j]==m[i][1] && m[i][j]!=0){
									     		m[i][j]=2*m[i][j];
											*score+=m[i][j];
								 	     		m[i][1]=0;
									     		(*alipiri)++;
			}
			else if (j==3 && m[i][2]==0 && m[i][1]==0 && m[i][j]==m[i][0] && m[i][j]!=0){
										       		      m[i][j]=2*m[i][j];
												      *score+=m[i][j];
								 	     	       		      m[i][0]=0;
										       		      (*alipiri)++;
			}
			else if (j==2 && m[i][1]==0 && m[i][j]==m[i][0] && m[i][j]!=0){
									 		m[i][j]=2*m[i][j];
											*score+=m[i][j];
									 		m[i][0]=0;
									 		(*alipiri)++;
			}
	for (i=0; i<4; i++){
		k=3;
		for (j=3; j>=0; j--)
			if (m[i][j]!=0){
					m[i][k]=m[i][j];
					if(k!=j){
						 m[i][j]=0;
 						 (*mutari)++;
					}
					k--;
			}
	}
	if ((*mutari) != 0 || (*alipiri) != 0) random_numbers(m);
}

/*
  * Functia "sus" realizeaza mutarea in sus
  * Matricea este parcursa de sus in jos pe coloane
*/
void sus(int (*m)[4], int *score, int *mutari, int *alipiri)
{
	int i, j, k;
	for (j=0; j<4; j++)
		for (i=0; i<3; i++)
			if (m[i][j]==m[i+1][j] && m[i][j]!=0){
								m[i][j]=2*m[i][j];
								*score+=m[i][j];
								m[i+1][j]=0;
								(*alipiri)++;
			}
			else if (i==0 && m[1][j]==0 && m[i][j]==m[2][j] && m[i][j]!=0){
									     		m[i][j]=2*m[i][j];
											*score+=m[i][j];
								 	     		m[2][j]=0;
									     		(*alipiri)++;
			}
			else if (i==0 && m[1][j]==0 && m[2][j]==0 && m[i][j]==m[3][j] && m[i][j]!=0){
										       		     m[i][j]=2*m[i][j];
												     *score+=m[i][j];
								 	     	       		     m[3][j]=0;
										            	     (*alipiri)++;
			}
			else if (i==1 && m[2][j]==0 && m[i][j]==m[3][j] && m[i][j]!=0){
									 		m[i][j]=2*m[i][j];
											*score+=m[i][j];
											m[3][j]=0;
									 		(*alipiri)++;
			}
	for (j=0; j<4; j++){
		k=0;
		for (i=0; i<4; i++)
			if (m[i][j]!=0){
					m[k][j]=m[i][j];
					if(k!=i){
						 m[i][j]=0;
						 (*mutari)++;
					}
					k++;
			}
	}
	if ((*mutari) != 0 || (*alipiri) !=0) random_numbers(m);
}

/*
  * Functia "jos" realizeaza mutarea in jos
  * Matricea este parcursa de jos in sus pe coloane
*/
void jos(int (*m)[4], int *score, int *mutari, int *alipiri)
{
	int i, j, k;
	for (j=0; j<4; j++)
		for (i=3; i>0; i--)
			if (m[i][j]==m[i-1][j] && m[i][j]!=0){
								m[i][j]=2*m[i][j];
								*score+=m[i][j];
								m[i-1][j]=0;
								(*alipiri)++;
			}
			else if (i==3 && m[2][j]==0 && m[i][j]==m[1][j] && m[i][j]!=0){
									    		m[i][j]=2*m[i][j];
											*score+=m[i][j];
								 	     		m[1][j]=0;
									     		(*alipiri)++;
			}
			else if (i==3 && m[2][j]==0 && m[1][j]==0 && m[i][j]==m[0][j] && m[i][j]!=0){
										       		      m[i][j]=2*m[i][j];
												      *score+=m[i][j];
								 	     	       		      m[0][j]=0;
										       		      (*alipiri)++;
			}
			else if (i==2 && m[1][j]==0 && m[i][j]==m[0][j] && m[i][j]!=0){
									 		m[i][j]=2*m[i][j];
											*score+=m[i][j];
									 		m[0][j]=0;
									 		(*alipiri)++;
			}
	for (j=0; j<4; j++){
		k=3;
		for (i=3; i>=0; i--)
			if (m[i][j]!=0){
					m[k][j] = m[i][j];
					if(k!=i){
						 m[i][j]=0;
						 (*mutari)++;
					}
					k--;
			}
	}
	if ((*mutari != 0) || (*alipiri != 0)) random_numbers(m);
}

/*
  * Functia "validare_stanga" verifica daca mutarea la stanga este valida
  * Se retine intr-o matrice noua, matricea primita ca parametru
  * Dupa ce se aplica functia de mutare la stanga pentru noua matrice, se verifica
    daca numarul de mutari si alipiri este 0
  * Functia returneaza 1 daca mutarea nu este valida si 0 in cazul favorabil
*/
int validare_stanga(int (*m)[4])
{
	int mutari, alipiri, n[4][4], i, j, s;
	mutari = 0;
	alipiri = 0;
	s = 0;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			n[i][j]=m[i][j];
	stanga(n, &s, &mutari, &alipiri);
	if (mutari==0 && alipiri==0) return 1;
	return 0;
}

/*
  * Functia "validare_dreapta" verifica daca mutarea la dreapta este valida
  * Algoritmul este acelasi ca la functia "verificare_stanga", singura diferenta
    constand in functia apelata pentru mutarea elementelor din matrice
*/
int validare_dreapta(int (*m)[4])
{
	int mutari, alipiri, n[4][4], i, j, s;
	mutari = 0;
	alipiri = 0;
	s = 0;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			n[i][j]=m[i][j];
	dreapta(n, &s, &mutari, &alipiri);
	if (mutari==0 && alipiri==0) return 1;
	return 0;
}

/*
  * Functia "validare_sus" verifica daca mutarea in sus este valida
  * Pentru noua matrice este apelata functia aferenta mutarii in sus
*/
int validare_sus(int (*m)[4])
{
	int mutari, alipiri, n[4][4], i, j, s;
	mutari = 0;
	alipiri = 0;
	s = 0;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			n[i][j]=m[i][j];
	sus(n, &s, &mutari, &alipiri);
	if (mutari==0 && alipiri==0) return 1;
	return 0;
}

/*
  * Functia "validare_jos" verifica daca mutarea in jos este valida
  * Variabila 's' este trimisa ca parametru pentru scor
*/
int validare_jos(int (*m)[4])
{
	int mutari, alipiri, n[4][4], i, j, s;
	mutari = 0;
	alipiri = 0;
	s = 0;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			n[i][j]=m[i][j];
	jos(n, &s, &mutari, &alipiri);
	if (mutari==0 && alipiri==0) return 1;
	return 0;
}	

/*
  * Functia "piesa" realizeaza afisarea pe ecran a piesei corespunzatoare unei valori
  * Primeste ca parametru fereastra in care se va afisa piesa, indicii elementului din matrice
    si valoare acestuia
  * In functie de indici, se calculeaza coordonatele unde se va afisa piesa precum si 
    utilizarea unor atribute (culoare, bold) specifice pentru fiecare valoare (2, 4, 8, ...)
  * 
*/
void piesa(WINDOW *win_game, int y, int x, int val)
{
	int i, j;
	if (val==0)  //In functie de valoare, se activeaza anumite atribute de culoare si scris cu bold
		wattron(win_game, COLOR_PAIR(7) | A_BOLD);
	else if (val>0 && val<200)
		wattron(win_game, COLOR_PAIR(val) | A_BOLD);
	else wattron(win_game, COLOR_PAIR(val/16+1) | A_BOLD);
	for (i=y-1; i<y+2; i++)
		for (j=x; j<x+6; j++)
			mvwaddch(win_game, i, j, ' ');
	
	if (val==0) //daca valoare este 0 se va afisa o casuta libera
		mvwaddch(win_game, y, x+2, ' '); 
	else if (val<1000 && val!=0)
		mvwprintw(win_game, y, x+2, "%d", val);
	else
		mvwprintw(win_game, y, x+1, "%d", val);
	
	if (val==0) 
		wattroff(win_game, COLOR_PAIR(7) | A_BOLD);
	else if (val>0 && val<200)
		wattroff(win_game, COLOR_PAIR(val) | A_BOLD);
	else wattroff(win_game, COLOR_PAIR(val/16+1) | A_BOLD);
}

/*
  * Functia "celule_libere" primeste ca parametru matricea si returneaza numarul 
    de elemente egale cu 0
*/
int celule_libere(int (*n)[4])
{
	int k, i, j;
	k = 0;
	for (i=0; i<4; i++)
		for(j=0; j<4; j++)
			if (n[i][j]==0) k++;
	return k; 
		
}

/*
  * Functia pentru mutarea automata primeste ca parametru doar matricea jocului
  * Se initializeaza patru matrici corespunzatoare celor patru mutari posibile si
    se apeleaza functia de mutare specifica fiecareia
  * Variabila 'k' retine mutarea in urma careia raman cele mai multe elemente egale cu zero
  * Dupa fiecare apelare a functiei de mutare, variabilele pentru score, mutari si alipiri
    se reinitializeaza cu 0 pentru a verifica si daca mutarea este valida
  * Functia returneaza valoare lui 'k' corespunzatoare mutarii care elibereaza cele mai
    multe casute
*/
int mutare_automata(int (*m)[4])
{
	int up[4][4], right[4][4], down[4][4], left[4][4], i, j, max, k, s, mutari, alipiri;
	s = 0;
	mutari = 0;   //Initializari
	alipiri = 0;
	max = -1;
	k = 1;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++){
			up[i][j]=m[i][j];
			right[i][j] = m[i][j];
			down[i][j] = m[i][j];
			left[i][j] = m[i][j];
		}
	sus(up, &s, &mutari, &alipiri);
	if (( celule_libere(up)>max ) && validare_sus(up)==0){
		max = celule_libere(up);
		k = 1;
	}
	s = 0;
	mutari = 0;
	alipiri = 0;
	dreapta(right, &s, &mutari, &alipiri);
	if (( celule_libere(right)>max ) && validare_dreapta(right)==0){
		max = celule_libere(right);
		k = 2;
	}
	s = 0;
	mutari = 0;
	alipiri = 0;
	jos(down, &s, &mutari, &alipiri);
	if (( celule_libere(down)>max ) && validare_jos(down)==0){
		max = celule_libere(down);
		k = 3;
	}
	s = 0;
	mutari = 0;
	alipiri = 0;
	stanga(left, &s, &mutari, &alipiri);
	if (( celule_libere(left)>max ) && validare_stanga(left)==0){
		max = celule_libere(left);
		k = 4;
	}
	return k;
}

/*
  * In functia "menu" este realizat meniul jocului si afisarea ferestrei de start a jocului
  * Functia primeste ca argument doua variabile de tip WINDOW si doua variabile de tip PANEL
  * Ferestrele sunt incadrate in panel-uri pentru a realiza trecerea din meniu in tabela de joc si invers
  * Schimbul intre fereastra meniului si cea a tabelei de joc se realizeaza prin ascunderea unui panel si 
    afisarea celuilalt
  * Pentru nagivarea in meniu se folosesc sagetile si tasta ENTER pentru alegerea optiunii
  * Optiunea selectata este afisata cu un efect de highlight pe scris
  * In variabila 'runde' se retine numarul de runde jucate pana la momentul respectiv;
    Daca numarul de runde este diferit de 0, atunci optiunea Resume devine activa
  * Variabila 'mutari' retine numarul de mutari; este egala cu zero daca nu s-a jucat nici o runda
    sau s-a pierdut/castigat, respectiv diferita de zero daca s-a revenit la meniu dintr-un joc aflat  
    in desfasurare
*/
int menu(PANEL *panel_menu, WINDOW *win_menu, PANEL *panel_game, WINDOW *win_game, int *runde, int (*m)[4], int *score, int *moves)
{
	int i, op, j, nfds, sel;
	char optiuni[3][11];     //matrice de siruri pentru cele trei optiuni ale meniului
	fd_set read_descriptors;
	struct timeval timeout;
	time_t t;

	hide_panel(panel_game);  //se ascunde fereastra tabelei de joc
	keypad(win_game, FALSE);
	show_panel(panel_menu);  //se afiseaza doar fereastra meniului
	keypad(win_menu, TRUE);
	update_panels();         
	doupdate();
	
	strcpy(optiuni[0],"Start Game");
	strcpy(optiuni[1],"Resume");
	strcpy(optiuni[2],"Quit");

	wattron(win_menu, COLOR_PAIR(6));           //PENTRU CIFRA 2
	for (i=31; i<37; i++){        
		mvwaddch(win_menu,1,i,'=');
		mvwaddch(win_menu,3,i,'=');
		mvwaddch(win_menu,5,i,'=');
	}
	mvwaddch(win_menu,2,35,'|');
	mvwaddch(win_menu,2,36,'|');
	mvwaddch(win_menu,4,31,'|');
	mvwaddch(win_menu,4,32,'|');
	for (i=1; i<6; i++){
		mvwaddch(win_menu,i,29,' ');
		mvwaddch(win_menu,i,30,' ');
		mvwaddch(win_menu,i,37,' ');
		mvwaddch(win_menu,i,38,' ');
	}
	for (i=31;i<35;i++){
		mvwaddch(win_menu,2,i,' ');
		mvwaddch(win_menu,4,i+2,' ');
	}

	wattroff(win_menu, COLOR_PAIR(6));

	wattron(win_menu,COLOR_PAIR(3));            //PENTRU CIFRA 0
	for (i=43; i<49; i++){
		mvwaddch(win_menu,1,i,'=');
		mvwaddch(win_menu,5,i,'=');
	}
	for (i=2; i<5; i++){
		mvwaddch(win_menu,i,43,'|');
		mvwaddch(win_menu,i,44,'|');
		mvwaddch(win_menu,i,47,'|');
		mvwaddch(win_menu,i,48,'|');
	}
	for (i=1; i<6; i++){
		mvwaddch(win_menu,i,41,' ');
		mvwaddch(win_menu,i,42,' ');
		mvwaddch(win_menu,i,49,' ');
		mvwaddch(win_menu,i,50,' ');
	}
	for (i=2; i<5; i++){
		mvwaddch(win_menu,i,45,' ');
		mvwaddch(win_menu,i,46,' ');
	}
	wattroff(win_menu,COLOR_PAIR(3));

	wattron(win_menu,COLOR_PAIR(9));           //PENTRU CIFRA 4
	for (i=31; i<37; i++)
		mvwaddch(win_menu,9,i,'=');
	for (i=7; i<9; i++){
		mvwaddch(win_menu,i,31,'|');
		mvwaddch(win_menu,i,32,'|');
		mvwaddch(win_menu,i,35,'|');
		mvwaddch(win_menu,i,36,'|');
	}
	for (i=10; i<12; i++){
		mvwaddch(win_menu,i,35,'|');
		mvwaddch(win_menu,i,36,'|');
	}
	for (i=7; i<12; i++){
		mvwaddch(win_menu,i,29,' ');
		mvwaddch(win_menu,i,30,' ');
		mvwaddch(win_menu,i,37,' ');
		mvwaddch(win_menu,i,38,' ');
	}
	for (i=31; i<35; i++){
		mvwaddch(win_menu,10,i,' ');
		mvwaddch(win_menu,11,i,' ');
	}
	for (i=33; i<35; i++){
		mvwaddch(win_menu,7,i,' ');
		mvwaddch(win_menu,8,i,' ');
	}
	wattroff(win_menu,COLOR_PAIR(9));

	wattron(win_menu,COLOR_PAIR(5));          //PENTRU CIFRA 8
	for (i=43; i<49; i++){
		mvwaddch(win_menu,7,i,'=');
		mvwaddch(win_menu,9,i,'=');
		mvwaddch(win_menu,11,i,'=');
	}
	for (i=43; i<45; i++){
		mvwaddch(win_menu,8,i,'|');
		mvwaddch(win_menu,10,i,'|');
	}
	for (i=47; i<49; i++){
		mvwaddch(win_menu,8,i,'|');
		mvwaddch(win_menu,10,i,'|');
	}
	for (i=7; i<12; i++){
		mvwaddch(win_menu,i,41,' ');
		mvwaddch(win_menu,i,42,' ');
		mvwaddch(win_menu,i,49,' ');
		mvwaddch(win_menu,i,50,' ');
	}
	for (i=45; i<47; i++){
		mvwaddch(win_menu,8,i,' ');
		mvwaddch(win_menu,10,i,' ');
	}
	wattroff(win_menu,COLOR_PAIR(5));


	for (i=33; i<47; i++){                 //PENTRU MENIU
		mvwaddch(win_menu,14,i,'#');
		mvwaddch(win_menu,19,i,'#');
	}
	for (i=15; i<19; i++){
		mvwaddch(win_menu,i,33,'#');
		mvwaddch(win_menu,i,46,'#');
	}

	for (i=0; i<3; i++){                  //Se afiseaza optiunile meniului, prima avand highlight pe scris
		if (i==0) 
			wattron(win_menu,A_STANDOUT);
		else 
			wattroff(win_menu,A_STANDOUT);
		mvwprintw(win_menu,i+15, 35, "%s", optiuni[i]);
	}
	time(&t);                                     //Se actualizeaza timpul si se afiseaza
	mvwprintw(win_menu, 22,3, "%s", ctime(&t));  
	box(win_menu,0,0);

	nfds = 1;
	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;	//Timpul dupa care se va actualiza fereastra

	wrefresh(win_menu);
	update_panels();
	doupdate();
	j=0;	/*Se initializeaza cu 0 variabila care retine pozitia optiunii din meniu care va fi afisata cu highlight
		  Valorile sunt 0, 1 si 2 corespunzatoare pentru New game, Resume si Quit*/	
	while (1){
		sel = select(nfds, &read_descriptors, NULL, NULL, &timeout); //Se asteapta apasarea unei taste
		if (sel == 1){			//Daca se apasa o tasta, se intra in instructiunea switch
			op = wgetch(win_menu); 
			time(&t);
			switch( op ) {
		        	case KEY_UP:		//Daca se apasa tasta "sus", se afiseaza cu highlight optiunea de deasupra (daca aceasta nu e 0)
					    if (j==0) j=0;
					    if (j==1 || j==2) j--;
					    break;		
				case KEY_DOWN:		//Daca se apasa tasta "jos", se afiseaza cu highlight optiunea de dedesupt (daca aceasta nu e 2)
					    if (j==2) j=2;
					    if (j==0 || j==1) j++;
					    break;
				case 10: 		//La apasareaa tastei ENTER se executa instructiunile corespunzatoare optiunii alese
					    if (j==0) 
						new_game(panel_menu, panel_game, win_menu, win_game, runde, m, score, moves);
					    else if (j==1 && *runde!=0) resume(panel_menu, panel_game, win_menu, win_game, runde, m, score, moves);
					    else if (j==2){
							   delwin(win_game);
							   delwin(win_menu);
							   endwin();
							   return 1;
							}
					    
					    break;
				}
		} else if (sel == 0) 
			time(&t);    //Daca nu este apasata nici o tasta, doar se actualizeaza timpul
		for (i=0; i<3; i++){ //Se afiseaza optiunile
			if (i==j) 
				wattron(win_menu, A_STANDOUT);
			else 
				wattroff(win_menu, A_STANDOUT);
			mvwprintw(win_menu, i+15, 35, "%s", optiuni[i]);
			wattroff(win_menu, A_STANDOUT);
		}
		mvwprintw(win_menu, 22,3, "%s", ctime(&t));
		box(win_menu,0,0);
		wrefresh(win_menu);
		update_panels();
		doupdate();
		
		FD_SET(0, &read_descriptors);
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000; //Timpul de asteptare al unei taste (0,5 sec)
	}
}

/*
  * Functia "new_game" realizeaza afisarea ecranului de joc
  * Fereastra de meniu este ascunsa si cea de joc este afisata
  * Este afisata legenda comenzilor valide (mutari - W,A,S,D; undo, restart, auto si quit)
    Sunt afisate si numarul de mutari, ultima mutare si un cronometru cu timpul pana la mutarea automata
  * Matricea 'm_undo' retine matricea jocului inaintea unei mutari pentru a putea realiza functia de undo
  * Fereastra se actualizeaza la fiecare 0,5 sec (parcurgerea structurii while). Mutarea automata se face 
    odata la 10 parcurgeri in care nu s-a apasat nici o tasta (5 sec)
*/
void new_game(PANEL *panel_menu, PANEL *panel_game, WINDOW *win_menu, WINDOW *win_game, int *runde, int (*m)[4], int *score, int *moves)
{
	int score_undo, m_undo[4][4], i, j, op, k, mutari, alipiri, nfds, sel, z, at, u;
	char mutare[5];
	fd_set read_descriptors;
	struct timeval timeout;
	time_t t;
	hide_panel(panel_menu);    //Se ascunde fereastra meniului
	keypad(win_menu, FALSE);
	show_panel(panel_game);    //Se afiseaza fereastra tabelei de jos
	keypad(win_game, TRUE);
	intrflush(win_game, TRUE);
	update_panels();
	doupdate();
	for (i=0; i<5; i++)       //Se initializeaza sirul de caractere pentru afisarea ultimei mutari
		mutare[i] = ' ';
	mutari = 0;      	  //Se initializeaza cu 0 numarul de mutari, alipiri, numarul total de mutari
	alipiri = 0;		 
	(*moves) = 0;
	u = 0;		//Variabila 'u' este folosita pentru a verifica daca s-a utilizat comanda de undo; aceasta ia valoarea 0 sau 1
	at = 5;         //Variabila 'at' retine secundele pana la mutarea automata
	(*runde)++;
	time(&t);
	for (i=0; i<4; i++)	   //Elementele matricei jocului iau valoare 0
		for (j=0; j<4; j++)
			m[i][j]=0;
	*score=0;
	random_numbers(m);	  //Se afiseaza doua numere random cu valoarea 2 sau 4
	random_numbers(m);
	
	for (i=0; i<4; i++)	  //Se initializeaza si matricea pentru undo cu cea initiala
		for (j=0; j<4; j++)
			m_undo[i][j]=m[i][j];
	score_undo = 0;
	
	for (i=0; i<4; i++)	//Pentru fiecare element al matricei se afiseaza in fereastra jocului, piesa corespunzatoare valorii
		for (j=0; j<4; j++)
			piesa(win_game, 4*(i+1), 24+8*j, m[i][j]);
	for (i=3; i<18; i++)     		//Afisarea marginilor tabelei de joc
		for (j=0; j<5; j++){
			mvwaddch(win_game, i, 22+8*j, '|');
			mvwaddch(win_game, i, 23+8*j, '|');
		}

	for (i=24; i<54; i++){	
		mvwaddch(win_game, 2, i, '=');
		mvwaddch(win_game, 6, i, '=');
		mvwaddch(win_game, 10, i, '=');
		mvwaddch(win_game, 14, i, '=');
		mvwaddch(win_game, 18, i, '=');
	}
	for (i=19; i<22; i++)
		for (j=30; j<50; j++)
			mvwaddch(win_game, i,j,' ');
	mvwprintw(win_game, 3, 3, "SCORE: %d        ", *score);
	wattron(win_game, COLOR_PAIR(12));
	
	for (i=3; i<16; i++)		       //Afisarea legendei cu comenzile valide
		mvwaddch(win_game,4,i, '-');
	mvwprintw(win_game,5, 3, "U - Undo");
	mvwprintw(win_game,7, 3, "R - Restart");
	mvwprintw(win_game,9, 3, "X - AUTO");
	mvwprintw(win_game, 11,3, "Q - Quit");
	for (i=3; i<16; i++)
		mvwaddch(win_game,12,i, '-');
	mvwprintw(win_game, 3, 59, "Moves: ");
	mvwprintw(win_game, 5, 59, "W - Up");
	mvwprintw(win_game, 7, 59, "A - Left");
	mvwprintw(win_game, 9, 59, "S - Down");
	mvwprintw(win_game, 11, 59, "D - Right");
	for (i=59; i<74; i++)
		mvwaddch(win_game, 12, i, '-');
	wattroff(win_game, COLOR_PAIR(12));
	mvwprintw(win_game, 14, 3, "Moves: %d         ", *moves); //Numarul total de mutari
	mvwprintw(win_game, 16, 3, "Last move:      ");    	  //Ultima mutare

	wattron(win_game, COLOR_PAIR(75));
	mvwprintw(win_game, 14, 59, "Auto move: %d sec.", at);    //Cronometru pentru automove
	wattroff(win_game, COLOR_PAIR(75));
	mvwprintw(win_game, 22,3, "%s", ctime(&t));
	box(win_game,0,0);
	
	nfds = 1;
	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;
	
	wrefresh(win_game); 
	update_panels();
	doupdate();
	z=0;       //Variabila 'z' se incrementeaza la fiecare parcurgere a struncturii while si ajuta la mutara automata
	while (1){
		sel = select(nfds, &read_descriptors, NULL, NULL, &timeout);
		k = 0;   //La fiecare parcurgere se initializeaza cu 0 indicele mutarii automate
		z++;
		if ( z%2 == 0) at--;  /*Daca numarul de parcurgeri al structurii este divizibil cu 2 (0,2,4,5,6,8,10) 
				        scade numarul de secunde pana la mutarea automata cu 1 (scade odata la 2 parcurgeri)*/
		if (sel == 1){
			op = wgetch(win_game);  
			mutari = 0;
			alipiri = 0;
			time(&t);
			switch( op ) {    //Se intra in structura switch
		        	case 'w':
					    if (validare_sus(m)==0){   /*Daca mutarea este valida, matricea de undo ia valoarea matricei
									 si dupa aceea se realizeaza mutarea si incrementarea numarului de mutari*/
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		sus(m, score, &mutari, &alipiri); 
					    		(*moves)++;
							u = 0;    //Daca se apasa o tasta valida (mutare) comanda de undo devine valida
							strcpy(mutare, "Up"); //Se copiaza ultima tasta apasata pentru afisare
					    }
					    z=0;   	//Se initializeaza cu 0 numarul de parcurgeri ale structurii while
					    at = 5;	//Numarul de secunde pana la mutare automata revine la 5
					    break;
				case 's':
					    if (validare_jos(m)==0){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		jos(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Down");
					    }
					    z=0;
					    at = 5;
					    break;
				case 'a': 
					    if (validare_stanga(m)==0){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		stanga(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Left");
					    }
				  	    z=0;
					    at = 5;
					    break;
				case 'd':
					    if (validare_dreapta(m)==0){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		dreapta(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Right");
					    }
					    z=0;
					    at = 5;
					    break;
				case 'x':    			     //Pentru mutarea automata
					    k = mutare_automata(m);  //Se apeleaza functia pentru mutare automata
					    if (k==1){		     /*Se realizeaza mutarea cu acelasi algoritm ca la mutarile 
								       obisnuite, in functie de valoare lui k */ 
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		sus(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==2){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		dreapta(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==3){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		jos(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==4){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		stanga(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    }
					    z=0;
					    at = 5;	
					    break;
				case 'r':
					    new_game(panel_menu,panel_game, win_menu,  win_game, runde, m, score, moves);
					    z=0;
					    at = 5;
					    break;
				case 'u':			//Se realizeaza comanda undo
					    for (i=0; i<4; i++)
						for (j=0; j<4; j++)
							m[i][j]=m_undo[i][j]; //Matricea ia valoarea matricei de undo, la fel si scorul
					    *score = score_undo;
					    z=0;
				   	    at = 5;
					    if ((*moves)>0 && u == 0) {  //Daca mutarea undo este valida, numarul de mutari scade cu 1
							(*moves)--;
							u = 1;		 //Mutarea undo devine inaccesibila
					    }	
					    strcpy(mutare, "Undo");				
					    break;
				case 'q':   			//Se iese din tabela de joc si se revine la meniu
					    menu(panel_menu, win_menu, panel_game, win_game, runde, m, score, moves);
					    break;
				}
		} else if (sel == 0) {
				time(&t);
				
		}
		if (z == 10){		    //Dupa parcurgerea de 10 ori a instructiunii while fara sa se apese nici o tasta, are loc mutarea automata
				k = mutare_automata(m);
					    if (k==1){	//Algoritmul este acelasi ca la apasarea tastei de mutare automata
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		sus(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==2){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		dreapta(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==3){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		jos(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==4){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		stanga(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    }
				z = 0;	
				at = 5;
		}
		
			
		for (i=0; i<4; i++)	//Se afiseara in tabela de joc piesele corespunzatoare valorilor elementelor matricii
			for (j=0; j<4; j++)
				piesa(win_game, 4*(i+1), 24+8*j, m[i][j]);
		mvwprintw(win_game, 3, 3, "SCORE: %d   ", *score);	//Se afiseaza scorul actualizat	
		mvwprintw(win_game, 14, 3, "Moves: %d   ", *moves);     //Numarul de mutari
		mvwprintw(win_game, 16, 3, "Last move: %s   ", mutare); //Ultima mutare
		wattron(win_game, COLOR_PAIR(15*at));
		mvwprintw(win_game, 14, 59, "Auto move: %d sec.", at);  //Se afiseaza cronometrul actualizat
		wattroff(win_game, COLOR_PAIR(15*at));
		for (i=0; i<4; i++)		//Dupa fiecare mutare se parcurge matrice. Daca un element este egal cu 2048, jocul este castigat
			for (j=0; j<4; j++)	//Daca nici o mutare nu este valida atunci jocul este pierdut
				if (m[i][j]==2048) castigat(panel_menu, panel_game, win_menu, win_game, runde, m, score, moves);
		if (validare_stanga(m)==1 && validare_dreapta(m)==1 && validare_jos(m)==1 && validare_sus(m)==1)
			 pierdut(panel_menu, panel_game, win_menu, win_game, runde, m, score, moves);
		mvwprintw(win_game, 22,3, "%s", ctime(&t));	
		box(win_game,0,0);	
		wrefresh(win_game);
		
		FD_SET(0, &read_descriptors);
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000;
	}
}

/*
  * Functia "castigat" realizeaza fereastra afisata la castigarea jocului
  * Fereastra coincide cu cea a jocului, singura diferenta fiind afisare unui mesaj corespunzator
  * Tastele de mutare devin invalide, singurele optiuni fiind inceperea unui joc nou sau revenirea la meniu
*/
void castigat(PANEL *panel_menu, PANEL *panel_game, WINDOW *win_menu, WINDOW *win_game, int *runde, int (*m)[4], int *score, int *moves)
{
	int op, nfds, sel;
	fd_set read_descriptors;
	struct timeval timeout;
	time_t t;
	time(&t);
	mvwprintw(win_game, 22,3, "%s", ctime(&t));
	box(win_game,0,0);
	wattron(win_game, COLOR_PAIR(10));
	mvwprintw(win_game, 19, 35, "YOU WIN!!!");  	 //Afisarea mesajului atunci cand se castiga
	wattroff(win_game, COLOR_PAIR(10));	
	wattron(win_game, COLOR_PAIR(12));
	mvwprintw(win_game, 20, 32, "Q - Back to menu"); //Afisarea optiunilor disponibile
	mvwprintw(win_game, 21, 34, "N - New Game");
	wattroff(win_game, COLOR_PAIR(12));
	
	nfds = 1;
	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;	
	wrefresh(win_game);
	*runde=0;
	while (1){
		sel = select(nfds, &read_descriptors, NULL, NULL, &timeout); //Se verifica daca se apasa o tasta
		if (sel == 1){
			op = wgetch(win_game);
			time(&t);
			switch( op ) {
				      case 'q':		//Revenire la meniu 
					    menu(panel_menu, win_menu, panel_game, win_game, runde, m, score, moves);
					    break;
				      case 'n':		//Inceperea unui joc nou
					    new_game(panel_menu, panel_game, win_menu, win_game, runde, m, score, moves);
					    break;
			}
		} else if (sel == 0)
			time(&t); //Daca nu se apasa nici o tasta, doar se actualizeaza timpul
	
	mvwprintw(win_game, 22,3, "%s", ctime(&t)); //Afisare timp
	box(win_game,0,0);
	wrefresh(win_game);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;
	}	
}

/*
  * Functia "pierdut" realizeaza fereastra afisata la pierderea jocului
  * Tastele de mutare devin invalide, singurele optiuni fiind inceperea unui joc nou sau revenirea la meniu
  * Functia are acelasi algoritm ca cel al functiei pentru castigarea jocului, singura diferenta fiind mesajul afisat
*/
void pierdut(PANEL *panel_menu, PANEL *panel_game, WINDOW *win_menu, WINDOW *win_game, int *runde, int (*m)[4], int *score, int *moves)
{
	int op, nfds, sel;
	fd_set read_descriptors;
	struct timeval timeout;
	time_t t;
	time(&t);
	mvwprintw(win_game, 22,3, "%s", ctime(&t));
	box(win_game,0,0);
	wattron(win_game, COLOR_PAIR(11));
	mvwprintw(win_game, 19, 35, "YOU LOSE!!!"); //Afisarea mesajului atunci cand se pierde
	wattroff(win_game, COLOR_PAIR(11));
	wattron(win_game, COLOR_PAIR(12));
	mvwprintw(win_game, 20, 32, "Q - Back to menu"); //Afisarea optiunilor disponibile
	mvwprintw(win_game, 21, 34, "N - New Game");
	wattroff(win_game, COLOR_PAIR(12));

	nfds = 1;
	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;
	wrefresh(win_game);
	*runde=0;
	while (1){
		sel = select(nfds, &read_descriptors, NULL, NULL, &timeout); //Verificarea apasarii unei taste (1-caz favorabil, 0-caz nefavorabil)
		if (sel == 1){
			op = wgetch(win_game);
			time(&t);
			switch( op ) {
				      case 'q':   
					    menu(panel_menu, win_menu, panel_game, win_game, runde, m, score, moves);
					    break;
				      case 'n':
					    new_game(panel_menu, panel_game, win_menu, win_game, runde, m, score, moves);
					    break;
			}
		} else if (sel == 0)
			time(&t);
	
	mvwprintw(win_game, 22,3, "%s", ctime(&t));
	box(win_game,0,0);
	wrefresh(win_game);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;
	}
}


/*
  * Functia "resume" are acelasi algoritm ca cel al functie new_game, cu cateva diferente
  * Nu se mai afiseaza legenda comenzilor valide deoarece, este deja afisata de functia new_game
  * De asemenea, structura while este aceiasi de la functia new_game pentru ca, aceasta functie realizeaza,
    in mare, redeschiderea ferestrei de joc si reluarea jocului din stadiul in care a fost inchis
  * Variabilele declarare in interiorul functiei au acelasi nume si rol ca si in functia new_game pentru a face
    codul mai usor de inteles 
*/
void resume(PANEL *panel_menu, PANEL *panel_game, WINDOW *win_menu, WINDOW *win_game, int *runde, int (*m)[4], int *score, int *moves)
{
	int score_undo, m_undo[4][4], i, j, k, op, mutari, alipiri, nfds, sel, z, at, u;
	char mutare[5];
	fd_set read_descriptors;
	struct timeval timeout;
	time_t t;
	hide_panel(panel_menu);
	keypad(win_menu, FALSE);
	show_panel(panel_game);
	keypad(win_game, TRUE);
	update_panels();
	doupdate();
	mutari = 0;
	alipiri = 0;
	u = 0;       //Variabila pentru verificarea validitatii comenzii de undo
	at = 5;      //Variabila pentru secundele ramase pana la mutare automata
	time(&t);

	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			m_undo[i][j]=m[i][j];
	score_undo = *score;
	wattron(win_game, COLOR_PAIR(75));
	mvwprintw(win_game, 14, 59, "Auto move: %d sec.", at);
	wattroff(win_game, COLOR_PAIR(75));
	mvwprintw(win_game, 16, 3, "Last move:      ");   
	mvwprintw(win_game, 22,3, "%s", ctime(&t));
	box(win_game,0,0);
	nfds = 1;
	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;

	wrefresh(win_game); 
	update_panels();
	doupdate();
	z = 0;
	while (1){
		sel = select(nfds, &read_descriptors, NULL, NULL, &timeout);
		k = 0;
		z++;
		if ( z%2 == 0) at--;
		if (sel == 1){
			op = wgetch(win_game);
			mutari = 0;
			alipiri = 0;
			time(&t);
			switch( op ) {
		        	case 'w':
					    if (validare_sus(m)==0){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		sus(m, score, &mutari, &alipiri);
					    		(*moves)++;
							u = 0;
							strcpy(mutare, "Up");
					    }
					    z=0;
					    at = 5;
					    break;
				case 's':
					    if (validare_jos(m)==0){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		jos(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Down");
					    }
					    z=0;
					    at = 5;
					    break;
				case 'a': 
					    if (validare_stanga(m)==0){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		stanga(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Left");
					    }
				  	    z=0;
					    at = 5;
					    break;
				case 'd':
					    if (validare_dreapta(m)==0){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		dreapta(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Right");
					    }
					    z=0;
					    at = 5;
					    break;
				case 'x': 
					    k = mutare_automata(m);
					    if (k==1){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		sus(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==2){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		dreapta(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==3){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		jos(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==4){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		stanga(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    }
					    z=0;
					    at = 5;	
					    break;
				case 'r':
					    new_game(panel_menu,panel_game, win_menu,  win_game, runde, m, score, moves);
					    z=0;
					    at = 5;
					    break;
				case 'u':
					    for (i=0; i<4; i++)
						for (j=0; j<4; j++)
							m[i][j]=m_undo[i][j];
					    *score = score_undo;
					    z=0;
				   	    at = 5;
					    if ((*moves)>0 && u == 0) {
							(*moves)--;
							u = 1;	
					    }	
					    strcpy(mutare, "Undo");				
					    break;
				case 'q':   
					    menu(panel_menu, win_menu, panel_game, win_game, runde, m, score, moves);
					    break;
				}
		} else if (sel == 0) {
				time(&t);
				
		}
		if (z == 10){
				k = mutare_automata(m);
					    if (k==1){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		sus(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==2){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		dreapta(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==3){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
				 	    		jos(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    } else if (k==4){
							for (i=0; i<4; i++)
								for (j=0; j<4; j++)
									m_undo[i][j]=m[i][j];
					    		score_undo = *score;
					    		stanga(m, score, &mutari, &alipiri);
							(*moves)++;
							u = 0;
							strcpy(mutare, "Auto");
					    }
				z = 0;	
				at = 5;
		}
		
			
		for (i=0; i<4; i++)
			for (j=0; j<4; j++)
				piesa(win_game, 4*(i+1), 24+8*j, m[i][j]);
		mvwprintw(win_game, 3, 3, "SCORE: %d   ", *score);
		mvwprintw(win_game, 14, 3, "Moves: %d   ", *moves);
		mvwprintw(win_game, 16, 3, "Last move: %s   ", mutare); 
		wattron(win_game, COLOR_PAIR(15*at));
		mvwprintw(win_game, 14, 59, "Auto move: %d sec.", at);
		wattroff(win_game, COLOR_PAIR(15*at));
		for (i=0; i<4; i++)
			for (j=0; j<4; j++)
				if (m[i][j]==2048) castigat(panel_menu, panel_game, win_menu, win_game, runde, m, score, moves);
		if (validare_stanga(m)==1 && validare_dreapta(m)==1 && validare_jos(m)==1 && validare_sus(m)==1)
			 pierdut(panel_menu, panel_game, win_menu, win_game, runde, m, score, moves);
		mvwprintw(win_game, 22,3, "%s", ctime(&t));	
		box(win_game,0,0);	
		wrefresh(win_game);
		
		FD_SET(0, &read_descriptors);
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000;
	}
}

/*
  * In main sunt initializate ferestrele si panel-urile
  * De aici se apeleaza functia "menu" iar restul functiilor se apeleaza din interiorul altora
*/
int main(){

	int i, j, runde, m[4][4], score, moves, b;
	WINDOW *win_game, *win_menu;
	PANEL *panel_game, *panel_menu;
	initscr();
	noecho();
	curs_set(0);
	runde=0;   //Initializare numarului de runde
	moves=0;   //Initializare numarului de mutari
	
	start_color();      //Initializare culori
	init_pair(1, 7, 237);
	init_pair(3, 7, 4);
	init_pair(5, 7, 3);
	init_pair(6, 7, 2);
	init_pair(7, 7, 7);
	init_pair(9, 7, 1);
	init_pair(10, 2, 237);
	init_pair(11, 1, 237);
	init_pair(12, 3, 237);
	init_pair(15, 1, 237);
	init_pair(30, 130, 237);
	init_pair(45, 3, 237);
	init_pair(60, 112, 237);
	init_pair(75, 2, 237);
	init_pair(2, 16, 231);
	init_pair(4, 16, 215);
	init_pair(8, 16, 172);
	init_pair(16, 16, 202);
	init_pair(32, 16, 9);
	init_pair(64, 16, 88);
	init_pair(128, 16, 11);
	init_pair(17, 16, 220);
	init_pair(33, 16, 136);
	init_pair(65, 16, 93);
	init_pair(129, 16, 82);
	keypad(stdscr, FALSE);

	win_menu=newwin(24,80,0,0);       //Initializarea ferestrei pentru meniu
	wbkgd(win_menu, COLOR_PAIR(1));
	wrefresh(win_menu);
	panel_menu = new_panel(win_menu); //Crearea unui nou panel pentru meniu
	hide_panel(panel_menu);
	update_panels();
	doupdate();	

	win_game=newwin(24,80,0,0);       //Initializarea ferestrei pentru joc
	wbkgd(win_game, COLOR_PAIR(1));
	wrefresh(win_game);
	panel_game = new_panel(win_game); //Crearea unui nou panel pentru joc
	hide_panel(panel_game);
	update_panels();
	doupdate();
	
	for (i=0; i<4; i++)		  //Elementele matricei sunt initializate cu 0
		for (j=0; j<4; j++)
			m[i][j]=0;
	score=0;
	//Variabila 'b' retine valoarea returnata de functia menu. Functia menu returneaza valoare 1 la alegerea optiunii Quit	
	b = menu(panel_menu, win_menu, panel_game, win_game, &runde, m, &score, &moves); 
	refresh();
	if (b==1) endwin(); //Daca valoarea este 1 se inchide fereastra jocului
	system("clear");	

	return 0;
}
