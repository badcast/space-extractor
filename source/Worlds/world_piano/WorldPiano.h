#include "../../SpaceMainMenu.h"

struct PianoNote {
    std::string name;
    bool state;
    KeyboardCode key;
    AudioSource* source;
    SpriteRenderer* render;
    Sprite* normal;
    Sprite* hover;
};

class PianoPlayer : public Behaviour
{
public:
    enum { NotesNum = 12 };
    Sprite* spr_black;
    Sprite* spr_lf;
    Sprite* spr_cf;
    Sprite* spr_rf;

    PianoNote notes[NotesNum];
    PianoPlayer()
        : Behaviour("Piano Player")
    {
    }
    void OnAwake();
    void OnUpdate();
};

class WorldPiano : public World
{

public:
    PianoPlayer* piano;
    WorldPiano()
        : World("World Piano")
    {
    }

    void OnStart();
};
