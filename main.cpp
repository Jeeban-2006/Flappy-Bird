#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace sf;
using namespace std;

// ---------- PIPE ----------
struct Pipe {
    Sprite top, bottom;
    bool passed;
};

// ---------- MAIN ----------
int main() {
    srand(time(0));

    RenderWindow window(VideoMode(600, 800), "Flappy Bird");
    window.setFramerateLimit(60);

    // ---------- TEXTURES ----------
    Texture birdTex, pipeTex;
    birdTex.loadFromFile("Graphics/Bird.png");
    pipeTex.loadFromFile("Graphics/Pipes.png");

    // ---------- BIRD ----------
    Sprite bird(birdTex);
    bird.setOrigin(bird.getLocalBounds().width/2, bird.getLocalBounds().height/2);
    bird.setPosition(100, 400);

    float velocity = 0;
    float gravity = 900;

    // ---------- PIPES ----------
    vector<Pipe> pipes;

    // ---------- FONT ----------
    Font font;
    font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");

    Text scoreText, gameOverText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setPosition(20, 20);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(40);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setString("GAME OVER\nR = Restart\nESC = Exit");
    gameOverText.setPosition(80, 300);

    int score = 0;
    bool gameOver = false;

    Clock clock;
    float spawnTimer = 0;

    // ---------- LOOP ----------
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {

                if (e.key.code == Keyboard::Escape)
                    window.close();

                if (!gameOver && e.key.code == Keyboard::Space) {
                    velocity = -350;
                }

                if (gameOver && e.key.code == Keyboard::R) {
                    pipes.clear();
                    bird.setPosition(100, 400);
                    velocity = 0;
                    score = 0;
                    gameOver = false;
                }
            }
        }

        // ---------- UPDATE ----------
        if (!gameOver) {

            // bird physics
            velocity += gravity * dt;
            bird.move(0, velocity * dt);

            // spawn pipes
       spawnTimer += dt;
if (spawnTimer > 2.0f) {

    Pipe p;

    float gapY = rand() % 250 + 200;   // center of gap
   float gapSize = 220 - (score / 10);
if (gapSize < 140) gapSize = 140;

    float pipeHeight = pipeTex.getSize().y;

    p.top.setTexture(pipeTex);
    p.bottom.setTexture(pipeTex);

    // NO rotation, NO scaling, NO origin changes

    // place top pipe ABOVE screen
    p.top.setPosition(600, gapY - gapSize/2 - pipeHeight);

    // place bottom pipe BELOW gap
    p.bottom.setPosition(600, gapY + gapSize/2);

    p.passed = false;

    pipes.push_back(p);
    spawnTimer = 0;
}
            // move pipes
            float pipeSpeed = 180;
            for (auto &p : pipes) {
                p.top.move(-pipeSpeed * dt, 0);
                p.bottom.move(-pipeSpeed * dt, 0);

                // score when passing
                if (!p.passed && p.top.getPosition().x < bird.getPosition().x) {
                    score++;
                    p.passed = true;
                }

                // FIX: fair collision box
                FloatRect birdBox = bird.getGlobalBounds();
                birdBox.width -= 10;
                birdBox.height -= 10;

                if (birdBox.intersects(p.top.getGlobalBounds()) ||
                    birdBox.intersects(p.bottom.getGlobalBounds())) {
                    gameOver = true;
                }
            }

            // remove pipes
            pipes.erase(
                remove_if(pipes.begin(), pipes.end(),
                          [](Pipe &p) {
                              return p.top.getPosition().x < -100;
                          }),
                pipes.end()
            );

            // ground / top collision
            if (bird.getPosition().y > 750 || bird.getPosition().y < 0) {
                gameOver = true;
            }
        }

        // ---------- UI ----------
        scoreText.setString("Score: " + to_string(score));

        // ---------- DRAW ----------
        window.clear(Color::Cyan);

        window.draw(bird);

        for (auto &p : pipes) {
            window.draw(p.top);
            window.draw(p.bottom);
        }

        window.draw(scoreText);

        if (gameOver)
            window.draw(gameOverText);

        window.display();
    }
}
