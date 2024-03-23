#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;
Color black={0,0,0,255};
Color dg={43,51,24,255};

int size=30;
int count=25;
double lastupdate=0;
int offset=75;

bool elementindeque(Vector2 element ,deque<Vector2> deque){
    for(unsigned int i=0;i<deque.size();i++){
        if(Vector2Equals(deque[i],element)){
            return true;
        }
    }
    return false;
}

bool eventtrig(double interval){
    double currenttime=GetTime();
    if(currenttime - lastupdate>= interval){
        lastupdate=currenttime;
        return true;
    }
    return false;
}

class Snake{
    public:
    deque<Vector2> body={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
    Vector2 direction={1,0};
    bool addsegment=false;

    void draw(){
        for(unsigned int i=0;i<body.size();i++){
            float x=body[i].x;
            float y=body[i].y;
            Rectangle segment=Rectangle{offset+x*size ,offset+y*size ,(float)size ,(float)size};
            DrawRectangleRounded(segment ,0.5 ,6 ,dg);
        }
    }

    void update(){
        body.push_front(Vector2Add(body[0] ,direction));
        if(addsegment==true){
            addsegment=false;
        }else{
        body.pop_back();
        }
    }

    void reset(){
        body={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        direction={1,0};
    }
};

class Food{
public:
    Vector2 position;
    Texture2D texture;
    Food(deque<Vector2> snakebody){
        Image image= LoadImage("image/food.png");
        texture=LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandompos(snakebody);
    }

    ~Food(){
        UnloadTexture(texture);
    }

    void draw(){
        DrawTexture(texture, offset+position.x*size , offset+position.y*size, WHITE );
    }

    Vector2 Generate(){
        float x = GetRandomValue(0,count-1);
        float y = GetRandomValue(0,count -1);
        return Vector2{x,y};
    }

    Vector2 GenerateRandompos(deque<Vector2> snakebody){

        Vector2 position=Generate();
        while (elementindeque(position,snakebody)){
                position=Generate();
        }
        return position;
    }
};

class Game{
    public:
    Snake snake=Snake();
    Food food=Food(snake.body);
    bool running=true;
    int score=0;
    Sound eat;
    Sound wall;

    Game(){
        InitAudioDevice();
        eat=LoadSound("sound/eat.mp3");
        wall=LoadSound("sound/wall.mp3");
    }
    
    ~Game(){
        UnloadSound(eat);
        UnloadSound(wall);
        CloseAudioDevice();
    }

    void draw(){
        food.draw();
        snake.draw();
    }

    void update(){
        if(running){
            snake.update();
            collision();
            collisionedge();
            collisiontail();
        }
    }

    void collision(){
        if(Vector2Equals(snake.body[0],food.position)){
            food.position=food.GenerateRandompos(snake.body);
            snake.addsegment=true;
            score=score+5;
            PlaySound(eat);
        }
    }

    void collisionedge(){
        if(snake.body[0].x==count||snake.body[0].x==-1||snake.body[0].y==count||snake.body[0].y==-1){
            gameover();
            PlaySound(wall);
        }
    }

    void gameover(){
        snake.reset();
        food.position=food.GenerateRandompos(snake.body);
        running=false;
        score=0;
    }

    void collisiontail(){
        deque<Vector2> headless=snake.body;
        headless.pop_front();
        if(elementindeque(snake.body[0],headless)){
            gameover();
            PlaySound(wall);
        }
    }
};


int main () {
    InitWindow(2*offset+size*count,2*offset+size*count,"Retro Snake");
    Game game=Game();
    SetTargetFPS(60);

    while (WindowShouldClose()==false)
    {
        BeginDrawing();
        if(eventtrig(0.2)){
            game.update();
        }

        if(IsKeyPressed(KEY_W) && game.snake.direction.y!=1){
            game.snake.direction={0,-1};
            game.running=true;
        }
        if(IsKeyPressed(KEY_S) && game.snake.direction.y!=-1){
            game.snake.direction={0,1};
            game.running=true;
        }
        if(IsKeyPressed(KEY_A) && game.snake.direction.x!=1){
            game.snake.direction={-1,0};
            game.running=true;
        }
        if(IsKeyPressed(KEY_D) && game.snake.direction.x!=-1){
            game.snake.direction={1,0};
            game.running=true;
        }
        
        ClearBackground(black);
        DrawRectangleLinesEx(Rectangle{(float)offset-5 ,(float)offset-5 ,(float)size*count+10 ,(float)size*count+10} ,5 ,dg);
        DrawText("Retro Snake", offset-5, 20, 40, dg);
        DrawText(TextFormat("%i",game.score), offset-5, offset+size*count+10, 40, dg);        
        game.draw();
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}