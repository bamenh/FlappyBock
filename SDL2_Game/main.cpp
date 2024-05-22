#include "CommonFunc.h"

#define FRAME_DELAY 100

const int chim_width = 65;
const int chim_height = 50;

int main(int argc, char* args[])
{
    int ob_delay;
    SDL_Texture* character[3]; 
    int bg_width, bg_height, bg_width2, bg_height2, bg_width3, bg_height3;
    int char_x = 0;
    int char_y = 0;
    int jumpHeight = 100;
    int jumpTime = 195;
    int jumpTimerUp = 0;
    int jumpTimerDown = 0;
    int force = 1; // lực
    int currentFrame = 0;
    Uint32 lastFrameTime = 0;
    Uint32 lastTime = 0;
    
    int score = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        return -1;
    } 
    if (TTF_Init() == -1) {
        printf("Failed to initialize TTF: %s\n", TTF_GetError());
        return -1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }
    TTF_Font* font = TTF_OpenFont("font/ARCADE.ttf", 35);

    SDL_Window* window = SDL_CreateWindow("Flappy Bock by NBM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load menu texture
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/menu.png"));
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/gameOverMenu.png"));

    // Load background
    SDL_Texture* background1 = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/bgDay.png"));
    SDL_Texture* background2 = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/bgNight.png"));
    SDL_Texture* background3 = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/bgLighting.png"));
    SDL_QueryTexture(background1, NULL, NULL, &bg_width, &bg_height);
    SDL_QueryTexture(background2, NULL, NULL, &bg_width2, &bg_height2);
    SDL_QueryTexture(background3, NULL, NULL, &bg_width3, &bg_height3);

    // Load ground
    SDL_Texture* ground = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/ground.png"));
    int groundWidth, groundHeight;
    SDL_QueryTexture(ground, NULL, NULL, &groundWidth, &groundHeight);
   
    //mix
    Mix_Music* menuSound = Mix_LoadMUS("audio/8_bit_rainy_city_lofi.mp3");
    Mix_Music* ingameSound = Mix_LoadMUS("audio/wind.mp3");
    Mix_Chunk* hitSound = Mix_LoadWAV("audio/sfx_hit.wav");
    Mix_Chunk* clickSound = Mix_LoadWAV("audio/mouse_click.wav");
    Mix_Chunk* wingSound = Mix_LoadWAV("audio/sfx_wing.wav");
    Mix_Chunk* lightSound = Mix_LoadWAV("audio/lighting.wav");


    
    int groundX1 = 0;
    int groundX2 = groundWidth;
  
    character[0] = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/bird0.png"));
    character[1] = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/bird1.png"));
    character[2] = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/bird2.png"));


    SDL_Texture* ob1 = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/rock.png"));
    SDL_Texture* ob2 = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/rock.png"));

    int y_ob1 = 327;
    int y_ob2 = 168;
    SDL_Rect obstacleRect1 = { screenWidth+200, y_ob1, OBSTACLE_WIDTH, OBSTACLE_HEIGHT };
    SDL_Rect obstacleRect2 = { screenWidth+300, y_ob2, OBSTACLE_WIDTH, OBSTACLE_HEIGHT };

    double obstacleAngle = 0.0;
    GameState gameState = MENU;
    bool quit = false;
    SDL_Event event;
    
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (gameState == MENU) {
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        Mix_HaltMusic();
                        Mix_PlayChannel(-1, clickSound, 0);
                        gameState = PLAY;
                        score = 0;
                    }
                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        gameState = EXIT;
                    }
                }
            }
            else if (gameState == PLAY) {
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP) {
                    Mix_PlayChannel(-1, wingSound, 0);
                    if (char_y >= 300) {
                        jumpTimerUp = jumpTime;
                    }
                }
                else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN) {
                    Mix_PlayChannel(-1, wingSound, 0);
                    if (char_y <= 300) {
                        jumpTimerDown = jumpTime;
                    }
                }
            }
            else if (gameState == GAME_OVER) {
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        gameState = PLAY;
                        char_x = 0;
                        char_y = 300;
                        jumpTimerUp = 0;
                        jumpTimerDown = 0;
                        currentFrame = 0;
                        lastFrameTime = SDL_GetTicks();
                        groundX1 = 0;
                        groundX2 = groundWidth;
                        obstacleRect1.x = screenWidth;
                        obstacleRect2.x = screenWidth + 300;
                        obstacleAngle = 0.0;
                        score = 0;
                    }
                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                    else if (event.key.keysym.sym == SDLK_m) { 
                        gameState = MENU;
                        continue;
                    }
                }
                
            }
        }
        if (gameState == MENU) {
            renderMenu(renderer, menuTexture);
            if (!Mix_PlayingMusic()) {
                Mix_PlayMusic(menuSound, -1);
            }
        }
        else if (gameState == PLAY) {
            
            if (!Mix_PlayingMusic()) {
                Mix_PlayMusic(ingameSound, -1);
            }
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastFrameTime > FRAME_DELAY) {
                currentFrame = (currentFrame + 2) % 3;
                lastFrameTime = currentTime;
                score+=2;
            }
            if (isNight(score)) {
                ob_delay = 2;
            }
            else {
                ob_delay = 3;
                int jumpHeight = 200;
                int jumpTime = 200;
            }
            if (currentTime - lastTime > ob_delay) {
                lastTime = currentTime;
                obstacleRect1.x -= OBSTACLE_SPEED;
                obstacleRect2.x -= OBSTACLE_SPEED;
            }

            srand(time(NULL));
            int randomChoice = rand() % 2;  //random rock
            
            
            obstacleAngle += 2;
            if (obstacleAngle >= 360) {
                obstacleAngle -= 360;
            }
            if (randomChoice == 0) {
                if (obstacleRect1.x + obstacleRect1.w < 0) {
                    obstacleRect1.x = screenWidth + (rand() % 300 + 150);
                }
            }
            else {
                if (obstacleRect2.x + obstacleRect2.w < 0) {
                    obstacleRect2.x = screenWidth + (rand() % 300 + 100);
                }
            }
           
            
            if (abs(obstacleRect1.x - obstacleRect2.x) < 280) {
                if (obstacleRect1.x <= obstacleRect2.x) {
                    obstacleRect2.x += 200;
                }
                else {
                    obstacleRect1.x += 200;
                }
            }
            
            groundX1 -= GROUND_SPEED;
            groundX2 -= GROUND_SPEED;

            if (groundX1 + groundWidth < 0) {
                groundX1 = groundX2 + groundWidth;
            }
            if (groundX2 + groundWidth < 0) {
                groundX2 = groundX1 + groundWidth;
            }

            if (jumpTimerUp > 0) { 
                char_y -= jumpHeight / (jumpTime/2); 
                jumpTimerUp--;
            }
            else if (char_y < 300) {
                char_y += force; 
                if (char_y >= 300) { 
                    char_y = 300;
                }
            }
            if (jumpTimerDown > 0) { 
                char_y += jumpHeight / (jumpTime/2); 
                jumpTimerDown--; 
            }
            else if (char_y > 300) { 
                char_y -= force; 
                if (char_y <= 300) { 
                    char_y = 300;
                }
            }
            if (char_x <= 70) { // để lao ra từ trong góc
                char_x++;
            }
            SDL_RenderClear(renderer);

            //bg depend score
            SDL_Rect bg_rect;
            if (!isNight(score)) {
                bg_rect = { 0, 0, bg_width, bg_height };
                SDL_RenderCopy(renderer, background1, NULL, &bg_rect);
            }
            else{
                bg_rect = { 0, 0, bg_width2, bg_height2 };
                SDL_RenderCopy(renderer, background2, NULL, &bg_rect);
            }
            if (score % 60 == 0 && isNight(score)) {
                Mix_PlayChannel(-1, lightSound, 0);
                bg_rect = { 0, 0, bg_width3, bg_height3 };
                SDL_RenderCopy(renderer, background3, NULL, &bg_rect);
            }

            SDL_Rect groundRect = { 0, bg_height - groundHeight, bg_width, groundHeight };
            SDL_RenderCopy(renderer, ground, NULL, &groundRect);

            SDL_Rect groundRect1 = { groundX1, bg_height - groundHeight, groundWidth, groundHeight };
            SDL_Rect groundRect2 = { groundX2, bg_height - groundHeight, groundWidth, groundHeight };
            SDL_RenderCopy(renderer, ground, NULL, &groundRect1);
            SDL_RenderCopy(renderer, ground, NULL, &groundRect2);

            SDL_Rect chim_rect = { char_x, char_y, chim_width, chim_height };
            SDL_RenderCopy(renderer, character[currentFrame], NULL, &chim_rect);

            // Vẽ texture của vật cản
            //SDL_Rect obstacleRect = { obstacleX, 310, OBSTACLE_WIDTH, OBSTACLE_HEIGHT };
            //SDL_RenderCopy(renderer, ob, NULL, &obstacleRect);

            SDL_RenderCopyEx(renderer, ob1, NULL, &obstacleRect1, obstacleAngle, NULL, SDL_FLIP_NONE);
            SDL_RenderCopyEx(renderer, ob2, NULL, &obstacleRect2, obstacleAngle, NULL, SDL_FLIP_NONE);


            if (checkCollision(chim_rect, obstacleRect1) || checkCollision(chim_rect, obstacleRect2)) {
                Mix_PlayChannel(-1, hitSound, 0);
                gameState = GAME_OVER;
                continue;
            }
            // Render score
            renderScore(renderer, font, score);
            SDL_RenderPresent(renderer);
        }
        else if (gameState == EXIT) {
            quit = true;
        }
        else if (gameState == GAME_OVER) {
            Mix_HaltMusic();
            renderGamerOver(renderer, gameOverTexture, font, score);
            continue;
        }
    }
        SDL_DestroyTexture(background1);
        SDL_DestroyTexture(background2);
        SDL_DestroyTexture(menuTexture);
        SDL_DestroyTexture(character[0]);
        SDL_DestroyTexture(character[1]);
        SDL_DestroyTexture(character[2]);
        SDL_DestroyTexture(ob1);
        SDL_DestroyTexture(ob2);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        Mix_FreeMusic(menuSound);
        Mix_FreeMusic(ingameSound);
        Mix_FreeChunk(hitSound);
        Mix_FreeChunk(wingSound);
        Mix_FreeChunk(lightSound);
        Mix_FreeChunk(clickSound);
        Mix_CloseAudio();
        SDL_Quit();
        
        return 0;
}
