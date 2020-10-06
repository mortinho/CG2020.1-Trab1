/*
 * main.cpp
 *
 *  Created on: 6 de out de 2020
 *      Author: rafael martinho
 */

#include "Image.h"
#include "ImageRGBA.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

#define TILE_H 256
#define TILE_W 256
#define GRID_H 12
#define GRID_W 16
#define ACID_LEVEL 2
#define CEILING 1

using namespace std;

int RNG(int max){
	return rand()%max;
}

Color toColor(RGBA c){
	return {c.r,c.g,c.b};
}

void tileBlit(Image &b,ImageRGBA &a, int x, int y){
	int hOffset = y * TILE_H;
	int wOffset = x * TILE_W;
	for(int w = 0; w< TILE_W; w++){
			for(int h = 0; h< TILE_H; h++){
				if (a(w,h).a) b(w + wOffset, h + hOffset) = toColor(a(w,h));
			}
	}
}

void tileBlit(Image &b,Image &a, int x, int y){
	int hOffset = y * TILE_H;
	int wOffset = x * TILE_W;
	for(int w = 0; w< TILE_W; w++){
			for(int h = 0; h< TILE_H; h++){
				b(w + wOffset, h + hOffset) = a(w,h);
			}
	}
}

enum {
	EMPTY,
	BG1,
	BG2,
	AB,
	AT1,
	AT2,
	PTL, //pillar top left, right etc
	PTR,
	PL,
	PR,
	PT,
	P,
	SAW
};

typedef int Grid[GRID_W][GRID_H];

void bgInit(Grid g){
	for(int w = 0; w< GRID_W; w++){
		for(int h = 0; h< GRID_H; h++){
			g[w][h] = (h+w)%2 ? BG1 : BG2;
		}
	}
}

void fgInit(Grid g){
	for(int w = 0; w< GRID_W; w++){
		for(int h = 0; h< GRID_H; h++){
			g[w][h] = EMPTY;
		}
	}
}

void acid(Grid bg,Grid fg){
	for(int h = 0; h< GRID_H; h++){
		for(int w = 0; w< GRID_W; w++){
			if(h<ACID_LEVEL) bg[w][h] = AB;
			if(h==ACID_LEVEL) bg[w][h] = (h+w)%2 ? AT1 : AT2;
		}
	}
}

void pillars(Grid bg,Grid fg){
	int maxH = GRID_H*3/5;
	cout<<maxH<<endl;
	int bigW = GRID_W*1/2 -  RNG(2);
	cout<<bigW<<endl;
	int bigH = RNG(maxH);
	if (bigH <= ACID_LEVEL) bigH = ACID_LEVEL;
	int bigStart = RNG(GRID_W - bigW);
	for (int w = bigStart; w<=bigStart+bigW;w++){
		for (int h = 0;h<=bigH;h++){
			if (w == bigStart){//left
				if (h == bigH) { //topleft
					fg[w][h] = PTL;
				} else fg[w][h] = PL;
			} else if (w==bigStart+bigW) { //right
				if (h == bigH) { //topright
					fg[w][h] = PTR;
				} else fg[w][h] = PR;
			} else{
				if (h == bigH) { //top
					bg[w][h] = PT;
				} else bg[w][h] = P;
			}
		}
	}

	fg[RNG(GRID_W-2) +1][bigH+2+RNG(3)] = SAW;
	fg[RNG(GRID_W-2) +1][bigH+2+RNG(3)] = SAW;

}

void gridPrint(Grid g){
	for(int h = 0; h< GRID_H; h++){
		for(int w = 0; w< GRID_W; w++){
			cout<<g[w][h]<<" ";
		}
		cout<<endl;
	}
}


int main(){
	srand((unsigned) time(NULL));
	ImageRGBA acidTop("png/tiles/acid (1).png");
	ImageRGBA acidBot("png/tiles/acid (2).png");
	ImageRGBA BgTile1("png/tiles/BGTile (3).png");
	ImageRGBA BgTile2("png/tiles/BGTile (4).png");
	ImageRGBA pillarTopLeft("png/tiles/Tile (1).png");
	ImageRGBA pillarTopRight("png/tiles/Tile (3).png");
	ImageRGBA pillarLeft("png/tiles/Tile (4).png");
	ImageRGBA pillarRight("png/tiles/Tile (6).png");
	ImageRGBA pillarTop("png/tiles/Tile (2).png");
	ImageRGBA pillar("png/tiles/Tile (5).png");
	ImageRGBA smallplat("png/tiles/Tile (15).png");

	Image saw("saw.png");
	tileBlit(saw, smallplat, 0, 0);

	Image acidTop1("png/tiles/BGtile (3).png");
	tileBlit(acidTop1, acidTop, 0,0);

	Image acidTop2("png/tiles/bgtile (4).png");
	tileBlit(acidTop2, acidTop, 0, 0);

	Image scenario( TILE_W * GRID_W, TILE_H * GRID_H); //output image
	scenario.fill(black);


	Grid background,foreground; //procedural formation WIP
	bgInit(background);
	fgInit(foreground);
	acid(background,foreground);
	pillars(background,foreground);
	gridPrint(background);
	gridPrint(foreground);


	for(int h = 0; h< GRID_H; h++){
		for(int w = 0; w< GRID_W; w++){
			switch(background[w][h]){ //background drawing is first
			case AB: tileBlit(scenario,acidBot,w,h); break;
			case AT1: tileBlit(scenario,acidTop1,w,h); break;
			case AT2: tileBlit(scenario,acidTop2,w,h); break;
			case BG1: tileBlit(scenario,BgTile1,w,h); break;
			case BG2: tileBlit(scenario,BgTile2,w,h); break;
			case PT: tileBlit(scenario,pillarTop,w,h); break;
			case P: tileBlit(scenario,pillar,w,h); break;
			}
			switch(foreground[w][h]){
			case PTL: tileBlit(scenario,pillarTopLeft,w,h); break;
			case PTR: tileBlit(scenario,pillarTopRight,w,h); break;
			case PL: tileBlit(scenario,pillarLeft,w,h); break;
			case PR: tileBlit(scenario,pillarRight,w,h); break;
			case SAW: tileBlit(scenario,saw,w,h); break;
			}
		}
	}
	if (scenario.height >1){
		cout<<"saving"<<endl;
		scenario.savePNG("scenario.png");
		cout<<"done"<<endl;
	} else return 1;
	return 0;
}


