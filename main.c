#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MLV/MLV_all.h>
#define TAILLE_PLATEAU 7
#define TAILLE_MAX_NOM 20
#define TAILLE_MARGE 20
#define TAILLE_CASE 60
#define CHAMP_INFORMATION 150

typedef struct
{
    char nom[TAILLE_MAX_NOM];
    char symbole;
    int score;
} Joueur;

typedef struct
{
    char plateau[TAILLE_PLATEAU + 2][TAILLE_PLATEAU + 2];
    Joueur *joueurs[2];
} Plateau;

typedef struct
{
    int x, y;
} Coord;

/*fonctions générales*/


/*
    BUT: Initialisation du plateau avec les valeurs demandées
    PARAMETRES: adresse: adresse du plateau du jeu
*/
void initialiserPLateau(Plateau *p)
{
    int i, j;
    for (i = 0; i < TAILLE_PLATEAU + 2; ++i)
    {
        for (j = 0; j < TAILLE_PLATEAU + 2; ++j)
        {
            if (i == 0 || j == 0 || j == TAILLE_PLATEAU + 1 || i == TAILLE_PLATEAU + 1)
                p->plateau[i][j] = '*';
            else if ((i == 1 && j == 1) || (i == TAILLE_PLATEAU && j == TAILLE_PLATEAU))
                p->plateau[i][j] = p->joueurs[0]->symbole;
            else if ((i == 1 && j == TAILLE_PLATEAU) || (i == TAILLE_PLATEAU && j == 1))
                p->plateau[i][j] = p->joueurs[1]->symbole;
            else
                p->plateau[i][j] = '.';
        }
    }
}
/*
    BUT: changement des tours entre les deux participants
         en faisant un échange de valeurs à chaque tour
    PARAMETRES: adresse: adresse du plateau du jeu
*/
void echangeTour(Plateau *p)
{
    Joueur tmp = *(p->joueurs[0]);
    *(p->joueurs[0]) = *(p->joueurs[1]);
    *(p->joueurs[1]) = tmp;
}

/*
    BUT: placement du coup du joueur dans le tableau 2D avec
         le symbole approprié
    PARAMETRES: adresse: adresse du plateau du jeu
                adresse: adresse de la ligne et la colonne saisie
*/
void placerCoupJoueur(Plateau *p, int lig, int col)
{
    p->plateau[lig][col] = p->joueurs[0]->symbole;
    p->joueurs[0]->score++;
}

/*
    BUT: vérification des coordonnées saisies par
         l'utilisateur s'ils dépassent pas les limites du plateau
    PARAMETRES: valeur: la ligne et la colonne saisie par l'utilisateur
    RETOUR: renvoie 1 si les coordonnées sont bien valides 
            0 sinon
*/
int coordonneValide(int lig, int col)
{
    if ((lig <= TAILLE_PLATEAU && lig > 0) && (col <= TAILLE_PLATEAU && col > 0))
        return 1;
    return 0;
}



/*
    BUT: vérification si les coordonnées de l'utilisateur n'occupent 
         aucune case (la case est vide) dans le plateau et qu'ils sont valides
    PARAMETRES: valeur: la ligne et la colonne saisie par l'utilisateur
                adresse: adresse du plateau du jeu
    RETOUR: renvoie 1 si la case est bien vide et valide 
            0 sinon
*/
int emplacementVide(Plateau *p, int lig, int col)
{
    if (p->plateau[lig][col] == '.' && coordonneValide(lig, col) == 1)
        return 1;
    return 0;
}


/*
    BUT: détection des voisins d'une case saisie par l'utilisateur
    PARAMETRES: valeur: la ligne et la colonne saisie par l'utilisateur
                adresse: l'adresse des nombres des voisins
                         l'adresse de la structure voisins qui s'occupent des couples
                         de coordonnées voisins.
*/
void detecterVoisins(Coord *voisins, int *nbVoisins, int lig, int col)
{
    int i, j;
    int indice = 0;
    for (i = lig - 1; i <= lig + 1; ++i)
    {
        for (j = col - 1; j <= col + 1; ++j)
        {
            if (coordonneValide(i, j) == 1 && (i != lig || j != col))
            {
                voisins[indice] = (Coord){i, j};
                indice++;
                (*nbVoisins)++;
            }
        }
    }
}



/*
    BUT: changement de la couleur des pions du joueur adverse si la case saisie
        par le joueur courant est bien placée et se trouve à côté
    PARAMETRES: valeur: la ligne et la colonne saisie par l'utilisateur
                        le nombre de voisins de la case saisie
                adresse: l'adresse du plateau du jeu
                         l'adresse de la structure voisins qui s'occupent des couples
                         de coordonnées voisins.
*/
void changementPions(Plateau *p, int nbVoisins, Coord *voisins)
{
    int i;
    for (i = 0; i < nbVoisins; ++i)
    {
        if (p->joueurs[1]->symbole == p->plateau[voisins[i].x][voisins[i].y])
        {
            placerCoupJoueur(p, voisins[i].x, voisins[i].y);
            p->joueurs[1]->score--;
        }
    }
}



/*
    BUT: vérification si le coup du joueur est valide en vérificant si l'emplacement
         est vide et que les coordonnées sont bien valides après on effectue 
         les changements des pions du joueur adverse.
    PARAMETRES: valeur: la ligne et la colonne saisie par l'utilisateur
                adresse: l'adresse du plateau du jeu
    RETOUR: 1 si le coup est valide et on a procédé au changement 
            0 sinon
*/
int coupValide(Plateau *p, int lig, int col)
{
    int nbVoisins = 0;
    int i;
    Coord voisins[8];
    if (coordonneValide(lig, col) == 1)
    {
        detecterVoisins(voisins, &nbVoisins, lig, col);
    }
    else
        return 0;
    if (emplacementVide(p, lig, col) == 1)
    {
        for (i = 0; i < nbVoisins; ++i)
        {
            if (p->joueurs[1]->symbole == p->plateau[voisins[i].x][voisins[i].y])
            {
                changementPions(p, nbVoisins, voisins);
                return 1;
            }
        }
        return 0;
    }
    else
        return 0;
}



/*
    BUT: vérification si le tableau est bien remplit en comparant
         le nombre de pions aux nombres de case existantes.
    PARAMETRES: adresse: l'adresse du plateau du jeu
    RETOUR: 1 si le tableau est bien remplit
            0 sinon
*/
int plateauRemplit(Plateau *p)
{
    if ((p->joueurs[0]->score + p->joueurs[1]->score) == TAILLE_PLATEAU * TAILLE_PLATEAU)
        return 1;
    return 0;
}


/*
    BUT: Affectation des valeurs ligne et colonne à des valeurs
         aléatoires générées par la fonction MLV_get_random_integer
    PARAMETRES: adresse: l'adresse du plateau du jeu et la ligne et la colonne
*/
void choixEmplacementPc(Plateau *p, int *lig, int *col) {
    *lig = MLV_get_random_integer(1, TAILLE_PLATEAU + 1);
    *col = MLV_get_random_integer(1, TAILLE_PLATEAU + 1);
}

/*
    BUT: vérification si y'a plus qu'un type de pion dans le tableau
    PARAMETRES: adresse: l'adresse du plateau du jeu
    RETOUR: 1 si y a qu'un seul type de pions dans le plateau
            0 sinon
*/
int resteUnTypePions(Plateau *p)
{
    if (p->joueurs[0]->score == 0 || p->joueurs[1]->score == 0)
        return 1;
    return 0;
}


/*
    BUT: vérification si le jeu est bien fini en faisant appel aux fonction
         responsables sur la détection de la victoire
    PARAMETRES: adresse: l'adresse du plateau du jeu
    RETOUR: 1 si le tableau est bien remplit ou y a plus qu'un seul type de pions 
            0 sinon
*/
int finduJeu(Plateau *p)
{
    if (plateauRemplit(p) == 1 || resteUnTypePions(p))
        return 1;
    return 0;
}

/*
    BUT: Affichage des modes d'utilisation du programme.
*/
void usage(){
    printf("./nomExecutable [-a ou -g] [-h ou -o]\n");
    printf("-a: mode ASCII\n-g: mode GRAPHIQUE\n-h: en BINOME\n-o: avec PC\n");
}

/*-----------------Fin Fonction générales-----------------------*/

/* fonctions affichage ascii*/


/*
    BUT: affichage du plateau dans le terminal
    PARAMETRES: adresse: adresse du plateau du jeu
*/
void affichePlateauAscii(Plateau *p)
{
    int i, j;
    for (i = 0; i < TAILLE_PLATEAU + 2; ++i)
    {
        for (j = 0; j < TAILLE_PLATEAU + 2; ++j)
        {
            printf("%c ", p->plateau[i][j]);
        }
        printf("\n");
    }
}
/*
    BUT: Initialisation des joueurs humains en affectant
         à chacun un nom et un symbole et un score de départ
    PARAMETRES: adresse: adresse du plateau du jeu
                         adresse du joueur 1
                         adresse du joueur 2
*/
void initJoueurHumain(Plateau *p, Joueur *j1, Joueur *j2)
{
    printf("Quel est le nom du premier joueur (symbol o) :");
    scanf("%s", j1->nom);
    j1->score = 2;
    j1->symbole = 'o';
    p->joueurs[0] = j1;
    printf("Quel est le nom du second  joueur (symbol x) :");
    scanf("%s", j2->nom);
    j2->score = 2;
    j2->symbole = 'x';
    p->joueurs[1] = j2;
}

/*
    BUT: Initialisation d'un seul joueur humain en lui 
         affectant un nom et un symbole et un score de départ
         et joueur 2 qui sera l'ordinateur
    PARAMETRES: adresse: adresse du plateau du jeu
                         adresse du joueur 1
                         adresse du joueur 2
*/
void initJoueurHumainPc(Plateau *p, Joueur *j1, Joueur *j2) {
    printf("Quel est le nom du joueur (symbol o) :");
    scanf("%s", j1->nom);
    sprintf(j2->nom, "Pc");
    j1->score = 2;
    j1->symbole = 'o';
    p->joueurs[0] = j1;
    j2->score = 2;
    j2->symbole = 'x';
    p->joueurs[1] = j2;
}
/*
    BUT: affichage du score dans le terminal
    PARAMETRES: adresse: adresse du plateau du jeu
*/
void afficheScore(Plateau *p)
{
    if (p->joueurs[0]->symbole == 'o')
        printf("Score actuel : %s(o) %d − %s(x) %d\n", p->joueurs[0]->nom, p->joueurs[0]->score, p->joueurs[1]->nom, p->joueurs[1]->score);
    else if (p->joueurs[1]->symbole == 'o')
        printf("Score actuel : %s(o) %d − %s(x) %d\n", p->joueurs[1]->nom, p->joueurs[1]->score, p->joueurs[0]->nom, p->joueurs[0]->score);
}

/*
    BUT: affichage du gagnant dans le terminal
    PARAMETRES: adresse: adresse du plateau du jeu
*/
void afficheGagnant(Plateau *p)
{
    if (p->joueurs[0]->score > p->joueurs[1]->score)
        printf("Bravo %s, vous avez gagné %d à %d.\n", p->joueurs[0]->nom, p->joueurs[0]->score, p->joueurs[1]->score);
    else if (p->joueurs[1]->score > p->joueurs[0]->score)
        printf("Bravo %s, vous avez gagné %d à %d.\n", p->joueurs[1]->nom, p->joueurs[1]->score, p->joueurs[0]->score);
    else
        printf("Egalité %d à %d.\n", p->joueurs[0]->score, p->joueurs[1]->score);
}

/*
    BUT: effectuer la saisie de l'emplacement du joueur dans le terminal
         en prenant les coordonnées de ligne et colonne
    PARAMETRES: adresse: adresse du plateau du jeu
                         adresse de la ligne et la colonne
*/
void choixEmplacement(Plateau *p, int *lig, int *col) {
    printf("%s (%c), veuillez saisir les coordonnées où jouer (entre 1 et %d) :", p->joueurs[0]->nom, p->joueurs[0]->symbole, TAILLE_PLATEAU);
    scanf("%d %d", lig, col);
}


void jeuAsciiBinome(void) {
    Plateau p;
    Joueur j1, j2;
    int finJeu = 0, lig, col;
    initJoueurHumain(&p, &j1, &j2);
    initialiserPLateau(&p);
    affichePlateauAscii(&p);
    while(!finJeu) {
        do {
            choixEmplacement(&p, &lig, &col);
        } while (coupValide(&p, lig, col) == 0);
        placerCoupJoueur(&p, lig, col);
        affichePlateauAscii(&p);
        afficheScore(&p);
        if ((finJeu = finduJeu(&p))) afficheGagnant(&p);
        echangeTour(&p);
    }
}


void jeuAsciiPc(void) {
    Plateau p;
    Joueur j1, j2;
    int finJeu = 0, lig, col;
    initJoueurHumainPc(&p, &j1, &j2);
    initialiserPLateau(&p);
    affichePlateauAscii(&p);
    while(!finJeu) {
        do {
            if (p.joueurs[0]->symbole == 'x') choixEmplacementPc(&p, &lig, &col);
            else if (p.joueurs[0]->symbole == 'o') choixEmplacement(&p, &lig, &col);
        } while (coupValide(&p, lig, col) == 0);
        placerCoupJoueur(&p, lig, col);
        affichePlateauAscii(&p);
        afficheScore(&p);
        if ((finJeu = finduJeu(&p))) afficheGagnant(&p);
        echangeTour(&p);
    }
}

/*-----------------Fin fonctions ascii------------------*/

/*fonctions graphique*/



/*
    BUT: dessine les barres horizontales du plateau
    PARAMETRES: adresse: l'adresse de la largeur et la hauteur du plateau
*/
void dessineBarreHorizontale(int *lar, int *haut)
{
    int j;
    for (j = TAILLE_MARGE; j <= TAILLE_MARGE + TAILLE_CASE * TAILLE_PLATEAU; j += TAILLE_CASE)
    {
        MLV_draw_line(TAILLE_MARGE, j, TAILLE_MARGE + TAILLE_CASE * TAILLE_PLATEAU, j, MLV_COLOR_BLACK);
    }
}



/*
    BUT: dessine les barres verticales du plateau
    PARAMETRES: adresse: l'adresse de la largeur et la hauteur du plateau
*/
void dessineBarreVerticale(int *lar, int *haut)
{
    int i;
    for (i = TAILLE_MARGE; i <= TAILLE_MARGE + TAILLE_CASE * TAILLE_PLATEAU; i += TAILLE_CASE)
    {
        MLV_draw_line(i, TAILLE_MARGE, i, TAILLE_MARGE + TAILLE_CASE * TAILLE_PLATEAU, MLV_COLOR_BLACK);
    }
}

void fondPlateauJeu(int *lar, int *haut)
{
    MLV_draw_filled_rectangle(0, 0, *lar, *haut, MLV_COLOR_SKY_BLUE);
}

/*
    BUT: dessine le champ d'information du plateau contenant les informations
         sur le joueur courant le score et le symbole
    PARAMETRES: adresse: l'adresse de la largeur et la hauteur du plateau et du plateau
*/
void dessinChampInformation(Plateau *p, int *lar, int *haut) {
    //Joueur courant
    char *sco0 = (char *)malloc(sizeof(char) * 40);
    sprintf(sco0, "Votre score est: %d\n",p->joueurs[0]->score);
    MLV_draw_filled_rectangle(*lar - CHAMP_INFORMATION, 0, *lar, *haut, MLV_COLOR_DARK_GRAY);
    MLV_draw_text(*lar - CHAMP_INFORMATION + 20, *haut/20, "Ataxx", MLV_COLOR_RED);
    MLV_draw_text(*lar - CHAMP_INFORMATION + 20, *haut/9, "Joueur Courant:", MLV_COLOR_BLACK);
    MLV_draw_text(*lar - CHAMP_INFORMATION + 20, *haut/9+20, p->joueurs[0]->nom, MLV_COLOR_WHITE);
    MLV_draw_text(*lar - CHAMP_INFORMATION + 20, *haut/9+40, sco0, MLV_COLOR_BLACK);
    if (p->joueurs[0]->symbole == 'o') {
        MLV_draw_filled_circle(*lar - CHAMP_INFORMATION + 20 + 70, *haut/9+20+5, 5, MLV_COLOR_RED);
        MLV_draw_circle(*lar - CHAMP_INFORMATION + 20 + 70, *haut/9+20+5, 5, MLV_COLOR_WHITE);
    } else if (p->joueurs[0]->symbole == 'x') {
        MLV_draw_filled_circle(*lar - CHAMP_INFORMATION + 20 + 70, *haut/9+20+5, 5, MLV_COLOR_DARK_VIOLET);
        MLV_draw_circle(*lar - CHAMP_INFORMATION + 20 + 70, *haut/9+20+5, 5, MLV_COLOR_WHITE);
    }

    //Joueur adverse
    char *sco1 = (char *)malloc(sizeof(char) * 40);
    sprintf(sco1, "Votre score est: %d\n",p->joueurs[1]->score);
    MLV_draw_text(*lar - CHAMP_INFORMATION + 20, *haut/9+100, "Joueur Adverse:", MLV_COLOR_BLACK);
    MLV_draw_text(*lar - CHAMP_INFORMATION + 20, *haut/9+120, p->joueurs[1]->nom, MLV_COLOR_WHITE);
    MLV_draw_text(*lar - CHAMP_INFORMATION + 20, *haut/9+140, sco1, MLV_COLOR_BLACK);
}

/*
    BUT: Transfère les coordonnées du clic de l'utilisateur en indice valide
    PARAMETRES: valeur INT les coordonnées de l'utilisateur
    RETOUR: l'indice du clic de l'utilisateur
*/
int coordVersIndice(int coor)
{
    return (coor - TAILLE_MARGE) / TAILLE_CASE + 1;
}


/*
    BUT: Transfère l'indice du plateau vers des coordoonées de la case
         correspandante
    PARAMETRES: valeur INT les coordonnées de la case du plateau
    RETOUR: les coordonnées de l'indice concerné
*/
int indiceVersCoord(int i)
{
    return (TAILLE_MARGE + i * TAILLE_CASE) - (TAILLE_CASE / 2);
}


/*
    BUT: Affichage des pions dans le graphique
    PARAMETRES: adresse: adresse du plateau du jeu
*/
void affichePions(Plateau *p)
{
    int i, j;
    for (i = 1; i <= TAILLE_PLATEAU; ++i)
    {
        for (j = 1; j <= TAILLE_PLATEAU; ++j)
        {
            if (p->plateau[i][j] == 'o')
            {
                MLV_draw_filled_circle(indiceVersCoord(j), indiceVersCoord(i), TAILLE_CASE / 2, MLV_COLOR_RED);
                MLV_draw_circle(indiceVersCoord(j), indiceVersCoord(i), TAILLE_CASE / 2, MLV_COLOR_WHITE);
            }
            else if (p->plateau[i][j] == 'x')
            {
                MLV_draw_filled_circle(indiceVersCoord(j), indiceVersCoord(i), TAILLE_CASE / 2, MLV_COLOR_DARK_VIOLET);
                MLV_draw_circle(indiceVersCoord(j), indiceVersCoord(i), TAILLE_CASE / 2, MLV_COLOR_WHITE);
            }
        }
    }
}


void affichePlateauGraphique(Plateau *p, int *lar, int *haut)
{
    MLV_clear_window(MLV_COLOR_BLACK);
    fondPlateauJeu(lar, haut);
    dessineBarreVerticale(lar, haut);
    dessineBarreHorizontale(lar, haut);
    dessinChampInformation(p, lar, haut);
    affichePions(p);
    MLV_actualise_window();
}

/*
    BUT: Affectation des coordoonées de clic de l'utilisateur
         à des indices valides du plateau
    PARAMETRES: adresse: adresse du plateau du jeu et la ligne et la colonne 
                         concernée.
*/
void choixEmplacementGraphique(Plateau *p, int *lig, int *col)
{
    int x, y;
    MLV_wait_mouse(&x, &y);
    *lig = coordVersIndice(y);
    *col = coordVersIndice(x);
}




void afficheGagnantgraphique(Plateau *p, int *haut){
    char *text = (char *)malloc(sizeof(char) * 80);
    if (p->joueurs[0]->score > p->joueurs[1]->score)
        sprintf(text, "Bravo %s, vous avez gagné %d à %d.", p->joueurs[0]->nom, p->joueurs[0]->score, p->joueurs[1]->score);
    else if (p->joueurs[1]->score > p->joueurs[0]->score)
        sprintf(text, "Bravo %s, vous avez gagné %d à %d.", p->joueurs[1]->nom, p->joueurs[1]->score, p->joueurs[0]->score);
    else
        sprintf(text, "Egalité %d à %d.", p->joueurs[0]->score, p->joueurs[1]->score);
    MLV_draw_adapted_text_box(50, *haut/2, text, 2, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
    MLV_actualise_window();
}


/*
    BUT: Ouverture du plateau sauvegardé de la partie précédente 
    PARAMETRES: adresse: adresse du plateau du jeu
*/
void initPlateauSauvegard(Plateau *p)
{
    int i, j;
    char l;
    FILE *f = fopen("sauvegardePlateau.txt", "r");
    for (i = 0; i < TAILLE_PLATEAU + 2; ++i)
    {
        for (j = 0; j < TAILLE_PLATEAU + 2; ++j)
        {
            if ((l = fgetc(f)) != EOF)
            {
                p->plateau[i][j] = l;
            }
        }
    }
    rewind(f);
    fclose(f);
}



int menuJeu(int *lar, int *haut) {
    int lar_box1, haut_box1, lar_box2, haut_box2, c1, c2;
    const char * n = "Nouvelle Partie";
    const char * s = "Restaure Partie";
    MLV_get_size_of_adapted_text_box(n, 2, &lar_box1, &haut_box1);
    MLV_get_size_of_adapted_text_box(s, 2, &lar_box2, &haut_box2);
    MLV_draw_filled_rectangle(0, 0, *lar, *haut, MLV_COLOR_SKY_BLUE);
    MLV_draw_text(*lar/2-20, *haut/8, "ATAXX", MLV_COLOR_RED);
    MLV_draw_adapted_text_box(*lar/2-50, *haut/4, n, 2, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
    MLV_draw_adapted_text_box(*lar/2-50, *haut/4 + 50, s, 2, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
    MLV_actualise_window();
    MLV_wait_mouse(&c1, &c2);
    if ( (c1 >= *lar/2-50 && c1 <= (*lar/2-50)+lar_box1) && (c2 >= *haut/4 && c2 <= (*haut/4 + haut_box1)) ) return 1;
    else if ((c1 >= *lar/2-50 && c1 <= (*lar/2-50)+lar_box2) && (c2 >= *haut/4 + 50 && c2 <= (*haut/4 + 50 + haut_box2))) return 2;
    return 0;
}

void initJoueurGraphique(Plateau *p, Joueur *j1, Joueur *j2, int *lar, int *haut) {
    char *text1, *text2;
    MLV_actualise_window();
    MLV_draw_text(*lar/2-20, *haut/8, "ATAXX", MLV_COLOR_RED);
    MLV_wait_input_box(*lar/2-50, *haut/4-20, *lar/2+150, *haut/4+20, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, "Nom Joueur 1: ", &text1);
    MLV_actualise_window();
    MLV_wait_input_box(*lar/2-50, *haut/4-20, *lar/2+150, *haut/4+20, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, "Nom Joueur 2: ", &text2);
    MLV_actualise_window();
    sprintf(j1->nom, text1);
    sprintf(j2->nom, text2);
    j1->score =2;
    j2->score = 2;
    j1->symbole = 'o';
    j2->symbole = 'x';
    p->joueurs[0] = j1;
    p->joueurs[1] = j2;
}

void initJoueurGraphiquePc(Plateau *p, Joueur *j1, Joueur *j2, int *lar, int *haut) {
    char *text1;
    MLV_actualise_window();
    MLV_draw_text(*lar/2-20, *haut/8, "ATAXX", MLV_COLOR_RED);
    MLV_wait_input_box(*lar/2-50, *haut/4-20, *lar/2+150, *haut/4+20, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, "Nom Joueur 1: ", &text1);
    MLV_actualise_window();
    sprintf(j1->nom, text1);
    sprintf(j2->nom, "Pc");
    j1->score =2;
    j2->score = 2;
    j1->symbole = 'o';
    j2->symbole = 'x';
    p->joueurs[0] = j1;
    p->joueurs[1] = j2;
}

void sauvegardPlateau(Plateau *p)
{
    int i, j;
    FILE *f = fopen("sauvegardePlateau.txt", "w+");
    for (i = 0; i < TAILLE_PLATEAU + 2; ++i)
    {
        for (j = 0; j < TAILLE_PLATEAU + 2; ++j)
        {
            fputc(p->plateau[i][j], f);
        }
    }
    rewind(f);
    fclose(f);
}

void jeuGraphiqueBinome() {
    Plateau p;
    Joueur j1, j2;
    int finJeu = 0, lig, col, res;
    int largeur = TAILLE_PLATEAU * TAILLE_CASE + 2 * TAILLE_MARGE + CHAMP_INFORMATION; // 100 c'est place pour informations
    int hauteur = TAILLE_PLATEAU * TAILLE_CASE + 2 * TAILLE_MARGE;
    MLV_create_window("Ataxx", "", largeur, hauteur);
    initJoueurGraphique(&p, &j1, &j2, &largeur, &hauteur);
    do
    {
        res = menuJeu(&largeur, &hauteur);
    } while (res == 0);
    switch (res)
    {
    case 1:
        initialiserPLateau(&p);
        break;
    case 2:
        initPlateauSauvegard(&p);
        break;
    }
    while (!finJeu) {
        affichePlateauGraphique(&p, &largeur, &hauteur);
        do
        {
            choixEmplacementGraphique(&p, &lig, &col);
        } while (coupValide(&p, lig, col) == 0);
        placerCoupJoueur(&p, lig, col);
        sauvegardPlateau(&p);
        if ((finJeu = finduJeu(&p)))
        {
            affichePlateauGraphique(&p, &largeur, &hauteur);
            afficheGagnantgraphique(&p, &hauteur);
        }
        echangeTour(&p);
    }
    MLV_wait_seconds(5);
    MLV_free_window();
}

void jeuGraphiquePc() {
    Plateau p;
    Joueur j1, j2;
    int finJeu = 0, lig, col, res;
    int largeur = TAILLE_PLATEAU * TAILLE_CASE + 2 * TAILLE_MARGE + CHAMP_INFORMATION; // 100 c'est place pour informations
    int hauteur = TAILLE_PLATEAU * TAILLE_CASE + 2 * TAILLE_MARGE;
    MLV_create_window("Ataxx", "", largeur, hauteur);
    initJoueurGraphiquePc(&p, &j1, &j2, &largeur, &hauteur);
    do
    {
        res = menuJeu(&largeur, &hauteur);
    } while (res == 0);
    switch (res)
    {
    case 1:
        initialiserPLateau(&p);
        break;
    case 2:
        initPlateauSauvegard(&p);
        break;
    }
    while (!finJeu) {
        affichePlateauGraphique(&p, &largeur, &hauteur);
        do
        {
            if (p.joueurs[0]->symbole == 'x') choixEmplacementPc(&p, &lig, &col);
            else if (p.joueurs[0]->symbole == 'o') choixEmplacementGraphique(&p, &lig, &col);
        } while (coupValide(&p, lig, col) == 0);
        placerCoupJoueur(&p, lig, col);
        sauvegardPlateau(&p);
        if ((finJeu = finduJeu(&p)))
        {
            affichePlateauGraphique(&p, &largeur, &hauteur);
            afficheGagnantgraphique(&p, &hauteur);
        }
        echangeTour(&p);
    }
    MLV_wait_seconds(5);
    MLV_free_window();
}



/*-----------------Fin fonctions graphique------------------*/


int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Commande Introuvable !\n");
        usage();
        exit(1);
    }
    printf("1: %c et 2: %c\n", argv[1][1], argv[2][1]);
    if (argv[1][1] == 'a' || argv[2][1] == 'a') {
        if (argv[1][1] == 'h' || argv[2][1] == 'h') jeuAsciiBinome();
        else if (argv[1][1] == 'o' || argv[2][1] == 'o') jeuAsciiPc();
        else exit(1);
    } else if (argv[1][1] == 'g' || argv[2][1] == 'g') {
        if (argv[1][1] == 'h' || argv[2][1] == 'h') jeuGraphiqueBinome();
        else if (argv[1][1] == 'o' || argv[2][1] == 'o') jeuGraphiquePc();
        else exit(1);
    } else exit(1);
    free(sco0);
    free(sco1);
    free(text1);
    free(text2);
    return 0;
}