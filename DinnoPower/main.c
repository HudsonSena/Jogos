#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
//  573 / 3 = 191 → largura
// 644 / 4 = 161  → altura


int main() {

    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY* display = al_create_display(1280, 920);
    al_set_window_position(display, 300, 100);
    al_set_window_title(display, "Dino Power");
    ALLEGRO_FONT* font = al_load_font("./font.ttf", 25, 0);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);

    ALLEGRO_BITMAP* sprite = al_load_bitmap("./dinopower.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("./bg.png");
    ALLEGRO_BITMAP* stone = al_load_bitmap("./Stone.png");
    ALLEGRO_BITMAP* grass = al_load_bitmap("./Grass.png");
    ALLEGRO_BITMAP* cactus = al_load_bitmap("./Cactus.png");
    ALLEGRO_BITMAP* bush = al_load_bitmap("./Bush.png");


    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    float frame = 0.f;
    int pos_x = 0, pos_y = 760;
    int current_frame_y = 160;
    float gravidade = 2.0f;
    float impulso_pulo = -50.0f;
    int lado = 1;

    // Variáveis do Personagem
    //float personagem_y = CHAO_Y; // Posição Y atual
    float pos_dy = 0.0;  // Velocidade vertical atual
    bool is_jumping = false;

    while (true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (pos_y < 760 || is_jumping) {
            //personagem_dy += GRAVIDADE;
			pos_dy = pos_dy + gravidade;
            frame += .2f;
            if (frame > 8) {
                frame -= 8;
            }
        }

        // 2. ATUALIZAR POSIÇÃO
        //personagem_y += personagem_dy;
		pos_y = pos_y + pos_dy;

        // 3. COLISÃO COM O CHÃO
        if (pos_y >= 760) {
            //personagem_y = CHAO_Y; // Garante que ele não passe do chão
            pos_y = 760;
            //personagem_dy = 0.0f;  // Zera a velocidade vertical
			pos_dy = 0.0f;
            is_jumping = false;    // Não está mais pulando
        }

        if (event.keyboard.keycode == ALLEGRO_KEY_SPACE || event.keyboard.keycode == ALLEGRO_KEY_UP) {
            // Só permite pular se estiver no chão
            if (pos_y == 760 && !is_jumping) {
                //personagem_dy = IMPULSO_PULO; // Dá o impulso para cima
                pos_dy += impulso_pulo;
                is_jumping = true; // Define que ele está no ar
                if (lado == 1) {
                    current_frame_y = 320;
                } else current_frame_y = 480;
            }
            
        }

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
        else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
			lado = 1;
            current_frame_y = 0;
            pos_x += 16;
			if (pos_x >= 1100) pos_x -= 20;
            frame += 1.2f;
            if (frame > 8) {
                frame -= 8;
            }
        }

        else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
            lado = 0;
            current_frame_y = 160;
            pos_x -= 16;
            if (pos_x <= 0) pos_x += 20;  
            frame += 1.2f;
            if (frame > 8) {
                frame -= 8;
            }
            
        }

        al_clear_to_color(al_map_rgb(255, 255, 255));
        al_draw_bitmap(bg, 0, 0, 0);
        al_draw_bitmap(stone, 100, 840, 0);
        al_draw_bitmap(grass, 400, 860, 0);
        al_draw_bitmap(bush, 700, 840, 0);
        al_draw_bitmap(cactus,1000, 820, 0);
        al_draw_text(font, al_map_rgb(0, 0, 0), 7, 7, 0, "SCORE: Dino Power");
        al_draw_text(font, al_map_rgb(255, 255, 255), 5, 5, 0, "SCORE: Dino Power");
        al_draw_bitmap_region(sprite, 160 * (int)frame, current_frame_y, 160, 160, pos_x, pos_y, 0);
        al_flip_display();
    }

    al_destroy_bitmap(bg);
    al_destroy_bitmap(sprite);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}