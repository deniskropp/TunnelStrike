#pragma once
// CreatureGenome.hpp
// MetaForge: unified-metaforge-TunnelStrike-2026-06-27 | Meta-Iteration 4
// C++ skeleton matching CreatureGenome Schema v1 (docs/CreatureGenome_Schema_v1.md)
//
// This header is intentionally minimal and non-breaking.
// It can be included by Target.hpp / Targets.cpp without changing existing behavior
// until the GA loader and motion/appearance application logic are wired in.

#include <cstdint>
#include <string>
#include <optional>

struct CreatureGenome
{
    // === Motion genes (affect Target update / world.Tick behavior) ===
    float forward_speed   = 1.5f;   // units per tick
    float turn_rate       = 1.0f;   // rad per tick
    float strafe_bias     = 0.0f;   // -1..+1
    float dodge_chance    = 0.3f;   // 0..0.8
    float vertical_bias   = 0.0f;   // -0.5..+0.5
    int   reaction_delay  = 3;      // ticks (1..8)

    // === Appearance / body genes (affect Quad / solid3d rendering) ===
    float scale           = 1.0f;   // 0.6..2.2
    int   segment_count   = 6;      // 3..12
    float hue             = 0.5f;   // 0..1 (color)
    float deformation     = 0.2f;   // 0..0.7 Quad distortion
    float glow_intensity  = 0.0f;   // 0..1

    // Optional behavioral personality seed (future RTA graph / complex behavior)
    uint32_t personality_seed = 0;

    // ============================================================
    // Future JSON (de)serialization hooks
    // ============================================================
    // Example using nlohmann/json (add #include <nlohmann/json.hpp> when ready):
    //
    // static CreatureGenome from_json(const nlohmann::json& j) {
    //     CreatureGenome g;
    //     j.at("forward_speed").get_to(g.forward_speed);
    //     j.at("turn_rate").get_to(g.turn_rate);
    //     // ... repeat for all fields
    //     return g;
    // }
    //
    // nlohmann::json to_json() const {
    //     return {
    //         {"forward_speed", forward_speed},
    //         {"turn_rate", turn_rate},
    //         // ...
    //     };
    // }

    // Placeholder for raw JSON string loader (implement when JSON lib is chosen)
    static std::optional<CreatureGenome> from_json_string(const std::string& /*json*/) {
        // TODO: parse and return CreatureGenome or std::nullopt on error
        return std::nullopt;
    }
};

// ============================================================
// Integration notes for Target / Targets
// ============================================================
// 1. Add optional<CreatureGenome> genome_ member to Target (or derive from Entity)
// 2. In Target constructor or spawn: if genome provided, applyGenome(genome)
// 3. void Target::applyGenome(const CreatureGenome& g) {
//        // scale visual Quad / solid3d
//        // store motion params for use in update()
//        // personality_seed can seed future RTA decision graph
//    }
// 4. In Targets::spawn_from_genome(const CreatureGenome& g) {
//        auto t = std::make_shared<Target>(...);
//        t->applyGenome(g);
//        targets.push_back(t);
//    }
// 5. In World::Tick or Targets update loop: use genome values for motion
//    instead of hardcoded logic (this is the GA hook point).
//
// This skeleton is designed to be included immediately without breaking
// the existing Entity/Target/Shot/Walls/World hierarchy.