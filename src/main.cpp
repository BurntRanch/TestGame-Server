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
#include <thread>
#include "camera.hpp"
#include "common.hpp"
#include "model.hpp"
#include "steamnetworkingtypes.h"
#include "isteamnetworkingutils.h"
#include "ui/arrows.hpp"
#include "ui/button.hpp"
#include "ui/label.hpp"
#include "ui/panel.hpp"
#include "util.hpp"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "tinyfiledialogs.h"

std::unique_ptr<Engine> engine;

namespace State {
}

int main() {
    engine = std::make_unique<Engine>();
    
    engine->InitNetworking();

    SteamNetworkingIPAddr ipAddr;
    ipAddr.Clear();
    ipAddr.ParseString("127.0.0.1:9582");

    engine->HostGameServer(ipAddr);

    std::this_thread::sleep_for(std::chrono::hours(90));

    return 0;
}
