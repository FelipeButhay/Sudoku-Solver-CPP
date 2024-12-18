#include "raylib.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <cstring>

#define BACKGORUND Color {40, 40, 40, 255}

bool mouseInRect(int posx, int posy, int width, int height){
    int mx = GetMouseX(), my = GetMouseY();
    return (mx > posx && mx < posx+width) && (my > posy && my < posy+height);
}

int strToInt(std::string str){
    int num = 0;

    for(int i = 0; i<(int)str.length(); i++){
        num += (str[i] - '0')*pow(10, str.length() - i - 1);
    }

    return num;
}

char* strToChar(std::string& str){
    char* cstr = new char[str.length() + 1];
    std::strcpy(cstr, str.c_str());
    return cstr;
}

std::string numToStr(float num){
    std::string str = std::to_string(num);
    return str;
}

class Sudoku{
    private:
        int sx, sy, sq, gap, sgap, bs;

        char visualBoard[9][9] = {' '};
        int saveBoard[9][9] = {0};
        int Board[9][9] = {0};
        int sqSelected;
        int sqIter;
        float msTime;
        std::string timeToSolveStr;

        // 0 no resuelto, 1 resuleto, -1 invalido.
        int solved;
        bool visualSolving;

    bool boardValid1Sq(){
        int sqx = sqIter%9 , sqy = sqIter/9;

        int number = Board[sqx][sqy];
        for (int i = 0; i<9; i++){ // linea en X
            if (i == sqx) continue;
            if (Board[i][sqy] == number){
                return false;
            } 
        }

        for (int j = 0; j<9; j++){ // linea en Y
            if (j == sqy) continue;
            if (Board[sqx][j] == number){
                return false;
            }
        }
        
        for (int i = (sqx/3)*3; i < (1 + sqx/3)*3; i++){ // el sq 3x3
            for (int j = (sqy/3)*3; j < (1 + sqy/3)*3; j++){
                if (i == sqx && j == sqy) continue;
                if (Board[i][j] == number){
                    return false;
                }
            }
        }

        return true;
    }

    void drawSolve(){
        BeginDrawing();

            ClearBackground(BACKGORUND);
    
            int posX, posY;
            for (int i = 0; i<9; i++){
                for (int j = 0; j<9; j++){
                    posX = gap + i*sq*1.1f + (i/3)*sgap;
                    posY = gap + j*sq*1.1f + (j/3)*sgap;
                    if (sqIter > i + j*9) DrawRectangle(posX, posY, sq, sq, (saveBoard[i][j] == 0) ? PURPLE : WHITE);
                    if (sqIter == i + j*9) DrawRectangle(posX, posY, sq, sq, BLUE);
                    if (sqIter < i + j*9) DrawRectangle(posX, posY, sq, sq, WHITE);
                    char number[2] = {visualBoard[i][j], '\0'};
                    DrawText(number, posX + (sq - MeasureText(number, sq*4/5))/2, posY + sq/7, sq*4/5, BACKGORUND);
                }
            }
    
            DrawRectangle(gap*2 + bs, gap, MeasureText("SOLVE", gap) + gap, gap*1.5f, DARKGRAY);
            DrawText("SOLVE", gap*2.5f + bs, gap*1.25f, gap, GRAY);

            DrawRectangle(gap*2 + bs, gap*3, MeasureText("Clear Board", gap) + gap, gap*1.5f, GRAY);
            DrawText("Clear Board", gap*2.5f + bs, gap*3.25f, gap, WHITE);

            DrawRectangle(gap*2 + bs, gap*5, gap, gap, GRAY);
            DrawRectangle(gap*2 + bs + gap*0.11f, gap*5 + gap*0.11f, gap*0.8f, gap*0.8f, BACKGORUND);
            DrawText("Visual Solving", gap*3.5f + bs, gap*5 + gap*0.11f, gap*0.8f, GRAY);

            if (visualSolving){
                DrawLineEx((Vector2){gap*2.2f + bs, gap*5.5f},
                           (Vector2){gap*2.53f + bs, gap*5.85f}, 
                           gap*0.12, PURPLE);
                DrawLineEx((Vector2){gap*2.5f + bs, gap*5.8f},
                           (Vector2){gap*3.1f + bs, gap*5.2f}, 
                           gap*0.12, PURPLE);
            }

        EndDrawing();

        WaitTime(0.01);
    }

    void solve() {
        if (sqIter == 81) return;

        int sqx = sqIter%9 , sqy = sqIter/9;
        if (Board[sqx][sqy] == 0){
            for(int i = 1; i<10; i++){
                this->Board[sqx][sqy] = i;
                if (boardValid1Sq()){
                    this->sqIter++;
                    solve();
                    if (sqIter == 81) return;
                }

                Board[sqx][sqy] = 0;
            }

            this->sqIter--;
            return;

        } else {
            this->sqIter++;
            solve();
            if (sqIter == 81) return;
            this->sqIter--;
        }
    }

    void visualSolve() {
        if (sqIter == 81) return;

        int sqx = sqIter%9 , sqy = sqIter/9;
        if (Board[sqx][sqy] == 0){
            for(int i = 1; i<10; i++){
                this->Board[sqx][sqy] = i;
                this->visualBoard[sqx][sqy] = 48 + i;
                drawSolve();
                if (boardValid1Sq()){
                    this->sqIter++;
                    visualSolve();
                    if (sqIter == 81) return;
                }

                this->Board[sqx][sqy] = 0;
                this->visualBoard[sqx][sqy] = ' ';
            }

            this->sqIter--;
            drawSolve();
            return;

        } else {
            this->sqIter++;
            drawSolve();
            visualSolve();
            if (sqIter == 81) return;
            this->sqIter--;
        }
    }

    public:
    Sudoku(){
        this->sx = 900;
        this->sy = 500;
        this->sq = sy*0.08f;
        this->sgap = sq/5;
        this->bs = sq*9.0f + sgap*2.0f + 8*sq*0.1f;
        this->gap = (sy - bs)/2;

        this->sqSelected = -1;
        this->sqIter = -1;

        this->solved = 0;
        this->msTime = 0;

        this->visualSolving = true;

        InitWindow(sx, sy, "Soudoku Solver");
        SetTargetFPS(60);

        std::ifstream file("test.txt");
        std::string line;
        std::getline(file, line);
        for (int i = 0; i<81; i++){
            this->visualBoard[i%9][i/9] = line[i] == '0' ? ' ' : line[i];
            this->Board[i%9][i/9] = line[i]-48;
        }
    }

    void loop(){
        if (IsMouseButtonPressed(0)){
            if (solved == 0){
                int posX, posY;
                this->sqSelected = -1;
                for (int i = 0; i<9; i++){
                    for (int j = 0; j<9; j++){
                        posX = gap + i*sq*1.1f + (i/3)*sgap;
                        posY = gap + j*sq*1.1f + (j/3)*sgap;
                        if (mouseInRect(posX, posY, sq, sq)){
                            this->sqSelected = i + j*9;
                        }
                    }
                }
            }
            
            if (mouseInRect(gap*2 + bs, gap, MeasureText("SOLVE", gap) + gap, gap*1.5f) && solved == 0){
                // check if valid (simple rules)
                bool valid = true;
                float start = GetTime() * 1000;
                for (this->sqIter = 0; sqIter<81; this->sqIter++){
                    if (valid){
                        valid = (Board[sqIter%9][sqIter/9] == 0) ? true : boardValid1Sq();
                    }
                    saveBoard[sqIter%9][sqIter/9] = Board[sqIter%9][sqIter/9];
                    if (visualSolving){
                        drawSolve();
                        WaitTime(0.05);
                    }
                }

                // Run the algorithm
                if (valid) {
                    this->sqIter = 0;
                    if (visualSolving){
                        SetTargetFPS(10000);
                        visualSolve();
                        SetTargetFPS(60);
                    } else {
                        solve();
                    }

                    this->solved = sqIter == -1 ? -1 : 1;
                } else {
                    this->solved = -1;
                }

                float end = GetTime() * 1000;

                for (int i = 0; i<81; i++){
                    visualBoard[i%9][i/9] = Board[i%9][i/9] == 0 ? ' ' : Board[i%9][i/9] + 48;
                }

                // post run
                this->msTime = end - start;
                this->timeToSolveStr = numToStr(msTime).erase(numToStr(msTime).find(".") + 4, numToStr(msTime).length()-1);
            }

            if (mouseInRect(gap*2 + bs, gap*5, gap, gap)){
                this->visualSolving = !visualSolving;
            }

            if (mouseInRect(gap*2 + bs, gap*3, MeasureText("Clear Board", gap) + gap, gap*1.5f)){
                this->solved = 0;
                this->msTime = 0;
                this->timeToSolveStr = "";

                for (int i = 0; i<9; i++){
                    for (int j = 0; j<9; j++){
                        this->Board[i][j] = 0;
                        this->visualBoard[i][j] = ' ';
                    }
                }
            }
        }
        if (sqSelected != -1 && solved == 0){
            for (int i = 0; i<10; i++){
                if (IsKeyPressed(48+i)){
                    this->visualBoard[sqSelected%9][sqSelected/9] = i == 0 ? ' ' : 48+i;
                    this->Board[sqSelected%9][sqSelected/9] = i;
                }
            }

            if(IsKeyPressed(KEY_UP) && sqSelected >= 9){
                sqSelected -= 9;
            }
            if(IsKeyPressed(KEY_DOWN) && sqSelected < 72){
                sqSelected += 9;
            }
            if(IsKeyPressed(KEY_LEFT) && sqSelected%9 != 0){
                sqSelected--;
            }
            if(IsKeyPressed(KEY_RIGHT) && sqSelected%9 != 8){
                sqSelected++;
            }
        }
    }

    void draw(){
        ClearBackground(BACKGORUND);

        int posX, posY;
        for (int i = 0; i<9; i++){
            for (int j = 0; j<9; j++){
                posX = gap + i*sq*1.1f + (i/3)*sgap;
                posY = gap + j*sq*1.1f + (j/3)*sgap;
                if (solved == 0 || saveBoard[i][j] != 0){
                    DrawRectangle(posX, posY, sq, sq, (i + j*9 == sqSelected && solved == 0) ? BLUE : WHITE);
                } else if (solved == 1){
                    DrawRectangle(posX, posY, sq, sq, GREEN);
                } else if (solved == -1){
                    DrawRectangle(posX, posY, sq, sq, RED);
                }
                char number[2] = {visualBoard[i][j], '\0'};
                DrawText(number, posX + (sq - MeasureText(number, sq*4/5))/2, posY + sq/7, sq*4/5, BACKGORUND);
            }
        }

        DrawRectangle(gap*2 + bs, gap, MeasureText("SOLVE", gap) + gap, gap*1.5f, 
                      mouseInRect(gap*2 + bs, gap, MeasureText("SOLVE", gap) + gap, gap*1.5f) ? DARKGRAY : GRAY);
        DrawText("SOLVE", gap*2.5f + bs, gap*1.25f, gap, 
                 mouseInRect(gap*2 + bs, gap, MeasureText("SOLVE", gap) + gap, gap*1.5f) ? GRAY : WHITE);

        DrawRectangle(gap*2 + bs, gap*3, MeasureText("Clear Board", gap) + gap, gap*1.5f, 
                      mouseInRect(gap*2 + bs, gap*3, MeasureText("Clear Board", gap) + gap, gap*1.5f) ? DARKGRAY : GRAY);
        DrawText("Clear Board", gap*2.5f + bs, gap*3.25f, gap, 
                 mouseInRect(gap*2 + bs, gap*3, MeasureText("Clear Board", gap) + gap, gap*1.5f) ? GRAY : WHITE);

        DrawRectangle(gap*2 + bs, gap*5, gap, gap, GRAY);
        DrawRectangle(gap*2 + bs + gap*0.11f, gap*5 + gap*0.11f, gap*0.8f, gap*0.8f, BACKGORUND);
        DrawText("Visual Solving", gap*3.5f + bs, gap*5 + gap*0.11f, gap*0.8f, GRAY);

        if (visualSolving){
            DrawLineEx((Vector2){gap*2.2f + bs, gap*5.5f},
                       (Vector2){gap*2.53f + bs, gap*5.85f}, 
                       gap*0.12, PURPLE);
            DrawLineEx((Vector2){gap*2.5f + bs, gap*5.8f},
                       (Vector2){gap*3.1f + bs, gap*5.2f}, 
                       gap*0.12, PURPLE);
        }

        if (solved == 1){
            DrawText("Solved", gap*2.0f + bs, sy-gap*3.5f , gap, GREEN);
            std::string textToSolveStr = "It took " + timeToSolveStr;
            char* textToSolveChar = strToChar(textToSolveStr);
            DrawText(textToSolveChar         , gap*2.0f + bs, sy-gap*14/6, gap*2/3, GRAY);
            DrawText("milliseconds to solve", gap*2.0f + bs, sy-gap*5/3 , gap*2/3, GRAY);
            delete[] textToSolveChar;
        }

        if (solved == -1){
            DrawText("Invalid", gap*2.0f + bs, sy-gap*3.5f , gap, RED);
            std::string textToSolveStr = "It took " + timeToSolveStr;
            char* textToSolveChar = strToChar(textToSolveStr);
            DrawText(textToSolveChar         , gap*2.0f + bs, sy-gap*14/6, gap*2/3, GRAY);
            DrawText("milliseconds to check", gap*2.0f + bs, sy-gap*5/3 , gap*2/3, GRAY);
            delete[] textToSolveChar;
        }
    }
};

int main(void) {
    Sudoku sudoku;

    while(!WindowShouldClose()){
        sudoku.loop();
        
        BeginDrawing();
            sudoku.draw();
        EndDrawing();
    }

    return 0;
}