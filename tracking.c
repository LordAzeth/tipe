
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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
    fscanf(f, " %s", format);
    fscanf(f, " %d %d", &res->larg, &res->haut);
    fscanf(f, " %d", &res->max_couleur);

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


int main(){
  printf("test1");
  image* im1 = charger_image("test1.png");
  image* im2 = charger_image("test2.png");
  image* im3 = charger_image("test3.png");

  printf("test2");
  
  return 0;
}
