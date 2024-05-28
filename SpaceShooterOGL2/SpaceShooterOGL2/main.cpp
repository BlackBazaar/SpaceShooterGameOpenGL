#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

float playerX = 0.0f;
float playerY = -0.8f;
float playerSize = 0.1f;

float enemyX = 0.0f;
float enemyY = 0.8f;
float enemySize = 0.1f;
float enemySpeed = 0.01f;

int score = 0;
int level = 1; // Starting level
bool gameOver = false;
bool showInstructions = true;

struct Bullet {
    float x;
    float y;
    float speed;
    float size;
};

std::vector<Bullet> bullets;

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void drawTriangle(float x, float y, float size) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + size);
    glVertex2f(x - size, y - size);
    glVertex2f(x + size, y - size);
    glEnd();
}

void drawHexagon(float x, float y, float size) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 6; ++i) {
        float angle = 2.0 * 3.14159 * float(i) / 6.0;
        glVertex2f(x + size * cos(angle), y + size * sin(angle));
    }
    glEnd();
}

void drawPlayer() {
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTriangle(playerX, playerY, playerSize);
}

void drawEnemy() {
    glColor3f(1.0f, 0.0f, 0.0f);
    drawHexagon(enemyX, enemyY, enemySize);
}

void drawBullet(float x, float y, float size) {
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(x - size / 2.0f, y + size);
    glVertex2f(x + size / 2.0f, y + size);
    glVertex2f(x + size / 2.0f, y - size * 2.0f); // Making bullet longer
    glVertex2f(x - size / 2.0f, y - size * 2.0f);
    glEnd();
}

void drawScore() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.9f, 0.9f);
    std::string scoreText = "Score: " + std::to_string(score);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

void drawLevel() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.9f, 0.8f);
    std::string levelText = "Level: " + std::to_string(level);
    for (char c : levelText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

void drawInstructions() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.5f, 0.0f);
    std::string instructionsText = "Press SPACE to shoot.";
    for (char c : instructionsText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    glRasterPos2f(-0.5f, -0.1f);
    std::string moveText = "Press A to move left, D to move right.";
    for (char c : moveText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    glRasterPos2f(-0.5f, -0.2f);
    std::string startText = "Press any key to start the game.";
    for (char c : startText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (showInstructions) {
        drawInstructions();
    }
    else {
        if (!gameOver) {
            drawPlayer();
            drawEnemy();
            for (const auto& bullet : bullets) {
                drawBullet(bullet.x, bullet.y, bullet.size);
            }
            drawScore();
            drawLevel();

            // Check collision between player and enemy
            float distance = sqrt(pow(enemyX - playerX, 2) + pow(enemyY - playerY, 2));
            if (distance < enemySize + playerSize) {
                gameOver = true;
            }
            // Check if the enemy passed the player and gets out of the screen
            else if (enemyY - enemySize < playerY - playerSize) {
                gameOver = true;
            }
        }
        else {
            glColor3f(1.0f, 0.0f, 0.0f); // Red color for "Game Over" text
            glRasterPos2f(-0.3f, 0.0f);
            std::string gameOverText = "Game Over!";
            for (char c : gameOverText) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
            }

            glColor3f(1.0f, 1.0f, 1.0f); // White color for score text
            glRasterPos2f(-0.3f, -0.1f);
            std::string scoreText = "Score: " + std::to_string(score);
            for (char c : scoreText) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
            }
        }
    }

    glutSwapBuffers();
}

void update(int value) {
    if (!gameOver && !showInstructions) {
        // Update enemy speed based on level
        switch (level) {
        case 1:
            enemySpeed = 0.01f;
            break;
        case 2:
            enemySpeed = 0.012f;
            break;
        case 3:
            enemySpeed = 0.014f;
            break;
        case 4:
            enemySpeed = 0.016f;
            break;
        case 5:
            enemySpeed = 0.019f;
            break;
        default:
            break;
        }

        // Update enemy
        enemyY -= enemySpeed;
        if (enemyY - enemySize < -1.0) { // Reset enemy position if it goes below the screen
            enemyX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
            enemyY = 1.0f;
        }

        // Update bullets
        for (auto& bullet : bullets) {
            bullet.y += bullet.speed;
        }

        // Check bullet-enemy collision
        for (auto it = bullets.begin(); it != bullets.end();) {
            if (sqrt(pow(enemyX - it->x, 2) + pow(enemyY - it->y, 2)) < enemySize + it->size) {
                // Enemy hit by bullet, spawn a new enemy at a random position
                enemyX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
                enemyY = 1.0f;
                it = bullets.erase(it);
                ++score; // Increment score

                // Increase level based on score
                if (score >= 10 && score < 20) {
                    level = 2;
                }
                else if (score >= 20 && score < 50) {
                    level = 3;
                }
                else if (score >= 50 && score < 70) {
                    level = 4;
                }
                else if (score >= 70 && score < 100) {
                    level = 5;
                }
            }
            else {
                ++it;
            }
        }

        // Check player bounds
        if (playerX - playerSize < -1.0f) {
            playerX = -1.0f + playerSize;
        }
        else if (playerX + playerSize > 1.0f) {
            playerX = 1.0f - playerSize;
        }

        // Check enemy bounds
        if (enemyX - enemySize < -1.0f) {
            enemyX = -1.0f + enemySize;
        }
        else if (enemyX + enemySize > 1.0f) {
            enemyX = 1.0f - enemySize;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void handleKeypress(unsigned char key, int x, int y) {
    if (showInstructions) {
        showInstructions = false;
    }
    else {
        switch (key) {
        case 27: // Escape key
            exit(0);
            break;
        case 'a':
            playerX -= 0.05f; // Move left
            break;
        case 'd':
            playerX += 0.05f; // Move right
            break;
        case ' ':
            bullets.push_back({ playerX, playerY + playerSize, 0.03f, 0.015f }); // Fire bullet, increased bullet size
            break;
        case 'q':
        case 'Q':
            exit(0); // End the game
            break;
        }
    }
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(nullptr))); // Initialize random seed
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Space Shooter");

    init();
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutKeyboardFunc(handleKeypress);

    glutMainLoop();
    return 0;
}
