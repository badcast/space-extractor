#include "../../SpaceMainMenu.h"
#include <cstring>

void WorldPiano::OnStart()
{
    switch_game_level(this);

    // Create camera2d
    Primitive::create_camera2D();

    piano = Primitive::create_empty_game_object()->AddComponent<PianoPlayer>();
    SpriteRenderer* background = create_game_object()->AddComponent<SpriteRenderer>();
    Sprite* spr = Sprite::CreateEmptySprite();

    spr->setSurface(Resources::GetImageSource(Resources::LoadImage("./data/piano/sprites/background.jpg", true)));
    background->set_sprite(spr);
    background->transform()->layer(-10);
}

void PianoPlayer::OnAwake()
{
    // init piano sources
    int x, y;
    char buffer[48];

    for (x = 0, y = 5; x < NotesNum; ++x, ++y) {
        sprintf(buffer, "piano_C%d", y);
        notes[x].name = buffer;
        sprintf(buffer, "piano_C%d-1", y);
        notes[++x].name = buffer;
    }

    // load clips
    for (x = 0; x < NotesNum; ++x) {
        // Add sprite
        notes[x].render = Primitive::create_empty_game_object()->AddComponent<SpriteRenderer>();
        //   notes[x].render->transform()->set_parent(this->transform());

        // Add AudioSource and load
        notes[x].source = this->AddComponent<AudioSource>();
        resource_id note_wav = Resources::LoadAudioClip(std::string("./data/piano/") + notes[x].name + std::string(".wav"), true);
        notes[x].source->setClip(Resources::GetAudioClipSource(note_wav));
        notes[x].source->setVolume(0.3f);
    }

    // load sprites
    std::string sprite_directory { "./data/piano/sprites/" };
    spr_black = Primitive::create_empty_sprite2D();
    spr_lf = Primitive::create_empty_sprite2D();
    spr_cf = Primitive::create_empty_sprite2D();
    spr_rf = Primitive::create_empty_sprite2D();

    spr_black->setSurface(Resources::GetImageSource(Resources::LoadImage(sprite_directory + "black.png", true)));
    spr_lf->setSurface(Resources::GetImageSource(Resources::LoadImage(sprite_directory + "lf.png", true)));
    spr_cf->setSurface(Resources::GetImageSource(Resources::LoadImage(sprite_directory + "cf.png", true)));
    spr_rf->setSurface(Resources::GetImageSource(Resources::LoadImage(sprite_directory + "rf.png", true)));

    // init key tone and layout
#define set_layout(note) note.render->transform()->position(pos);
    notes[x = 0].key = KB_Q;
    notes[x].render->set_sprite(spr_lf);
    Vec2 scale = spr_cf->size();
    Vec2 pos = transform()->position();

    pos.x = -scale.x * 3;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_W;
    notes[x].render->set_sprite(spr_cf);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_E;
    notes[x].render->set_sprite(spr_rf);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_R;
    notes[x].render->set_sprite(spr_lf);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_T;
    notes[x].render->set_sprite(spr_cf);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_Y;
    notes[x].render->set_sprite(spr_rf);
    pos.x += scale.x;
    set_layout(notes[x]);

    // init key half-tone
    notes[x = 1].key = KB_2;
    notes[x].render->set_sprite(spr_black);
    scale = spr_black->size();
    pos.y = spr_cf->size().y / 2 - scale.y / 2;
    pos.x = notes[0].render->transform()->position().x + 0.015;

    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_3;
    notes[x].render->set_sprite(spr_black);
    pos.x += scale.x*2;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_5;
    notes[x].render->set_sprite(spr_black);
    pos.x += scale.x*4;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_6;
    notes[x].render->set_sprite(spr_black);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_7;
    notes[x].render->set_sprite(spr_black);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_9;
   // notes[x].render->set_sprite(spr_black);
    pos.x += scale.x;
    set_layout(notes[x]);
#undef set_layout
}

void PianoPlayer::OnUpdate()
{
    for (int note = 0; note < NotesNum; ++note) {
        if (Input::GetKeyDown(notes[note].key)) {
            if (notes[note].state == false) {
                notes[note].state = true;
                notes[note].source->Play();
            }
        } else {
            notes[note].state = false;
        }
    }
}
