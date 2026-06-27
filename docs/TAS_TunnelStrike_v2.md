# TAS Decomposition for TunnelStrike v2 (Purified Blocks)

**Forge ID**: unified-metaforge-TunnelStrike-2026-06-27
**Meta-Iteration**: 1
**Status**: Approved & Pushed

## Overview
Full Task-Agnostic Steps decomposition of the current codebase (src/main.cpp + World/Target/Shot/Walls/Crosshair/Sfx + geometry) with purification for GA-ready evolutionary system.

## INIT Layer
- TAS-INIT-01: SFML context + window + Parameters
- TAS-INIT-02: Camera3d singleton + initial translate
- TAS-INIT-03: Sfx::instance() load sounds
- TAS-INIT-04: World construction (Walls tunnel, empty Targets/Shots, Crosshair)
- TAS-INIT-05: Random seed + cursor init

## INPUT Layer (pure event→action)
- TAS-INPUT-01: ProcessEvents (close, resize, fire)
- TAS-INPUT-02: HandleCamera 6DOF keyboard
- TAS-INPUT-03: Mouse delta → tc aim offset + cursor
- TAS-INPUT-04: Shoot() → create Shot with rotated dir, Sfx::PlayShot()

## SIM Layer (decoupled fixed-timestep)
- TAS-SIM-01: Fixed 0.03s accumulator + world.Tick()
- TAS-SIM-02: Walls tunnel advance
- TAS-SIM-03: Targets spawn + per-creature motion update (GA hook)
- TAS-SIM-04: Shots advance + expiry
- TAS-SIM-05: Collision Shot vs Target (+ kills)
- TAS-SIM-06: Optional auto-advance camera

## RENDER Layer (decoupled)
- TAS-RENDER-01..05: clear, World::draw (Walls+Targets+Shots), Crosshair, HUD, display

## GA-EXTENSION + PERSISTENCE Layer
- TAS-GA-01: CreatureGenome encoding (motion + appearance)
- TAS-GA-02: Fitness = survival + human_novelty
- TAS-GA-03..04: Mutation, Crossover, RTA graph hook
- TAS-PERSIST-01..02: Evolution DB checkpoint + enhanced ExportWorld

**Purification achieved**: Simulation strictly time-driven, all creature behavior surfaces explicit strategy points, input produces only spawn events, render is pure read.
