
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

void liberer_grille(int** tableau, int hauteur){
    for (int a=0; a<hauteur; a++){
        free(tableau[a]);
    }
    free(tableau);
}

void liberer_image(image* img){
    for (int a=0; a<img->haut; a++){
        free(img->pix[a]);
    }
    free(img->pix);
    free(img);
}

void liberer_tab_image(image** tab_img, int taille_tab){
    for (int a=0; a<taille_tab; a++){
        liberer_image(tab_img[a]);
    }
    free(tab_img);
}

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


int** tab_moyenne(image** tab_image, int taille_tab){
    int hauteur = tab_image[0]->haut;
    int largeur = tab_image[0]->larg;
    int** tab_moyenne = (int**) malloc (hauteur * sizeof(int*));
    for (int a=0; a<hauteur; a++){
        tab_moyenne[a] = (int*) malloc(largeur* sizeof(int));
        for (int b=0; b<largeur; b++){
            tab_moyenne[a][b] = 0;
            for (int im=0; im<taille_tab; im++){
            printf("a = %d, b = %d, im = %d\n", a, b, im);
                tab_moyenne[a][b] += tab_image[im]->pix[a][b];
            }
            tab_moyenne[a][b] = tab_moyenne[a][b] / taille_tab;
        }
    }
    printf("bug5\n");
    return tab_moyenne;
}

int** tab_ecart_type(image** tab_image, int taille_tab, int** tab_moyenne){
    int hauteur = tab_image[0]->haut;
    int largeur = tab_image[0]->larg;
    int** tab_ecart_type = (int**) malloc (hauteur * sizeof(int*));
    for (int a=0; a<hauteur; a++){
        tab_ecart_type[a] = (int*) malloc(largeur* sizeof(int));
        for (int b=0; b<largeur; b++){
            tab_ecart_type[a][b] = 0;
            for (int im=0; im<taille_tab; im++){
                tab_ecart_type[a][b] += (tab_image[im]->pix[a][b] - tab_moyenne[a][b]) * (tab_image[im]->pix[a][b] - tab_moyenne[a][b]);
            }
            tab_ecart_type[a][b] = tab_ecart_type[a][b] / (taille_tab - 1);
            tab_ecart_type[a][b] = sqrt(tab_ecart_type[a][b]);
        }
    }
    return tab_ecart_type;
}

image* detection_mvt_par_fond(image* img, int** tab_ecart_type, int** tab_moyenne){
    int hauteur = img->haut;
    int largeur = img->larg;
    image* res = creer_image(hauteur, largeur);

    for (int a=0; a<hauteur; a++){
        for (int b=0; b<largeur; b++){
            if ((img->pix[a][b] > tab_moyenne[a][b] + tab_ecart_type[a][b]) || (img->pix[a][b] < tab_moyenne[a][b] - tab_ecart_type[a][b])){
                res->pix[a][b] = 255;
            }
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

    /*
    printf("-----------------------------\n");
    printf("PREMIER TEST\n");
    printf("-----------------------------\n");

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

    int** tab_moy = tab_moyenne(tab_img, nb_image_fond);
    int** tab_et = tab_ecart_type(tab_img, nb_image_fond, tab_moy);

    image* img5 = charger_image("mvtfond5.pgm");
    image* detect_img5 = detection_mvt_par_fond(img5, tab_et, tab_moy);
    sauvegarder_image("res.pgm", detect_img5);


    printf("\ndebut free : ");
    liberer_grille(tab_moy, tab_img[0]->haut);
    printf("1 - ");
    liberer_grille(tab_et, tab_img[0]->haut);
    printf("2 - ");
    free(tab_nom);
    printf("3 - ");
    liberer_tab_image(tab_img, nb_image_fond);
    printf("4 - ");
    free(img5);
    free(detect_img5);
    printf("fin free\n");

    */

    printf("-----------------------------\n");
    printf("DEUXIEME TEST\n");
    printf("-----------------------------\n");

    int nb_image_fond2 = 7;
    char** tab2_nom = (char**) malloc(nb_image_fond2 * sizeof(char*)); 
    for (int i=0; i<nb_image_fond2; i++){
        tab2_nom[i] = (char*) malloc(12*sizeof(char));
    }
    tab2_nom[0] = "testbruit0.pgm";
    tab2_nom[1] = "testbruit1.pgm";
    tab2_nom[2] = "testbruit2.pgm";
    tab2_nom[3] = "testbruit3.pgm";
    tab2_nom[4] = "testbruit4.pgm";
    tab2_nom[5] = "testbruit5.pgm";
    tab2_nom[6] = "testbruit6.pgm";

    printf("crash test\n");
    for (int i=0; i<nb_image_fond2; i++){
        printf("%s\n", tab2_nom[i]);
    }
    printf("crash test\n");
    image** tab2_img = conversion_tab_image(tab2_nom, nb_image_fond2);

    printf("crash mhein\n");

    int** tab2_moy = tab_moyenne(tab2_img, nb_image_fond2);
    printf("crash mhein\n");
    int** tab2_et = tab_ecart_type(tab2_img, nb_image_fond2, tab2_moy);

    printf("crash test\n");
    image* imtestbruit4 = charger_image("testbruit4.pgm");
    image* detect_testbruit4 = detection_mvt_par_fond(imtestbruit4, tab2_et, tab2_moy);
    sauvegarder_image("res_test_bruit4.pgm", detect_testbruit4);

    printf("crash test\n");


    printf("\ndebut free : ");
    liberer_grille(tab2_moy, tab2_img[0]->haut);
    printf("1 - ");
    liberer_grille(tab2_et, tab2_img[0]->haut);
    printf("2 - ");
    free(tab2_nom);
    printf("3 - ");
    liberer_tab_image(tab2_img, nb_image_fond2);
    printf("4 - ");
    free(imtestbruit4);
    free(detect_testbruit4);
    printf("fin free\n");


    printf("\nFin du programme.");
    return 0;
}
