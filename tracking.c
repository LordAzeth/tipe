
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

struct image {
    int haut;
    int larg;
    int max_couleur;
    uint16_t** pix;
};

typedef struct image image;

image* charger_image(char* fichier){
    FILE* f = fopen(fichier, "r");
    image* res = (image*)malloc(sizeof(image));
    char format[30];
    char auteur[30];
    
    fscanf(f, "%s", &format);
    fscanf(f, "%s\n", &auteur);
    for (int a=0; a<7; a++){
    fscanf(f, "%s\n", &auteur);
    }
    fscanf(f, "%d %d", &(res->larg), &(res->haut));
    fscanf(f, "%d", &(res->max_couleur));
    res->pix=(uint16_t**)malloc(res->haut * sizeof(uint16_t*));
    for (int i=0; i<res->haut; i++){
        res->pix[i]=(uint16_t*)malloc(res->larg * sizeof(uint16_t));
        for (int j=0; j<res->larg; j++){
            fscanf(f, " %hu", &res->pix[i][j]);
        }
    }
    fclose(f);
    return res;
}

image* creer_image(int hauteur, int largeur){
    image* res = (image*)malloc(sizeof(image));
    res->haut = hauteur;
    res->larg = largeur;
    res->max_couleur = 255;
    res->pix=(uint16_t**)malloc(res->haut * sizeof(uint16_t*));
    for (int i=0; i<hauteur; i++){
        res->pix[i]=(uint16_t*)malloc(res->larg * sizeof(uint16_t));
        for (int j=0; j<largeur; j++){
            res->pix[i][j] = 0;
        }
    }
    return res;
}

int sauvegarder_image(char* fichier, image* img){
    FILE* f = fopen(fichier, "w");
    fprintf(f, "P2\n");
    fprintf(f, "# Created by GIMP version 2.10.24 PNM plug-in\n");
    fprintf(f, "%d %d\n", img->larg, img->haut);
    fprintf(f, "%d\n", img->max_couleur);

    for (int i=0; i<img->haut; i++){
        for (int j=0; j<img->larg; j++){
            fprintf(f, "%hu ", img->pix[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return 1;
}

int nombre_pixel_differents(image* im1, image* im2){
    int nb_pix = 0;
    for (int a = 0; a<im1->haut; a++){
        for (int b = 0; b<im1->larg; b++){
            if (im1->pix[a][b] != im2->pix[a][b]){
                nb_pix ++;
            }
        }
    }
    return nb_pix;
}

image* soustraction_image(image* im1, image* im2){
    image* res = (image*)malloc(sizeof(image));
    res->pix = (uint16_t**) malloc(im1->haut * sizeof(uint16_t*));
    for (int i=0; i<im1->haut; i++){
        res->pix[i]=(uint16_t*)malloc(res->larg * sizeof(uint16_t));
    }
    res->haut = im1->haut;
    res->larg = im1->larg;
    res->max_couleur = 0; /// ou 255
    for (int i=0; i<im1->haut; i++){
        for (int j=0; j<im2->larg; j++){
            ///printf("%d - %d - %d : %d et %d\n", res->pix[i][j], im1->pix[i][j], im2->pix[i][j], i, j);
            if (im1->pix[i][j] >= im2->pix[i][j]){
                res->pix[i][j] = im1->pix[i][j] - im2->pix[i][j]; // OU 255 - im1->pix[i][j] + im2->pix[i][j];
            }
            else{
                res->pix[i][j] = im2->pix[i][j] - im1->pix[i][j]; // OU 255 - im2->pix[i][j] + im1->pix[i][j];
            }
            if (res->pix[i][j] > res->max_couleur){
                res->max_couleur = res->pix[i][j];
            }
        }
    }
    return res;
}

image* detection_mvt(image* im1, image* im2){
    image* res = (image*)malloc(sizeof(image));
    res->pix = (uint16_t**) malloc(im1->haut * sizeof(uint16_t*));
    for (int i=0; i<im1->haut; i++){
        res->pix[i]=(uint16_t*)malloc(res->larg * sizeof(uint16_t));
    }
    res->haut = im1->haut;
    res->larg = im1->larg;
    res->max_couleur = 255;
    for (int i=0; i<im1->haut; i++){
        for (int j=0; j<im2->larg; j++){
            if (im1->pix[i][j] != im2->pix[i][j]){
                res->pix[i][j] = 255; 
            }
            else{
                res->pix[i][j] = 0;
            }
        }
    }
    return res;
}

image** conversion_tab_image(char** tab_nom, int taille_tab){
    image** tab_image = (image**) malloc(taille_tab * sizeof(image*)); 
    for (int i=0; i<taille_tab; i++){
        tab_image[i] = charger_image(tab_nom[i]);
    }
    return tab_image;
}

image* detection_fond(image** tab_image, int taille_tab){
    int hauteur = tab_image[0]->haut;
    int largeur = tab_image[0]->larg;
    image* res = creer_image(hauteur, largeur);

    int** tab_moyenne = (int**) malloc (hauteur * sizeof(int*));
    for (int a=0; a<hauteur; a++){
        tab_moyenne[a] = (int*) malloc(largeur* sizeof(int));
        for (int b=0; b<largeur; b++){
            tab_moyenne[a][b] = 0;
            for (int im=0; im<taille_tab; im++){
                tab_moyenne[a][b] += tab_image[im]->pix[a][b];
            }
            tab_moyenne[a][b] = tab_moyenne[a][b] / taille_tab;
        }
    }

    int** tab_ecart_type = (int**) malloc (hauteur * sizeof(int*));
    for (int a=0; a<hauteur; a++){
        tab_ecart_type[a] = (int*) malloc(largeur* sizeof(int));
        for (int b=0; b<largeur; b++){
            tab_ecart_type[a][b] = 0;
            for (int im=0; im<taille_tab; im++){
                tab_ecart_type += (tab_image[im]->pix[a][b] - tab_moyenne[a][b]) * (tab_image[im]->pix[a][b] - tab_moyenne[a][b]);
            }
            tab_ecart_type[a][b] = tab_ecart_type[a][b] / (taille_tab - 1);
            tab_ecart_type[a][b] = sqrt(tab_ecart_type[a][b]);
            res->pix[a][b] = tab_ecart_type[a][b];
            /* IL FAUT CORRIGER CETTE FONCTION QUI DOIT NE RENVOIE PAS LES BONS PIXELS ACTUELLEMENT */
        }
    }

    return res;
}

int main(){
    printf("Debut du programme.\n");
    /*
    image* im1 = charger_image("test1.pgm");
    image* im2 = charger_image("test2.pgm");
    image* im3 = charger_image("test3.pgm");

    image* im4 = creer_image(50, 50);

    sauvegarder_image("test4.pgm", im4);
    printf("Difference 1 a 2 : %d\n", nombre_pixel_differents(im1, im2));

    image* differenceim1im2 = soustraction_image(im1, im2);
    image* detect_mouv1 = detection_mvt(im1, im2);

    sauvegarder_image("new_image1.pgm", differenceim1im2);
    sauvegarder_image("new_image2.pgm", detect_mouv1);
    */

    int nb_image_fond = 5;
    char** tab_nom = (char**) malloc(nb_image_fond * sizeof(char*)); 
    for (int i=0; i<nb_image_fond; i++){
        tab_nom[i] = (char*) malloc(12*sizeof(char));
    }
    tab_nom[0] = "mvtfond0.pgm";
    tab_nom[1] = "mvtfond1.pgm";
    tab_nom[2] = "mvtfond2.pgm";
    tab_nom[3] = "mvtfond3.pgm";
    tab_nom[4] = "mvtfond4.pgm";
   
    for (int i=0; i<nb_image_fond; i++){
        printf("%s\n", tab_nom[i]);
    }
    
    image** tab_img = conversion_tab_image(tab_nom, nb_image_fond);

    for (int i=0; i<nb_image_fond; i++){
        printf("%d\n", tab_img[i]->pix[5][1]);
    }
    image* fond = detection_fond(tab_img, nb_image_fond);
    sauvegarder_image("fond0", fond);

    printf("\nFin du programme.");
    return 0;
}
