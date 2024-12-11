#include "camera.hpp"
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <chrono>
#include <engine.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include <memory>
#include <new>
#include <settings.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <stdexcept>
#include <string>
#include "camera.hpp"
#include "common.hpp"
#include "model.hpp"
#include "ui/arrows.hpp"
#include "ui/button.hpp"
#include "ui/label.hpp"
#include "ui/panel.hpp"
#include "util.hpp"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "tinyfiledialogs.h"

Camera cam(glm::vec3(-1.0f, 0.0f, 0.0f));
Settings settings("settings.toml");
std::unique_ptr<Engine> engine;

namespace State {
}

float lastX, lastY;
Uint32 lastMouseState;

void Update() {
    //fmt::println("Hi!");
}

void FixedUpdate(const std::array<bool, 322> &keyMap) {

}

void onButtonClick(std::string id) {
    if (id == "playButton") {
        fmt::println("*plays*");

        engine->ImportScene("viking_room.xml");

        UI::GenericElement *mainMenuGroup = engine->GetElementByID("mainMenuGroup");

        UI::GenericElement *hudGroup = engine->GetElementByID("hudGroup");

        if (mainMenuGroup && mainMenuGroup->type == UI::SCALABLE) {
            mainMenuGroup->SetVisible(false);
            hudGroup->SetVisible(true);
        }
    }
}

int main() {
    cam.MouseSensitivity = settings.MouseSensitivity;
    cam.FOV = settings.FieldOfView;
    cam.MovementSpeed = settings.Velocity;

    engine = std::make_unique<Engine>();
    
    try {
        
    } catch(const std::runtime_error &e) {
        fmt::println("Exception has occurred: {}", e.what());
        return -1;
    }

    return 0;
}
