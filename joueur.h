/**
 * @file joueur.h
 * @brief classe joueur permettant de faire fonctionner une partie de bataille navale
 * @author Léopaul ESTOUR
 * @date 01/12/2025
 * @version V0.4
 */
#include <vector>
#include <tuple>
#include <iostream>
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
        void operator=(unsigned short a) { grille = vector<vector<unsigned short>>(this->size(), vector<unsigned short>(this[0].size(), a)); }
        /**
         * @brief Copie la grille du joueur 'a'
         * 
         * @param a joueur à copier
         */
        void operator=(Joueur& a) {
            // if (a.size()!= this->grille.size() || a[0].size()!= this->grille[0].size())
            //     throw length_error("Les grilles ne font pas la même taille");
            grille = vector<vector<unsigned short>>(a.size(), vector<unsigned short>(a[0].size(), EMPTY));
            for (int i=0; i<grille.size();i++)
                for (int j=0; j<grille.size();j++)
                    this->grille[i][j] = a(i,j);
        }

        /**
         * @brief Fais faire un tour au joueur
         * 
         * @param ennemi joueur à attaquer
         */
        void play(Joueur& ennemi) {
            display(grille);
            display(grille_check);
            if (hasLost())
                cout << num_joueur << " a perdu" << endl;
            else {
                int y=-1,x;
                string xs=0;
                if (!isBot) {
                    demande("Quelle ligne attaquer ?", y, 1, size());
                    demande("Quelle colonne attaquer ?",xs,"A",colonne(this[0].size()));
                } else {
                    do {
                        y = rand()%size();
                        x = rand()%this[0].size();
                    } while(grille_check[y][x]!=EMPTY);
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
            } while(min <= out && out <= max);
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
            } while(invColonne(min) <= invColonne(out) && invColonne(out) <= invColonne(max));
        }

        /**
         * @brief Positionnement manuel des bateaux
         * 
         */
        void position_ships() {
            pair<int,int> coords;
            vector<tuple<int,int,bool>> placements;
            vector<pair<unsigned short,vector<vector<unsigned short>>>> bateaux = {{FORM_SHIP_CARRIER_NB,FORM_SHIP_CARRIER},{FORM_SHIP_BATTLESHIP_NB,FORM_SHIP_BATTLESHIP},{FORM_SHIP_CRUISER_NB,FORM_SHIP_CRUISER},{FORM_SHIP_DESTROYER_NB,FORM_SHIP_DESTROYER}};
            for (int d=0; d<bateaux.size();d++)
                for (int i = 0; i<bateaux[d].first;i++) {
                    string x=0;
                    int n;
                    demande("Quelle ligne placer ?",coords.first,1,size());
                    demande("Quelle colonne placer ?",x,"A",colonne(this[0].size())); coords.second = invColonne(x);
                    placements = placementPossibles(coords.first-=1, coords.second-=1,bateaux[d].second.size(),bateaux[d].second[0].size());
                    demande("",n,1,placements.size());
                    placer(coords,placements[n-=1],bateaux[d].second);
                }
        }
        /**
         * @brief Positionnement aléatoire des bateaux
         * 
         */
        void random_ships() {
            pair<int,int> coords;
            vector<tuple<int,int,bool>> placements;
            vector<pair<unsigned short,vector<vector<unsigned short>>>> bateaux = {{FORM_SHIP_CARRIER_NB,FORM_SHIP_CARRIER},{FORM_SHIP_BATTLESHIP_NB,FORM_SHIP_BATTLESHIP},{FORM_SHIP_CRUISER_NB,FORM_SHIP_CRUISER},{FORM_SHIP_DESTROYER_NB,FORM_SHIP_DESTROYER}};
            for (int d=0; d<bateaux.size();d++)
                for (int i = 0; i<bateaux[d].first;i++) {
                    coords = {rand()%DIMENSIONS,rand()%DIMENSIONS};
                    placements = placementPossibles(coords.first, coords.second,bateaux[d].second.size(),bateaux[d].second[0].size());
                    placer(coords,placements[rand()%placements.size()],bateaux[d].second);
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
        vector<tuple<int,int,bool>> placementPossibles(int y, int x, int height, int width) {
            vector<tuple<int,int,bool>> rtr;
            for (int o : {+1,-1})
                for (int p : {+1,-1}) {
                    bool possible = true;
                    for (int i = 0; i<height && possible; i++)
                        for (int j = 0; j<width && possible; j++)
                            possible = at(y+i*o,x+j*p);
                    if (possible) rtr.push_back({o,p,true});
                }
            for (int o : {+1,-1})
                for (int p : {+1,-1}) {
                    bool possible = true;
                    for (int i = 0; i<height && possible; i++)
                        for (int j = 0; j<width && possible; j++)
                            possible = at(x+j*p,y+i*o);
                    if (possible) rtr.push_back({p,o,false});
                }
            if (!isBot)
                cout << "Quel numero de placement ? "; for (int i = 0;i<rtr.size();i++) cout << "(" << colonne(get<2>(rtr[i]) ? x*get<1>(rtr[i]) : y*get<0>(rtr[i])) << "," << (get<2>(rtr[i]) ? x*get<1>(rtr[i]) : y*get<0>(rtr[i])) <<"), ";
            return rtr;
        }
        /**
         * @brief Placement du bateau
         * 
         * @param coords Centre du bateau
         * @param param Paramètres du placement possible
         * @param forme Forme du bateau à placer
         */
        void placer(pair<int,int> coords, tuple<int,int,bool> param, vector<vector<unsigned short>> forme) {
            for (int i = 0; i<forme.size(); i++)
                for (int j = 0; j<forme[0].size(); j++)
                    at((get<2>(param)?(coords.first+i*get<0>(param)):(coords.second+j*get<1>(param))),(get<2>(param)?(coords.second+j*get<1>(param)):(coords.first+i*get<0>(param)))) = (forme[i][j]==1 ? SHIP : EMPTY);
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
            bool rtr = !at(y,x)==SHIP;
            if (rtr && y > 0) rtr = hasDrowned(y-1,x);
            if (rtr && y < size()) rtr = hasDrowned(y+1,x);
            if (rtr && x > 0) rtr = hasDrowned(y,x-1);
            if (rtr && x < this[0].size()) rtr = hasDrowned(y,x+1);
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
                    for (int j = 0 ; j<this[0].size() && rtr;j++)
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
        unsigned short& at(int y, int x) { return grille[y][x]; }
       /**
        * @brief Renvoie la case sélectionnée
        * 
        * @param p point souhaitée
        * @return unsigned& Case renvoyée
        */
        unsigned short& at(pair<int,int> p) { return grille[p.first][p.second]; }
        /**
         * @brief Renvoie le nombre de ligne de la grille de positionnement des bateaux
         * 
         * @return const int Taille de la grille
         */
        const int size() { return grille.size(); }
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
        string carac(unsigned short yx) {
            string rtr;
            switch(yx) {
                case EMPTY:
                    rtr = " ";
                    break;
                case CHECK:
                    rtr = "✓";
                    break;
                case CROSS:
                    rtr = "✗";
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
            }
            return rtr;
        }

        /**
         * @brief Affichage de la grille 'g'
         * 
         * @param g Grille à afficher
         */
        void display(vector<vector<unsigned short>>& g) {
            for (int j = 0; j < g[0].size(); j++) cout << (j<=0 ? "╔═══╦" : "") << "═══" << (j>=g[0].size() ? "╗\n" : "╤");
            cout << "║" << (num_joueur >= 100 ? "" : " ") << num_joueur << (num_joueur >= 10 ? "" : " ") << "║"; for (int j = 0; j < g[0].size(); j++) cout << (colonne(j).size()>=3 ? "" : " ") << colonne(j) << (colonne(j).size()>=2 ? "" : " ") << (j>=g[0].size() ? "╗\n" : "╤");
            for (int j = 0; j < g[0].size(); j++) cout << (j<=0 ? "╠═══╬" : "") << "═══" << (j>=g[0].size() ? "╣\n" : "╪");
            for (int i = 0; i < g.size(); i++) {
                cout << "║" << (g.size()>=100 ? "" : " ") << i+1 << (g.size()>=10 ? "" : " ") << "║"; for (int j = 0; j < g[0].size(); j++) cout << (carac(g[i][j]).size()>=3 ? "" : " ") << carac(g[i][j]) << (carac(g[i][j]).size()>=2 ? "" : " ") << (j>=g[0].size() ? "║\n" : "│");
                for (int j = 0; j < g[0].size(); j++) cout << (j<=0 ? "╟───╫" : "") << "───" << (j>=g[0].size() ? "╢\n" : "┼");
            }
            for (int j = 0; j < g[0].size(); j++) cout << (j<=0 ? "╚═══╩" : "") << "═══" << (j>=g[0].size() ? "╝\n" : "╧");
            cout << endl;
        }
};