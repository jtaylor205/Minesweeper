#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <random>
using namespace sf;
using namespace std;


struct Writing{
    string name;
    Color font;
    int size;
    int width;
    int height;

    Font textFont;
    Writing(string name, Color font, int size, int width, int height){
        this->name = name;
        this->font = font;
        this->size = size;
        this->width = width;
        this->height = height;

    }

    Text getText(){
        Text text;
        textFont.loadFromFile("files/font.ttf");
        text.setString(name);
        text.setFillColor(font);
        text.setFont(textFont);
        text.setCharacterSize(size);
        FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.width/2,textRect.height/2);
        text.setPosition(sf::Vector2f(width,height));
        return text;
    }

};

struct Player{
    string name;
    string minutes;
    string seconds;
    Player(string name, string minutes, string seconds){
        this->name = name;
        this->minutes = minutes;
        this->seconds = seconds;
    }
    Player(){
        name = "";
        minutes = "";
        seconds = "";
    }

    void setName(string name){
        this->name = name;
    }
};

struct Tile{
    bool isMine;
    bool isFlagged;
    bool isTouched;
    Tile* leftTile;
    Tile* rightTile;
    Tile* aboveTile;
    Tile* belowTile;
    Tile* diagonalTopRight;
    Tile* diagonalTopLeft;
    Tile* diagonalBottomRight;
    Tile* diagonalBottomLeft;
    int surroundingMines = 0;
    int xPos;
    int yPos;


    Tile(){
        isTouched = false;
        isFlagged = false;
        isMine = false;
        leftTile = nullptr;
        rightTile = nullptr;
        aboveTile = nullptr;
        belowTile = nullptr;
        diagonalTopRight = nullptr;
        diagonalTopLeft = nullptr;
        diagonalBottomLeft = nullptr;
        diagonalBottomRight = nullptr;
        xPos = 0;
        yPos = 0;
    }

    void flag(){
        isFlagged = !isFlagged;
    }

};

struct Board{
    vector<Tile*> tiles;
    int height;
    int width;
    int numMines;
    int clearTiles;
    int clickedTiles = 0;
    int remainingMines;
    Board(int height, int width, int numMines){
        this->height = height;
        this->width = width;
        this->numMines = numMines;
        this->remainingMines = numMines;
        clearTiles = (width*height) - numMines;
    }

    bool checkWin(){
        if (clickedTiles == clearTiles){
            return true;
        } else {
            return false;
        }
    }

    void drawBoard(){
        vector<Tile*> gameTiles;
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j ++){
                Tile* tile = new Tile();
                tile->xPos = j * 32;
                tile->yPos = i * 32;
                gameTiles.push_back(tile);
            }
        }
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j ++){
                Tile* tile = gameTiles[i * width + j];
                if (i > 0) {
                    tile->aboveTile = gameTiles[(i-1) * width + j];
                    tile->aboveTile->belowTile = tile;
                    if (j > 0) {
                        tile->diagonalTopLeft = gameTiles[(i-1) * width + (j-1)];
                        tile->diagonalTopLeft->diagonalBottomRight = tile;
                    }
                    if (j < width-1) {
                        tile->diagonalTopRight = gameTiles[(i-1) * width + (j+1)];
                        tile->diagonalTopRight->diagonalBottomLeft = tile;
                    }
                }
                if (j > 0) {
                    tile->leftTile = gameTiles[i * width + (j-1)];
                    tile->leftTile->rightTile = tile;
                }
                if (j < width-1) {
                    tile->rightTile = gameTiles[i * width + (j+1)];
                    tile->rightTile->leftTile = tile;
                }
            }
            this->remainingMines = numMines;
            clearTiles = (width*height) - numMines;
            clickedTiles = 0;
        }

        for(int i = 0; i < numMines; i ++){
            int randNum = rand() % (height * width);
            if(!gameTiles.at(randNum)->isMine){
                gameTiles.at(randNum)->isMine = true;
            } else {
                i--;
            }
        }

        for(int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                Tile *tile = gameTiles[i * width + j];
                if (tile->isMine) {
                    continue; // don't count the surrounding mines of a mine tile
                }
                if (tile->aboveTile != nullptr && tile->aboveTile->isMine) {
                    tile->surroundingMines += 1;
                }
                if (tile->belowTile != nullptr && tile->belowTile->isMine) {
                    tile->surroundingMines += 1;
                }
                if (tile->leftTile != nullptr && tile->leftTile->isMine) {
                    tile->surroundingMines += 1;
                }
                if (tile->rightTile != nullptr && tile->rightTile->isMine) {
                    tile->surroundingMines += 1 ;
                }
                if (tile->diagonalTopLeft != nullptr && tile->diagonalTopLeft->isMine) {
                    tile->surroundingMines += 1;
                }
                if (tile->diagonalTopRight != nullptr && tile->diagonalTopRight->isMine) {
                    tile->surroundingMines += 1;
                }
                if (tile->diagonalBottomRight != nullptr && tile->diagonalBottomRight->isMine) {
                    tile->surroundingMines += 1;
                }
                if (tile->diagonalBottomLeft != nullptr && tile->diagonalBottomLeft->isMine) {
                    tile->surroundingMines += 1;
                }
            }
        }


        tiles = gameTiles;
    }

    void clickSurroundingTiles(Tile* tile, bool first){
            if(tile->isTouched & first == false){
                return;
            }
            if(tile->surroundingMines !=0){
                tile->isTouched = true;
                clickedTiles += 1;
                return;
            }
        if(!tile->isFlagged) {
            tile->isTouched = true;
            clickedTiles += 1;
        }
            if(tile->rightTile != nullptr) {
                clickSurroundingTiles(tile->rightTile, false);
            }
            if(tile->leftTile != nullptr) {
                clickSurroundingTiles(tile->leftTile, false);
            }
            if(tile->aboveTile != nullptr) {
                clickSurroundingTiles(tile->aboveTile, false);
            }
            if(tile->belowTile != nullptr) {
                clickSurroundingTiles(tile->belowTile, false);
            }
            if(tile->diagonalTopRight != nullptr) {
                clickSurroundingTiles(tile->diagonalTopRight, false);
            }
            if(tile->diagonalTopLeft != nullptr) {
                clickSurroundingTiles(tile->diagonalTopLeft, false);
            }
            if(tile->diagonalBottomLeft != nullptr) {
                clickSurroundingTiles(tile->diagonalBottomLeft, false);
            }
            if(tile->diagonalBottomRight != nullptr) {
                clickSurroundingTiles(tile->diagonalBottomRight, false);
            }
    }
};

void drawNumSprite(sf::RenderWindow& window, string texture, int rectLeft, int rectTop, int recWidth, int recHeight, int x, int y)
{
    Texture text;
    text.loadFromFile(texture);
    Sprite sprite(text);
    sprite.setTextureRect(sf::IntRect(rectLeft, rectTop, recWidth, recHeight));
    sprite.setPosition(x, y);

    window.draw(sprite);
}

Sprite drawSprite(sf::RenderWindow& window,string texture, int x, int y)
{
    Texture text;
    text.loadFromFile(texture);
    Sprite sprite(text);

    sprite.setPosition(x, y);

    window.draw(sprite);
    return sprite;
}

