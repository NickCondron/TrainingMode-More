#include "events.h"

void Exit(GOBJ *menu);

enum menu_options {
    OPT_MODE,
    OPT_EXIT,
};

enum mode {
    DASH_DANCE,
    GROUNDED,
    ANY,
};

static const char *Options_Mode[] = { "Dash Dance", "Grounded", "Any" };

static EventOption Options_Main[] = {
    {
        .kind = OPTKIND_STRING,
        .value_num = countof(Options_Mode),
        .name = "Mode",
        .desc = {"Flag clear mode"},
        .values = Options_Mode,
    },
    {
        .kind = OPTKIND_INFO,
        .name = "Help",
        .desc =
            {"Move through the flags and back to score",
             "Press Z in the pause menu to reset",
             "High score is saved after 1 minute, but keep playing"},
    },
    {
        .kind = OPTKIND_FUNC,
        .name = "Exit",
        .desc = {"Return to the Event Select Screen."},
        .OnSelect = Exit,
    },
};

static EventMenu Menu_Main = {
    .name = "Slalom",
    .option_num = sizeof(Options_Main) / sizeof(EventOption),
    .options = Options_Main,
};

void Draw_Flag(Vec2 pos) {
    COBJ *cur_cam = COBJ_GetCurrent();
    CObj_SetCurrent(*stc_matchcam_cobj);

    PRIM_DrawMode draw_mode = {
        .line_width = 16,
        .z_compare_enable = true,
        .z_logic_eq = true,
        .z_logic_lt = true,
    };
    PRIM_BlendMode blend_mode = { 0 };
    u32 color = 0xff0000ff;

    draw_mode.shape = PRIM_SHAPE_LINE_STRIP;
    PRIM_NEW(2, draw_mode, blend_mode);
    PRIM_DRAW(pos.X, pos.Y, 0, color);
    PRIM_DRAW(pos.X, pos.Y + 15, 0, color);

    draw_mode.shape = PRIM_SHAPE_TRIANGLES;
    PRIM_NEW(3, draw_mode, blend_mode);
    PRIM_DRAW(pos.X, pos.Y + 15, 0, color);
    PRIM_DRAW(pos.X - 3, pos.Y + 12, 0, color);
    PRIM_DRAW(pos.X, pos.Y + 12, 0, color);

    PRIM_CLOSE();
    CObj_SetCurrent(cur_cam);
}

void Event_Init(GOBJ *menu) {
    GOBJ *draw_gobj = GObj_Create(0, 0, 0);
    GObj_AddGXLink(draw_gobj, Draw_Flag, 3, 0);
    KOCount_Init(0);
}


void Event_Think(GOBJ *menu) {
    GOBJ *player = Fighter_GetGObj(0);
    FighterData *player_data = player->userdata;

    KOCount_Update(2);

    Vec2 pos = { 0, 0 };
    Draw_Flag(pos);
}

void Exit(GOBJ *menu) {
    stc_match->state = 3;
    Match_EndVS();
}

EventMenu *Event_Menu = &Menu_Main;
