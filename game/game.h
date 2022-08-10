#include <list>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <ncurses.h>
#include <thread>
#include <unistd.h>

class Position {
public:
    int16_t x;
    int16_t y;
    Position(int16_t x, int16_t y) {
        this->x = x;
        this->y = y;
    }

    bool operator ==(Position position) const {
        return this->x == position.x && this->y == position.y;
    }
};

class Snake {
private:
    Position head = Position(0, 0);
    Position fruit = Position(0, 0);
    std::list<Position> tail;
    uint16_t width;
    uint16_t height;
    uint16_t score;
    int8_t x_direction = 1;
    int8_t y_direction = 0;
    uint32_t h_delay = 200 * 1000; /* horizontal speed (bigger number means slower), (1 millisecond is 1000 microsecond) */
    uint8_t aspect_ratio = 21 / 9; /* 21 and 9 are height and width of console character */
    uint32_t v_delay = this->h_delay * this->aspect_ratio;
    uint32_t delay = this->h_delay;
    bool game_over = false;
    bool paused = false;
    bool solid_border;
    bool move_made = false;

    const char HEAD = '@';
    const char TAIL = 'o';
    const char FRUIT = '$';
    const char BOARD = ' ';
    const char SOLID_BORDER = '#';
    const char NOT_SOLID_BORDER = '+';
    char BORDER;

    void turn_right() {
        if (this->y_direction != 0 && this->move_made) {
            this->x_direction = 1;
            this->y_direction = 0;
            this->delay = this->h_delay;
            this->move_made = false;
        }
    }
    void turn_left() {
        if (this->y_direction != 0 && this->move_made) {
            this->x_direction = -1;
            this->y_direction = 0;
            this->delay = this->h_delay;
            this->move_made = false;
        }
    }
    void turn_up() {
        if (this->x_direction != 0 && this->move_made) {
            this->x_direction = 0;
            this->y_direction = -1;
            this->delay = this->v_delay;
            this->move_made = false;
        }
    }
    void turn_down() {
        if (this->x_direction != 0 && this->move_made) {
            this->x_direction = 0;
            this->y_direction = 1;
            this->delay = this->v_delay;
            this->move_made = false;
        }
    }
    void move() {
        this->tail.push_front(Position(this->head.x, this->head.y));
        this->tail.pop_back();

        this->head.x += this->x_direction;
        this->head.y += this->y_direction;

        if (this->is_game_over()) {
            this->game_over = true;
            return;
        }

        if (this->head.x == this->width) {
            this->head.x = 0;
        } else if (this->head.x == -1) {
            this->head.x = this->width;
        } else if (this->head.y == this->height) {
            this->head.y = 0;
        } else if (this->head.y == -1) {
            this->head.y = this->height;
        }

        if (this->head == this->fruit) {
            this->tail.push_back(this->tail.back());
            this->score++;
            this->generate_fruit();
            this->h_delay -= 1000;
            this->v_delay = this->h_delay * this->aspect_ratio;
        }
        this->move_made = true;
    }

    bool is_game_over() {
        bool head_on_tail = false;
        for (Position &position: this->tail) {
            if (position == head) {
                head_on_tail = true;
                break;
            }
        }
        return
            head_on_tail ||
            (this->solid_border &&
                (this->head.x == this->width || this->head.x == -1 || this->head.y == -1 || this->head.y == this->height)
            );
    }

    void generate_fruit() {
        bool on_tail = false;
        uint16_t seed = 0;
        do {
            srand(time(nullptr) + seed);
            this->fruit.x = rand() % this->width;
            this->fruit.y = rand() % this->height;
            uint16_t i = 0;
            for (Position position: this->tail) {
                if (position == this->fruit) {
                    on_tail = true;
                    break;
                }
                ++i;
            }

            if (i == this->tail.size()) {
                on_tail = false;
            }
            seed++;
        } while (on_tail);
    }

    void listen_key_press() {
        while (!this->game_over) {
            switch (getch()) {
                case 'A':
                case 'a':
                    this->turn_left();
                    break;
                case 'S':
                case 's':
                    this->turn_down();
                    break;
                case 'D':
                case 'd':
                    this->turn_right();
                    break;
                case 'W':
                case 'w':
                    this->turn_up();
                    break;
                case 'P':
                case 'p':
                    this->paused = !this->paused;
                    break;
            }
            refresh();
        }
    }
    void update_ui() const {
        system("clear");
        std::cout << "Score: " << this->score << "\r\n";
        for (uint8_t j = 0; j < width + 2; ++j) {
            std::cout << this->BORDER;
        }
        std::cout << "\r\n";

        for (uint16_t i = 0; i < height; ++i) {
            for (uint16_t j = 0; j < width + 2; ++j) {
                if (j == 0 || j == width + 1) {
                    std::cout << this->BORDER;
                } else if (this->fruit.x == j - 1 && this->fruit.y == i) {
                    std::cout << this->FRUIT;
                } else if (this->head.x == j - 1 && this->head.y == i) {
                    std::cout << this->HEAD;
                } else {
                    bool tail_printed = false;
                    for (Position tail_pos: this->tail) {
                        if (tail_pos.x == j - 1 && tail_pos.y == i) {
                            std::cout << this->TAIL;
                            tail_printed = true;
                            break;
                        }
                    }
                    if (!tail_printed) {
                        std::cout << this->BOARD;
                    }
                }
            }
            std::cout << "\r\n";
        }

        for (uint8_t j = 0; j < width + 2; ++j) {
            std::cout << this->BORDER;
        }
        std::cout << "\r\n";
    }

public:
    Snake(uint8_t width, uint8_t height, bool solid_border = true) {
        this->width = width * this->aspect_ratio;
        this->height = height;
        this->score = 0;
        this->solid_border = solid_border;
        if (this->solid_border) {
            this->BORDER = this->SOLID_BORDER;
        } else {
            this->BORDER = this->NOT_SOLID_BORDER;
        }

        this->head.x = this->width / 2;
        this->head.y = this->height / 2;
        this->tail.emplace_back(this->head.x, this->head.y);
        this->generate_fruit();

        initscr();
        noecho();
        std::thread key_press_thread(&Snake::listen_key_press, this);
        this->update_ui();
        while (!this->game_over) {
            if (!this->paused) {
                this->move();
                this->update_ui();
                usleep(this->delay);
            }
        }
        key_press_thread.detach();
    }

    ~Snake() {
        endwin();
    }
};

