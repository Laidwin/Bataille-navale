/**
 * @file main.cpp
 * @author Léopaul ESTOUR (Leopaul.Estour@eleves.ec-nantes.fr)
 * @brief Programme principal de la bataille navale
 * @version V0.2
 * @date 2025-12-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "joueur.h"

/**
 * @brief Programme principal faisant fonctionner la bataille navale et initialisant les joueurs
 * 
 * @return int Renvoie normal de main
 */
int main() {
    srand(time(NULL));
    int iter = 0;
    bool fin = false;
    vector<Joueur> joueurs;
    Joueur::nb_joueurs=0;
    Joueur::nb_lost=0;
    for (int i = 0; i<NB_JOUEURS; i++)
        joueurs.push_back(Joueur());
    short lost = 0;
    while(!fin) {
        if (!joueurs[iter%joueurs.size()].hasLost()) {
            joueurs[iter%joueurs.size()].play(joueurs[iter%(joueurs.size()+1)]);
            fin = Joueur::nb_joueurs-Joueur::nb_lost <= 1;
        }
        ++iter;
    }
    cout << (iter>=ITER_MAX) ? "Le vainqueur est : " : "Les vainqueurs sont :\n";
    for (int i = 0; i < joueurs.size();i++)
        if (!joueurs[i].hasLost())
            cout << ((iter>=ITER_MAX) ? " - " : "") << joueurs[i].getNum_joueur() << (iter>=ITER_MAX) ? ",\n" : "";
    return 0;
}