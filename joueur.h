/**
 * @file joueur.h
 * @brief classe joueur permettant de faire fonctionner une partie de bataille navale
 * @author Léopaul ESTOUR
 * @date 01/12/2025
 * @version V0.4
 */
#pragma once

#include <vector>
#include <tuple>
#include <iostream>
#include <string>
#include <cstdlib>
#include <conio.h>
using namespace std;

/**
 * @defgroup STATE État des case de la grille
 * @{
 */
/// Case vide
#define EMPTY 1
/// Bateau ennemi touché
#define CHECK 2
/// Pas de bateau ennemi
#define CROSS 3
/// Bateau du joueur
#define SHIP 4
/// Bateau coulé du joueur
#define DROWNED_SHIP 5
/** @defgroup OVERLAP États non utilisés */
/// Si bateau du joueur + pas de bateau ennemi
#define SHIP_CROSS 6
/// Si bateau du joueur + bateau ennemi touché
#define SHIP_CHECK 7
/**
 * @}
*/
/// Position de la coordonnée centrale de placement de bateau
#define COORD_CENTER 8
/// Position de la coordonnée possible de placement de bateau
#define COORD_POSSIBLE 9
/**
 * @}
*/

/// Formation en grille du porte-avion
const vector<vector<unsigned short>> FORM_SHIP_CARRIER {{1,1},{1,1},{0,1}};
/// Nombre de porte-avions
#define FORM_SHIP_CARRIER_NB 1
/// Formation en grille du croiseur
const vector<vector<unsigned short>> FORM_SHIP_BATTLESHIP {{1},{1},{1},{1}};
/// Nombre de croiseurs
#define FORM_SHIP_BATTLESHIP_NB 1
/// Formation en grille du contre-torpilleurs
const vector<vector<unsigned short>> FORM_SHIP_CRUISER {{1},{1},{1}};
/// Nombre de contre-torpilleurs
#define FORM_SHIP_CRUISER_NB 2
/// Formation en grille du torpilleur
const vector<vector<unsigned short>> FORM_SHIP_DESTROYER {{1},{1}};
/// Nombre de torpilleurs
#define FORM_SHIP_DESTROYER_NB 1

/// Nombre maximum d'itérations possible dans une partie
#define ITER_MAX 100
/// Nombre de joueurs avec un minimum de 2 joueurs
#define NB_JOUEURS 2
/// Taille de la grille avec un minimum de 2
#define DIMENSIONS 10

/**
 * @brief Affichage de la grille et de ses différents états
 *
 * Exemple de grille avec les différents états possibles
 * @code
 * ╔═══╦═══╤═══╤═══╗
 * ║   ║ A │ B │ C ║
 * ╠═══╬═══╪═══╪═══╣
 * ║ 1 ║ ✓ │ ☑ │ ■ ║
 * ╟───╫───┼───┼───╢
 * ║ 2 ║ ■ │ ☒ │ ✗ ║
 * ╟───╫───┼───┼───╢
 * ║ 3 ║ ☐ │   │   ║
 * ╚═══╩═══╧═══╧═══╝
 * @endcode
 */

/**
 * @brief Affichage de la grille et des différents positionnement possible de porte-avion à partir du centre choisis
 * 
 * Exemple de grille avec les différentes positions possibles de porte-avion
 * @code
 * ╔═══╦═══╤═══╤═══╤═══╤═══╗
 * ║   ║ A │ B │ C │ D │ E ║
 * ╠═══╬═══╪═══╪═══╪═══╪═══╣
 * ║ 1 ║   │ x │   │ x │   ║
 * ╟───╫───┼───┼───┼───┼───╢
 * ║ 2 ║ x │   │   │   │ x ║
 * ╟───╫───┼───┼───┼───┼───╢
 * ║ 3 ║   │   │ o │   │   ║
 * ╟───╫───┼───┼───┼───┼───╢
 * ║ 4 ║ x │   │   │   │ x ║
 * ╟───╫───┼───┼───┼───┼───╢
 * ║ 5 ║   │ x │   │ x │   ║
 * ╚═══╩═══╧═══╧═══╧═══╧═══╝
 * @endcode
 */

 /**
  * @class Joueur
  * @brief Représente un joueur, sa grille et tout ce qui lui permet de jouer
  */
class Joueur {
    private:
        /// Grille de positionnement des bateaux
        vector<vector<unsigned short>> grille;
        /// Grille des tirs réalisés sur l'ennemi
        vector<vector<unsigned short>> grille_check;
        /// Numéro unique du joueur
        unsigned short num_joueur;
        /// Booléen de si le joueur est un robot ou non
        bool isBot;
        /// Booléen de si le joueur a perdu, utile pour les parties à plus de 2 joueurs
        bool lost;

    public:
        /// Variable static définissant le nombre actuel de joueur créé, utile pour le 'num_joueur'
        static unsigned short nb_joueurs;
        /// Nombre actuel de joueurs ayant perdu
        static unsigned short nb_lost;

        /**
         * @brief Construction d'un joueur
         * 
         * Initialise le joueur avec une grille de taille DIMENSIONS remplie d'EMPTY.
         * Puis lance le positionnement automatique ou non des bateaux
         * 
         * @param _isBot Définit si le joueur est un robot ou un humain
         */
        Joueur(bool _isBot = true) : num_joueur(nb_joueurs++), isBot(_isBot), lost(false) {
            grille = vector<vector<unsigned short>>(DIMENSIONS, vector<unsigned short>(DIMENSIONS, EMPTY));
            grille_check = vector<vector<unsigned short>>(DIMENSIONS, vector<unsigned short>(DIMENSIONS, EMPTY));
            if (_isBot) random_ships();
            else position_ships();
        }
        /**
         * @brief Accède à la ligne via l'opérateur []
         * 
         * @param y ligne souhaitée
         * @return vector<unsigned short>& ligne renvoyée
         */
        vector<unsigned short>& operator[](int y) { return grille[y]; }
        /**
         * @brief Accède à la case grille[y][x]
         * 
         * @param y ligne souhaitée
         * @param x colonne souhaitée
         * @return unsigned& case renvoyée
         */
        unsigned short& operator()(int y, int x) { return grille[y][x]; }
        /**
         * @brief Accède à la case grille[y][x]
         * 
         * @param p position souhaitée
         * @return unsigned& case renvoyée
         */
        unsigned short& operator()(pair<int,int> p) { return grille[p.first][p.second]; }
        /**
         * @brief Affecte la valeur 'a' à toutes les cases
         * 
         * @param a valeur à affecter
         */
        void operator=(unsigned short a) { grille = vector<vector<unsigned short>>(this->size(), vector<unsigned short>(this[0].size(), a)); grille_check = vector<vector<unsigned short>>(this->size(), vector<unsigned short>(this[0].size(), a)); }
        /**
         * @brief Copie la grille du joueur 'a'
         * 
         * @param a joueur à copier
         */
        void operator=(Joueur& a) {
            // if (a.size()!= this->grille.size() || a[0].size()!= this->grille[0].size())
            //     throw length_error("Les grilles ne font pas la même taille");
            grille = vector<vector<unsigned short>>(a.size(), vector<unsigned short>(a[0].size(), EMPTY));
            grille_check = vector<vector<unsigned short>>(a.size(), vector<unsigned short>(a[0].size(), EMPTY));
            for (int i=0; i< static_cast<int>(grille.size());i++)
                for (int j=0; j< static_cast<int>(grille[0].size()); j++) {
                    this->grille[i][j] = a(i, j);
                    this->grille_check[i][j] = a.at_check(i, j);
                }
        }

        /**
         * @brief Fais faire un tour au joueur
         * 
         * @param ennemi joueur à attaquer
         */
        void play(Joueur& ennemi) {
            // display(grille);
            // display(grille_check);
            afficherGrille(grille, true);
            afficherGrille(grille_check, true);
            if (hasLost())
                cout << num_joueur << " a perdu" << endl;
            else {
                int y = -1;
                int x = -1;
                string xs;
                if (!isBot) {
                    demande("Quelle ligne attaquer ?", y, 1, size());
                    demande("Quelle colonne attaquer ?", xs, "A", colonne(this[0].size()));
                    x = invColonne(xs);
                } else {
                    do {
                        y = rand() % size();
                        x = rand() % this[0].size();
                    } while (grille_check[y][x] != EMPTY);
                }
                if (ennemi.isTouched(y, x)) {
                    cout << num_joueur << (hasDrowned(y,x) ? " a coulé " : " a touché ")  << ennemi.num_joueur << " en " << x << y << endl;
                    grille_check[y][x] = CHECK;
                } else grille_check[y][x] = CROSS;
            }
        }
        /**
         * @brief Demande au joueur un entier compris entre 'min' et 'max'
         * 
         * @param phrase Phrase à afficher
         * @param out valeur de sortie
         * @param min Valeur minimum
         * @param max Valeur maximum
         */
        void demande(string phrase, int& out,int min, int max) {
            cout << phrase << " [" << min << ";" << max << "]" << endl;
            do {
                cin >> out;
            } while (min > out || out > max);
        }
        /**
         * @brief Demande au joueur une lettre de colonne comprise entre 'min' et 'max'
         * 
         * @param phrase Phrase à afficher
         * @param out Valeur de sortie
         * @param min Lettre minimum
         * @param max Lettre maximum
         */
        void demande(string phrase, string& out,string min, string max) {
            cout << phrase << " [" << min << ";" << max << "]" << endl;
            do {
                cin >> out;
            } while(invColonne(min) > invColonne(out) || invColonne(out) > invColonne(max));
        }
        /**
         * @brief Positionnement manuel des bateaux
         * 
         */
        void position_ships() {
            pair<int,int> coords;
            vector<vector<int>> placements;
            vector<pair<unsigned short,vector<vector<unsigned short>>>> bateaux = {{FORM_SHIP_CARRIER_NB,FORM_SHIP_CARRIER},{FORM_SHIP_BATTLESHIP_NB,FORM_SHIP_BATTLESHIP},{FORM_SHIP_CRUISER_NB,FORM_SHIP_CRUISER},{FORM_SHIP_DESTROYER_NB,FORM_SHIP_DESTROYER}};
            for (size_t d = 0; d < bateaux.size(); d++)
                for (int i = 0; i < static_cast<int>(bateaux[d].first); i++) {
                    string x;
                    int n;
                    // display(grille);
                    afficherGrille(grille, true);
                    do {
                        demande("Quelle ligne placer ?", coords.first, 1, size());
                        demande("Quelle colonne placer ?", x, "A", colonne(this[0].size())); coords.second = invColonne(x);
                    } while (at(coords) != EMPTY); // system("cls");
                    placements = placementPossibles(coords.first-=1, coords.second-=1,bateaux[d].second.size(),bateaux[d].second[0].size());
                    // display(grille);
                    afficherGrille(grille, true);
                    demande("",n,1,placements.size());
                    for (auto y : grille)
                        for (auto x : y)
                            if (x == COORD_CENTER || x == COORD_POSSIBLE)
                                x = EMPTY;
                    placer(coords,placements[n-=1],bateaux[d].second);
                    // system("cls");
                }
        }
        /**
         * @brief Positionnement aléatoire des bateaux
         * 
         */
        void random_ships() {
            pair<int,int> coords;
            vector<vector<int>> placements;
            vector<pair<unsigned short,vector<vector<unsigned short>>>> bateaux = {{FORM_SHIP_CARRIER_NB,FORM_SHIP_CARRIER},{FORM_SHIP_BATTLESHIP_NB,FORM_SHIP_BATTLESHIP},{FORM_SHIP_CRUISER_NB,FORM_SHIP_CRUISER},{FORM_SHIP_DESTROYER_NB,FORM_SHIP_DESTROYER}};
            for (int d=0; d< static_cast<int>(bateaux.size());d++)
                for (int i = 0; i<bateaux[d].first;i++) {
                    // display(grille);
                    afficherGrille(grille, true);
                    do {
                        coords = {rand()%DIMENSIONS,rand()%DIMENSIONS};
                    } while (at(coords) != EMPTY);
                    do {
                        placements = placementPossibles(coords.first, coords.second, bateaux[d].second.size(), bateaux[d].second[0].size());
					} while (placements.empty());
                    
                    display(grille, placements);
                    // display(grille);
                    afficherGrille(grille, true);
                    placer(coords,placements[rand()%placements.size()],bateaux[d].second);
                    // system("cls");
                }
        }
        /**
         * @brief Calcul des position possible du bateau en fonction de ses dimensions et des limites et autres bateaux avoisinnant
         * 
         * @param y ligne du point d'origine du bateau
         * @param x colonne du point d'origine du bateau
         * @param height hauteur du bateau
         * @param width largeur du bateau
         * @return vector<tuple<int,int,bool>> paramètres permettant de déterminer quelle position est sélectionnée
         */
        vector<vector<int>> placementPossibles(int y, int x, int height, int width) {
            if (!isBot) at(y, x) = COORD_CENTER;
            vector<vector<int>> rtr;
            for (int n : {0,1})
                for (int o : {+1,-1})
                    for (int p : {+1,-1}) {
                        bool possible = true;
                        for (int i = 0; i < height && possible; i++)
                            for (int j = 0; j < width && possible; j++) {
                                int H = (int)grille.size();
                                int W = (int)grille[0].size();

                                int ay = y + ((n == 1) ? i * o : j * p);
                                int ax = x + ((n == 0) ? i * o : j * p);

                                possible = (0 <= ay && ay < H && 0 <= ax && ax < W);
                                if (possible) possible = (at(ay, ax) == EMPTY);

                            }
                        if (possible) {
                            rtr.push_back({ o,p,n });
                            if (!isBot) at(((n == 1) ? (y + (height - 1) * o) : (y + (width - 1) * p)), ((n == 0) ? (x + (height - 1) * o) : (x + (width - 1) * p))) = COORD_POSSIBLE;
                        }
                    }
            if (!isBot) {
                cout << "Quel numero de placement ? ";
                for (int i = 0; i < static_cast<int>(rtr.size()); i++)
                    cout << colonne((rtr[i][2] == 1) ? x + (width - 1) * rtr[i][1] : x + (height - 1) * rtr[i][0]) << ((rtr[i][2] == 1) ? y + (height - 1) * rtr[i][1] : y + (width - 1) * rtr[i][0]) << (i < static_cast<int>(rtr.size()) ? ", " : "");
            }
            return rtr;
        }
        /**
         * @brief Placement du bateau
         * 
         * @param coords Centre du bateau
         * @param param Paramètres du placement possible {direction horizontale, direction verticale, lecture horizontale ou verticale}
         * @param forme Forme du bateau à placer
         */
        void placer(pair<int,int> coords, vector<int> param, vector<vector<unsigned short>> forme) {
            for (int i = 0; i < static_cast<int>(forme.size()); i++)
                for (int j = 0; j < static_cast<int>(forme[0].size()); j++)
                    at(((param[2] == 1) ? (coords.first + i * param[0]) : (coords.first + j * param[1])), ((param[2] == 1) ? (coords.second + j * param[1]) : (coords.second + i * param[0]))) = (forme[i][j] == 1 ? SHIP : EMPTY);
        }

        /**
         * @brief Renvoi si à la position envoyée, un bateau ennemi est touché et le couler si touché
         * 
         * @param y ligne souhaitée
         * @param x colonne souhaitée
         * @return true Le bateau est touché
         * @return false Rien n'a été touché
         */
        bool isTouched(int y, int x) {
            bool rtr = at(y,x) == SHIP;
            if (rtr)
                at(y,x) = DROWNED_SHIP;
            return rtr;
        }
        /**
         * @brief Vérifie si le bateau à la poisition (x,y) à coulé
         * 
         * @param y ligne souhaitée
         * @param x colonne souhaitée
         * @return true Le bateau à coulé
         * @return false Le bateau n'a pas encore coulé
         */
        bool hasDrowned(int y, int x) {
            bool rtr = !(at(y,x)==SHIP);
            if (rtr && y > 0              && (at(y - 1, x) == SHIP || at(y - 1, x) == DROWNED_SHIP))      rtr = hasDrowned(y - 1, x);
            if (rtr && y < size()         && (at(y + 1, x) == SHIP || at(y + 1, x) == DROWNED_SHIP))      rtr = hasDrowned(y+1,x);
            if (rtr && x > 0              && (at(y, x - 1) == SHIP || at(y, x - 1) == DROWNED_SHIP))      rtr = hasDrowned(y, x - 1);
            if (rtr && x < this[0].size() && (at(y, x + 1) == SHIP || at(y, x + 1) == DROWNED_SHIP))      rtr = hasDrowned(y,x+1);
            return rtr;
        }
        /**
         * @brief Renvoie si le joueur à perdu après vérification, si oui, met à jour 'lost' et 'hasLost'
         * 
         * @return true Le joueur à perdu
         * @return false Le joueur n'a pas encore perdu
         */
        bool hasLost() {
            if (!lost) {
                short rtr = true;
                for (int i = 0; i < size() && rtr; i++)
                    for (int j = 0 ; j< static_cast<int>(this[0].size()) && rtr;j++)
                        rtr = at(i,j) == num_joueur;
                lost = rtr == 0;
                if (lost) nb_lost++;
            }
            return lost;
        }

        /**
         * @brief Renvoie la case sélectionnée
         * 
         * @param y ligne souhaitée
         * @param x colonne souhaitée
         * @return unsigned& Case renvoyée
         */
        unsigned short& at(int y, int x) {
            if (y < 0 || y >= (int)grille.size() || x < 0 || x >= (int)grille[0].size())
                throw out_of_range("at(): indices hors grille");
            return grille[y][x]; 
        }
        /**
        * @brief Renvoie la case sélectionnée
        * 
        * @param p point souhaitée
        * @return unsigned& Case renvoyée
        */
        unsigned short& at(pair<int,int> p) { return grille[p.first][p.second]; }
        /**
         * @brief Renvoie la case sélectionnée
         *
         * @param y ligne souhaitée
         * @param x colonne souhaitée
         * @return unsigned& Case renvoyée
         */
        unsigned short& at_check(int y, int x) { return grille_check[y][x]; }
        /**
        * @brief Renvoie la case sélectionnée
        *
        * @param p point souhaitée
        * @return unsigned& Case renvoyée
        */
        unsigned short& at_check(pair<int, int> p) { return grille_check[p.first][p.second]; }
        /**
         * @brief Renvoie le nombre de ligne de la grille de positionnement des bateaux
         * 
         * @return const int Taille de la grille
         */
        const int size() { return grille.size(); }
        /**
         * @brief Renvoie le nombre de ligne de la grille de positionnement des bateaux
         *
         * @return const int Taille de la grille
         */
        const int size_check() { return grille_check.size(); }
        /**
         * @brief Réinitialise les différentes variables dont les grilles du joueur
         * 
         */
        void reset() {
            grille = vector<vector<unsigned short>>(DIMENSIONS, vector<unsigned short>(DIMENSIONS, EMPTY));
            grille_check = vector<vector<unsigned short>>(DIMENSIONS, vector<unsigned short>(DIMENSIONS, EMPTY));
            lost = false;
            nb_lost = 0;
        }
        /**
         * @brief Getter du 'num_joueur'
         * 
         * @return const short Valeur de 'num_joueur'
         */
        const short getNum_joueur() { return num_joueur; }

        /**
         * @brief Renvoie la colonne associé à un entier
         * 
         * Eemple : 12 -> "L" ; 36 -> "AJ"
         * 
         * @param x Entier associé à la colonne
         * @return string Colonne renvoyée
         */
        string colonne(int x) {
            string rtr = "";
            while(x >= 0) {
                int r = x % 26;
                rtr = char('A' + r) + rtr;
                x = x / 26 - 1;
            }
            return rtr;
        }
        /**
         * @brief Renvoie l'entier associé à la colonne
         * 
         * @param s Colonne associé à l'entier
         * @return int Entier associé à la colonne
         */
        int invColonne(const string& s) {
            int n = 0;
            for (char c : s) {
                n = n * 26 + (c - 'A' + 1);
            }
            return n - 1;
        }
        /**
         * @brief Renvoie le caractère associé à la définition de l'état des cases des grilles
         * 
         * @param yx État défini
         * @return string Caractère renvoyé
         */
        string carac(unsigned short yx, vector<vector<int>> placements = {}) {
            string rtr;
            switch(yx) {
                case EMPTY:
                    rtr = " ";
                    break;
                case CHECK:
                    rtr = "o";//"✓";
                    break;
                case CROSS:
                    rtr = "x";//"✗";
                    break;
                case SHIP:
                    rtr = "■";
                    break;
                case SHIP_CROSS:
                    rtr = "☒";
                    break;
                case SHIP_CHECK:
                    rtr = "☑";
                    break;
                case DROWNED_SHIP:
                    rtr = "☐";
                    break;
                case COORD_CENTER:
                    rtr = "o";
                    break;
                case COORD_POSSIBLE:
                    rtr = "x";
                    break;
            }
            return rtr;
        }

        /**
         * @brief Affichage de la grille 'g'
         * 
         * @param g Grille à afficher
         */
        void display(vector<vector<unsigned short>>& g, vector<vector<int>> placements = {}) {
            int width = static_cast<int>(g[0].size());
            int height = static_cast<int>(g.size());
            for (int j = 0; j < width; j++) cout << (j<=0 ? "╔═══╦" : "") << "═══" << (j>= width ? "╗\n" : "╤");
            cout << "║" << (num_joueur >= 100 ? "" : " ") << num_joueur << (num_joueur >= 10 ? "" : " ") << "║"; for (int j = 0; j < width; j++) cout << (colonne(j).size()>=3 ? "" : " ") << colonne(j) << (colonne(j).size()>=2 ? "" : " ") << (j>= width ? "╗\n" : "╤");
            for (int j = 0; j < width; j++) cout << (j<=0 ? "╠═══╬" : "") << "═══" << (j>= width ? "╣\n" : "╪");
            for (int i = 0; i < height; i++) {
                cout << "║" << (height >=100 ? "" : " ") << i+1 << (height >=10 ? "" : " ") << "║"; for (int j = 0; j < width; j++) cout << (carac(g[i][j]).size()>=3 ? "" : " ") << carac(g[i][j]) << (carac(g[i][j], placements).size()>=2 ? "" : " ") << (j>= width ? "║\n" : "│");
                for (int j = 0; j < width; j++) cout << (j<=0 ? "╟───╫" : "") << "───" << (j>= width ? "╢\n" : "┼");
            }
            for (int j = 0; j < width; j++) cout << (j<=0 ? "╚═══╩" : "") << "═══" << (j>= width ? "╝\n" : "╧");
            cout << endl;
        }

        static string colLabel(int c) { // 0->A, 1->B, ...
            return string(1, char('A' + c));
        }

        // À adapter à tes codes internes (EMPTY, SHIP, HIT, MISS, etc.)
        static string cellToStr(unsigned short v, bool revealShips) {
            // Exemple : adapte aux valeurs de ton projet
            const unsigned short EMPTY = 0;
            const unsigned short SHIP = 1;
            const unsigned short HIT = 2;
            const unsigned short MISS = 3;

            if (v == EMPTY) return " ";
            if (v == SHIP)  return revealShips ? "■" : " ";
            if (v == HIT)   return "X";
            if (v == MISS)  return "•";
            return "?";
        }

        void afficherGrille(const vector<vector<unsigned short>>& g, bool revealShips = true) {
            const int H = (int)g.size();
            const int W = H ? (int)g[0].size() : 0;
            if (H == 0 || W == 0) return;

            // ── Ligne du haut
            cout << "╔═══╦";
            for (int c = 0; c < W; ++c) {
                cout << "═══";
                cout << (c == W - 1 ? "╗" : "╤");
            }
            cout << "\n";

            // ── En-tête colonnes
            cout << "║   ║";
            for (int c = 0; c < W; ++c) {
                string lab = colLabel(c);
                cout << " " << lab << " ";
                cout << (c == W - 1 ? "║" : "│");
            }
            cout << "\n";

            // ── Séparateur sous l’en-tête
            cout << "╠═══╬";
            for (int c = 0; c < W; ++c) {
                cout << "═══";
                cout << (c == W - 1 ? "╣" : "┼");
            }
            cout << "\n";

            // ── Lignes
            for (int r = 0; r < H; ++r) {
                // Numéro de ligne (1..10) aligné sur 2 chars
                if (r + 1 < 10) cout << "║ " << (r + 1) << " ║";
                else            cout << "║" << (r + 1) << " ║";

                for (int c = 0; c < W; ++c) {
                    cout << " " << cellToStr(g[r][c], revealShips) << " ";
                    cout << (c == W - 1 ? "║" : "│");
                }
                cout << "\n";

                // Séparateur entre lignes
                if (r != H - 1) {
                    cout << "╟───╫";
                    for (int c = 0; c < W; ++c) {
                        cout << "───";
                        cout << (c == W - 1 ? "╢" : "┼");
                    }
                    cout << "\n";
                }
            }

            // ── Ligne du bas
            cout << "╚═══╩";
            for (int c = 0; c < W; ++c) {
                cout << "═══";
                cout << (c == W - 1 ? "╝" : "╧");
            }
            cout << "\n";
        }

};