#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>

using namespace std;
using namespace sf;

const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid

// The following exist purely for readability.
const int x = 0;
const int y = 1;
const int exists = 2;
int score=0;
/////////////////////////////////////////////////////////////////////////////
// Function Declarations                                                   //
/////////////////////////////////////////////////////////////////////////////
void movePlayer(float player[]);
void drawPlayer(RenderWindow &window, float player[], Sprite &playerSprite);
void moveBullet(float bullet[], Clock &bulletClock, float player[]);
void drawBullet(RenderWindow &window, float bullet[], Sprite &bulletSprite);
void drawMushrooms(RenderWindow &window, Sprite mushroomSprites[], float mushrooms[][3]);
void moveCentipede(float &centipedeX, float &centipedeY, Sprite centipede[], bool &move, int centipedeSegments, float centipedePositions[][2]);
void drawCentipede(RenderWindow &window, float centipedeX, float centipedeY, Sprite centipede[], bool centipedeExists);
void collisionMushroom(float bullet[], float mushrooms[][3], int collisions[], Music& destroySound);
void collisionCentipede(float centipede[][2], bool &move, float &centipedeX, float &centipedeY, float mushrooms[][3]);
void splittingCentipede(float bullet[], float &centipedeX, float &centipedeY, bool &move, bool &centipedeExists, float centipedePositions[][2], float mushrooms[][3], Sprite centipede[]);
bool checkPlayerCentipedeCollision(float player[], float centipedeX, float centipedeY);
void showWin(RenderWindow& window);
void showLose(RenderWindow& window);


int main() {
    
    bool playerWon = false;// Initializing boolean of player Winning
    srand(time(0));
    int numMushrooms = rand() % 11 + 20; // Generating random numbe of mushrooms between 20-30
    float mushrooms[30][3] = {};
    int collisions[30] = {0}; // Initialize collision counts to zero
    const int centipedeSegments = 12;
    
    float centipedePositions[centipedeSegments][2];
    for (int i = 0; i < numMushrooms; i++) // Placing mushrooms on random locations
    {
        float mushroomX, mushroomY;
        bool isDestroyed = false;

        do
        {
            mushroomX = static_cast<float>(rand() % gameColumns) * boxPixelsX;
            mushroomY = static_cast<float>(rand() % (gameRows - 5)) * boxPixelsY;

            // Check if the mushroom is at the position of a destroyed mushroom
            for (int j = 0; j < i; ++j)
            {
                if (!mushrooms[j][exists] && mushrooms[j][x] == mushroomX && mushrooms[j][y] == mushroomY)
                {
                    isDestroyed = true;
                    break;
                }
            }
        } while (mushroomY < 0 || mushroomY > 810 || isDestroyed);

        mushrooms[i][x] = mushroomX;
        mushrooms[i][y] = mushroomY;
        mushrooms[i][exists] = true; // Set exists to true
    }

    RenderWindow window(VideoMode(resolutionX, resolutionY), "Centipede", Style::Close | Style::Titlebar);

    window.setSize(Vector2u(640, 640));
    window.setPosition(Vector2i(100, 0));

    Music bgMusic;
    bgMusic.openFromFile("Music/field_of_hopes.ogg");
    bgMusic.play();
    bgMusic.setVolume(50);


    Music destroySound; //sound effect of mushroom being destroyed
    if (!destroySound.openFromFile("Sound Effects/kill.wav")) {
        cerr << "Failed to load sound file!" << endl;
        return -1;
    }
    destroySound.setVolume(50);  // Adjust the volume as needed



    Texture backgroundTexture;
    Sprite backgroundSprite;
    backgroundTexture.loadFromFile("Textures/bg.png");
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setColor(Color(255, 255, 255, 255 * 0.175));

    Texture mushroomTexture;
    mushroomTexture.loadFromFile("Textures/mushroom.png");

    float player[2] = {};
    player[x] = (gameColumns / 2) * boxPixelsX;
    player[y] = (gameRows - 3) * boxPixelsY; // setting player position on centre of playing area
    Texture playerTexture;
    Sprite playerSprite;
    playerTexture.loadFromFile("Textures/player.png");
    playerSprite.setTexture(playerTexture);
    playerSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

    float bullet[3] = {};
    bullet[x] = player[x];
    bullet[y] = player[y] - boxPixelsY;
    bullet[exists] = false;
    Clock bulletClock;
    Texture bulletTexture;
    Sprite bulletSprite;
    bulletTexture.loadFromFile("Textures/bullet.png");
    bulletSprite.setTexture(bulletTexture);
    bulletSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

    Sprite mushroomSprites[30];
    for (int i = 0; i < 30; i++)
    {
        mushroomSprites[i].setTexture(mushroomTexture);
        mushroomSprites[i].setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
    }

    Texture centipedeTexture;
    centipedeTexture.loadFromFile("Textures/c_head_left_walk.png");

    // Create an array of sprites for each segment of the centipede

Sprite centipede[centipedeSegments];

// Initialize the centipede segments
for (int i = 0; i < centipedeSegments; i++) 
{
    centipede[i].setTexture(centipedeTexture);
    centipede[i].setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
}
float centipedeX = 960;  
float centipedeY = 0;    
// Initialize positions in the centipedePositions array
for (int i = 0; i < centipedeSegments; i++) 
{
    centipede[i].setPosition(centipedeX + i * boxPixelsX, centipedeY);
    centipedePositions[i][x] = centipedeX + i * boxPixelsX;
    centipedePositions[i][y] = centipedeY;
}

    bool move = true;
    bool centipedeExists = true;

    while (window.isOpen() && !playerWon) 
    {
    Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == Event::Closed) 
        {
            window.close();
        } else if (e.key.code == Keyboard::Space) 
        {
            moveBullet(bullet, bulletClock, player);
        }
    }

    movePlayer(player);

    window.clear();
    window.draw(backgroundSprite);
    drawPlayer(window, player, playerSprite);

    // Move the bullet and check for collision with mushrooms
    if (bullet[exists]) 
    {
        moveBullet(bullet, bulletClock, player);
        drawBullet(window, bullet, bulletSprite);

        // Pass bulletHits to the collisionMushroom function
        collisionMushroom(bullet, mushrooms, collisions,  destroySound);
        splittingCentipede(bullet, centipedeX, centipedeY, move, centipedeExists, centipedePositions, mushrooms, centipede);

       if (!centipedeExists) //if head of centipede is destroyed, player wins
        {
            playerWon = true;
            showWin(window);
        }
    }

    drawMushrooms(window, mushroomSprites, mushrooms);
    drawCentipede(window, centipedeX, centipedeY, centipede, centipedeExists);
    window.display();

    moveCentipede(centipedeX, centipedeY, centipede, move, centipedeSegments, centipedePositions);
    collisionCentipede(centipedePositions, move, centipedeX, centipedeY, mushrooms);

    if (checkPlayerCentipedeCollision(player, centipedeX, centipedeY)) 
    {
        showLose(window);
        playerWon=false;
        window.close();
    }
}

    return 0;
}  

/////////////////////////////////////////////////////////////////////////////
// Function Definitions                                                    //
/////////////////////////////////////////////////////////////////////////////

// Assigning Keybinds to move player in specific boundaries of player area
void movePlayer(float player[]) 
{
    if ((Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) && player[x] > 0)
    {
        player[x] -= 1;
    }
    if ((Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) && player[x] < 928)
    {
        player[x] += 1;
    }
    if ((Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) && player[y] > 810)
    {
        player[y] -= 1;
    }
    if ((Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) && player[y] < 928)
    {
        player[y] += 1;
    }
}

// Showing the player on screen on specific coordinates when game starts
void drawPlayer(RenderWindow &window, float player[], Sprite &playerSprite) 
{
    playerSprite.setPosition(player[x], player[y]);
    window.draw(playerSprite);
}

// Shooting bullet from player position, needed to set condtions when bullet couldnt be fired and when it should stop appearing
void moveBullet(float bullet[], Clock &bulletClock, float player[])
{
    if (bulletClock.getElapsedTime().asMilliseconds() < 20)
    {
        return;
    }

    bulletClock.restart();

    if (!bullet[exists])
    {
        bullet[x] = player[x];
        bullet[y] = player[y] - boxPixelsY;
        bullet[exists] = true;
    }
    else
    {
        bullet[y] -= 20;

        if (bullet[y] + boxPixelsY < 0)
        {
            bullet[exists] = false;
        }
    }
}

// Showing the bullet on screen
void drawBullet(RenderWindow &window, float bullet[], Sprite &bulletSprite)
{
    bulletSprite.setPosition(bullet[0], bullet[1]);
    window.draw(bulletSprite);
}

// Creating 20-30 mushrooms on random locations needed
void drawMushrooms(RenderWindow &window, Sprite mushroomSprites[], float mushrooms[][3])
{
    for (int i = 0; i < 30; i++)
    {
        if (mushrooms[i][exists])
        {
            mushroomSprites[i].setPosition(mushrooms[i][x], mushrooms[i][y]);
            window.draw(mushroomSprites[i]);
        }
    }
}

// Moving to right and reversing movement and going down upon collision
void moveCentipede(float &centipedeX, float &centipedeY, Sprite centipede[], bool &move, int centipedeSegments, float centipedePositions[][2]) 
{
    
    // Update centipede position based on movement factor
    if (move) 
    {
        centipedeX -= 0.6; // Move left
    } 
    else 
    {
        centipedeX += 0.6; // Move right
    }

    // Check if the centipede is at the first or last column to reverse movement
    if (centipedeX < -boxPixelsX || centipedeX > resolutionX) 
    {
        move = !move;
        centipedeY += boxPixelsY;
    }

    // Update the positions in the centipedePositions array
    for (int i = 0; i < centipedeSegments; i++) 
    {
        if (centipedeX < -boxPixelsX) {
            centipedePositions[i][x] = resolutionX + i * boxPixelsX;
        } 
        else 
        {
            centipedePositions[i][x] = centipedeX + i * boxPixelsX;
        }
        centipedePositions[i][y] = centipedeY;

        // Use centipedePositions array for positioning
        centipede[i].setPosition(centipedePositions[i][x], centipedePositions[i][y]);
    }
}

// Drawing 12 centipede segments and each copying the movement in a flow
void drawCentipede(RenderWindow &window, float centipedeX, float centipedeY, Sprite centipede[], bool centipedeExists)
{
    for (int i = 0; i < 12; i++)
    {
        if (centipedeExists)
        {
            float segmentX = centipedeX + i * boxPixelsX;
            float segmentY = centipedeY;
            centipede[i].setPosition(segmentX, segmentY);
            window.draw(centipede[i]);
        }
    }
}

// Comparing and looking for bullet collisions with head or body of centipede
void splittingCentipede(float bullet[], float &centipedeX, float &centipedeY, bool &move, bool &centipedeExists, float centipedePositions[][2], float mushrooms[][3], Sprite centipede[])
{
        // Check if the bullet exists before checking for collisions
    if (bullet[exists] && centipedeExists) 
    {
        // Get the position of the bullet
        float bulletX = bullet[x];
        float bulletY = bullet[y];

        // Check if the bullet hit the head (1st segment) of the centipede
        float headX = centipedePositions[0][x];
        float headY = centipedePositions[0][y];

        if ((bulletX + boxPixelsX > headX) && (bulletX < headX + boxPixelsX) && (bulletY + boxPixelsY > headY) && (bulletY < headY + boxPixelsY)) 
            {
            // Reset the movement direction if needed
            move = !move;

            // Remove the centipede
            centipedeExists = false;
            
            // Clear the centipede segments from the graphical array
            for (int i = 0; i < 12; i++) 
            {
                centipede[i].setPosition(0, 0);
            }

            // Clear the logical array
            for (int i = 0; i < 12; i++) 
            {
                centipedePositions[i][x] = 0;
                centipedePositions[i][y] = 0;
            }
            score+=20;
            }
            else
            {
                score+=10;
            }
    }
}

        

// Checking collision of bullet and mushroom
void collisionMushroom(float bullet[], float mushrooms[][3], int collisions[], Music& destroySound)
{
    // Check if the bullet exists before checking for collisions
    if (bullet[exists]) {
        // Iterate over mushrooms
        for (int i = 0; i < 30; i++) 
        {
            if (mushrooms[i][exists]) 
            {
                // Get the positions of the mushroom and bullet
                float mushroomX = mushrooms[i][x];
                float mushroomY = mushrooms[i][y];
                float bulletX = bullet[x];
                float bulletY = bullet[y];

                // Check if the bullet is within the bounding box of the mushroom
                if ((bulletX + boxPixelsX > mushroomX) && (bulletX < mushroomX + boxPixelsX) && (bulletY + boxPixelsY > mushroomY) && (bulletY < mushroomY + boxPixelsY)) 
                    {
                    
                    collisions[i]++;

                    if (collisions[i] == 1) 
                    {  // First hit, set mushroom state to damaged
                        mushrooms[i][exists] = true;  // Set to 1 for damaged state
                        bullet[exists] = false;  // Destroy the bullet
                    } 
                    else if (collisions[i] == 2) 
                    {  // Second hit, destroy the mushroom
                        mushrooms[i][exists] = false;
                        bullet[exists]=false; 
                        destroySound.play();
                        score=+1;
                    }
                }
            }
        }
    }
}
// Check for collision with mushrooms
void collisionCentipede(float centipede[][2], bool &move, float &centipedeX, float &centipedeY, float mushrooms[][3]) 
{
    
    for (int i = 0; i < 30; i++) 
    {
        if (mushrooms[i][exists]) 
        {
            float mushroomX = mushrooms[i][x];
            float mushroomY = mushrooms[i][y];

            for (int j = 0; j < 12; ++j) 
            {
                float segmentX = centipede[j][x];
                float segmentY = centipede[j][y];

                if (segmentX < mushroomX + boxPixelsX && segmentX + boxPixelsX > mushroomX && segmentY < mushroomY + boxPixelsY && segmentY + boxPixelsY > mushroomY) 
                {
                    // Collision detected, adjust centipede position
                    centipedeY += boxPixelsY;
                    move = !move; // Reverse the movement direction
                }
            }
        }
    }
}

// Checking if Player and Centipede collide
bool checkPlayerCentipedeCollision(float player[], float centipedeX, float centipedeY) 
{
    // Get the position of the player
    float playerX = player[x];
    float playerY = player[y];

    // Check for collision
    if (playerX + boxPixelsX > centipedeX && playerX < centipedeX + 12 * boxPixelsX && playerY + boxPixelsY > centipedeY && playerY < centipedeY + boxPixelsY) 
        {
        // Collision detected, game over
        return true;
        }

    return false;
}
//displaying Win Screen
void showWin(RenderWindow& window) 
{
    
    window.clear(Color::White);


    Font font;
    if (!font.loadFromFile("Textures/font.ttf")) 
    {
        // Handle font loading error
        cerr << "Error loading font!" << endl;
        return;
    }

    
    Text winText("YOU WON!\n Score: " + to_string(score), font, 75);
    winText.setPosition(200, 350);
    winText.setFillColor(Color::Black);

    
    window.draw(winText);
    window.display();

    // Wait for a 
    sleep(seconds(2));

    
    window.close();
}
//Displaying Lose Screen
void showLose(RenderWindow& window) 
{
    
    window.clear(Color::Black);

    
    Font font;
    if (!font.loadFromFile("Textures/font.ttf")) 
    {
        // Handle font loading error
        cerr << "Error loading font!" << endl;
        return;
    }

    
    Text loseText("YOU LOST!\n Score: " + to_string(score), font, 75);
    loseText.setPosition(200, 350);
    loseText.setFillColor(Color::White);


    window.draw(loseText);
    window.display();

    
    sleep(seconds(2));


    window.close();
}