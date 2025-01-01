#include "camera.hpp"
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <chrono>
#include <cmath>
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
#include <unordered_map>
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
    std::unordered_map<HSteamNetConnection, Object *> ConnectionToObjectMap;
}

void TickUpdate(int tickNumber) {
    Object *obj = engine->GetObjectByID(2);
    UTILASSERT(obj);

    /* tickrate = 64.0f */
    obj->SetScale(glm::vec3(sin(tickNumber*(1.0f/64.0f))));
}

void onClientConnect(HSteamNetConnection conn) {
    /* TODO: I basically want to add an object for each client and let them move together, but I have to attach cameras. */
    /* The problem being, how would I be able to tell each client which camera they should use? */
    /* it shouldn't be too difficult, Maybe the update packet could include a top-level array of Cameras? */

    Object *obj = new Object();
    obj->ImportFromFile("untitled.glb");

    /* Maybe something like this..? */
    // engine->AddObject(obj);  // obj would have a camera attachment, which would be added with AddObject.
    // engine->AttachCameraToConnection(cam, conn); // When an update is sent to conn, it would send a bool flag next to the camera that indicates it's the primary camera.

    State::ConnectionToObjectMap[conn] = obj;
}

void onClientDisconnect(HSteamNetConnection conn) {
    // engine->RemoveObject(State::ConnectionToObjectMap[conn]);
    // delete State::ConnectionToObjectMap[conn]->GetCameraAttachment();
    // delete State::ConnectionToObjectMap[conn];
}

int main() {
    engine = std::make_unique<Engine>();
    
    engine->InitNetworking();
    // engine->RegisterTickUpdateHandler(TickUpdate, NETWORKING_THREAD_ACTIVE_SERVER);
    engine->RegisterNetworkListener(onClientConnect, EVENT_CLIENT_CONNECTED);
    engine->RegisterNetworkListener(onClientDisconnect, EVENT_CLIENT_DISCONNECTED);

    // engine->ImportScene("untitled.glb"); /* Get the objects ready to sync */

    SteamNetworkingIPAddr ipAddr;
    ipAddr.Clear();
    ipAddr.ParseString("127.0.0.1:9582");

    engine->HostGameServer(ipAddr);

    std::this_thread::sleep_for(std::chrono::hours(90));

    return 0;
}
