#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <string>

enum EstadoDoJogo { MENU_INICIAL, JOGANDO, GAME_OVER };
EstadoDoJogo estadoAtual = MENU_INICIAL;

// Estruturas para a nave, os tiros e os obstáculos
struct Nave {
    float x, y;
};

struct Tiro {
    float x, y;
    bool ativo;
};

struct Obstaculo {
    float x, y;
    bool ativo;
};

// Variáveis globais
Nave nave = { 0.0f, -0.8f };
std::vector<Tiro> tiros;
std::vector<Obstaculo> obstaculos;
int pontos = 0;
bool gameOver = false;

// Função para desenhar texto na tela
void DesenhaTexto(const std::string& texto, float x, float y) {
    glRasterPos2f(x, y);
    for (char c : texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Função para desenhar a nave
void DesenhaNave() {
    glPushMatrix();
    glTranslatef(nave.x, nave.y, 0.0f);
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(0.0f, 0.1f);
    glVertex2f(-0.05f, -0.05f);
    glVertex2f(0.05f, -0.05f);
    glEnd();
    glPopMatrix();
}

// Função para desenhar os tiros
void DesenhaTiros() {
    glColor3f(1.0f, 0.0f, 0.0f); // Tiros vermelhos
    for (auto& tiro : tiros) {
        if (tiro.ativo) {
            glPushMatrix();
            glTranslatef(tiro.x, tiro.y, 0.0f);
            glBegin(GL_QUADS);
            glVertex2f(-0.01f, 0.02f);
            glVertex2f(0.01f, 0.02f);
            glVertex2f(0.01f, -0.02f);
            glVertex2f(-0.01f, -0.02f);
            glEnd();
            glPopMatrix();
        }
    }
}

// Função para desenhar os obstáculos
void DesenhaObstaculos() {
    glColor3f(0.0f, 0.0f, 1.0f); // Obstáculos azuis
    for (auto& obst : obstaculos) {
        if (obst.ativo) {
            glPushMatrix();
            glTranslatef(obst.x, obst.y, 0.0f);
            glutSolidCube(0.1f);
            glPopMatrix();
        }
    }
}

// Função para detectar colisão (simples: colisão de quadrados)
bool Colisao(float x1, float y1, float x2, float y2, float tamanho) {
    return fabs(x1 - x2) < tamanho && fabs(y1 - y2) < tamanho;
}

// Função de atualização do jogo
void AtualizaCena(int valor) {
    if (estadoAtual == JOGANDO) {
        // Atualiza a posição dos tiros
        for (auto& tiro : tiros) {
            if (tiro.ativo) {
                tiro.y += 0.05f;
                if (tiro.y > 1.0f) {
                    tiro.ativo = false;
                }
            }
        }

        // Atualiza a posição dos obstáculos
        for (auto& obst : obstaculos) {
            if (obst.ativo) {
                obst.y -= 0.02f;
                if (obst.y < -1.0f) {
                    obst.ativo = false;
                }

                // Verifica colisão com a nave
                if (Colisao(nave.x, nave.y, obst.x, obst.y, 0.1f)) {
                    estadoAtual = GAME_OVER; // Fim de jogo
                }
            }
        }

        // Verifica colisão entre tiros e obstáculos
        for (auto& obst : obstaculos) {
            if (obst.ativo) {
                for (auto& tiro : tiros) {
                    if (tiro.ativo && Colisao(tiro.x, tiro.y, obst.x, obst.y, 0.1f)) {
                        tiro.ativo = false;
                        obst.ativo = false;
                        pontos++; // Incrementa os pontos
                    }
                }
            }
        }

        // Gera novos obstáculos
        if (rand() % 100 < 2) { // Pequena chance de gerar novo obstáculo
            obstaculos.push_back({ (rand() % 200 / 100.0f) - 1.0f, 1.0f, true });
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, AtualizaCena, 0);
}

// Função de controle de teclado
void Teclado(unsigned char tecla, int x, int y) {
    if (estadoAtual == JOGANDO) {
        if (tecla == 'a') nave.x -= 0.05f;
        if (tecla == 'd') nave.x += 0.05f;
        if (tecla == 'w') nave.y += 0.05f;
        if (tecla == 's') nave.y -= 0.05f;

        // Dispara tiro ao pressionar espaço
        if (tecla == ' ') {
            tiros.push_back({ nave.x, nave.y, true });
        }
    } else if (estadoAtual == MENU_INICIAL) {
        if (tecla == '1') {
            estadoAtual = JOGANDO;
            nave = { 0.0f, -0.8f };
            obstaculos.clear();
            tiros.clear();
            pontos = 0;
        } else if (tecla == '2') {
            exit(0); // Sair do jogo
        }
    } else if (estadoAtual == GAME_OVER) {
        if (tecla == '1') { // Reiniciar jogo
            estadoAtual = JOGANDO;
            nave = { 0.0f, -0.8f };
            obstaculos.clear();
            tiros.clear();
            pontos = 0;
        } else if (tecla == '2') { // Voltar ao menu inicial
            estadoAtual = MENU_INICIAL;
        }
    }
}

// Função para capturar movimento do mouse e mover a nave
void MovimentoMouse(int x, int y) {
    if (estadoAtual == JOGANDO) {
        // Converte a posição do mouse para as coordenadas do mundo (OpenGL usa um sistema de coordenadas de -1 a 1)
        nave.x = (x / 400.0f) - 1.0f; // 400 é metade da largura da tela (800)
        nave.y = 1.0f - (y / 300.0f); // 300 é metade da altura da tela (600)
    }
}

// Função de desenho do menu inicial
void DesenhaMenuInicial() {
    DesenhaTexto("MENU INICIAL", -0.2f, 0.2f);
    DesenhaTexto("1. Iniciar Jogo", -0.2f, 0.0f);
    DesenhaTexto("2. Sair", -0.2f, -0.2f);
}

// Função de desenho do menu de game over
void DesenhaMenuGameOver() {
    DesenhaTexto("GAME OVER", -0.2f, 0.2f);
    DesenhaTexto("1. Reiniciar Jogo", -0.2f, 0.0f);
    DesenhaTexto("2. Voltar ao Menu Inicial", -0.2f, -0.2f);
}

// Função de desenho da cena
void DesenhaCena() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (estadoAtual == MENU_INICIAL) {
        DesenhaMenuInicial();
    } else if (estadoAtual == JOGANDO) {
        // Desenha a nave
        DesenhaNave();

        // Desenha os tiros
        DesenhaTiros();

        // Desenha os obstáculos
        DesenhaObstaculos();

        // Mostra os pontos na tela
        DesenhaTexto("Pontos: " + std::to_string(pontos), -0.9f, 0.9f);
    } else if (estadoAtual == GAME_OVER) {
        DesenhaMenuGameOver();
    }

    glutSwapBuffers();
}

// Função de inicialização
void Inicializa() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Cor de fundo
}

// Função principal
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Jogo de Nave");

    Inicializa();

    glutDisplayFunc(DesenhaCena);
    glutKeyboardFunc(Teclado);
    glutPassiveMotionFunc(MovimentoMouse); // Função de controle de movimento do mouse
    glutTimerFunc(16, AtualizaCena, 0); // Atualiza ~60 FPS

    glutMainLoop();
    return 0;
}
