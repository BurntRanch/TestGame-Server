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
    struct PlayerState {
        Object *object;
        float direction;
    };

    std::unordered_map<HSteamNetConnection, PlayerState> ConnectionToObjectMap;

    enum packetType {
        PACKET_MOVEMENT_REQUEST,
    };

    struct Packet {
        packetType type;

        /* if packetType == PACKET_MOVEMENT_REQUEST */
        float movementDirection;
    };
}

void onClientConnect(HSteamNetConnection conn) {
    // Object *obj = new Object();
    // Camera *primaryCam = nullptr;

    // obj->ImportFromFile("resources/untitled.glb", primaryCam);

    // UTILASSERT(primaryCam);

    // /* Maybe something like this..? */
    // engine->AddObject(obj);  // obj would have a camera attachment, which would be added with AddObject.
    // engine->AttachCameraToConnection(primaryCam, conn); // When an update is sent to conn, it would send a bool flag next to the camera that indicates it's the primary camera.

    // State::PlayerState playerState{};

    // playerState.object = obj;
    // playerState.direction = 0.0f;

    // State::ConnectionToObjectMap[conn] = playerState;

    auto &cameras = engine->GetCameras();
    UTILASSERT(cameras.size() > 0);

    engine->AttachCameraToConnection(cameras[0], conn);
}

void onClientDisconnect(HSteamNetConnection conn) {
    engine->RemoveObject(State::ConnectionToObjectMap[conn].object);

    delete State::ConnectionToObjectMap[conn].object->GetCameraAttachment();
    delete State::ConnectionToObjectMap[conn].object;

    State::ConnectionToObjectMap.erase(conn);
}

void onData(HSteamNetConnection conn, std::vector<std::byte> data) {
    fmt::println("Got data!");

    State::Packet packet{};
    Deserialize(data, packet.type);
    Deserialize(data, packet.movementDirection);

    fmt::println("Client wants to move {} units along the X axis.", packet.movementDirection);
    fmt::println("We're an insecure server, so lets just allow them.");

    State::PlayerState &state = State::ConnectionToObjectMap[conn];
    state.direction = packet.movementDirection;
}

void TickUpdate(int tickNumber) {
    for (auto &i : State::ConnectionToObjectMap) {
        Object *obj = i.second.object;
        float movementDirection = i.second.direction;

        obj->SetPosition(obj->GetPosition() + glm::vec3(movementDirection, 0.0f, 0.0f));
    }
}

int main(int argc, char *argv[]) {
    engine = std::make_unique<Engine>();
    
    engine->InitNetworking();
    engine->InitPhysics();

    engine->RegisterNetworkEventListener(onClientConnect, EVENT_CLIENT_CONNECTED);
    // engine->RegisterNetworkEventListener(onClientDisconnect, EVENT_CLIENT_DISCONNECTED);

    engine->RegisterTickUpdateHandler(TickUpdate, NETWORKING_THREAD_ACTIVE_SERVER);

    // engine->RegisterNetworkDataListener(onData);

    engine->ImportScene("resources/untitled.glb"); /* Get the objects ready to sync */

    SteamNetworkingIPAddr ipAddr;
    ipAddr.Clear();

    if (argc >= 2) {
        fmt::println("Hosting server at {}!", argv[1]);
        ipAddr.ParseString(argv[1]);

        if (ipAddr.m_port == 0) {
            ipAddr.m_port = 9582;
        }
    } else {
        fmt::println("Hosting server at 127.0.0.1!");
        ipAddr.ParseString("127.0.0.1:9582");
    }

    engine->HostGameServer(ipAddr);

    std::this_thread::sleep_for(std::chrono::hours(90));

    return 0;
}
