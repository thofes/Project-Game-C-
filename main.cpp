#include "mcigraph.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>


using namespace std;

class Figure {
protected:
    string _img;
public:
    int x, y;

    Figure(int x1, int y1, string tile) {
        x = x1;
        y = y1;
        _img = tile;
    }

    Figure(string tile) {
        x = rand() % 64;
        y = rand() % 48;
        _img = tile;
    }


    void draw_figure() {
        draw_image(_img, x * 16, y * 16);
    };

    void move_up(int* stop) {
        y--;
        if (stop[y * 64 + x] == 1 || y < 0) y++;
    }
    void move_down(int* stop) {
        y++;
        if (stop[y * 64 + x] == 1 || y > 47) y--;
    }
    void move_left(int* stop) {
        x--;
        if (stop[y * 64 + x] == 1 || x < 0) x++;
    }
    void move_right(int* stop) {
        x++;
        if (stop[y * 64 + x] == 1 || x > 63) x--;
    }

    void check_movement(int leftkey, int rightkey, int upkey, int downkey, int* stop) {
        if (is_pressed(leftkey)) move_left(stop);
        if (is_pressed(upkey)) move_up(stop);
        if (is_pressed(downkey)) move_down(stop);
        if (is_pressed(rightkey)) move_right(stop);

    }

    void check_movement_endgame(int leftkey, int rightkey, int* stop) {
        if (is_pressed(leftkey)) move_left(stop);
        if (is_pressed(rightkey)) move_right(stop);

    }



};

class Player : public Figure {
private:
    int _health;

public:
    Player(int x1, int y1, string tile) : Figure(x1, x1, tile) {
        _health = 100;
    }

    void draw_figure() {
        Figure::draw_figure();
        draw_line(x * 16, y * 16 - 3, x * 16 + (16.0 / 100) * _health, y * 16 - 3, 255, 0);
        draw_line(x * 16, y * 16 - 4, x * 16 + (16.0 / 100) * _health, y * 16 - 4, 255, 0);
    }
    bool damage() {
        bool dead = false;
        _health -= 10;

        if (_health == 0) {
            dead = true;
            return dead;
        }
        return dead;
    }

    void endgame(int health) {
        _health = health;
    }

};

class Ball : public Figure {
private:
    int _hits;
    bool _done;
    int _direction[2] = { 0 };


public:

    Ball(string tile, int hits) : Figure(tile) {
        _hits = hits;
        _done = false;
        int _direction[2] = { 0 };
    }



    void ball_movement(int* stop) {
        // 0 up / 1 down

        // 0 left / 1 right


        if (_direction[0] == 1)
            move_right(stop);
        if (_direction[0] == 0)
            move_left(stop);

        if (x == 63)
            _direction[0] = 0;
        if (x == 0)
            _direction[0] = 1;

        if (_direction[1] == 1)
            move_down(stop);
        if (_direction[1] == 0)
            move_up(stop);

        if (y == 43)
            _direction[1] = 0;
        if (y == 0)
            _direction[1] = 1;

    }

    void hit() {
        _hits -= 1;
        if (_hits == 0)
            _done = true;
    }

    bool is_done() {
        return _done;
    }
};

class Monster : public Figure {
private:
    int _health;
    bool _dead;


public:


    Monster(string tile) : Figure(tile) {
        _dead = false;
        _health = 100;
    }
    bool is_dead() {
        return _dead;
    }

    void randmove(int* stop) {
        int direction = rand() % 4;
        if (direction == 0)
            move_up(stop);
        if (direction == 1)
            move_down(stop);
        if (direction == 2)
            move_left(stop);
        if (direction == 3)
            move_right(stop);
    }
    void draw_figure() {
        Figure::draw_figure();
        draw_line(x * 16, y * 16 - 3, x * 16 + (16.0 / 100) * _health, y * 16 - 3, 255, 0);
        draw_line(x * 16, y * 16 - 4, x * 16 + (16.0 / 100) * _health, y * 16 - 4, 255, 0);
    }

    void hit() {
        _health -= 50;
        if (_health == 0) {
            _dead = true;
        }

    }
    void endgame() {
        _health = 0;
    }

};

class Gun : public Figure {
private:
    int _range;
public:
    Gun(int x1, int y1, string tile) : Figure(x1, x1, tile) {
        _range = 5;
    }

    void range() {
        _range += 2;
    }

    int get_range() {
        return _range;
    }


};

class Object : public Figure {
private:
    bool _collectable;
    bool _range;
    bool _time;


public:

    Object(string tile, bool collectable, bool range, bool time) : Figure(tile) {
        _collectable = collectable;
        _range = range;
        _time = time;
    }

    bool is_collectable() {
        return _collectable;
    }

    bool range() {
        return _range;
    }
    bool clock() {
        return _time;
    }

    void draw_figure() {
        Figure::draw_figure();
    }
};




bool are_colliding(Figure* f1, Figure* f2) {
    bool colliding = false;
    if (f1->x == f2->x && f1->y == f2->y)
        colliding = true;
    return colliding;
}


void generate_mapyx(int y1, int y2, int x1, int x2, int type, int randomizer, int* map) {
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            if (rand() % randomizer == 0)
                map[y * 64 + x] = type;
        }
    }
}

void generate_mapx(int y, int x1, int x2, int type, int randomizer, int* map) {
    for (int x = x1; x < x2; x++) {
        if (rand() % randomizer == 0)
            map[y * 64 + x] = type;
    }

}

void draw_map(int* map) {
    for (int y = 0; y < 48; y++) {
        for (int x = 0; x < 64; x++) {
            if (map[y * 64 + x] == 0)
                draw_image("grass.bmp", x * 16, y * 16);
            else if (map[y * 64 + x] == 1)
                draw_image("lake.bmp", x * 16, y * 16);
            else if (map[y * 64 + x] == 2)
                draw_image("gravel.bmp", x * 16, y * 16);
            else if (map[y * 64 + x] == 3)
                draw_image("wall.bmp", x * 16, y * 16);

        }
    }
}






int main(int argc, char* argv[]) {
    int time_delay = 0;
    int clock = 25;
    int amount_monsters = 0;
    int amount_balls = 0;
    int monster_kill = 0;
    int runindex = 0;
    bool endofgame = false;
    bool lost = false;





    srand(time(0));
    set_delay(100);
    Player c1(32, 24, "char1.bmp");
    Gun g1(32, 24, "gun.bmp");
    vector<Monster> monsters;
    vector<Object> objects;
    vector<Ball> balls;

    int map[64 * 48] = { 0 };
    int map_2[64 * 48] = { 0 };
    int map_3[64 * 48] = { 0 };

    int stop[64 * 48] = { 0 };
    int stop_2[64 * 48] = { 0 };
    int stop_3[64 * 48] = { 0 };
    generate_mapyx(0, 48, 0, 64, 1, 300, map); // Lake (kleine Pfützen)
    generate_mapyx(30, 40, 10, 30, 2, 1, map); // Gravel
    generate_mapx(15, 3, 50, 3, 1, map); // Wall
    generate_mapyx(0, 48, 0, 64, 3, 1, map_3); // Hintergrund Wall
    generate_mapyx(44, 48, 0, 64, 2, 1, map_3); // Gravel als Boden



    for (int y = 0; y < 48; y++) { // Wall and Lake nicht begehbar
        for (int x = 0; x < 64; x++) {
            if (map[y * 64 + x] == 3 || map[y * 64 + x] == 1)
                stop[y * 64 + x] = 1;
        }
    }



    while (running()) {
        while (monster_kill < 10) { // erste Map läuft so lange, bis 10 Monster abgechossen wurden

            draw_map(map);
            c1.check_movement(KEY_A, KEY_D, KEY_W, KEY_S, stop);


            if (rand() % 5 == 0 && amount_monsters < 20) { // 20 Monster erstellen
                monsters.push_back(Monster("monster.bmp"));
                amount_monsters++;
            }


            for (int i = 0; i < monsters.size(); i++) {  // Löschen von Monstern
                if (monsters[i].is_dead() == true) {
                    monsters.erase(monsters.begin() + i);
                    monster_kill++;
                }
            }

            for (auto& monster : monsters) { // Monster zeichnen
                monster.randmove(stop); //Monster bewegen sich unwillkürlich
                monster.draw_figure();
            }

            if (was_pressed(KEY_LEFT)) {
                if (time_delay > clock) { // Verzögerung, damit man nicht urchgehend schießen kann
                    g1.x = c1.x;
                    g1.y = c1.y;
                    int range = g1.get_range(); // holt sich die Reichweite des Schusses
                    int z = 0;
                    while (z < range) {
                        g1.move_left(stop);
                        g1.draw_figure();
                        z++;
                        for (auto& monster : monsters) {
                            if (are_colliding(&g1, &monster)) {// Treffer
                                monster.hit();
                            }
                        }
                    }
                    time_delay = 0;
                }

            }

            if (was_pressed(KEY_RIGHT)) {
                if (time_delay > clock) {
                    g1.x = c1.x;
                    g1.y = c1.y;
                    int range = g1.get_range();
                    int z = 0;
                    while (z < range) {
                        g1.move_right(stop);
                        g1.draw_figure();
                        z++;
                        for (auto& monster : monsters) {
                            if (are_colliding(&g1, &monster)) {// Treffer
                                monster.hit();
                            }
                        }
                    }
                    time_delay = 0;
                }

            }


            if (was_pressed(KEY_UP)) {
                if (time_delay > clock) {
                    g1.x = c1.x;
                    g1.y = c1.y;
                    int range = g1.get_range();
                    int z = 0;
                    while (z < range) {
                        g1.move_up(stop);
                        g1.draw_figure();
                        z++;
                        for (auto& monster : monsters) {
                            if (are_colliding(&g1, &monster)) {// Treffer
                                monster.hit();
                            }
                        }
                    }
                    time_delay = 0;
                }

            }
            if (was_pressed(KEY_DOWN)) {
                if (time_delay > clock) {
                    g1.x = c1.x;
                    g1.y = c1.y;
                    int range = g1.get_range();
                    int z = 0;
                    while (z < range) {
                        g1.move_down(stop);
                        g1.draw_figure();
                        z++;
                        for (auto& monster : monsters) {
                            if (are_colliding(&g1, &monster)) {// Treffer
                                monster.hit();
                            }
                        }
                    }
                    time_delay = 0;
                }

            }



            if (rand() % 55 == 0) { // Objecte erstellen
                objects.push_back(Object("fire.bmp", false, false, false));
                objects.push_back(Object("gold.bmp", true, true, false));
                objects.push_back(Object("clock.bmp", true, false, true));
            }



            for (auto monster : monsters) {
                if (are_colliding(&c1, &monster)) {// Kollision mit Monster
                    if (c1.damage() == true) {
                        return 0;
                    }
                }
            }

            for (int i = 0; i < objects.size(); i++) { // Goldbarren für mehr Reichweite
                if (are_colliding(&c1, &objects[i]) && objects[i].is_collectable() == true && objects[i].range() == true) {
                    objects.erase(objects.begin() + i);
                    g1.range();
                }
            }
            for (int i = 0; i < objects.size(); i++) { // Objekt für weniger Verzögerung zwischen den Schüssen
                if (are_colliding(&c1, &objects[i]) && objects[i].is_collectable() == true && objects[i].clock() == true) {
                    objects.erase(objects.begin() + i);
                    clock -= 2;
                }
            }
            for (int i = 0; i < objects.size(); i++) { // Feuerstellen die Schaden am Spieler ausrichten
                if (are_colliding(&c1, &objects[i]) && objects[i].is_collectable() == false) {
                    c1.damage();
                }
            }


            for (auto object : objects) // Objekte zeichnen
                object.draw_figure();

            c1.draw_figure();

            draw_line(0, 1, 5 * (clock - time_delay), 1, 255, 0, 0); //Balken für time_delay 
            draw_line(0, 2, 5 * (clock - time_delay), 2, 255, 0, 0);
            draw_line(0, 3, 5 * (clock - time_delay), 3, 255, 0, 0);
            draw_line(0, 4, 5 * (clock - time_delay), 4, 255, 0, 0);

            time_delay++;
            present();
        }


        while (runindex < 1) { //Zwischenmap
            objects.clear(); // Lösche den gesamten Objectektor
            objects.push_back(Object("door.bmp", false, false, false)); // Erstellung einer Tür, die irgendwo am Spielfeld erscheint
            runindex++;

        }



        while (runindex < 2 && are_colliding(&c1, &objects[0]) == false) { // diese Map mit der Tür wird angezeigt, bis der Spieler in die Tür eintritt
            draw_map(map_2);
            c1.check_movement(KEY_A, KEY_D, KEY_W, KEY_S, stop_2);
            objects[0].draw_figure();
            c1.draw_figure();
            present();
        }



        while (endofgame == false && lost == false) { // Nach dem Eintritt in die Tür erscheint eine neue Map und ein neues Spiel
            while (runindex < 2) {

                monsters.clear(); // alle Monster entfernen
                c1.x = 0;
                c1.y = 43;
                c1.endgame(0); // Charakter hat nun nur mehr ein Leben
                time_delay = 0;
                runindex++;

            }



            if (amount_balls < 5) { // Bälle erstellen
                balls.push_back(Ball("ball1.bmp", 2)); // dieser Ball muss zweimal getroffen werden
                balls.push_back(Ball("ball2.bmp", 1)); // dieser Ball muss nur einmal getroffen werden
                amount_balls++;
            }
  

            draw_map(map_3);
            c1.check_movement_endgame(KEY_LEFT, KEY_RIGHT, stop_3); // nur mehr rechts links möglich und ab jetzt mit den Pfeiltasten
            c1.draw_figure();


            if (was_pressed(KEY_SPACE)) { // mit der Leertaste wird ein Schuss nach oben abgegeben
                if (time_delay > clock / 2) {
                    g1.x = c1.x;
                    g1.y = c1.y;
                    int z = 0;
                    while (z < 44) {
                        g1.move_up(stop_3);
                        g1.draw_figure();
                        z++;
                        for (auto& ball : balls) {
                            if (are_colliding(&g1, &ball)) {// Treffer
                                ball.hit();
                            }
                        }
                    }
                    time_delay = 0;
                }

            }

            for (int i = 0; i < balls.size(); i++) {  // Löschen von Bällen
                if (balls[i].is_done() == true) {
                    balls.erase(balls.begin() + i);
                }
            }

            for (auto& ball : balls) { // Bälle zeichnen
                if (time_delay % 2 == 0) {
                    ball.ball_movement(stop_3);
                }
                ball.draw_figure();
            }

            for (auto& ball : balls) {
                if (are_colliding(&c1, &ball)) {// Charakter wird vom Ball getroffen
                    lost = true;
                    return 0;
                }
            }

            if (balls.size() == 0) { // alle Bälle sind abgeschossen
                endofgame = true;
                return 0;
            }


            time_delay++;
            present();

        }

        present();


    }

    present();


    return 0;


}







// Compile (Linux and MacOS):
// g++ -std=c++11 -lpthread mandelbrot.cpp -I/usr/include/SDL2 -D_REENTRANT -L/usr/lib -pthread -lSDL2
