#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

void meteoro(ALLEGRO_BITMAP* meteor) {
    float frame = 0.f;
    /*while(true) {
        frame += .2f;
            if (frame > 3) {
                frame -= 3;
            }
    }*/
    int current_frame_y = 0;
    //al_draw_bitmap_region(meteor, 0, current_frame_y, 208.3, 195, 200, 200, 0);
    al_draw_bitmap_region(meteor, 160 * (int)frame, current_frame_y, 160, 160, 200, 200, 0);
    al_draw_bitmap_region(meteor, 160 * (int)frame, 160, 160, 160, 200, 200, 0);
}

void menu(ALLEGRO_BITMAP* menu) {
    al_draw_bitmap(menu, 300, 200, 0);
}


int main() {

    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY* display = al_create_display(1280, 920);

    al_set_window_position(display, 300, 100);
    al_set_window_title(display, "Dinno Meteor");

    ALLEGRO_BITMAP* menuimg = al_load_bitmap("./assets/menu.png");
    ALLEGRO_FONT* font = al_load_font("./assets/font.ttf", 25, 0);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_BITMAP* sprite = al_load_bitmap("./assets/dinopowerRed.png");
    ALLEGRO_BITMAP* meteor = al_load_bitmap("./assets/meteoroIlustrator.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("./assets/bg.png");
    ALLEGRO_BITMAP* stone = al_load_bitmap("./assets/Stone.png");
    ALLEGRO_BITMAP* grass = al_load_bitmap("./assets/Grass.png");
    ALLEGRO_BITMAP* cactus = al_load_bitmap("./assets/Cactus.png");
    ALLEGRO_BITMAP* bush = al_load_bitmap("./assets/Bush.png");
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    float frame = 0.f;
    int pos_x = 0, pos_y = 720;
    int current_frame_y = 160;
    float gravidade = 2.0f;
    float impulso_pulo = -50.0f;
    int lado = 1;
    float pos_dy = 0.0;
    bool is_jumping = false;
    float time_pulo = 0.0f;
    float maxtime_pulo = 1.0f;


    while (true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
        else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
            lado = 1;
            current_frame_y = 0;

            if (pos_x >= 1116) {
                pos_x -= 16;
            }
            else {
                pos_x += 20;
            }
            frame += .6f;
            if (frame > 8) {
                frame -= 8;
            }
        }

        else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
            lado = 0;
            current_frame_y = 160;
            if (pos_x <= 15) {
                pos_x += 16;
            }
            else {
                pos_x -= 20;
            }
            frame += .6f;
            if (frame > 8) {
                frame -= 8;
            }
        }

        //************PULO**************
        if (pos_y < 760 || is_jumping) {
            pos_dy = pos_dy + gravidade;
            frame += .2f;
            if (frame > 8) {
                frame -= 8;
            }
        }

        // 2. ATUALIZAR POSI��O
        //personagem_y += personagem_dy;
        pos_y += pos_dy;

        // 3. COLIS�O COM O CH�O
        if (pos_y >= 760) {
            //personagem_y = CHAO_Y; // Garante que ele n�o passe do ch�o
            pos_y = 760;
            //personagem_dy = 0.0f;  // Zera a velocidade vertical
            pos_dy = 0.0f;
            is_jumping = false;    // N�o est� mais pulando
        }

        if ((event.keyboard.keycode == ALLEGRO_KEY_SPACE || event.keyboard.keycode == ALLEGRO_KEY_UP) && time_pulo > maxtime_pulo) {
            // S� permite pular se estiver no ch�o
            if (pos_y == 760 && !is_jumping) {
                //personagem_dy = IMPULSO_PULO; // D� o impulso para cima
                pos_dy += impulso_pulo;
                is_jumping = true; // Define que ele est� no ar
                if (lado == 1) {
                    current_frame_y = 320;
                }
                else current_frame_y = 480;
            }
            time_pulo = 0;
        }


        time_pulo += 1.0f / 30.0f;

        al_clear_to_color(al_map_rgb(255, 255, 255));
        al_draw_bitmap(bg, 0, 0, 0);
        //menu(menuimg);
        al_draw_bitmap(stone, 100, 840, 0);
        al_draw_bitmap(grass, 400, 860, 0);
        al_draw_bitmap(bush, 700, 840, 0);
        al_draw_bitmap(cactus, 1000, 800, 0);
        meteoro(meteor);
        //menu(menuimg);
        al_draw_text(font, al_map_rgb(0, 0, 0), 7, 7, 0, "Score");
        al_draw_text(font, al_map_rgb(255, 255, 255), 5, 5, 0, "Score");
        al_draw_bitmap_region(sprite, 160 * (int)frame, current_frame_y, 160, 160, pos_x, pos_y, 0);
        //al_draw_bitmap(meteor, pos_meteor_x, pos_meteor_y_f, 0);
        al_flip_display();
    }

    al_destroy_bitmap(bg);
    al_destroy_bitmap(sprite);
    al_destroy_font(font);
    al_destroy_bitmap(stone);
    al_destroy_bitmap(grass);
    al_destroy_bitmap(bush);
    al_destroy_bitmap(cactus);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}