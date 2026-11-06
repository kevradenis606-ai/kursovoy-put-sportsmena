#pragma execution_character_set("utf-8") 

#include "raylib.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// Состояния игры / экраны
enum GameState {
    SCREEN_LOADING,
    SCREEN_MENU,
    SCREEN_STORY,
    SCREEN_MAP,
    SCREEN_LEVEL1,
    SCREEN_LEVEL2,
    SCREEN_LEVEL3,
    SCREEN_LEVEL4,
    SCREEN_LEVEL5,
    SCREEN_ARMRESTLING,
    SCREEN_WIN,
    SCREEN_LOSE,
    SCREEN_CONTROLS,
    SCREEN_ABOUT,
    SCREEN_PAUSE,
    SCREEN_DIFFICULTY,
    SCREEN_EXIT_CONFIRM,
    SCREEN_SETTINGS
};

// Перечисление для языков
enum Language {
    LANG_RU,
    LANG_EN,
    LANG_BY
};

// Уровни сложности для Армрестлинга
enum Difficulty {
    DIFF_EASY,
    DIFF_MEDIUM,
    DIFF_HARD,
    DIFF_HARDMODE
};

// Режимы кликов для армрестлинга
enum ClickMode {
    MODE_CLASSIC,
    MODE_ALTERNATE
};

struct Dumbbell {
    float x;
    float y;
    Color color;
    bool isRed;
    bool active;
};

struct Product {
    const char* nameRU;
    const char* nameEN;
    const char* nameBY;
    int type;
    int bzhu;
    float x;
    float y;
    bool isDragged;
    bool correctPlaced;
    float speedX;
    float speedY;
    Texture2D texture;
};

// Структура для поз в 5 уровне
struct Pose {
    const char* nameRU;
    const char* nameEN;
    const char* nameBY;
    bool completed;
    Texture2D texture;
};

// Глобальные переменные прогресса
GameState currentState = SCREEN_LOADING;
GameState levelToRetry = SCREEN_LEVEL1;
GameState previousState = SCREEN_LEVEL1;
GameState exitReturnState = SCREEN_MENU;
Difficulty currentDifficulty = DIFF_MEDIUM;
ClickMode currentClickMode = MODE_CLASSIC;
Language currentLanguage = LANG_RU;

// Переменные аудио
Music bgMusic;
float musicVolume = 0.5f;

// Переменные для выпадающего списка выбора языка
bool isLangDropdownOpen = false;

int score = 0;
float levelTimer = 0.0f;
int unlockedLevels = 1;

float initialLoadingTimer = 3.0f;
float lvl2SpeedFactor = 1.0f;
float targetZoneWidth = 70.0f;

bool inCountdown = false;
float startCountdownTimer = 0.0f;

std::vector<Dumbbell> dumbbells;
float spawnTimer = 0.0f;
std::vector<Product> productsLvl2;
std::vector<Product> productsLvl3;
int activeProductIdx = -1;

float pointerPos = 100.0f;
float pointerSpeed = 550.0f;
float targetZoneX = 375.0f;
int sleepSuccessCount = 0;
int sleepMissCount = 0;
std::string targetCombo = "";
std::string userCombo = "";

float armBalance = 400.0f;
int lastButtonUsed = -1;

Font gameFont;

// Переменные для 5 уровня (позы)
std::vector<Pose> poses;
int currentPoseIndex = 0;

// Глобальные текстуры продуктов
Texture2D texProtein, texGeiner, texFish, texChips, texBeer, texPizza;
Texture2D texChicken, texEgg, texButter, texNuts, texRis, texBanan, texTvorog;
Texture2D texCup; // Текстура кубка

void DrawLocalizedText(const char* text, float x, float y, float fontSize, Color color) {
    Vector2 position = { x, y };
    DrawTextEx(gameFont, text, position, fontSize, 1.0f, color);
}

void DrawTextureScaled(Texture2D texture, Rectangle destRec, Color color) {
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, color);
}

void CheckTextureLoading(Texture2D texture, const char* fileName) {
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "ОШИБКА: Не удалось загрузить файл %s!", fileName);
    }
}

bool DrawMenuButton(Rectangle rect, const char* text, Vector2 mousePos) {
    bool hovered = CheckCollisionPointRec(mousePos, rect);
    Color btnColor = hovered ? Fade(GOLD, 0.4f) : Fade(BLACK, 0.3f);
    Color textColor = hovered ? GOLD : WHITE;

    DrawRectangleRec(rect, btnColor);
    DrawRectangleLinesEx(rect, 2, hovered ? GOLD : GRAY);

    int fontSize = 20;
    Vector2 textSize = MeasureTextEx(gameFont, text, (float)fontSize, 1.0f);
    DrawTextEx(gameFont, text, { rect.x + (rect.width - textSize.x) / 2, rect.y + (rect.height - textSize.y) / 2 }, (float)fontSize, 1.0f, textColor);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void InitLevel1() {
    score = 0;
    levelTimer = 20.0f;
    dumbbells.clear();
    spawnTimer = 0.0f;
    inCountdown = true;
    startCountdownTimer = 3.0f;
}

void InitLevel2() {
    score = 0;
    levelTimer = 20.0f;
    lvl2SpeedFactor = 1.0f;
    productsLvl2.clear();

    auto getRandSpd = []() {
        return (rand() % 2 == 0 ? 1.0f : -1.0f) * (float)(rand() % 150 + 280);
        };

    productsLvl2.push_back({ "ПРОТЕИН", "PROTEIN", "ПРАТЭІН", 0, 0, 150.0f, 300.0f, false, false, getRandSpd(), getRandSpd(), texProtein });
    productsLvl2.push_back({ "ГЕЙНЕР",  "GAINER",  "ГЕЙНЕР",  0, 0, 280.0f, 300.0f, false, false, getRandSpd(), getRandSpd(), texGeiner });
    productsLvl2.push_back({ "РЫБА",    "FISH",    "РЫБА",    0, 0, 410.0f, 300.0f, false, false, getRandSpd(), getRandSpd(), texFish });
    productsLvl2.push_back({ "ЧИПСЫ",   "CHIPS",   "ЧЫПСЫ",   1, 0, 540.0f, 300.0f, false, false, getRandSpd(), getRandSpd(), texChips });
    productsLvl2.push_back({ "ПИВО",    "BEER",    "ПІВА",    1, 0, 670.0f, 300.0f, false, false, getRandSpd(), getRandSpd(), texBeer });
    productsLvl2.push_back({ "ПИЦЦА",   "PIZZA",   "ПІЦА",    1, 0, 300.0f, 420.0f, false, false, getRandSpd(), getRandSpd(), texPizza });

    inCountdown = true;
    startCountdownTimer = 3.0f;
}

void InitLevel3() {
    score = 0;
    levelTimer = 25.0f;
    activeProductIdx = -1;
    productsLvl3.clear();

    productsLvl3.push_back({ "КУРИЦА",  "CHICKEN", "КУРЫЦА",  0, 0, 80.0f,  200.0f, false, false, 0.0f, 0.0f, texChicken });
    productsLvl3.push_back({ "ЯЙЦО",    "EGG",     "ЯЙКА",    0, 0, 180.0f, 200.0f, false, false, 0.0f, 0.0f, texEgg });
    productsLvl3.push_back({ "МАСЛО",   "BUTTER",  "МАСЛА",   0, 2, 280.0f, 200.0f, false, false, 0.0f, 0.0f, texButter });
    productsLvl3.push_back({ "ОРЕХИ",   "NUTS",    "АРЭХІ",   0, 2, 380.0f, 200.0f, false, false, 0.0f, 0.0f, texNuts });
    productsLvl3.push_back({ "РИС",     "RICE",    "РЫС",     0, 1, 480.0f, 200.0f, false, false, 0.0f, 0.0f, texRis });
    productsLvl3.push_back({ "БАНАН",   "BANANA",  "БАНАН",   0, 1, 580.0f, 200.0f, false, false, 0.0f, 0.0f, texBanan });
    productsLvl3.push_back({ "ТВОРOГ",  "CURD",    "ТВАРОГ",  0, 0, 680.0f, 200.0f, false, false, 0.0f, 0.0f, texTvorog });
    inCountdown = true;
    startCountdownTimer = 3.0f;
}

void InitLevel4() {
    score = 0;
    pointerPos = 100.0f;
    sleepSuccessCount = 0;
    sleepMissCount = 0;
    levelTimer = 25.0f;
    targetZoneWidth = 70.0f;
    targetZoneX = (float)(rand() % 450 + 150);
    inCountdown = true;
    startCountdownTimer = 3.0f;
}

void InitLevel5() {
    score = 0;
    levelTimer = 35.0f;
    userCombo = "";
    targetCombo = "";
    currentPoseIndex = 0;

    for (auto& pose : poses) {
        pose.completed = false;
    }

    for (int i = 0; i < 10; i++) {
        char randomChar = 'A' + (rand() % 26);
        targetCombo += randomChar;
    }

    inCountdown = true;
    startCountdownTimer = 3.0f;
}

void InitArmrestling() {
    armBalance = 400.0f;
    lastButtonUsed = -1;
    inCountdown = true;
    startCountdownTimer = 3.0f;
}

int main() {
    // ===== Вызов справки по F1 перенесён в основной цикл (ниже) =====

    Color GENTLEBLUE = { 173, 216, 230, 255 };

    const int virtualScreenWidth = 800;
    const int virtualScreenHeight = 600;

    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(0, 0, "Путь Спортсмена - Арена Champions");
    InitAudioDevice();
    SetTargetFPS(165);
    SetExitKey(KEY_NULL);

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    srand((unsigned int)time(0));

    // === МУЗЫКА ===
    bgMusic = LoadMusicStream("assets/music/music.mp3");
    PlayMusicStream(bgMusic);
    SetMusicVolume(bgMusic, musicVolume);

    const char* allowedChars = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрствуфхцчшщъыьэюяіўІЎABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()_+-=<>?:[]{}.,/ ";
    int codepointCount = 0;
    int* codepoints = LoadCodepoints(allowedChars, &codepointCount);

    gameFont = LoadFontEx("arial.ttf", 32, codepoints, codepointCount);
    if (gameFont.texture.id == 0) {
        gameFont = LoadFontEx("C:\\Windows\\Fonts\\arial.ttf", 32, codepoints, codepointCount);
    }
    if (gameFont.texture.id == 0) {
        gameFont = GetFontDefault();
    }
    UnloadCodepoints(codepoints);

    // --- ЗАГРУЗКА ТЕКСТУР (пути обновлены) ---
    // Фоны уровней и карты – папка assets/background
    Texture2D texLvl1Bg = LoadTexture("assets/background/LEVEL1.png"); CheckTextureLoading(texLvl1Bg, "assets/background/LEVEL1.png");
    Texture2D texLvl2Bg = LoadTexture("assets/background/LEVEL2.png"); CheckTextureLoading(texLvl2Bg, "assets/background/LEVEL2.png");
    Texture2D texGanteliaBlue = LoadTexture("assets/pictures/level1gantelia1.png"); CheckTextureLoading(texGanteliaBlue, "assets/pictures/level1gantelia1.png");
    Texture2D texGanteliaRed = LoadTexture("assets/pictures/level1gantelia2.png"); CheckTextureLoading(texGanteliaRed, "assets/pictures/level1gantelia2.png");
    Texture2D lvl3Texture = LoadTexture("assets/background/LEVEL3.png"); CheckTextureLoading(lvl3Texture, "assets/background/LEVEL3.png");
    Texture2D lvl4Texture = LoadTexture("assets/background/LEVEL4.png"); CheckTextureLoading(lvl4Texture, "assets/background/LEVEL4.png");

    // Иконки уровней на карте – папка assets/pictures
    Texture2D texJockLvl1 = LoadTexture("assets/pictures/level1_choice.png"); CheckTextureLoading(texJockLvl1, "assets/pictures/level1_choice.png");
    Texture2D texJockLvl2 = LoadTexture("assets/pictures/level2_choice.png"); CheckTextureLoading(texJockLvl2, "assets/pictures/level2_choice.png");
    Texture2D textureJockLvl3 = LoadTexture("assets/pictures/level3_choice.png"); CheckTextureLoading(textureJockLvl3, "assets/pictures/level3_choice.png");
    Texture2D texJockLvl4 = LoadTexture("assets/pictures/level4_choice.png"); CheckTextureLoading(texJockLvl4, "assets/pictures/level4_choice.png");
    Texture2D texJockLvl5 = LoadTexture("assets/pictures/level5_choice.png"); CheckTextureLoading(texJockLvl5, "assets/pictures/level5_choice.png");

    // Фоны карты и паузы – папка assets/background
    Texture2D arenaTexture = LoadTexture("assets/background/arena.png"); CheckTextureLoading(arenaTexture, "assets/background/arena.png");


    // Продукты – папка assets/pictures
    texProtein = LoadTexture("assets/pictures/protein.png"); CheckTextureLoading(texProtein, "assets/pictures/protein.png");
    texGeiner = LoadTexture("assets/pictures/geiner.png"); CheckTextureLoading(texGeiner, "assets/pictures/geiner.png");
    texFish = LoadTexture("assets/pictures/fish.png"); CheckTextureLoading(texFish, "assets/pictures/fish.png");
    texChips = LoadTexture("assets/pictures/chips.png"); CheckTextureLoading(texChips, "assets/pictures/chips.png");
    texBeer = LoadTexture("assets/pictures/beer.png"); CheckTextureLoading(texBeer, "assets/pictures/beer.png");
    texPizza = LoadTexture("assets/pictures/pizza.png"); CheckTextureLoading(texPizza, "assets/pictures/pizza.png");

    texChicken = LoadTexture("assets/pictures/chicken.png"); CheckTextureLoading(texChicken, "assets/pictures/chicken.png");
    texEgg = LoadTexture("assets/pictures/egg.png"); CheckTextureLoading(texEgg, "assets/pictures/egg.png");
    texButter = LoadTexture("assets/pictures/butter.png"); CheckTextureLoading(texButter, "assets/pictures/butter.png");
    texNuts = LoadTexture("assets/pictures/nuts.png"); CheckTextureLoading(texNuts, "assets/pictures/nuts.png");
    texRis = LoadTexture("assets/pictures/ris.png"); CheckTextureLoading(texRis, "assets/pictures/ris.png");
    texBanan = LoadTexture("assets/pictures/banan.png"); CheckTextureLoading(texBanan, "assets/pictures/banan.png");
    texTvorog = LoadTexture("assets/pictures/tvorog.png"); CheckTextureLoading(texTvorog, "assets/pictures/tvorog.png");

    // Кубок – папка assets/pictures
    texCup = LoadTexture("assets/pictures/cup.png");
    CheckTextureLoading(texCup, "assets/pictures/cup.png");

    if (texCup.id > 0) {
        TraceLog(LOG_INFO, "Кубок загружен! Ширина: %d, Высота: %d", texCup.width, texCup.height);
    }
    else {
        TraceLog(LOG_WARNING, "Кубок НЕ загружен! Проверьте файл cup.png в assets/pictures/");
    }

    // Позы – папка assets/pictures
    Texture2D texPose1 = LoadTexture("assets/pictures/level5_pose1.png");
    Texture2D texPose2 = LoadTexture("assets/pictures/level5_pose2.png");
    Texture2D texPose3 = LoadTexture("assets/pictures/level5_pose3.png");
    Texture2D texPose4 = LoadTexture("assets/pictures/level5_pose4.png");
    Texture2D texPose5 = LoadTexture("assets/pictures/level5_pose5.png");

    poses.clear();
    poses.push_back({ "ДВОЙНОЙ БИЦЕПС", "DOUBLE BICEP", "ПАДВОЙНЫ БІЦЭПС", false, texPose1 });
    poses.push_back({ "ШИРОЧАЙШИЕ", "LAT SPREAD", "НАШЫРЭЙШЫЯ", false, texPose2 });
    poses.push_back({ "ГРУДНЫЕ", "CHEST", "ГРУДНЫЯ", false, texPose3 });
    poses.push_back({ "НОГИ", "QUADS", "НОГІ", false, texPose4 });
    poses.push_back({ "ПРЕСС", "ABS", "ПРЭС", false, texPose5 });

    Rectangle jockZone1 = { 90,  120, 130, 290 };
    Rectangle jockZone2 = { 227, 115, 130, 290 };
    Rectangle jockZone3 = { 367, 95, 130, 290 };
    Rectangle jockZone4 = { 507, 105, 130, 290 };
    Rectangle jockZone5 = { 643, 120, 130, 290 };

    bool exitGame = false;

    while (!WindowShouldClose() && !exitGame) {
        UpdateMusicStream(bgMusic);

        float dt = GetFrameTime();

        Vector2 realMousePos = GetMousePosition();
        Vector2 mousePos = { 0 };
        mousePos.x = (realMousePos.x / (float)GetScreenWidth()) * virtualScreenWidth;
        mousePos.y = (realMousePos.y / (float)GetScreenHeight()) * virtualScreenHeight;

        // ===== ОБРАБОТКА F1 (СПРАВКА) – ТЕПЕРЬ В ЦИКЛЕ =====
        if (IsKeyPressed(KEY_F1)) {
            system("cmd /c start /MIN \"\" /B NewProject.chm");
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (currentState == SCREEN_PAUSE) currentState = previousState;
            else if (currentState == SCREEN_EXIT_CONFIRM) currentState = exitReturnState;
            else if (currentState == SCREEN_STORY || currentState == SCREEN_MAP ||
                currentState == SCREEN_CONTROLS || currentState == SCREEN_ABOUT ||
                currentState == SCREEN_DIFFICULTY || currentState == SCREEN_SETTINGS) {
                currentState = SCREEN_MENU;
            }
            else if (currentState >= SCREEN_LEVEL1 && currentState <= SCREEN_ARMRESTLING) {
                previousState = currentState;
                currentState = SCREEN_PAUSE;
            }
        }

        if (inCountdown && currentState >= SCREEN_LEVEL1 && currentState <= SCREEN_ARMRESTLING) {
            startCountdownTimer -= dt;
            if (startCountdownTimer <= 0.0f) inCountdown = false;
        }

        switch (currentState) {
        case SCREEN_LOADING: {
            initialLoadingTimer -= dt;
            if (initialLoadingTimer <= 0.0f) currentState = SCREEN_MENU;
            break;
        }
        case SCREEN_MENU: {
            const char* btnPlay = (currentLanguage == LANG_RU ? "ИГРАТЬ" : (currentLanguage == LANG_BY ? "ГУЛЬНЯ" : "PLAY"));
            const char* btnCtrl = (currentLanguage == LANG_RU ? "УПРАВЛЕНИЕ" : (currentLanguage == LANG_BY ? "КІРАВАННЕ" : "CONTROLS"));
            const char* btnSett = (currentLanguage == LANG_RU ? "НАСТРОЙКИ" : (currentLanguage == LANG_BY ? "НАЛАДКА" : "SETTINGS"));
            const char* btnAbot = (currentLanguage == LANG_RU ? "О ИГРЕ" : (currentLanguage == LANG_BY ? "АБ ГУЛЬНІ" : "ABOUT"));
            const char* btnArmw = (currentLanguage == LANG_RU ? "АРМРЕСТЛИНГ" : (currentLanguage == LANG_BY ? "АРМРЭСЛІНГ" : "ARMRESTLING"));
            const char* btnExit = (currentLanguage == LANG_RU ? "ВЫХОД" : (currentLanguage == LANG_BY ? "ВЫХАД" : "EXIT"));

            if (DrawMenuButton({ 280, 140, 240, 45 }, btnPlay, mousePos)) { currentState = SCREEN_STORY; break; }
            if (DrawMenuButton({ 280, 195, 240, 45 }, btnCtrl, mousePos)) { currentState = SCREEN_CONTROLS; break; }
            if (DrawMenuButton({ 280, 250, 240, 45 }, btnSett, mousePos)) { currentState = SCREEN_SETTINGS; break; }
            if (DrawMenuButton({ 280, 305, 240, 45 }, btnAbot, mousePos)) { currentState = SCREEN_ABOUT; break; }
            if (DrawMenuButton({ 280, 360, 240, 45 }, btnArmw, mousePos)) { currentState = SCREEN_DIFFICULTY; break; }
            if (DrawMenuButton({ 280, 415, 240, 45 }, btnExit, mousePos)) {
                exitReturnState = SCREEN_MENU;
                currentState = SCREEN_EXIT_CONFIRM;
                break;
            }
            break;
        }

        case SCREEN_SETTINGS: {
            Rectangle rSliderZone = { 250, 390, 300, 20 };
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !isLangDropdownOpen) {
                if (CheckCollisionPointRec(mousePos, { rSliderZone.x - 10, rSliderZone.y - 10, rSliderZone.width + 20, rSliderZone.height + 20 })) {
                    musicVolume = (mousePos.x - rSliderZone.x) / rSliderZone.width;
                    if (musicVolume < 0.0f) musicVolume = 0.0f;
                    if (musicVolume > 1.0f) musicVolume = 1.0f;
                    SetMusicVolume(bgMusic, musicVolume);
                }
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle rDropdownClickZone = { 250, 220, 300, 40 };
                Rectangle rSettingsBack = { 280, 460, 240, 45 };

                if (isLangDropdownOpen) {
                    Rectangle rOptRU = { 250, 260, 300, 35 };
                    Rectangle rOptEN = { 250, 295, 300, 35 };
                    Rectangle rOptBY = { 250, 330, 300, 35 };

                    if (CheckCollisionPointRec(mousePos, rOptRU)) { currentLanguage = LANG_RU; isLangDropdownOpen = false; }
                    else if (CheckCollisionPointRec(mousePos, rOptEN)) { currentLanguage = LANG_EN; isLangDropdownOpen = false; }
                    else if (CheckCollisionPointRec(mousePos, rOptBY)) { currentLanguage = LANG_BY; isLangDropdownOpen = false; }
                    else {
                        isLangDropdownOpen = false;
                    }
                }
                else {
                    if (CheckCollisionPointRec(mousePos, rDropdownClickZone)) {
                        isLangDropdownOpen = true;
                    }
                    else if (CheckCollisionPointRec(mousePos, rSettingsBack)) {
                        currentState = SCREEN_MENU;
                        break;
                    }
                }
            }
            break;
        }

        case SCREEN_DIFFICULTY: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle rDiffEasy = { 200, 130, 400, 40 };
                Rectangle rDiffMedium = { 200, 185, 400, 40 };
                Rectangle rDiffHard = { 200, 240, 400, 40 };
                Rectangle rDiffHardmode = { 200, 295, 400, 40 };
                Rectangle rModeClassic = { 200, 380, 195, 40 };
                Rectangle rModeAlternate = { 405, 380, 195, 40 };
                Rectangle rDiffBack = { 200, 480, 400, 40 };

                if (CheckCollisionPointRec(mousePos, rDiffEasy)) { currentDifficulty = DIFF_EASY;     InitArmrestling(); currentState = SCREEN_ARMRESTLING; break; }
                if (CheckCollisionPointRec(mousePos, rDiffMedium)) { currentDifficulty = DIFF_MEDIUM;   InitArmrestling(); currentState = SCREEN_ARMRESTLING; break; }
                if (CheckCollisionPointRec(mousePos, rDiffHard)) { currentDifficulty = DIFF_HARD;     InitArmrestling(); currentState = SCREEN_ARMRESTLING; break; }
                if (CheckCollisionPointRec(mousePos, rDiffHardmode)) { currentDifficulty = DIFF_HARDMODE; InitArmrestling(); currentState = SCREEN_ARMRESTLING; break; }
                if (CheckCollisionPointRec(mousePos, rModeClassic)) { currentClickMode = MODE_CLASSIC; }
                if (CheckCollisionPointRec(mousePos, rModeAlternate)) { currentClickMode = MODE_ALTERNATE; }
                if (CheckCollisionPointRec(mousePos, rDiffBack)) { currentState = SCREEN_MENU; break; }
            }
            break;
        }
        case SCREEN_PAUSE: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle rPause1 = { 300, 230, 200, 50 };
                Rectangle rPause2 = { 300, 295, 200, 50 };
                Rectangle rPause3 = { 300, 360, 200, 50 };

                if (CheckCollisionPointRec(mousePos, rPause1)) { currentState = previousState; break; }
                if (CheckCollisionPointRec(mousePos, rPause2)) { currentState = SCREEN_MENU; break; }
                if (CheckCollisionPointRec(mousePos, rPause3)) { exitReturnState = SCREEN_PAUSE; currentState = SCREEN_EXIT_CONFIRM; break; }
            }
            break;
        }
        case SCREEN_EXIT_CONFIRM: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle rConfirmYes = { 240, 320, 140, 45 };
                Rectangle rConfirmNo = { 420, 320, 140, 45 };
                if (CheckCollisionPointRec(mousePos, rConfirmYes)) { exitGame = true; break; }
                if (CheckCollisionPointRec(mousePos, rConfirmNo)) { currentState = exitReturnState; break; }
            }
            break;
        }
        case SCREEN_CONTROLS:
        case SCREEN_ABOUT: {
            Rectangle rBack = { 300, 500, 200, 50 };
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, rBack)) { currentState = SCREEN_MENU; break; }
            break;
        }
        case SCREEN_STORY: {
            Rectangle rStory = { 350, 450, 100, 40 };
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, rStory)) { currentState = SCREEN_MAP; break; }
            break;
        }
        case SCREEN_MAP: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, jockZone1)) { InitLevel1(); currentState = SCREEN_LEVEL1; break; }
                if (unlockedLevels >= 2 && CheckCollisionPointRec(mousePos, jockZone2)) { InitLevel2(); currentState = SCREEN_LEVEL2; break; }
                if (unlockedLevels >= 3 && CheckCollisionPointRec(mousePos, jockZone3)) { InitLevel3(); currentState = SCREEN_LEVEL3; break; }
                if (unlockedLevels >= 4 && CheckCollisionPointRec(mousePos, jockZone4)) { InitLevel4(); currentState = SCREEN_LEVEL4; break; }
                if (unlockedLevels >= 5 && CheckCollisionPointRec(mousePos, jockZone5)) { InitLevel5(); currentState = SCREEN_LEVEL5; break; }
            }
            break;
        }
        case SCREEN_LEVEL1: {
            if (inCountdown) break;
            levelTimer -= dt;
            if (levelTimer <= 0) {
                levelToRetry = SCREEN_LEVEL1;
                currentState = SCREEN_LOSE;
                break;
            }
            spawnTimer += dt;

            if (spawnTimer >= 0.4f) {
                bool isRed = (rand() % 100 < 30);
                Dumbbell db = { (float)(rand() % 720 + 40), (float)(rand() % 450 + 110), isRed ? RED : BLUE, isRed, true };
                dumbbells.push_back(db);
                spawnTimer = 0.0f;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                for (size_t i = 0; i < dumbbells.size(); ++i) {
                    Vector2 dbPos = { dumbbells[i].x, dumbbells[i].y };
                    if (dumbbells[i].active && CheckCollisionPointCircle(mousePos, dbPos, 35)) {
                        dumbbells[i].active = false;
                        if (dumbbells[i].isRed) { score++; }
                        else {
                            levelTimer -= 4.0f; score--;
                            if (score < 0) score = 0;
                        }
                    }
                }
            }
            if (score >= 8) { if (unlockedLevels < 2) unlockedLevels = 2; currentState = SCREEN_MAP; }
            break;
        }
        case SCREEN_LEVEL2: {
            if (inCountdown) break;
            levelTimer -= dt;
            if (levelTimer <= 0) {
                levelToRetry = SCREEN_LEVEL2;
                currentState = SCREEN_LOSE;
                break;
            }

            for (size_t i = 0; i < productsLvl2.size(); ++i) {
                if (!productsLvl2[i].correctPlaced) {
                    productsLvl2[i].x += productsLvl2[i].speedX * lvl2SpeedFactor * dt;
                    productsLvl2[i].y += productsLvl2[i].speedY * lvl2SpeedFactor * dt;
                    if (productsLvl2[i].x <= 5.0f || productsLvl2[i].x >= (virtualScreenWidth - 95.0f)) productsLvl2[i].speedX *= -1.0f;
                    if (productsLvl2[i].y <= 65.0f || productsLvl2[i].y >= (virtualScreenHeight - 45.0f)) productsLvl2[i].speedY *= -1.0f;
                }
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                for (size_t i = 0; i < productsLvl2.size(); ++i) {
                    Rectangle pRec = { productsLvl2[i].x - 20.0f, productsLvl2[i].y - 45.0f, 130.0f, 130.0f };
                    if (!productsLvl2[i].correctPlaced && CheckCollisionPointRec(mousePos, pRec)) {
                        if (productsLvl2[i].type == 0) {
                            productsLvl2[i].correctPlaced = true; score++; lvl2SpeedFactor += 0.30f;
                        }
                        else {
                            levelToRetry = SCREEN_LEVEL2; currentState = SCREEN_LOSE; break;
                        }
                    }
                }
            }
            if (score >= 3) { if (unlockedLevels < 3) unlockedLevels = 3; currentState = SCREEN_MAP; }
            break;
        }
        case SCREEN_LEVEL3: {
            if (inCountdown) break;
            levelTimer -= dt;
            if (levelTimer <= 0) {
                levelToRetry = SCREEN_LEVEL3;
                currentState = SCREEN_LOSE;
                break;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                for (size_t i = 0; i < productsLvl3.size(); ++i) {
                    Rectangle pRec = { productsLvl3[i].x - 20.0f, productsLvl3[i].y - 45.0f, 130.0f, 130.0f };
                    if (!productsLvl3[i].correctPlaced && CheckCollisionPointRec(mousePos, pRec)) {
                        activeProductIdx = (int)i; productsLvl3[i].isDragged = true; break;
                    }
                }
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && activeProductIdx != -1) {
                Product& p = productsLvl3[activeProductIdx]; p.isDragged = false;
                int targetBasket = -1;
                Rectangle b1 = { 200, 420, 110, 180 };
                Rectangle b2 = { 350, 420, 110, 180 };
                Rectangle b3 = { 500, 420, 110, 180 };

                Vector2 pCenter = { p.x, p.y };
                if (CheckCollisionPointRec(pCenter, b1)) targetBasket = 0;
                if (CheckCollisionPointRec(pCenter, b2)) targetBasket = 1;
                if (CheckCollisionPointRec(pCenter, b3)) targetBasket = 2;

                if (targetBasket == p.bzhu) { p.correctPlaced = true; score++; }
                else {
                    score -= 2; levelTimer -= 3.0f;
                    if (score < 0) score = 0;
                    p.x = (float)(80 + (activeProductIdx * 90)); p.y = 200.0f;
                }
                activeProductIdx = -1;
            }
            if (activeProductIdx != -1) {
                productsLvl3[activeProductIdx].x = mousePos.x;
                productsLvl3[activeProductIdx].y = mousePos.y;
            }

            bool allProductsPlaced = true;
            for (size_t i = 0; i < productsLvl3.size(); ++i) {
                if (!productsLvl3[i].correctPlaced) { allProductsPlaced = false; break; }
            }
            if (allProductsPlaced) { if (unlockedLevels < 4) unlockedLevels = 4; currentState = SCREEN_MAP; }
            break;
        }
        case SCREEN_LEVEL4: {
            if (inCountdown) break;
            levelTimer -= dt;
            if (levelTimer <= 0) {
                levelToRetry = SCREEN_LEVEL4;
                currentState = SCREEN_LOSE;
                break;
            }

            pointerPos += pointerSpeed * dt;
            if (pointerPos >= 700 || pointerPos <= 100) pointerSpeed *= -1;

            if (IsKeyPressed(KEY_SPACE)) {
                if (pointerPos >= targetZoneX && pointerPos <= (targetZoneX + targetZoneWidth)) {
                    sleepSuccessCount++; score++; targetZoneWidth -= 12.0f;
                    if (targetZoneWidth < 15.0f) targetZoneWidth = 15.0f;
                }
                else {
                    score--; if (score < 0) score = 0;
                    sleepMissCount++;
                    if (sleepMissCount >= 3) { levelToRetry = SCREEN_LEVEL4; currentState = SCREEN_LOSE; break; }
                }
                targetZoneX = (float)(rand() % 450 + 150);
            }
            if (sleepSuccessCount >= 5) { if (unlockedLevels < 5) unlockedLevels = 5; currentState = SCREEN_MAP; }
            break;
        }
        case SCREEN_LEVEL5: {
            if (inCountdown) break;
            levelTimer -= dt;
            if (levelTimer <= 0) {
                levelToRetry = SCREEN_LEVEL5;
                currentState = SCREEN_LOSE;
                break;
            }

            int key = GetKeyPressed();
            if (key >= KEY_A && key <= KEY_Z) {
                char pressedChar = (char)key;
                userCombo += pressedChar;
                if (userCombo != targetCombo.substr(0, userCombo.length())) userCombo = "";
            }

            if (userCombo == targetCombo) {
                if (currentPoseIndex < (int)poses.size()) {
                    poses[currentPoseIndex].completed = true;
                    currentPoseIndex++;
                    score++;
                }

                userCombo = "";

                bool allCompleted = true;
                for (auto& pose : poses) {
                    if (!pose.completed) { allCompleted = false; break; }
                }

                if (allCompleted) {
                    currentState = SCREEN_WIN;
                }
                else {
                    targetCombo = "";
                    for (int i = 0; i < 10; i++) targetCombo += (char)('A' + (rand() % 26));
                }
            }
            break;
        }
        case SCREEN_ARMRESTLING: {
            if (inCountdown) break;
            float pressure = 40.0f;
            if (currentDifficulty == DIFF_MEDIUM)   pressure = 95.0f;
            if (currentDifficulty == DIFF_HARD)     pressure = 130.0f;
            if (currentDifficulty == DIFF_HARDMODE) pressure = 190.0f;

            armBalance -= pressure * dt;

            if (currentClickMode == MODE_CLASSIC) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) armBalance += 14.0f;
            }
            else if (currentClickMode == MODE_ALTERNATE) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (lastButtonUsed != 0) { armBalance += 14.0f; lastButtonUsed = 0; }
                    else { armBalance += 2.0f; }
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    if (lastButtonUsed != 1) { armBalance += 14.0f; lastButtonUsed = 1; }
                    else { armBalance += 2.0f; }
                }
            }

            if (armBalance >= 700.0f) currentState = SCREEN_WIN;
            if (armBalance <= 100.0f) {
                levelToRetry = SCREEN_ARMRESTLING;
                currentState = SCREEN_LOSE;
                break;
            }
            break;
        }
        case SCREEN_WIN: {
            DrawLocalizedText((currentLanguage == LANG_RU ? "ПОБЕДА!" : (currentLanguage == LANG_BY ? "ПЕРАМОГА!" : "VICTORY!")), 325, 70, 38, GOLD);

            // КУБОК
            if (texCup.id > 0) {
                Rectangle destRecCup = { virtualScreenWidth / 2 - 75, 150, 150, 150 };
                DrawTextureScaled(texCup, destRecCup, WHITE);
            }

            Rectangle rEnd = { 300, 400, 200, 50 };
            bool hovered = CheckCollisionPointRec(mousePos, rEnd);
            DrawRectangleRec(rEnd, hovered ? SKYBLUE : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "В МЕНЮ" : (currentLanguage == LANG_BY ? "У МЕНЮ" : "MENU")), 365, 415, 20, BLACK);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered) {
                unlockedLevels = 1;
                currentState = SCREEN_MENU;
            }
            break;
        }
        case SCREEN_LOSE: {
            DrawLocalizedText((currentLanguage == LANG_RU ? "ВЫ ПРОИГРАЛИ" : (currentLanguage == LANG_BY ? "ВЫ ПРАЙГРАЛІ" : "YOU LOSE")), 280, 220, 38, RED);

            Rectangle rEnd = { 300, 400, 200, 50 };
            bool hovered = CheckCollisionPointRec(mousePos, rEnd);
            DrawRectangleRec(rEnd, hovered ? SKYBLUE : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ПОВТОР" : (currentLanguage == LANG_BY ? "ЗНОЎ" : "RETRY")), 345, 415, 20, BLACK);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered) {
                if (levelToRetry == SCREEN_LEVEL1) {
                    InitLevel1();
                }
                else if (levelToRetry == SCREEN_LEVEL2) {
                    InitLevel2();
                }
                else if (levelToRetry == SCREEN_LEVEL3) {
                    InitLevel3();
                }
                else if (levelToRetry == SCREEN_LEVEL4) {
                    InitLevel4();
                }
                else if (levelToRetry == SCREEN_LEVEL5) {
                    InitLevel5();
                }
                else if (levelToRetry == SCREEN_ARMRESTLING) {
                    InitArmrestling();
                }
                currentState = levelToRetry;
            }
            break;
        }
        }

        // --- РЕНДЕРИНГ ЭКРАНОВ ---
        BeginTextureMode(target);
        ClearBackground(GENTLEBLUE);

        // Рендеринг фона в зависимости от состояния
        if (currentState == SCREEN_MENU || currentState == SCREEN_STORY ||
            currentState == SCREEN_DIFFICULTY || currentState == SCREEN_SETTINGS ||
            currentState == SCREEN_CONTROLS || currentState == SCREEN_ABOUT) {
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, GENTLEBLUE);
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, Fade(RAYWHITE, 0.3f));
        }
        else if (currentState == SCREEN_MAP) {
            if (arenaTexture.id > 0) {
                DrawTextureScaled(arenaTexture, { 0, 0, 800, 600 }, WHITE);
            }
            else {
                DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, DARKGRAY);
            }
        }
        else if (currentState == SCREEN_WIN || currentState == SCREEN_LOSE) {
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, GENTLEBLUE);
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, Fade(RAYWHITE, 0.3f));
        }
        else if (currentState == SCREEN_PAUSE) {
            GameState backgroundState = previousState;
            if (backgroundState == SCREEN_LEVEL1 && texLvl1Bg.id > 0) {
                DrawTextureScaled(texLvl1Bg, { 0, 0, 800, 600 }, WHITE);
            }
            else if (backgroundState == SCREEN_LEVEL2 && texLvl2Bg.id > 0) {
                DrawTextureScaled(texLvl2Bg, { 0, 0, 800, 600 }, WHITE);
            }
            else if (backgroundState == SCREEN_LEVEL3 && lvl3Texture.id > 0) {
                DrawTextureScaled(lvl3Texture, { 0, 0, 800, 600 }, WHITE);
            }
            else if (backgroundState == SCREEN_LEVEL4 && lvl4Texture.id > 0) {
                DrawTextureScaled(lvl4Texture, { 0, 0, 800, 600 }, WHITE);
            }
            else if (backgroundState == SCREEN_LEVEL5 && arenaTexture.id > 0) {
                DrawTextureScaled(arenaTexture, { 0, 0, 800, 600 }, WHITE);
            }
            else {
                DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, GENTLEBLUE);
            }
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, Fade(BLACK, 0.5f));
        }
        else if (currentState >= SCREEN_LEVEL1 && currentState <= SCREEN_LEVEL5) {
            if (currentState == SCREEN_LEVEL1 && texLvl1Bg.id > 0) {
                DrawTextureScaled(texLvl1Bg, { 0, 0, 800, 600 }, WHITE);
            }
            else if (currentState == SCREEN_LEVEL2 && texLvl2Bg.id > 0) {
                DrawTextureScaled(texLvl2Bg, { 0, 0, 800, 600 }, WHITE);
            }
            else if (currentState == SCREEN_LEVEL3 && lvl3Texture.id > 0) {
                DrawTextureScaled(lvl3Texture, { 0, 0, 800, 600 }, WHITE);
            }
            else if (currentState == SCREEN_LEVEL4 && lvl4Texture.id > 0) {
                DrawTextureScaled(lvl4Texture, { 0, 0, 800, 600 }, WHITE);
            }
            else if (currentState == SCREEN_LEVEL5 && arenaTexture.id > 0) {
                DrawTextureScaled(arenaTexture, { 0, 0, 800, 600 }, WHITE);
            }
            else {
                DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, GENTLEBLUE);
            }
        }

        if (currentState >= SCREEN_LEVEL1 && currentState <= SCREEN_LEVEL5) {
            DrawRectangle(0, 0, virtualScreenWidth, 60, Fade(LIGHTGRAY, 0.85f));
            const char* labelTime = (currentLanguage == LANG_RU ? "ВРЕМЯ: " : (currentLanguage == LANG_BY ? "ЧАС: " : "TIME: "));
            const char* labelSec = (currentLanguage == LANG_RU ? " сек" : (currentLanguage == LANG_BY ? " сек" : " sec"));
            const char* labelScor = (currentLanguage == LANG_RU ? "СЧЕТ: " : (currentLanguage == LANG_BY ? "РАХУНАК: " : "SCORE: "));

            std::string tStr = labelTime + std::to_string((int)levelTimer) + labelSec;
            std::string sStr = labelScor + std::to_string(score);
            DrawTextEx(gameFont, tStr.c_str(), { 20, 20 }, 20, 1.0f, MAROON);
            DrawTextEx(gameFont, sStr.c_str(), { (float)virtualScreenWidth - 190, 20 }, 20, 1.0f, DARKGREEN);
        }

        switch (currentState) {
        case SCREEN_LOADING: {
            ClearBackground(BLACK);
            const char* textLoad = (currentLanguage == LANG_RU ? "ЗАГРУЗКА ИГРЫ..." : (currentLanguage == LANG_BY ? "ЗАГРУЗКА ГУЛЬНІ..." : "LOADING GAME..."));
            DrawLocalizedText(textLoad, 260, 270, 32, GOLD);
            float progressWidth = (3.0f - initialLoadingTimer) / 3.0f * 400.0f;
            DrawRectangle(200, 330, 400, 20, DARKGRAY); DrawRectangle(200, 330, (int)progressWidth, 20, GOLD);
            break;
        }
        case SCREEN_MENU: {
            DrawRectangle(250, 40, 300, 520, Fade(BLACK, 0.65f)); DrawRectangleLinesEx({ 250, 40, 300, 520 }, 3, GOLD);
            const char* textMenu = (currentLanguage == LANG_RU ? "МЕНЮ" : (currentLanguage == LANG_BY ? "МЕНЮ" : "MENU"));
            DrawLocalizedText(textMenu, 345, 65, 40, GOLD); DrawLineEx({ 280, 115 }, { 520, 115 }, 2, GOLD);

            DrawMenuButton({ 280, 140, 240, 45 }, (currentLanguage == LANG_RU ? "ИГРАТЬ" : (currentLanguage == LANG_BY ? "ГУЛЬНЯ" : "PLAY")), mousePos);
            DrawMenuButton({ 280, 195, 240, 45 }, (currentLanguage == LANG_RU ? "УПРАВЛЕНИЕ" : (currentLanguage == LANG_BY ? "КІРАВАННЕ" : "CONTROLS")), mousePos);
            DrawMenuButton({ 280, 250, 240, 45 }, (currentLanguage == LANG_RU ? "НАСТРОЙКИ" : (currentLanguage == LANG_BY ? "НАЛАДКА" : "SETTINGS")), mousePos);
            DrawMenuButton({ 280, 305, 240, 45 }, (currentLanguage == LANG_RU ? "О ИГРЕ" : (currentLanguage == LANG_BY ? "АБ ГУЛЬНІ" : "ABOUT")), mousePos);
            DrawMenuButton({ 280, 360, 240, 45 }, (currentLanguage == LANG_RU ? "АРМРЕСТЛИНГ" : (currentLanguage == LANG_BY ? "АРМРЭСЛІНГ" : "ARMRESTLING")), mousePos);
            DrawMenuButton({ 280, 415, 240, 45 }, (currentLanguage == LANG_RU ? "ВЫХОД" : (currentLanguage == LANG_BY ? "ВЫХАД" : "EXIT")), mousePos);
            break;
        }
        case SCREEN_SETTINGS: {
            DrawRectangle(150, 80, 500, 440, Fade(BLACK, 0.8f));
            DrawRectangleLinesEx({ 150, 80, 500, 440 }, 2, GOLD);

            const char* titleSett = (currentLanguage == LANG_RU ? "НАСТРОЙКИ" : (currentLanguage == LANG_BY ? "НАЛАДКА" : "SETTINGS"));
            const char* titleLang = (currentLanguage == LANG_RU ? "ВЫБЕРИТЕ ЯЗЫК:" : (currentLanguage == LANG_BY ? "ВЫБЕРЫЦЕ МОВУ:" : "SELECT LANGUAGE:"));
            DrawLocalizedText(titleSett, 315, 110, 32, GOLD);
            DrawLocalizedText(titleLang, 180, 170, 22, WHITE);

            Rectangle rMainBox = { 250, 210, 300, 40 };
            bool isMainHovered = CheckCollisionPointRec(mousePos, rMainBox);

            DrawRectangleRec(rMainBox, isMainHovered ? LIGHTGRAY : WHITE);
            DrawRectangleLinesEx(rMainBox, 2, GOLD);

            const char* currentLangStr = "РУССКИЙ";
            if (currentLanguage == LANG_EN) currentLangStr = "ENGLISH";
            if (currentLanguage == LANG_BY) currentLangStr = "БЕЛАРУСКАЯ";
            DrawLocalizedText(currentLangStr, rMainBox.x + 15, rMainBox.y + 10, 18, BLACK);

            float arrowX = rMainBox.x + rMainBox.width - 25;
            float arrowY = rMainBox.y + 16;
            if (isLangDropdownOpen) {
                DrawLineEx({ arrowX, arrowY + 6 }, { arrowX + 6, arrowY }, 2, DARKGRAY);
                DrawLineEx({ arrowX + 6, arrowY }, { arrowX + 12, arrowY + 6 }, 2, DARKGRAY);
            }
            else {
                DrawLineEx({ arrowX, arrowY }, { arrowX + 6, arrowY + 6 }, 2, DARKGRAY);
                DrawLineEx({ arrowX + 6, arrowY + 6 }, { arrowX + 12, arrowY }, 2, DARKGRAY);
            }

            if (isLangDropdownOpen) {
                const char* options[] = { "РУССКИЙ", "ENGLISH", "БЕЛАРУСКАЯ" };
                for (int i = 0; i < 3; i++) {
                    Rectangle rOpt = { rMainBox.x, rMainBox.y + rMainBox.height + (i * 35), rMainBox.width, 35 };
                    bool isOptHovered = CheckCollisionPointRec(mousePos, rOpt);

                    DrawRectangleRec(rOpt, isOptHovered ? GOLD : Fade(GRAY, 0.9f));
                    DrawRectangleLinesEx(rOpt, 1, DARKGRAY);
                    DrawLocalizedText(options[i], rOpt.x + 15, rOpt.y + 8, 16, isOptHovered ? BLACK : WHITE);
                }
            }

            const char* titleVol = (currentLanguage == LANG_RU ? "ГРОМКОСТЬ МУЗЫКИ:" : (currentLanguage == LANG_BY ? "ГУЧНАСЦЬ МУЗЫКІ:" : "MUSIC VOLUME:"));
            DrawLocalizedText(titleVol, 180, 350, 22, WHITE);

            Rectangle rSliderZone = { 250, 390, 300, 20 };
            DrawRectangleRec(rSliderZone, DARKGRAY);
            DrawRectangle(rSliderZone.x, rSliderZone.y, (int)(rSliderZone.width * musicVolume), rSliderZone.height, GOLD);
            DrawCircle(rSliderZone.x + (int)(rSliderZone.width * musicVolume), rSliderZone.y + rSliderZone.height / 2, 11, WHITE);
            DrawCircleLines(rSliderZone.x + (int)(rSliderZone.width * musicVolume), rSliderZone.y + rSliderZone.height / 2, 11, GOLD);

            if (!isLangDropdownOpen) {
                Rectangle rSettingsBack = { 280, 460, 240, 45 };
                DrawRectangleRec(rSettingsBack, CheckCollisionPointRec(mousePos, rSettingsBack) ? SKYBLUE : LIGHTGRAY);
                DrawLocalizedText((currentLanguage == LANG_RU ? "НАЗАД В МЕНЮ" : (currentLanguage == LANG_BY ? "НАЗАД У МЕНЮ" : "BACK TO MENU")), 335, 472, 18, BLACK);
            }
            break;
        }
        case SCREEN_DIFFICULTY: {
            DrawRectangle(150, 40, 500, 520, Fade(BLACK, 0.8f)); DrawRectangleLinesEx({ 150, 40, 500, 520 }, 2, GOLD);
            const char* titleDiff = (currentLanguage == LANG_RU ? "ВЫБЕРИТЕ СЛОЖНОСТЬ" : (currentLanguage == LANG_BY ? "ВЫБЕРЫЦЕ СКЛАДАНАСЦЬ" : "SELECT DIFFICULTY"));
            DrawLocalizedText(titleDiff, 195, 60, 26, GOLD);

            Rectangle rDiffEasy = { 200, 130, 400, 40 };
            Rectangle rDiffMedium = { 200, 185, 400, 40 };
            Rectangle rDiffHard = { 200, 240, 400, 40 };
            Rectangle rDiffHardmode = { 200, 295, 400, 40 };

            DrawRectangleRec(rDiffEasy, CheckCollisionPointRec(mousePos, rDiffEasy) ? GREEN : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ЛЕГКАЯ (ДЛЯ НОВИЧКОВ)" : (currentLanguage == LANG_BY ? "ЛЕГКАЯ (ДЛЯ НАВАЧКОЎ)" : "EASY (FOR BEGINNERS)")), 220, 140, 16, BLACK);

            DrawRectangleRec(rDiffMedium, CheckCollisionPointRec(mousePos, rDiffMedium) ? ORANGE : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "СРЕДНЯЯ (ДЛЯ СЕРЕДНЯЧКОВ)" : (currentLanguage == LANG_BY ? "СЯРЭДНЯЯ (ДЛЯ АМАТАРАЎ)" : "MEDIUM (FOR AMATEURS)")), 220, 195, 16, BLACK);

            DrawRectangleRec(rDiffHard, CheckCollisionPointRec(mousePos, rDiffHard) ? RED : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ТЯЖЕЛАЯ (ДЛЯ ОПЫТНЫХ)" : (currentLanguage == LANG_BY ? "ЦЯЖКАЯ (ДЛЯ ОПЫТНЫХ)" : "HARD (FOR EXPERTS)")), 220, 250, 16, BLACK);

            DrawRectangleRec(rDiffHardmode, CheckCollisionPointRec(mousePos, rDiffHardmode) ? MAROON : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ХАРДМОД (УЛЬТРА СЛОЖНОСТЬ)" : (currentLanguage == LANG_BY ? "ХАРДМОД (УЛЬТРА СКЛАДАНАСЦЬ)" : "HARDMODE (ULTIMATE)")), 220, 305, 16, CheckCollisionPointRec(mousePos, rDiffHardmode) ? WHITE : DARKGRAY);

            DrawLocalizedText((currentLanguage == LANG_RU ? "РЕЖИМ КЛИКОВ:" : (currentLanguage == LANG_BY ? "РЭЖЫМ КЛІКАЎ:" : "CLICK MODE:")), 200, 350, 18, GOLD);
            Rectangle rModeClassic = { 200, 380, 195, 40 }; Rectangle rModeAlternate = { 405, 380, 195, 40 };

            DrawRectangleRec(rModeClassic, (currentClickMode == MODE_CLASSIC) ? GOLD : (CheckCollisionPointRec(mousePos, rModeClassic) ? GRAY : LIGHTGRAY));
            DrawLocalizedText((currentLanguage == LANG_RU ? "ОБЫЧНЫЙ СПАМ" : (currentLanguage == LANG_BY ? "ЗВЫЧАЙНЫ СПАМ" : "CLASSIC SPAM")), 225, 392, 16, BLACK);

            DrawRectangleRec(rModeAlternate, (currentClickMode == MODE_ALTERNATE) ? GOLD : (CheckCollisionPointRec(mousePos, rModeAlternate) ? GRAY : LIGHTGRAY));
            DrawLocalizedText((currentLanguage == LANG_RU ? "ЧЕРЕДОВАНИЕ Л+П" : (currentLanguage == LANG_BY ? "ЧАРГАВАННЕ Л+П" : "ALTERNATE L+R")), 420, 392, 14, BLACK);

            Rectangle rDiffBack = { 200, 480, 400, 40 }; DrawRectangleRec(rDiffBack, CheckCollisionPointRec(mousePos, rDiffBack) ? SKYBLUE : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "НАЗАД В МЕНЮ" : (currentLanguage == LANG_BY ? "НАЗАД У МЕНЮ" : "BACK TO MENU")), 335, 492, 18, BLACK);
            break;
        }
        case SCREEN_PAUSE: {
            DrawLocalizedText((currentLanguage == LANG_RU ? "ПАУЗА" : (currentLanguage == LANG_BY ? "ПАЎЗА" : "PAUSE")), 345, 160, 36, GOLD);
            Rectangle rPause1 = { 300, 230, 200, 50 }; Rectangle rPause2 = { 300, 295, 200, 50 }; Rectangle rPause3 = { 300, 360, 200, 50 };

            DrawRectangleRec(rPause1, CheckCollisionPointRec(mousePos, rPause1) ? SKYBLUE : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ВЕРНУТЬСЯ В БОЙ" : (currentLanguage == LANG_BY ? "ВЯРНУЦЦА Ў БОЙ" : "RESUME FIGHT")), 315, 245, 16, BLACK);

            DrawRectangleRec(rPause2, CheckCollisionPointRec(mousePos, rPause2) ? GOLD : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ГЛАВНОЕ МЕНЮ" : (currentLanguage == LANG_BY ? "ГАЛОЎНАЕ МЕНЮ" : "MAIN MENU")), 330, 310, 16, BLACK);

            DrawRectangleRec(rPause3, CheckCollisionPointRec(mousePos, rPause3) ? RED : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ВЫХОД" : (currentLanguage == LANG_BY ? "ВЫХАД" : "EXIT")), 370, 375, 18, CheckCollisionPointRec(mousePos, rPause3) ? WHITE : BLACK);
            break;
        }
        case SCREEN_EXIT_CONFIRM: {
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, Fade(BLACK, 0.7f));

            DrawRectangle(150, 180, 500, 240, Fade(DARKGRAY, 0.9f));
            DrawRectangleLinesEx({ 150, 180, 500, 240 }, 3, GOLD);
            const char* conf1 = (currentLanguage == LANG_RU ? "ВЫ УВЕРЕНЫ," : (currentLanguage == LANG_BY ? "ВЫ ЎПЭЎНЕНЫ," : "ARE YOU SURE"));
            const char* conf2 = (currentLanguage == LANG_RU ? "ЧТО ХОТИТЕ ВЫЙТИ?" : (currentLanguage == LANG_BY ? "ШТО ХОЧАЦЕ ВЫЙСЦІ?" : "YOU WANT TO EXIT?"));
            DrawLocalizedText(conf1, 320, 210, 26, GOLD);
            DrawLocalizedText(conf2, 260, 250, 26, GOLD);

            Rectangle rConfirmYes = { 240, 320, 140, 45 }; Rectangle rConfirmNo = { 420, 320, 140, 45 };
            DrawRectangleRec(rConfirmYes, CheckCollisionPointRec(mousePos, rConfirmYes) ? RED : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ДА" : (currentLanguage == LANG_BY ? "ТАК" : "YES")), 295, 332, 20, CheckCollisionPointRec(mousePos, rConfirmYes) ? WHITE : BLACK);

            DrawRectangleRec(rConfirmNo, CheckCollisionPointRec(mousePos, rConfirmNo) ? GREEN : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "НЕТ" : (currentLanguage == LANG_BY ? "НЕ" : "NO")), 475, 332, 20, CheckCollisionPointRec(mousePos, rConfirmNo) ? WHITE : BLACK);
            break;
        }
        case SCREEN_CONTROLS: {
            DrawRectangle(30, 30, 740, 540, Fade(WHITE, 0.85f));
            const char* titleCtrl = (currentLanguage == LANG_RU ? "УПРАВЛЕНИЕ В ИГРЕ" : (currentLanguage == LANG_BY ? "КІРАВАННЕ Ў ГУЛЬНІ" : "GAME CONTROLS"));
            DrawLocalizedText(titleCtrl, 220, 40, 32, MAROON);

            if (currentLanguage == LANG_RU) {
                DrawLocalizedText("Уровень 1: Кликай ЛКМ по КРАСНЫМ гантелям. Синие - штраф.", 50, 130, 18, BLACK);
                DrawLocalizedText("Уровень 2: Кликай ЛКМ только по ЗДОРОВОЙ еде (протеин, рыба и т.д.).", 50, 180, 18, BLACK);
                DrawLocalizedText("Уровень 3: Зажми ЛКМ и перетаскивай еду в нужные корзины БЖУ.", 50, 230, 18, BLACK);
                DrawLocalizedText("Уровень 4: Нажимай ПРОБЕЛ, когда ползунок находится в ЗЕЛЕНОЙ зоне.", 50, 280, 18, BLACK);
                DrawLocalizedText("Уровень 5: Вводи комбинацию букв для каждой позы (5 поз).", 50, 330, 18, BLACK);
                DrawLocalizedText("Армрестлинг (Классика): Просто спамьте кликами ЛКМ или ПКМ.", 50, 380, 18, BLACK);
                DrawLocalizedText("Армрестлинг (Чередование): Ритмично нажимайте ЛКМ -> ПКМ -> ЛКМ.", 50, 410, 18, MAROON);
            }
            else if (currentLanguage == LANG_BY) {
                DrawLocalizedText("Узровень 1: Клікай ЛКМ па ЧЫРВОНЫХ гантэлях. Сінія - штраф.", 50, 130, 18, BLACK);
                DrawLocalizedText("Узровень 2: Клікай ЛКМ толькі па ЗДАРОВАЙ ежы (пратэін, рыба і г.д.).", 50, 180, 18, BLACK);
                DrawLocalizedText("Узровень 3: Зацісні ЛКМ і перацягвай ежу ў патрэбныя кошыкі БВВ.", 50, 230, 18, BLACK);
                DrawLocalizedText("Узровень 4: Націскай ПРАБЕЛ, калі паўзунок знаходзіцца ў ЗЯЛЁНАЙ зоне.", 50, 280, 18, BLACK);
                DrawLocalizedText("Узровень 5: Уводзь камбінацыю літар для кожнай пазы (5 паз).", 50, 330, 18, BLACK);
                DrawLocalizedText("Армрэслінг (Класіка): Проста спамце клікамі ЛКМ або ПКМ.", 50, 380, 18, BLACK);
                DrawLocalizedText("Армрэслінг (Чаргаванне): Рэтмічна націскайце ЛКМ -> ПКМ -> ЛКМ.", 50, 410, 18, MAROON);
            }
            else {
                DrawLocalizedText("Level 1: Click LMB on RED dumbbells. Blue ones are penalties.", 50, 130, 18, BLACK);
                DrawLocalizedText("Level 2: Click LMB on HEALTHY food only (protein, fish, etc.).", 50, 180, 18, BLACK);
                DrawLocalizedText("Level 3: Hold LMB and drag food into the correct PFC baskets.", 50, 230, 18, BLACK);
                DrawLocalizedText("Level 4: Press SPACE when the slider is inside the GREEN zone.", 50, 280, 18, BLACK);
                DrawLocalizedText("Level 5: Type letter combos for each pose (5 poses total).", 50, 330, 18, BLACK);
                DrawLocalizedText("Armwrestling (Classic): Just spam LMB or RMB clicks.", 50, 380, 18, BLACK);
                DrawLocalizedText("Armwrestling (Alternate): Rhythmical clicks LMB -> RMB -> LMB.", 50, 410, 18, MAROON);
            }

            Rectangle rBack = { 300, 500, 200, 50 }; DrawRectangleRec(rBack, CheckCollisionPointRec(mousePos, rBack) ? SKYBLUE : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "НАЗАД" : (currentLanguage == LANG_BY ? "НАЗАД" : "BACK")), 370, 515, 20, BLACK);
            break;
        }
        case SCREEN_ABOUT: {
            DrawRectangle(30, 30, 740, 540, Fade(WHITE, 0.85f));
            DrawLocalizedText((currentLanguage == LANG_RU ? "О Б ИГРЕ" : (currentLanguage == LANG_BY ? "АБ ГУЛЬНІ" : "ABOUT")), 330, 50, 32, GOLD);
            if (currentLanguage == LANG_RU) {
                DrawLocalizedText("Путь Спортсмена - это спортивный симулятор жизни настоящего бодибилдера.", 50, 150, 18, BLACK);
                DrawLocalizedText("Вам предстоит примерить на себя роль начинающего качка Ивана.", 50, 200, 18, BLACK);
            }
            else if (currentLanguage == LANG_BY) {
                DrawLocalizedText("Шлях Спартсмена - гэта спартыўны сімулятар жыцця сапраўднага бодзібілдара.", 50, 150, 18, BLACK);
                DrawLocalizedText("Вам трэба прымераць на сябе ролю пачаткоўца Хітаругана Івана.", 50, 200, 18, BLACK);
            }
            else {
                DrawLocalizedText("The Athlete's Path is a simulator of a real bodybuilder's life.", 50, 150, 18, BLACK);
                DrawLocalizedText("You will take on the role of an amateur lifter named Ivan.", 50, 200, 18, BLACK);
            }
            Rectangle rBack = { 300, 500, 200, 50 }; DrawRectangleRec(rBack, CheckCollisionPointRec(mousePos, rBack) ? SKYBLUE : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "НАЗАД" : (currentLanguage == LANG_BY ? "НАЗАД" : "BACK")), 370, 515, 20, BLACK);
            break;
        }
        case SCREEN_STORY: {
            const char* textStory = (currentLanguage == LANG_RU ? "ИСТОРИЯ: ИВАН ВЫХОДИТ НА АРЕНУ И ХОЧЕТ ДОКОЗАТЬ ВСЕМ, ЧТО СПОСОБЕН НА ВСЕ" : (currentLanguage == LANG_BY ? "ГІСТОРЫЯ: ІВАН ВЫХОДЗІЦЬ НА АРЭНУ І ХОЧА ДАКАЗАЦЬ УСІМ, ШТО СПОСОБНЫ НА ЎСЕ" : "STORY : IVAN ENTERS THE ARENA AND WANTS TO PROVE TO EVERYONE THAT HE IS CAPABLE OF ANYTHING"));
            DrawLocalizedText(textStory, 60, 200, 15, DARKGRAY);
            Rectangle rStoryBtn = { 350, 450, 100, 40 }; DrawRectangleRec(rStoryBtn, LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ДАЛЕЕ" : (currentLanguage == LANG_BY ? "ДАЛЕЙ" : "NEXT")), 372, 460, 18, BLACK);
            break;
        }
        case SCREEN_MAP: {
            // Уровень 1
            if (texJockLvl1.id > 0) DrawTextureScaled(texJockLvl1, jockZone1, WHITE);
            else DrawRectangleRec(jockZone1, BLUE);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ЛВЛ 1" : (currentLanguage == LANG_BY ? "УЗР 1" : "LVL 1")), jockZone1.x + 35, jockZone1.y - 25, 20, GOLD);
            if (CheckCollisionPointRec(mousePos, jockZone1)) DrawRectangleLinesEx(jockZone1, 3, GOLD);

            // Уровень 2
            if (unlockedLevels >= 2) {
                if (texJockLvl2.id > 0) DrawTextureScaled(texJockLvl2, jockZone2, WHITE);
                else DrawRectangleRec(jockZone2, ORANGE);
            }
            else {
                if (texJockLvl2.id > 0) DrawTextureScaled(texJockLvl2, jockZone2, Fade(BLACK, 0.4f));
                else DrawRectangleRec(jockZone2, DARKGRAY);
            }
            DrawLocalizedText((currentLanguage == LANG_RU ? "ЛВЛ 2" : (currentLanguage == LANG_BY ? "УЗР 2" : "LVL 2")), jockZone2.x + 35, jockZone2.y - 25, 20, (unlockedLevels >= 2) ? GOLD : GRAY);
            if (unlockedLevels >= 2 && CheckCollisionPointRec(mousePos, jockZone2)) DrawRectangleLinesEx(jockZone2, 3, GOLD);

            // Уровень 3
            if (unlockedLevels >= 3) {
                if (textureJockLvl3.id > 0) DrawTextureScaled(textureJockLvl3, jockZone3, WHITE);
                else DrawRectangleRec(jockZone3, GREEN);
            }
            else {
                if (textureJockLvl3.id > 0) DrawTextureScaled(textureJockLvl3, jockZone3, Fade(BLACK, 0.4f));
                else DrawRectangleRec(jockZone3, DARKGRAY);
            }
            DrawLocalizedText((currentLanguage == LANG_RU ? "ЛВЛ 3" : (currentLanguage == LANG_BY ? "УЗР 3" : "LVL 3")), jockZone3.x + 35, jockZone3.y - 25, 20, (unlockedLevels >= 3) ? GOLD : GRAY);
            if (unlockedLevels >= 3 && CheckCollisionPointRec(mousePos, jockZone3)) DrawRectangleLinesEx(jockZone3, 3, GOLD);

            // Уровень 4
            if (unlockedLevels >= 4) {
                if (texJockLvl4.id > 0) DrawTextureScaled(texJockLvl4, jockZone4, WHITE);
                else DrawRectangleRec(jockZone4, PURPLE);
            }
            else {
                if (texJockLvl4.id > 0) DrawTextureScaled(texJockLvl4, jockZone4, Fade(BLACK, 0.4f));
                else DrawRectangleRec(jockZone4, DARKGRAY);
            }
            DrawLocalizedText((currentLanguage == LANG_RU ? "ЛВЛ 4" : (currentLanguage == LANG_BY ? "УЗР 4" : "LVL 4")), jockZone4.x + 35, jockZone4.y - 25, 20, (unlockedLevels >= 4) ? GOLD : GRAY);
            if (unlockedLevels >= 4 && CheckCollisionPointRec(mousePos, jockZone4)) DrawRectangleLinesEx(jockZone4, 3, GOLD);

            // УРОВЕНЬ 5 (БОСС)
            if (unlockedLevels >= 5) {
                if (texJockLvl5.id > 0) DrawTextureScaled(texJockLvl5, jockZone5, WHITE);
                else DrawRectangleRec(jockZone4, PURPLE);
            }
            else {
                if (texJockLvl5.id > 0) DrawTextureScaled(texJockLvl5, jockZone5, Fade(BLACK, 0.4f));
                else DrawRectangleRec(jockZone4, DARKGRAY);
            }
            DrawLocalizedText((currentLanguage == LANG_RU ? "БОСС" : (currentLanguage == LANG_BY ? "БОС" : "BOSS")), jockZone5.x + 25, jockZone5.y - 25, 22, (unlockedLevels >= 5) ? RED : GRAY);
            if (unlockedLevels >= 5 && CheckCollisionPointRec(mousePos, jockZone5)) DrawRectangleLinesEx(jockZone5, 3, GOLD);

            const char* mapTitle = (currentLanguage == LANG_RU ? "ВЫБЕРИТЕ ТРЕНИРОВКУ ИВАНА" : (currentLanguage == LANG_BY ? "ВЫБЕРЫЦЕ ТРЭНІРОЎКУ ІВАНА" : "CHOOSE IVAN'S WORKOUT"));
            DrawLocalizedText(mapTitle, 200, 30, 22, GOLD);
            break;
        }
        case SCREEN_LEVEL1: {
            const char* l1Title = (currentLanguage == LANG_RU ? "УРОВЕНЬ 1: ЛОВЛЯ ГАНТЕЛЕЙ" : (currentLanguage == LANG_BY ? "УЗРОВЕНЬ 1: ЛОВЛЯ ГАНТЭЛЯЎ" : "LEVEL 1: DUMBBELL CATCH"));
            DrawLocalizedText(l1Title, 40, 80, 20, DARKGRAY);
            for (size_t i = 0; i < dumbbells.size(); ++i) {
                if (dumbbells[i].active) {
                    Rectangle destRec = { dumbbells[i].x - 60, dumbbells[i].y - 60, 120, 120 };
                    if (dumbbells[i].isRed) {
                        if (texGanteliaRed.id > 0) DrawTextureScaled(texGanteliaRed, destRec, WHITE);
                        else DrawCircleV({ dumbbells[i].x, dumbbells[i].y }, 40, RED);
                    }
                    else {
                        if (texGanteliaBlue.id > 0) DrawTextureScaled(texGanteliaBlue, destRec, WHITE);
                        else DrawCircleV({ dumbbells[i].x, dumbbells[i].y }, 40, BLUE);
                    }
                }
            }
            break;
        }
        case SCREEN_LEVEL2: {
            const char* l2Title = (currentLanguage == LANG_RU ? "УРОВЕНЬ 2: ЗДОРОВОЕ ПИТАНИЕ" : (currentLanguage == LANG_BY ? "УЗРОВЕНЬ 2: ЗДАРОВАЕ ХАРЧАВАННЕ" : "LEVEL 2: HEALTHY DIET"));
            DrawLocalizedText(l2Title, 40, 80, 20, DARKGRAY);

            for (size_t i = 0; i < productsLvl2.size(); ++i) {
                if (!productsLvl2[i].correctPlaced) {
                    float imgSize = 130.0f;
                    Rectangle destRec = {
                        productsLvl2[i].x - 20.0f,
                        productsLvl2[i].y - 45.0f,
                        imgSize,
                        imgSize
                    };
                    if (productsLvl2[i].texture.id > 0) {
                        DrawTextureScaled(productsLvl2[i].texture, destRec, WHITE);
                    }
                    else {
                        DrawRectangle((int)productsLvl2[i].x, (int)productsLvl2[i].y, 90, 40, ORANGE);
                        DrawRectangleLines((int)productsLvl2[i].x, (int)productsLvl2[i].y, 90, 40, DARKGRAY);
                        const char* pName = (currentLanguage == LANG_RU ? productsLvl2[i].nameRU : (currentLanguage == LANG_BY ? productsLvl2[i].nameBY : productsLvl2[i].nameEN));
                        DrawLocalizedText(pName, productsLvl2[i].x + 10, productsLvl2[i].y + 12, 14, WHITE);
                    }
                }
            }
            break;
        }
        case SCREEN_LEVEL3: {
            const char* l3Title = (currentLanguage == LANG_RU ? "УРОВЕНЬ 3: СОРТИРОВКА БЖУ" : (currentLanguage == LANG_BY ? "УЗРОВЕНЬ 3: САРТАВАННЕ БВВ" : "LEVEL 3: PFC SORTING"));
            DrawLocalizedText(l3Title, 40, 80, 20, DARKGRAY);
            DrawRectangle(200, 420, 110, 180, Fade(BLUE, 0.25f));  DrawLocalizedText((currentLanguage == LANG_RU ? "БЕЛКИ" : (currentLanguage == LANG_BY ? "БЯЛКІ" : "PROTEIN")), 220, 432, 16, BLUE);
            DrawRectangle(350, 420, 110, 180, Fade(GREEN, 0.25f)); DrawLocalizedText((currentLanguage == LANG_RU ? "УГЛЕВ." : (currentLanguage == LANG_BY ? "ВУГЛЯВ." : "CARBS")), 370, 432, 16, GREEN);
            DrawRectangle(500, 420, 110, 180, Fade(RED, 0.25f));   DrawLocalizedText((currentLanguage == LANG_RU ? "ЖИРЫ" : (currentLanguage == LANG_BY ? "ТЛУШЧЫ" : "FATS")), 525, 432, 16, RED);

            for (size_t i = 0; i < productsLvl3.size(); ++i) {
                if (!productsLvl3[i].correctPlaced) {
                    float imgSize = 130.0f;
                    Rectangle destRec = {
                        productsLvl3[i].x - 20.0f,
                        productsLvl3[i].y - 45.0f,
                        imgSize,
                        imgSize
                    };
                    if (productsLvl3[i].texture.id > 0) {
                        DrawTextureScaled(productsLvl3[i].texture, destRec, WHITE);
                    }
                    else {
                        DrawRectangle((int)productsLvl3[i].x, (int)productsLvl3[i].y, 90, 40, PURPLE);
                        DrawRectangleLines((int)productsLvl3[i].x, (int)productsLvl3[i].y, 90, 40, WHITE);
                        const char* pName = (currentLanguage == LANG_RU ? productsLvl3[i].nameRU : (currentLanguage == LANG_BY ? productsLvl3[i].nameBY : productsLvl3[i].nameEN));
                        DrawLocalizedText(pName, productsLvl3[i].x + 8, productsLvl3[i].y + 14, 12, WHITE);
                    }
                }
            }
            break;
        }
        case SCREEN_LEVEL4: {
            const char* l4Title = (currentLanguage == LANG_RU ? "УРОВЕНЬ 4: СИМУЛЯТОР СНА" : (currentLanguage == LANG_BY ? "УЗРОВЕНЬ 4: СІМУЛЯТАР СНУ" : "LEVEL 4: SLEEP SIMULATOR"));
            DrawLocalizedText(l4Title, 40, 80, 20, DARKGRAY);
            std::string missStr = (currentLanguage == LANG_RU ? "ПРОМАХИ: " : (currentLanguage == LANG_BY ? "ПРОМАХІ: " : "MISSES: ")) + std::to_string(sleepMissCount) + " / 3";
            DrawLocalizedText(missStr.c_str(), 40, 115, 18, RED);

            DrawRectangle(100, 300, 600, 40, Fade(LIGHTGRAY, 0.7f));
            DrawRectangle((int)targetZoneX, 300, (int)targetZoneWidth, 40, Fade(GREEN, 0.8f));
            DrawRectangle((int)pointerPos, 290, 10, 60, RED);
            break;
        }
        case SCREEN_LEVEL5: {
            const char* l5Title = (currentLanguage == LANG_RU ? "УРОВЕНЬ 5: ПОЗИРОВАНИЕ" : (currentLanguage == LANG_BY ? "УЗРОВЕНЬ 5: ПАЗІРАВАННЕ" : "LEVEL 5: POSING"));
            DrawLocalizedText(l5Title, 40, 80, 20, DARKGRAY);

            if (currentPoseIndex < (int)poses.size()) {
                Texture2D currentTexture = poses[currentPoseIndex].texture;
                if (currentTexture.id > 0) {
                    float texWidth = (float)currentTexture.width;
                    float texHeight = (float)currentTexture.height;
                    float targetWidth = 150.0f;
                    float targetHeight = 200.0f;

                    float aspect = texWidth / texHeight;
                    if (aspect > 1.0f) {
                        targetHeight = targetWidth / aspect;
                    }
                    else {
                        targetWidth = targetHeight * aspect;
                    }

                    Rectangle destRec = { 550, 150, targetWidth, targetHeight };
                    DrawTextureScaled(currentTexture, destRec, WHITE);
                }
                else {
                    DrawRectangle(50, 150, 150, 200, Fade(GRAY, 0.5f));
                    DrawRectangleLines(50, 150, 150, 200, GOLD);
                    DrawLocalizedText("NO TEXTURE", 65, 230, 14, RED);
                }

                std::string poseNum = (currentLanguage == LANG_RU ? "ПОЗА " : (currentLanguage == LANG_BY ? "ПАЗА " : "POSE ")) +
                    std::to_string(currentPoseIndex + 1) + "/" + std::to_string(poses.size());
                DrawLocalizedText(poseNum.c_str(), 230, 190, 18, DARKGRAY);
            }

            const char* comboText = (currentLanguage == LANG_RU ? "ВВЕДИТЕ КОМБИНАЦИЮ:" : (currentLanguage == LANG_BY ? "УВЯДЗІЦЕ КАМБІНАЦЫЮ:" : "TYPE THE COMBINATION:"));
            DrawLocalizedText(comboText, 230, 240, 18, BLACK);
            DrawTextEx(gameFont, targetCombo.c_str(), { 230, 270 }, 42, 2.0f, MAROON);
            DrawTextEx(gameFont, userCombo.c_str(), { 230, 320 }, 35, 1.0f, GREEN);
            break;
        }
        case SCREEN_ARMRESTLING: {
            if (currentDifficulty == DIFF_HARDMODE) {
                const char* hmStr = (currentLanguage == LANG_RU ? "ХАРДМОД АКТИВИРОВАН! СОПРОТИВЛЕНИЕ СУМАСШЕДШЕЕ!" : (currentLanguage == LANG_BY ? "ХАРДМОД АКТЫВАВАНЫ! КРАЙНЯЯ СКЛАДАНАСЦЬ!" : "HARDMODE ACTIVATED! INSANE RESISTANCE!"));
                DrawLocalizedText(hmStr, 60, 40, 18, RED);
            }

            if (currentClickMode == MODE_CLASSIC) {
                const char* mCl = (currentLanguage == LANG_RU ? "РЕЖИМ: КЛАССИКА (СПАМЬ ЛКМ ИЛИ ПКМ)" : (currentLanguage == LANG_BY ? "РЭЖЫМ: КЛАСІКА (СПАМ ЛКМ АБО ПКМ)" : "MODE: CLASSIC (SPAM LMB OR RMB)"));
                DrawLocalizedText(mCl, 40, 80, 22, DARKGRAY);
            }
            else {
                const char* mAl = (currentLanguage == LANG_RU ? "РЕЖИМ: ЧЕРЕДОВАНИЕ! ЖМИ ЛКМ -> ПКМ -> ЛКМ" : (currentLanguage == LANG_BY ? "РЭЖЫМ: ЧАРГАВАННЕ! ЦІСНІ ЛКМ -> ПКМ -> ЛКМ" : "MODE: ALTERNATE! PRESS LMB -> RMB -> LMB"));
                DrawLocalizedText(mAl, 40, 80, 20, MAROON);
            }

            DrawRectangle(100, 280, 600, 50, LIGHTGRAY);
            DrawRectangle((int)armBalance, 265, 15, 80, RED);
            DrawRectangleLines(100, 280, 600, 50, DARKGRAY);
            break;
        }
        case SCREEN_WIN: {
            DrawLocalizedText((currentLanguage == LANG_RU ? "ПОБЕДА!" : (currentLanguage == LANG_BY ? "ПЕРАМОГА!" : "VICTORY!")), 325, 70, 38, GOLD);

            if (texCup.id > 0) {
                Rectangle destRecCup = { virtualScreenWidth / 2 - 75, 150, 150, 150 };
                DrawTextureScaled(texCup, destRecCup, WHITE);
            }

            Rectangle rEnd = { 300, 400, 200, 50 };
            bool hovered = CheckCollisionPointRec(mousePos, rEnd);
            DrawRectangleRec(rEnd, hovered ? SKYBLUE : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "В МЕНЮ" : (currentLanguage == LANG_BY ? "У МЕНЮ" : "MENU")), 365, 415, 20, BLACK);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered) {
                unlockedLevels = 1;
                currentState = SCREEN_MENU;
            }
            break;
        }
        case SCREEN_LOSE: {
            DrawLocalizedText((currentLanguage == LANG_RU ? "ВЫ ПРОИГРАЛИ" : (currentLanguage == LANG_BY ? "ВЫ ПРАЙГРАЛІ" : "YOU LOSE")), 280, 220, 38, RED);

            Rectangle rEnd = { 300, 400, 200, 50 };
            bool hovered = CheckCollisionPointRec(mousePos, rEnd);
            DrawRectangleRec(rEnd, hovered ? SKYBLUE : LIGHTGRAY);
            DrawLocalizedText((currentLanguage == LANG_RU ? "ПОВТОР" : (currentLanguage == LANG_BY ? "ЗНОЎ" : "RETRY")), 345, 415, 20, BLACK);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered) {
                if (levelToRetry == SCREEN_LEVEL1) InitLevel1();
                else if (levelToRetry == SCREEN_LEVEL2) InitLevel2();
                else if (levelToRetry == SCREEN_LEVEL3) InitLevel3();
                else if (levelToRetry == SCREEN_LEVEL4) InitLevel4();
                else if (levelToRetry == SCREEN_LEVEL5) InitLevel5();
                else if (levelToRetry == SCREEN_ARMRESTLING) InitArmrestling();
                currentState = levelToRetry;
            }
            break;
        }
        }

        if (inCountdown && currentState >= SCREEN_LEVEL1 && currentState <= SCREEN_ARMRESTLING) {
            DrawRectangle(0, 0, virtualScreenWidth, virtualScreenHeight, Fade(BLACK, 0.4f));
            int displayNum = (int)startCountdownTimer + 1;
            if (displayNum > 3) displayNum = 3;
            std::string numStr = std::to_string(displayNum);
            Vector2 numSize = MeasureTextEx(gameFont, numStr.c_str(), 120, 1.0f);
            DrawTextEx(gameFont, numStr.c_str(), { (float)virtualScreenWidth / 2 - numSize.x / 2, (float)virtualScreenHeight / 2 - numSize.y / 2 }, 120, 1.0f, GOLD);
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture,
            { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height },
            { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() },
            { 0.0f, 0.0f }, 0.0f, WHITE);
        EndDrawing();
    }

    // Выгрузка текстур
    UnloadMusicStream(bgMusic);
    CloseAudioDevice();

    UnloadTexture(texLvl1Bg);
    UnloadTexture(texLvl2Bg);
    UnloadTexture(texGanteliaBlue);
    UnloadTexture(texGanteliaRed);
    UnloadTexture(texJockLvl1);
    UnloadTexture(texJockLvl2);
    UnloadTexture(textureJockLvl3);
    UnloadTexture(texJockLvl4);
    UnloadTexture(texJockLvl5);
    UnloadTexture(arenaTexture);
    UnloadTexture(lvl3Texture);
    UnloadTexture(lvl4Texture);

    UnloadTexture(texProtein);
    UnloadTexture(texGeiner);
    UnloadTexture(texFish);
    UnloadTexture(texChips);
    UnloadTexture(texBeer);
    UnloadTexture(texPizza);
    UnloadTexture(texChicken);
    UnloadTexture(texEgg);
    UnloadTexture(texButter);
    UnloadTexture(texNuts);
    UnloadTexture(texRis);
    UnloadTexture(texBanan);
    UnloadTexture(texTvorog);

    UnloadTexture(texCup);

    UnloadTexture(texPose1);
    UnloadTexture(texPose2);
    UnloadTexture(texPose3);
    UnloadTexture(texPose4);
    UnloadTexture(texPose5);

    UnloadRenderTexture(target);
    UnloadFont(gameFont);
    CloseWindow();
    return 0;
}