#include "AssetList.h"

namespace AssetList
{
    // Player
    const char* const Player[] =
    {
        "asset/Player/Player_Idle.png",
        "asset/Player/Player_Walk.png",
        "asset/Player/Player_SmallJump.png",
        "asset/Player/Player_Damage.png",
        "asset/Player/Player_Ribbon.png",
        "asset/Player/Player_RibbonJump.png",
        "asset/Player/Player_Pulled.png",
        "asset/Player/Player_Roll.png",
        "asset/Player/GuideLine.png",
        "asset/Player/Circle2.png",
        "asset/Player/back.png",
        "asset/Player/front.png",
    };
    const size_t PlayerCount = sizeof(Player) / sizeof(Player[0]);

    // UI
    const char* const UI[] =
    {
        "asset/UI/title.png",
        "asset/UI/cursor.png",
        "asset/UI/Pouse_Back.png",
        "asset/UI/Pouse.png",              // ★追加（使ってるなら）
        "asset/UI/GameOver.png",
        "asset/UI/GrayStar.png",
        "asset/UI/Star.png",
        "asset/UI/Button_Retry.png",
        "asset/UI/Button_Continue.png",
        "asset/UI/Button_StageSelect.png",
        "asset/UI/クリア画面.png",
    };
    const size_t UICount = sizeof(UI) / sizeof(UI[0]);

    // Field
    const char* const Field[] =
    {
        "asset/Field/aa.png",
        "asset/Field/block.png",
        "asset/Field/floar.png",                 // ★追加
        "asset/Field/stage.png",               // ★追加

        "asset/Field/Boad.png",

        "asset/Field/PinDeco.png",                    // ★追加
        "asset/Field/PinRoll.png",                    // ★追加
        "asset/Field/PinPulled.png",                  // ★追加
        "asset/Field/PinJump.png",

        "asset/Field/s_w.png",                // ★追加
        "asset/Field/f_rippa.png",  // ★追加
        "asset/Field/リボンちゃん ワッペン.png",       // ★追加

        "asset/Field/Tutorial_PinDeco.png",
        "asset/Field/Tutorial_NeedleFloor.png",
        "asset/Field/Tutorial_Rippa.png",
        "asset/Field/Tutorial_Pinjump.png",           // ★追加

        "asset/Field/needle_floor.png",
        "asset/Field/Needle_Floor_Decorated.png",     // ★追加（使うなら）

        "asset/Field/rippa.png",
        "asset/Field/Wing_Rippa.png",
        "asset/Field/rippa_Deco.png",                 // ★追加（飾られ後）
        // "asset/Field/Rippa_Decorated.png",          // ★必要なら（今のコードでは未使用っぽい）
    };
    const size_t FieldCount = sizeof(Field) / sizeof(Field[0]);

    // All
    const char* const All[] =
    {
        // Player
        "asset/Player/Player_Idle.png",
        "asset/Player/Player_Walk.png",
        "asset/Player/Player_SmallJump.png",
        "asset/Player/Player_Damage.png",
        "asset/Player/Player_Ribbon.png",
        "asset/Player/Player_RibbonJump.png",
        "asset/Player/Player_Pulled.png",
        "asset/Player/Player_Roll.png",
        "asset/Player/GuideLine.png",
        "asset/Player/Circle2.png",
        "asset/Player/back.png",
        "asset/Player/front.png",

        // UI
        "asset/UI/title.png",
        "asset/UI/cursor.png",
        "asset/UI/Pouse_Back.png",
        "asset/UI/Pouse.png",              // ★追加（使ってるなら）
        "asset/UI/GameOver.png",
        "asset/UI/GrayStar.png",
        "asset/UI/Star.png",
        "asset/UI/Button_Retry.png",
        "asset/UI/Button_Continue.png",
        "asset/UI/Button_StageSelect.png",
        "asset/UI/クリア画面.png",

        // Field
        "asset/Field/aa.png",
        "asset/Field/block.png",
        "asset/Field/floar.png",                 // ★追加
        "asset/Field/stage.png",               // ★追加

        "asset/Field/Boad.png",

        "asset/Field/PinDeco.png",                    // ★追加
        "asset/Field/PinRoll.png",                    // ★追加
        "asset/Field/PinPulled.png",                  // ★追加
        "asset/Field/PinJump.png",

        "asset/Field/s_w.png",                // ★追加
        "asset/Field/f_rippa.png",  // ★追加
        "asset/Field/フェルトワッペン 羽リッパー.png",  // ★追加
        "asset/Field/リボンちゃん ワッペン.png",       // ★追加

        "asset/Field/Tutorial_PinDeco.png",
        "asset/Field/Tutorial_NeedleFloor.png",
        "asset/Field/Tutorial_Rippa.png",
        "asset/Field/Tutorial_Pinjump.png",           // ★追加

        "asset/Field/needle_floor.png",
        "asset/Field/Needle_Floor_Decorated.png",     // ★追加（使うなら）

        "asset/Field/rippa.png",
        "asset/Field/Wing_Rippa.png",
        "asset/Field/rippa_Deco.png",                 // ★追加
        // "asset/Field/Rippa_Decorated.png",          // ★必要なら
    };
    const size_t AllCount = sizeof(All) / sizeof(All[0]);
}
