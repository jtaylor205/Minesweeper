#include <iostream>
#include <SFML/Graphics.hpp>
#include "minesweeper.h"
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
using namespace std;
using namespace sf;
using namespace chrono;

int main()
{
    int gameWidth;
    int gameHeight;
    int gameNumMines;
    float leaderBoardWidth;
    float leaderBoardHeight;
    bool playingGame = false;
    bool gameOver = false;
    bool isDebugging = false;
    bool isPause = false;
    bool isLeaderboard = false;
    bool wasPaused = false;
    bool gameWon = false;
    string name = "|";
    ifstream dimensions;
    ifstream leaderboardFile;
    ofstream leaderboardWrite;
    vector<Player> players;
    vector<Player> leaderboardPlayers;
    Player user;
    string width;
    string height;
    dimensions.open("files/board_config.cfg");
    leaderboardFile.open("files/leaderboard.txt");

    if (dimensions.is_open()){
        string numMines;
        getline(dimensions, width, '\n');
        getline(dimensions, height, '\n');
        getline(dimensions, numMines, '\n');
        gameWidth = stoi(width) * 32;
        gameHeight = (stoi(height) * 32) + 100;
        gameNumMines = stoi(numMines);
    }
    if(leaderboardFile.is_open()){
        for(int i = 0; i < 5; i++) {
            string name;
            string min;
            string sec;
            getline(leaderboardFile, min, ':');
            getline(leaderboardFile, sec, ',');
            getline(leaderboardFile, name, '\n');
            Player newPlayer = Player(name, min, sec);
            players.push_back(newPlayer);
        }
    }

    RenderWindow startWindow(sf::VideoMode(gameWidth, gameHeight), "Minesweeper", Style::Close);
    while (startWindow.isOpen()) {
        Event event;
            while (startWindow.pollEvent(event)) {
                if (event.type == sf::Event::TextEntered && name.size() < 11) {
                    if (isalpha(event.text.unicode)) {
                        name.pop_back();
                        name += event.text.unicode;
                    }
                    if (name != "|") {
                    name += "|";
                }
                        name[0] = toupper(name[0]);
                        for (int i = 1; i < name.size() - 1; i++) {
                            name[i] = tolower(name[i]);
                        }
                }
                if (Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
                    if (name.size() > 1) {
                        name.resize(name.size()-2);
                        name += "|";
                    }
                }

                if (Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                    if (name.size() > 2) {
                        string userName = "";
                        userName += toupper(name[0]);
                        for (int i = 1; i < name.size() - 1; i++) {
                            userName += tolower(name[i]);
                        }
                        name = userName;
                        startWindow.close();
                        playingGame = true;
                    }
                }
                if (event.type == Event::Closed)
                    startWindow.close();

            }
            startWindow.clear(sf::Color(0, 0, 255, 255));
            Writing welcomeWriting = Writing("WELCOME TO MINESWEEPER!", Color::White, 24, gameWidth / 2.0f,gameHeight / 2.0f - 150);
            Text welcomeText = welcomeWriting.getText();
            welcomeText.setStyle(Text::Bold | Text::Underlined);
            startWindow.draw(welcomeText);
            Writing nameWriting = Writing("Enter your name:", Color::White, 20, gameWidth / 2.0f,gameHeight / 2.0f - 75);
            Text enterNameText = nameWriting.getText();
            enterNameText.setStyle(Text::Bold);
            startWindow.draw(enterNameText);
            Writing UserNameWriting = Writing(name, Color::Yellow, 18, gameWidth / 2.0f, gameHeight / 2.0f - 45);
            Text UserName = UserNameWriting.getText();
            UserName.setStyle(Text::Bold);
            startWindow.draw(UserName);
            startWindow.display();

    }

    RenderWindow gameWindow(sf::VideoMode(gameWidth, gameHeight), "Minesweeper", Style::Close);
    Board gameBoard = Board((gameHeight-100)/32, gameWidth / 32, gameNumMines);
    gameBoard.drawBoard();
    if (playingGame){
        Sprite FaceButton;
        Sprite debugButton;
        Sprite playPause;
        Sprite leaderboard;
        string digitsTexture = "files/images/digits.png";
        auto start_time = steady_clock::now();
        int elapsed;
        int elapsed_seconds = 0;
        int total_time;
        bool newGame = false;
        while(gameWindow.isOpen()){
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
            Event event;
            gameWindow.clear(sf::Color::White);
            while (gameWindow.pollEvent(event)) {
                if (Mouse::isButtonPressed(Mouse::Right)) {
                    if(gameOver == false && isPause == false) {
                    for (int i = 0; i < gameBoard.tiles.size(); i++) {
                        Tile *current = gameBoard.tiles.at(i);
                        Vector2i position = Mouse::getPosition(gameWindow);
                        if (position.x >= current->xPos && position.x <= current->xPos + 32 &&
                            position.y >= current->yPos && position.y <= current->yPos + 32) {
                            if (current->isTouched == false) {
                                if (current->isFlagged) {
                                    gameBoard.remainingMines += 1;
                                } else {
                                    gameBoard.remainingMines -= 1;
                                }
                                current->flag();
                                break;
                            }
                        }
                        }
                    }
                }
                if (Mouse::isButtonPressed(Mouse::Left)) {
                    Vector2i position = Mouse::getPosition(gameWindow);
                    if(!gameOver && !isPause) {
                        for (int i = 0; i < gameBoard.tiles.size(); i++) {
                            Tile *current = gameBoard.tiles.at(i);
                            if (position.x >= current->xPos && position.x <= current->xPos + 32 &&
                                position.y >= current->yPos && position.y <= current->yPos + 32 && !current->isFlagged) {
                                if (!current->isTouched) {
                                    current->isTouched = true;
                                    if (!current->isMine){
                                    gameBoard.clickSurroundingTiles(current, true);
                                        }
                                    break;
                                }
                                break;
                            }
                        }
                        FloatRect debugBounds = debugButton.getGlobalBounds();
                        if (debugBounds.contains(static_cast<sf::Vector2f> (position))) {
                            isDebugging = !isDebugging;
                        }
                    }
                    FloatRect faceBounds = FaceButton.getGlobalBounds();
                    if (faceBounds.contains(static_cast<sf::Vector2f> (position))) {
                        gameBoard.drawBoard();
                        gameOver = false;
                        gameWon = false;
                        gameBoard.clickedTiles = 0;
                        gameBoard.remainingMines = gameBoard.numMines;
                        newGame = true;

                    }
                    FloatRect pauseBounds = playPause.getGlobalBounds();
                    if (pauseBounds.contains(static_cast<sf::Vector2f> (position))) {
                        if(isPause){
                            start_time = steady_clock::now();
                        } else {
                            elapsed_seconds += static_cast<int>(elapsed_time.count());
                            start_time = steady_clock::now();
                        }
                        isPause = !isPause;

                    }
                    FloatRect leaderboardBounds = leaderboard.getGlobalBounds();
                    if (leaderboardBounds.contains(static_cast<sf::Vector2f> (position)) && !isLeaderboard) {
                        start_time = steady_clock::now();
                        if(isPause){
                            wasPaused = true;
                        } else {
                            wasPaused = false;
                        }
                        isPause = true;
                        isLeaderboard = true;
                    }
                }
                if (event.type == Event::Closed)
                    gameWindow.close();
            }

            for (int i = 0; i < gameBoard.tiles.size(); i++) {
                Tile *current = gameBoard.tiles.at(i);
                if(isPause) {
                    drawSprite(gameWindow, "files/images/tile_revealed.png", current->xPos, current->yPos);
                    start_time = steady_clock::now();
                } else {
                if (!current->isTouched) {
                    drawSprite(gameWindow, "files/images/tile_hidden.png", current->xPos, current->yPos);
                    if (current->isFlagged) {
                        drawSprite(gameWindow, "files/images/flag.png", current->xPos, current->yPos);
                    }
                }
                if (current->isTouched) {
                    if (current->isMine) {
                        gameOver = true;
                        drawSprite(gameWindow, "files/images/tile_revealed.png", current->xPos, current->yPos);
                        drawSprite(gameWindow, "files/images/mine.png", current->xPos, current->yPos);
                    } else {
                        drawSprite(gameWindow, "files/images/tile_revealed.png", current->xPos, current->yPos);
                        if (current->surroundingMines == 1) {
                            drawSprite(gameWindow, "files/images/number_1.png", current->xPos, current->yPos);
                        } else if (current->surroundingMines == 2) {
                            drawSprite(gameWindow, "files/images/number_2.png", current->xPos, current->yPos);
                        } else if (current->surroundingMines == 3) {
                            drawSprite(gameWindow, "files/images/number_3.png", current->xPos, current->yPos);
                        } else if (current->surroundingMines == 4) {
                            drawSprite(gameWindow, "files/images/number_4.png", current->xPos, current->yPos);
                        } else if (current->surroundingMines == 5) {
                            drawSprite(gameWindow, "files/images/number_5.png", current->xPos, current->yPos);
                        } else if (current->surroundingMines == 6) {
                            drawSprite(gameWindow, "files/images/number_6.png", current->xPos, current->yPos);
                        } else if (current->surroundingMines == 7) {
                            drawSprite(gameWindow, "files/images/number_7.png", current->xPos, current->yPos);
                        } else if (current->surroundingMines == 8) {
                            drawSprite(gameWindow, "files/images/number_8.png", current->xPos, current->yPos);
                        } else if (current->surroundingMines == 0 & !current->isMine) {
                            drawSprite(gameWindow, "files/images/tile_revealed.png", current->xPos, current->yPos);
                        }
                    }
                }
                if(gameWon){
                    leaderboardWrite.open("files/leaderboard.txt");
                    gameBoard.remainingMines = 0;
                    if (current->isMine) {
                        drawSprite(gameWindow, "files/images/flag.png", current->xPos, current->yPos);
                    }
                    start_time = steady_clock::now();
                    bool leaderboardTime = true;
                    int position = 1;
                    string userMin = to_string(total_time / 60 / 10) + to_string(total_time / 60 % 10);
                    string userSec = to_string(total_time % 60 / 10) + to_string((total_time % 60) % 10);
                    user.name = name;
                    user.minutes = userMin;
                    user.seconds = userSec;
                    for(int i = 0; i < 5; i ++){
                        if(stoi(user.minutes) < stoi(players.at(i).minutes)){
                            leaderboardTime = true;
                            position = i;
                            break;
                        }if(stoi(user.minutes) == stoi(players.at(i).minutes)){
                            if(stoi(user.seconds) < stoi(players.at(i).seconds)) {
                                leaderboardTime = true;
                                position = i;
                                break;
                            }
                        }
                    }
                    if(leaderboardTime){
                        for(int i = 0; i < position; i ++){
                            leaderboardPlayers.push_back(players.at(i));
                        }
                        leaderboardPlayers.push_back(user);
                        if(position < 5) {
                            for (int i = position; i < 5; i++) {
                                leaderboardPlayers.push_back(players.at(i));
                            }
                        }
                        for(int i = 0; i < 5; i ++){
                            players.at(i) = leaderboardPlayers.at(i);
                        }

                        if(leaderboardWrite.is_open()){
                            for(int i = 0; i < 5; i ++) {
                                leaderboardWrite << players.at(i).minutes << ":" << players.at(i).seconds <<","<<players.at(i).name << endl;
                            }
                        }
                    }

                    isLeaderboard = true;
                }else if (isDebugging || gameOver) {
                    if (current->isMine) {
                        drawSprite(gameWindow, "files/images/mine.png", current->xPos, current->yPos);
                    }
                }
                }
            }
            if(!gameBoard.checkWin()) {
                if (!gameOver) {
                    FaceButton = drawSprite(gameWindow, "files/images/face_happy.png", ((stoi(width) / 2.0) * 32) - 32, 32 *(stoi(height)+0.5f));
                } else {
                    FaceButton = drawSprite(gameWindow, "files/images/face_lose.png", ((stoi(width) / 2.0) * 32) - 32, 32 *(stoi(height)+0.5f));
                }
            } else {
                FaceButton = drawSprite(gameWindow, "files/images/face_win.png", ((stoi(width) / 2.0) * 32) - 32, 32 *(stoi(height)+0.5f));
                gameWon = true;
                gameOver = true;
            }

            debugButton = drawSprite(gameWindow, "files/images/debug.png", (stoi(width) * 32) -304, 32 * (stoi(height)+0.5f));
            leaderboard = drawSprite(gameWindow, "files/images/leaderboard.png", (stoi(width) * 32) -176, 32 * (stoi(height)+0.5f));


            elapsed = static_cast<int>(elapsed_time.count());
            
            total_time = elapsed_seconds + elapsed;
            if(newGame){
                start_time = steady_clock::now();
                total_time = 0;
                elapsed_seconds = 0;
                elapsed = 0;
                newGame = false;
            }


            if (gameBoard.remainingMines < 0){
                drawNumSprite(gameWindow, digitsTexture, 10* 21, 0, 21, 32, 12, 32 * (stoi(height) + 0.5f) + 16);
            }
            int remainingHundreds = abs(gameBoard.remainingMines) / 100;
            int remainingTens = abs(gameBoard.remainingMines) / 10;
            int remainingOnes = abs(gameBoard.remainingMines) % 10;
            drawNumSprite(gameWindow, digitsTexture, remainingHundreds * 21, 0, 21, 32, 33 , 32 * (stoi(height) + 0.5f) + 16);
            drawNumSprite(gameWindow, digitsTexture, remainingTens * 21, 0, 21, 32, 33 + (21), 32 * (stoi(height) + 0.5f) + 16);
            drawNumSprite(gameWindow, digitsTexture, remainingOnes * 21, 0, 21, 32, 33 + (21*2), 32 * (stoi(height) + 0.5f) + 16);

            int minTens = (total_time / 60) / 10;
            int minOnes = (total_time / 60) % 10;
            int tensSec = (total_time % 60) / 10;
            int onesSec = (total_time % 60) % 10;
            drawNumSprite(gameWindow, digitsTexture, minTens * 21, 0, 21, 32, (stoi(width) * 32) - 97, 32 * (stoi(height) + 0.5f) + 16);
            drawNumSprite(gameWindow, digitsTexture, minOnes * 21, 0, 21, 32, (stoi(width) * 32) - 97 + 21, 32 * (stoi(height) + 0.5f) + 16);


            drawNumSprite(gameWindow, digitsTexture, tensSec * 21, 0, 21, 32, (stoi(width) * 32) - 54, 32 * (stoi(height) + 0.5f) + 16);

            drawNumSprite(gameWindow, digitsTexture, onesSec * 21, 0, 21, 32, (stoi(width) * 32) - 54 + 21, 32 * (stoi(height) + 0.5f) + 16);

            if(!isPause) {
                playPause = drawSprite(gameWindow, "files/images/pause.png", (stoi(width) * 32) -240, 32 * (stoi(height)+0.5f));
            } else {
                playPause = drawSprite(gameWindow, "files/images/play.png", (stoi(width) * 32) -240, 32 * (stoi(height)+0.5f));
            }

            if (isLeaderboard){
                gameWindow.display();
                leaderBoardWidth = 16 * stoi(width);
                leaderBoardHeight = (stoi(height) * 16) + 50;
                RenderWindow leaderboardWindow(sf::VideoMode(leaderBoardWidth, (stoi(height) * 16) + 50), "Minesweeper", Style::Close);
                while (leaderboardWindow.isOpen()){
                    start_time = steady_clock::now();
                    Event event;
                    leaderboardWindow.clear(sf::Color(0, 0, 255, 255));
                    while (leaderboardWindow.pollEvent(event)) {
                        if (event.type == Event::Closed){
                            leaderboardWindow.close();
                            isLeaderboard = false;
                            elapsed_seconds += static_cast<int>(elapsed_time.count());
                            if(wasPaused){
                                isPause = true;
                            } else {
                                isPause = false;
                            }if(gameWon){
                                gameBoard.drawBoard();
                                gameWon = false;
                                gameOver = false;
                            }
                        }

                    }

                    Writing leaderboardWriting = Writing("LEADERBOARD", Color::White, 20, (leaderBoardWidth / 2.0f),
                                                     leaderBoardHeight / 2.0f - 120);
                    Text welcomeText = leaderboardWriting.getText();
                    welcomeText.setStyle(Text::Bold | Text::Underlined);
                    leaderboardWindow.draw(welcomeText);


                    string leaderBoardNames;

                    for(int i = 0; i< 5; i++){
                        leaderBoardNames += to_string(i+1);
                        leaderBoardNames += ".     ";
                        leaderBoardNames += players.at(i).minutes;
                        leaderBoardNames += ":";
                        leaderBoardNames += players.at(i).seconds;
                        leaderBoardNames += "     ";
                        leaderBoardNames += players.at(i).name;
                        if(players.at(i).name == user.name){
                        leaderBoardNames+= "*";
                        }
                        leaderBoardNames += "\n\n";
                    }

                    Writing leaderNamesWriting = Writing(leaderBoardNames, Color::White, 20, (leaderBoardWidth / 2.0f),leaderBoardHeight / 2.0f + 20);
                    Text namesText = leaderNamesWriting.getText();
                    namesText.setStyle(Text::Bold);
                    leaderboardWindow.draw(namesText);
                    leaderboardWindow.display();
                }

            }
            gameWindow.display();

        }


    }



    return 0; }
