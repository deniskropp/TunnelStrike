# TunnelStrike

## Description

3D Shooter, moving through endless tunnel with creatures (enemies).

![Screenshot](https://github.com/deniskropp/TunnelStrike/raw/main/screenshots/TunnelStrike1.png)

## Features

### Minimum requirements
    Mouse movements for aiming
    Click or tap to Shoot

### Advanced features
    Sound effects
    Self-running mode

## Planned Features

### Evolving creatures

    GA based logic controlling motion depending on position relative to player, bullets...
    Appearance or structure of the bodies evolving based upon survival or human assessment

### Simulation decoupled from display

    Even with low FPS the bullets and creatures do well behave

### Other features

    Permanent storage of information from evolution

## MetaForge Progress (June 2026)

This repository is being evolved using the Unified MetaForge process (unified-metaforge-TunnelStrike-2026-06-27).

- **TAS v2 Decomposition** — Full purified Task-Agnostic Steps for the game loop, input, simulation, render, and GA extension points. See `docs/TAS_TunnelStrike_v2.md`.
- **CreatureGenome Schema v1** — KickLang-native genome definition (motion + appearance genes) with fitness model supporting survival time + optional human novelty rating. See `docs/CreatureGenome_Schema_v1.md`.
- **GA Engine Prototype v1** — Fully runnable reference implementation demonstrating the complete evolutionary loop (initialize → evaluate in decoupled sim → select → mutate/crossover → persist). Located at `tools/ga_tunnelstrike_prototype.py`. Run with `python tools/ga_tunnelstrike_prototype.py`.

All work is on branch `feature/tas-ga-evolution-v1`.