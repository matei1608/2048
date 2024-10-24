#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
// ca sa evit anumite warning-uri adaug un prototip
// al functiei meniu_principal la inceputul codului
void meniu_principal(int y0, int x0, int mat[4][4], int resume, int *punctaj);
void tablou_joc(WINDOW *joc, WINDOW * panou, int mat[4][4],int punctaj)
{
    //functie care imi creaza tabla de joc si panoul de control
    int i, j;
    char numar[6], punctaj_char[15], timp_s[26];
    time_t timp;
    //creez tabla de joc
    box(joc, 0, 0); //il face cutie
    mvwprintw(joc, 0, 0, "JOC"); //ii pune titlu
    //creez liniile si coloanele ca sa formez o tabla 4X4
    wmove(joc, 5, 1);
    whline(joc, ACS_HLINE, 43);
    wmove(joc, 10, 1);
    whline(joc, ACS_HLINE, 43);
    wmove(joc, 15, 1);
    whline(joc, ACS_HLINE, 43);
    wmove(joc, 1, 11.25);
    wvline(joc, ACS_VLINE, 19);
    wmove(joc, 1, 22.5);
    wvline(joc, ACS_VLINE, 19);
    wmove(joc, 1, 33.75);
    wvline(joc, ACS_VLINE, 19);
    wrefresh(joc);
    for(i = 0; i < 4; i ++)
    {
        for(j = 0; j < 4; j ++)
        {
            if(mat[i][j] != 0)
            {
                //transform numarul in char
                sprintf(numar, "%d", mat[i][j]);
                //toata acea formula am creat-o
                //ca sa centrez numarul in casuta
                wmove(joc, 2.5 + 5 * i, 5 + 11 * j - strlen(numar) + 1 + ((10 - strlen(numar)) / 2 - 5 + strlen(numar)));
                //adaug numarul in tablou
                waddstr(joc, numar);
                wrefresh(joc);
                
            }
        }
    }
    wrefresh(joc); 
    //crez panoul de control
    // pun in variabila timp_s ora curenta
    time(&timp);
    strcpy(timp_s, ctime(&timp) + 4);
    timp_s[20] = '\0';
    //adaug timpul
    mvwaddstr(panou, 2, 1, timp_s);
    //adaug scorul
    mvwaddstr(panou, 6, 1, "SCORE: ");
    // transform scorul din int in char
    sprintf(punctaj_char, "%d", punctaj); 
    waddstr(panou, punctaj_char);
    // adaug legenda comenzilor
    mvwaddstr(panou, 10, 1, "Arrow Up - MOVE UP\n Arrow Down - MOVE DOWN");
    mvwaddstr(panou, 12, 1, "Arrow Left - MOVE LEFT\n Arrow Right - Move Right");
    mvwaddstr(panou, 14, 1, "Q - Go Back To Menu");
    wrefresh(panou);
}
void creare_meniu(int ys, int xs, char optiuni[4][20], WINDOW *win)
{
    //functie care imi creaza meniul
    wmove(win, ys, xs);
    //adaug optiunea NEW GAME
    waddstr(win, optiuni[0]);
    wmove(win, ys + 2, xs);
    wrefresh(win);
    //adaug optiunea RESUME
    waddstr(win, optiuni[1]);
    wmove(win, ys + 4, xs);
    wrefresh(win);
    //adaug optiunea QUIT
    waddstr(win, optiuni[2]);
    wrefresh(win);
}
void colorare(int ys, int xs, char optiuni[4][20], WINDOW *win, int option)
{
    //functie care imi da highlight la optiunea pe care sunt cu cursorul pe meniul principal
    option--;
    //activezi scrierea textului cu highlight
    wattron(win, COLOR_PAIR(1) | A_STANDOUT);
    wmove(win, ys + 2 * option, xs);
    wrefresh(win);
    //matricea optiuni cuprinde cele 3 optiuni:
    //index 0 - New Game
    //index 1 - Resume
    //index 2 - Quit
    //waddstr(win, optiuni[option + 1]);
    waddstr(win, optiuni[option]);
    wrefresh(win);
    //dezactivez scrierea textului cu highlight
    wattroff(win, COLOR_PAIR(1) | A_STANDOUT);
}
void randomm(int mat[4][4])
{
    //functie pentru punerea unei valori random in matrice
    int poz_random, val_random, x, y;
    //setez ca sa fie random
    //si sa nu apara aceleasi numere
    srand(time(0));
    poz_random = 0;
    do
    {
        //aleg pozitia random
        poz_random = (rand() % 16);
        //aleg valoarea random
        val_random = (rand() % 2 + 1) * 2;
        //aflu coordonatele pozitiei in matricea in care retin valorile
        x = poz_random / 4;
        y = poz_random % 4;
        //se executa loop-ul pana cand gasesc o pozitie libera
        //pe care sa pun valoarea
    }while(mat[x][y] != 0);
    if(mat[x][y] == 0) mat[x][y] = val_random;
}
void init_0(int ok[4][4])
{
    //functie pentru initializarea cu 0 a unei matrici
    int i, j;
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++) ok[i][j] = 0;
    }
}
void key_down(int mat[4][4], int *punctaj)
{
    //functie pentru mutarea in jos
    int i, j, k;
    //ok[i][j] = 1 => elementul de pe pozitia i si j a aparut
    // prin contopirea a doua valori de ex: 4 = 2 + 2
    //ok[i][j] = 0 => elementul nu a aparut prin contopire
    //si se afla deja pe tabla dinainte mutarii
    int ok[4][4];
    //initializam cu 0 matricea ok
    init_0(ok);
    for(i = 3; i >= 0; i--)
    {
        for(j = 0; j < 4; j++)
        {
            //daca gasim un element diferit de 0 incercam sa-l mutam
            if(mat[i][j] != 0)
            {
                k = 4;
                //cautam pozitie libera unde sa mutam elementul
                for(k = i + 1; k < 4; k++)
                {
                    if(mat[k][j] != 0)
                        break;
                }
                k--;
                //daca am gasit pozitie libera, mutam elementul
                if(k != i && mat[k][j] == 0)
                {
                    mat[k][j] = mat[i][j];
                    mat[i][j] = 0;
                }
                //daca vecinul elementului pe care l-am mutat
                //are aceeasi valoare ca elementul nostru
                //le contopim
                if(k < 3 && ok[k][j] == 0 && ok[k + 1][j] == 0 && mat[k][j] == mat[k + 1][j])
                {
                    mat[k + 1][j] = 2 * mat[k + 1][j];
                    mat[k][j] = 0;
                    //marcam elementul nou aparut format din contopirea
                    //celorlalte doua elemente cu 1 in matricea ok
                    ok[k + 1][j] = 1;
                    //crestem scorul
                    *punctaj = *punctaj + mat[k + 1][j];
                }
            }
            
        }
    }
}
void key_up(int mat[4][4], int *punctaj)
{
    //functie pentru mutarea in sus
    int i, j, k;
    //ok[i][j] = 1 => elementul de pe pozitia i si j a aparut
    // prin contopirea a doua valori de ex: 4 = 2 + 2
    //ok[i][j] = 0 => elementul nu a aparut prin contopire
    //si se afla deja pe tabla dinainte mutarii
    int ok[4][4];
    //initializam cu 0 matricea ok
    init_0(ok);
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            //daca gasim un element diferit de 0 incercam sa-l mutam
            if(mat[i][j] != 0)
            {
                k = 4;
                //cautam o pozitie libera unde sa mutam elementul
                for(k = i - 1; k >= 0; k--)
                {
                    if(mat[k][j] != 0)
                        break;
                }
                k++;
                //daca am gasit o pozitie libera, mutam elementul
                if(k != i && mat[k][j] == 0)
                {
                    mat[k][j] = mat[i][j];
                    mat[i][j] = 0;
                }
                //daca vecinul elementului pe care l-am mutat
                //are aceeasi valoare ca elementul nostru
                //le contopim
                if(k > 0 && ok[k][j] == 0 && ok[k - 1][j] == 0 && mat[k][j] == mat[k - 1][j])
                {
                    mat[k - 1][j] = 2 * mat[k - 1][j];
                    mat[k][j] = 0;
                    //marcam elementul nou aparut format din contopirea
                    //celorlalte doua elemente cu 1 in matricea ok
                    ok[k - 1][j] = 1;
                    //crestem scorul
                    *punctaj = *punctaj + mat[k - 1][j];
                }
            }
            
        }
    }   
}
void key_left(int mat[4][4], int *punctaj)
{
    //functie pentru mutarea in stanga
    int i, j, k;
    //ok[i][j] = 1 => elementul de pe pozitia i si j a aparut
    // prin contopirea a doua valori de ex: 4 = 2 + 2
    //ok[i][j] = 0 => elementul nu a aparut prin contopire
    //si se afla deja pe tabla dinainte mutarii
    int ok[4][4];
    //initializam cu 0 matricea ok
    init_0(ok);
    for(j = 0; j < 4; j++)
    {
        for(i = 0; i < 4; i++)
        {
            //verificam daca gasim un element valid pe care sa-l mutam
            if(mat[i][j] != 0)
            {
                //cautam o pozitie libera unde sa mutam elementul
                for(k = j - 1; k >= 0; k--)
                {
                    if(mat[i][k] != 0)
                        break;
                }
                k ++;
                //daca am gasit o pozitie libera, mutam elementul
                if(k != j)
                {
                    mat[i][k] = mat[i][j];
                    mat[i][j] = 0;
                }
                //verificam daca vecinul elementului pe care l-am mutat,
                //se poate contopi cu elementul mutat
                if(k > 0 && ok[i][k - 1] == 0 && ok[i][k] == 0 && mat[i][k] == mat[i][k - 1])
                {
                    //marcam elementul nou aparut format din contopirea
                    //celorlalte doua elemente cu 1 in matricea ok
                    ok[i][k - 1] = 1;
                    mat[i][k - 1] = 2 * mat[i][k - 1];
                    mat[i][k] = 0;
                    //crestem scorul
                    *punctaj = *punctaj + mat[i][k - 1];
                }
            }
        }
    }
}
void key_right(int mat[4][4], int *punctaj)
{
    //functie pentru mutarea la dreapta
    int i, j, k;
    //ok[i][j] = 1 => elementul de pe pozitia i si j a aparut
    // prin contopirea a doua valori de ex: 4 = 2 + 2
    //ok[i][j] = 0 => elementul nu a aparut prin contopire
    //si se afla deja pe tabla dinainte mutarii
    int ok[4][4];
    //initializam cu 0 matricea ok
    init_0(ok);
    for(j = 3; j >= 0; j--)
    {
        for(i = 0; i < 4; i++)
        {
            //verificam daca gasim un element valid de mutat
            if(mat[i][j] != 0)
            {
                //cautam pozitie libera pentru element
                for(k = j + 1; k < 4; k++)
                {
                    if(mat[i][k] != 0)
                        break;
                }
                k --;
                //daca am gasit pozitie libera, mutam elementul
                if(k != j)
                {
                    mat[i][k] = mat[i][j];
                    mat[i][j] = 0;
                }
                //verificam daca vecinul elementului pe care l-am mutat,
                //se poate contopi cu elementul mutat
                if(k < 3 && ok[i][k + 1] == 0 && ok[i][k] == 0 && mat[i][k] == mat[i][k + 1])
                {
                    //marcam elementul nou aparut format din contopirea
                    //celorlalte doua elemente cu 1 in matricea ok
                    ok[i][k + 1] = 1;
                    mat[i][k + 1] = 2 * mat[i][k + 1];
                    mat[i][k] = 0;
                    //crestem scorul
                    *punctaj = *punctaj + mat[i][k + 1];
                }
            }
        }
    } 
}
void copiere_mat(int mat[4][4], int copie[4][4])
{
    //functie pentru a face o copie a matricei mat
    int i, j;
    for(i = 0; i < 4; i ++)
    {
        for(j = 0; j < 4; j ++)
            copie[i][j] = mat[i][j];
    }
}
void num_ocupate(int mat[4][4], int *ocupate)
{
    //functie pentru numarare casute ocupate
    int i, j;
    *ocupate = 0;
    for(i = 0; i < 4; i ++)
    {
        for(j = 0; j < 4; j ++)
        {
            if(mat[i][j] != 0)
                *ocupate = *ocupate + 1;
        }
    }
}
void mutare_auto(int mat[4][4], int *punctaj)
{
    //functie pentru mutarea automata dupa o anumita perioada
    //de timp
    int copie[4][4], best[4][4];
    int ocupate, minn, copie_punctaj, best_punctaj;
    minn = 20;
    //mutare in jos
    copie_punctaj = *punctaj; //facem copie la punctaj
    copiere_mat(mat, copie); //facem copie la matrice
    //simulam mutarea in jos a matricei
    key_down(copie, &copie_punctaj); 
    //numaram cate casute avem libere
    num_ocupate(copie, &ocupate);
    if(minn > ocupate) //vedem daca am gasit minimul de casute ocupate
    {
        minn = ocupate;
        copiere_mat(copie, best);
        best_punctaj = copie_punctaj;
    }

    //mutare in sus
    //facem copii la punctaj si matrice
    copie_punctaj = *punctaj;
    copiere_mat(mat, copie);
    //simulam mutarea in sus a matricei
    key_up(copie, &copie_punctaj);
    //numar cate sunt ocupate
    num_ocupate(copie, &ocupate);
    if(minn > ocupate) //vedem daca am gasit minimul de casute ocupate
    {
        minn = ocupate;
        copiere_mat(copie, best);
        best_punctaj = copie_punctaj;
    }

    //mutare in dreapta
    //facem copii la punctaj si mat
    copie_punctaj = *punctaj;
    copiere_mat(mat, copie);
    //simulam mutarea in dreapta a matricei
    key_right(copie, &copie_punctaj);
    //numar cate sunt ocupate
    num_ocupate(copie, &ocupate);
    if(minn > ocupate) //vedem daca am gasit minimul de casute ocupate
    {
        minn = ocupate;
        copiere_mat(copie, best);
        best_punctaj = copie_punctaj;
    }

    //mutare in stanga
    //facem copii la punctaj si mat
    copie_punctaj = *punctaj;
    copiere_mat(mat, copie);
    //simulam mutarea in stanga a matricei
    key_left(copie, &copie_punctaj);
    //numar cate sunt ocupate
    num_ocupate(copie, &ocupate);
    if(minn > ocupate) //vedem daca am gasit minimul de casute ocupate
    {
        minn = ocupate;
        copiere_mat(copie, best);
        best_punctaj = copie_punctaj;
    }
    // best - matricea cu cele mai putine casute ocupate
    // best_punctaj - punctajul matricei best
    // setarea matricei cu tabela de joc cu mutarea cea mai eficienta
    *punctaj = best_punctaj;
    copiere_mat(best, mat);
    
}
int matr_egal(int mat1[4][4], int mat2[4][4])
{
    //verificam daca doua matrici sunt egale
    int i, j;
    for(i = 0; i < 4; i ++)
    {
        for(j = 0; j < 4; j ++)
        {
            if(mat1[i][j] != mat2[i][j])
                return 0;
        }
    }
    return 1;
}
int verif_final(int mat[4][4])
{
    //verificam daca jocul s-a pierdut
    //adica verificam daca nu se mai poate face nicio mutare
    //nici in stanga nici in dreapta nici sus nici jos
    //pentru fiecare mutare fac o copie a matricei si simulez mutarea
    //pe copie
    //daca copia e diferita de matricea initiala, inseamna ca exista o mutare
    //valida si jocul nu s-a terminat ( return 0)
    int punctaj, copie[4][4];
    punctaj = 0;
    //stanga
    copiere_mat(mat, copie);
    key_left(copie, &punctaj);
    if(matr_egal(copie, mat) == 0)
        return 0;
    
    //dreapta
    copiere_mat(mat, copie);
    key_right(copie, &punctaj);
    if(matr_egal(copie, mat) == 0)
        return 0;

    //jos
    copiere_mat(mat, copie);
    key_down(copie, &punctaj);
    if(matr_egal(copie, mat) == 0)
        return 0;
    
    //sus
    copiere_mat(mat, copie);
    key_up(copie, &punctaj);
    if(matr_egal(copie, mat) == 0)
        return 0;
    
    //returnez 1 daca nu se mai poate face nici o mutare
    //jocul s-a terminat
    return 1;
}
int verif_2048(int mat[4][4])
{
    //verificam daca s-a castigat jocul
    //daca am gasit numarul 2048
    int i, j;
    for(i = 0; i < 4; i ++)
    {
        for(j = 0; j < 4; j ++)
        {
            if(mat[i][j] == 2048)
                return 1;
        }
    }
    return 0;
}
void joc_nou(int y0, int x0, int mat[4][4], int resume, int *punctaj)
{
    //functia in care se joaca 2048
    // mai jos am definit textele pentru Game Over si You Won
    const char *text;
    const char *text_won;
    WINDOW * joc;
    WINDOW * panou;
    WINDOW *game_over;
    int ch, won, ocupate;
    time_t timp_init, timp_curent, timp_refresh;
    text =
    "    ######      ###    ##     ## ########    \n\
    ##    ##    ## ##   ###   ### ##          \n\
    ##         ##   ##  #### #### ##          \n\
    ##   #### ##     ## ## ### ## ######      \n\
    ##    ##  ######### ##     ## ##          \n\
    ##    ##  ##     ## ##     ## ##          \n\
     ######   ##     ## ##     ## ########    \n\
                                              \n\
                                              \n\
     #######  ##     ## ######## ########     \n\
    ##     ## ##     ## ##       ##     ##    \n\
    ##     ## ##     ## ##       ##     ##    \n\
    ##     ## ##     ## ######   ########     \n\
    ##     ##  ##   ##  ##       ##   ##      \n\
    ##     ##   ## ##   ##       ##    ##     \n\
     #######     ###    ######## ##     ##";
     text_won=
    "##    ##  #######  ##     ##\n\
      ##  ##  ##     ## ##     ##\n\
       ####   ##     ## ##     ##\n\
        ##    ##     ## ##     ##\n\
        ##    ##     ## ##     ##\n\
        ##    ##     ## ##     ##\n\
        ##     #######   ####### \n\
                             \n\
                             \n\
    ##      ##  #######  ##    ##\n\
    ##  ##  ## ##     ## ###   ##\n\
    ##  ##  ## ##     ## ####  ##\n\
    ##  ##  ## ##     ## ## ## ##\n\
    ##  ##  ## ##     ## ##  ####\n\
    ##  ##  ## ##     ## ##   ###\n\
     ###  ###   #######  ##    ##";
    timp_init = time(NULL); //setam timpul cand a inceput jocul
    //setam momentul cand a inceput jocul pt timpul de refresh
    timp_refresh = time(NULL);
    //daca e joc nou(resume = 0) scorul se face 0
    if(resume == 0) *punctaj = 0;
    joc = newwin(21, 45, y0, x0); //creez fereastra pentru tabele de joc
    // creez fereastra pentru panaoul de comanda
    panou = newwin(21, 35, y0, x0 + 65);
    refresh();
    //daca e joc nou, punem doua valori random in matrice
    if(resume == 0)
    {   
        randomm(mat);
        randomm(mat);
    }
    //creem tabloul jocului
    tablou_joc(joc, panou, mat, *punctaj);
    while(TRUE)
    {
        timp_curent = time(NULL); //timpul curent
        //daca a trecut o secunda dam refresh
        if(timp_curent - timp_refresh >= 1)
        {
            tablou_joc(joc, panou, mat, *punctaj);
            timp_refresh = time(NULL); //resetam timpul de refresh
        }
        //daca userul nu a raspuns timp de 30sec se realizeaza mutarea automata
        if(timp_curent - timp_init >= 30 ) 
        {
            mutare_auto(mat, punctaj);
            wclear(joc);
            wrefresh(joc);
            randomm(mat);
            tablou_joc(joc, panou, mat, *punctaj);
            //dupa ce s-a realizat mutarea automata timpul initial se reseteaza
            //la momentul mutarii automate
            timp_init = time(NULL); 
        }
        //verific daca s-a pierdut jocul
        if(verif_final(mat) == 1)
        {
            // se marcheaza cu 0 daca s-a pierdut jocul
            won = 0; 
            // s - a terminat jocul
            break;
        } 
        //verific daca s-a castigat jocul
        if(verif_2048(mat))
        {
            // se marcheaza cu 1 daca s-a castigat jocul
            won = 1;
            // s - a terminat jocul
            break;
        }
        ch = getch();
        if(ch != ERR)//daca userul a raspuns
        {
            ocupate = 0;
            //setam timpul initial la momentul cand s-a efectuat
            //ultima mutare
            timp_init = time(NULL); 
            if(ch == KEY_DOWN)//realizam mutare in jos
            {
                key_down(mat, punctaj);
                wclear(joc);
                wrefresh(joc);
                num_ocupate(mat, &ocupate); //numaram cate ocupate sunt
                //daca avem 16 elemnente dupa o mutare nu mai 
                //adaugam valoarea random
                if(ocupate != 16) randomm(mat);
                //afisez noua matrice updatata 
                tablou_joc(joc, panou, mat, *punctaj);
            }
            if(ch == KEY_UP)//mutare in sus
            {
                key_up(mat, punctaj);
                wclear(joc);
                wrefresh(joc);
                num_ocupate(mat, &ocupate); //numaram cate ocupate sunt
                //daca avem 16 elemnente dupa 
                //o mutare nu mai adaugam valoarea random
                if(ocupate != 16) randomm(mat);
                //afisez noua matrice updatata  
                tablou_joc(joc, panou, mat, *punctaj);
            }
            if(ch == KEY_LEFT)//mutare in stanga
            {
                key_left(mat, punctaj);
                wclear(joc);
                wrefresh(joc);
                num_ocupate(mat, &ocupate); //numaram cate ocupate sunt
                //daca avem 16 elemnente dupa 
                //o mutare nu mai adaugam valoarea random
                if(ocupate != 16) randomm(mat); 
                //afisez noua matrice updatata  
                tablou_joc(joc, panou, mat, *punctaj);
            }
            if(ch == KEY_RIGHT)//mutare in dreapta
            {
                key_right(mat, punctaj);
                wclear(joc);
                wrefresh(joc);
                num_ocupate(mat, &ocupate); //numaram cate ocupate sunt
                //daca avem 16 elemnente dupa 
                //o mutare nu mai adaugam valoarea random
                if(ocupate != 16) randomm(mat); 
                //afisez noua matrice updatata 
                tablou_joc(joc, panou, mat, *punctaj);
            }
            //daca se apasa q se iese din joc
            if(ch == 'q') 
            {
                //stergem ecranul jocului si revenim la
                //meniul principal
                wclear(joc);
                wclear(panou);
                wrefresh(joc);
                wrefresh(panou);
                delwin(joc);
                delwin(panou);
                refresh();
                meniu_principal(y0, x0, mat, 1, punctaj);
            }
        }
        
    }
    //ajungem aici doar daca jocul este castigat sau
    //pierdut
    //stergem interfata jocului si afisam ecranul de win sau game over
    refresh();
    wclear(joc);
    wclear(panou);
    wrefresh(joc);
    wrefresh(panou);
    delwin(joc);
    delwin(panou);
    refresh();
    refresh();
    game_over = newwin(50, 150, y0, x0);
    refresh();
    if(won == 0)
    {
        mvwaddstr(game_over, 1, 1, text); //adaug textul de gameover
    }
    else
    {
        //adaug textul ca userul a castigat
        mvwaddstr(game_over, 1, 1, text_won);
    }
    mvwaddstr(game_over, 18, 1, 
    "Apasa orice tasta ca jocul sa se incheie...");
    wrefresh(game_over);
    int carac;
    //loop ca sa asteptam input de la jucator
    //"Apasa orice tasta ca jocul sa se incheie"
    while(true)
    {
        carac = getch();
        if(carac != ERR)
            break;
    }
}
void meniu_principal(int y0, int x0, int mat[4][4], int resume, int *punctaj)
{
    int ch, option, ys, xs;
    char optiuni[4][20];
    WINDOW * win;
    //punem in optiuni optiunile din
    //meniul principal
    strcpy(optiuni[0], "NEW GAME");
    strcpy(optiuni[1], "RESUME");
    strcpy(optiuni[2], "QUIT");
    ys = 5;
    xs = 10;
    option = 1;
    win = newwin(15, 50, y0, x0);
    refresh();
    box(win, 0, 0); //il face cutie
    wprintw(win, "MENU"); //ii pune titlu
    wrefresh(win);
    //creaza meniul
    creare_meniu(ys, xs, optiuni, win);
    //coloreaza prima optiune
    colorare(ys, xs, optiuni, win, 1);
    wmove(win, ys, xs); // setam cursorul la pozitia initiala
    wrefresh(win);
    nodelay(stdscr, TRUE); //ca sa nu fie delay
    while(TRUE)
    {
        if ((ch = getch()) == ERR) // user nu a rasp
        {

        }
        else //user a primit rasp
        {
            if(ch == KEY_DOWN && option <= 2)
            {
                //punem cursorul pe urmatoarea optiunea
                option ++;
                creare_meniu(ys, xs, optiuni, win);
                colorare(ys, xs, optiuni, win, option);
            }
            if(ch == KEY_UP && option >= 2)
            {
                //punem cursorul pe optiunea anterioara
                option --;
                creare_meniu(ys, xs, optiuni, win);
                colorare(ys, xs, optiuni, win, option);
            }
            if(ch == 10)
            {
                //daca s-a apasat enter verificam ce optiune
                //avem setata
                if(option == 1) //joc nou
                {
                    //initializam cu 0 matricea ca sa cream un joc nou
                    //si eliberam ecranul
                    init_0(mat); 
                    wclear(win);
                    wrefresh(win);
                    delwin(win);
                    refresh();
                    //creem un joc nou
                    joc_nou(y0, x0, mat, 0, punctaj);   
                    break;
                    
                }
                if(option == 2) //resume
                {
                    //resume este 1 doar daca s-a inceput un joc deja
                    //si userul a lasat jocul neterminat
                    if(resume == 1)
                    {
                        wclear(win);
                        wrefresh(win);
                        delwin(win);
                        refresh();
                        //afisam jocul deja inceput
                        joc_nou(y0, x0, mat, 1, punctaj);
                        break;
                    }
                }
                if(option == 3) //quit
                    break;
            }
        }
    }
    endwin(); //se opreste interfata grafica
}
int main()
{
    int y0, x0, mat[4][4], punctaj;
    //mat - este matricea in care retinem valorile
    //de pe tabla de joc
    y0 = 3;
    x0 = 40;
    start_color(); //color suport
    initscr(); //seteaza mediul pentru ncurses
    //cbreak nu trebuie sa asteptam enter ca un input
    //sa fie procesat
    cbreak(); 
    noecho(); //dezactiveaza echo-ul caracterelor
    keypad(stdscr, TRUE); //activeaza keypadul userului
    //apare meniul principal
    meniu_principal(y0, x0, mat, 0, &punctaj);
    return 0;
}