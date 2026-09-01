# TunnelStrike — OCS Protocol Document

```
⨯protocol/ocs: [node=OCS/Root]
Project: TunnelStrike
Mode: Hybrid
Surface: protocol document
Branch: main
Meta-DNA: tunnelstrike-ocs-proto-v0.6
Coherence-target: 0.88
```

## 1. Living Objective

TunnelStrike als gebundenes OCS-Protokoll-Dokument führen und den C++/SFML-Kern entlang der README-Roadmap fortschreiben:

1. Simulationsschritt vom Display-Takt trennen (`SimClock`, 30 ms).
2. Kreaturen mit GA-steuerbarer Motion + Körperstruktur.
3. Evolutionszustand persistent speichern (`evo/`).
4. Körper-Erscheinung/Limb-Topologie erblich und deterministisch; Fitness aus Survival oder menschlicher Bewertung (`evo/assess.txt`).
5. Motion genes that sense player XY and nearest shot (`seek`, `dodge`, `reaction`, `strafe`, `range`, `lead`).
6. Selection with truncation, crowding, elite clones + mutated elites, adaptive mutation, spawn bias.
7. `--self-run` evolving player (pilot) intelligence, persisted beside creature `evo/`.

Nicht in diesem Frame: Merge mit `t162` oder `TunnelStrike-svelte`.

## 2. Executed slice (v0.2)

| TAS | Status |
|---|---|
| P0 Protocol document | done — `docs/ocs-protocol.md` |
| P1 SimClock | done — `src/sim/SimClock.hpp`, wired in `Main::run` |
| P2 Creature genome | done — `src/evo/Genome.hpp` + `Target::Act` |
| P3 Fitness | done — survive vs. killed scoring |
| P4 GA step | done — `Population::maybeEvolve` after World tick |
| P5 Archive | done — `evo/generation.jsonl`, `pool.txt`, `snapshot.txt` |
| P6 Headless verify | done — `make sim` → `TunnelStrike-sim` |

## 2b. Executed slice (v0.3) — body structure

| TAS | Status |
|---|---|
| P7 Heritable limb genes | done — `limbs`, `limb_len`, `fork`, `twist` on `Genome` |
| P8 Deterministic body | done — `Target::generate` from genome, no per-frame `rand` |
| P9 Human assess overlay | done — `evo/assess.txt` (`*` or exact genome line) |
| P10 Fitness edge | done — `World::recordFitness` (survive / kill + bonus) |
| P11 Structure verify | done — `TunnelStrike-sim` checks topology, roundtrip, assess, then 18000 ticks |

## 2c. Executed slice (v0.4) — GA + simulator

| TAS | Status |
|---|---|
| P12 Context motion genes | done — `seek`, `dodge`, `reaction` on `Genome` |
| P13 Deterministic Act | done — per-target RNG, no `::rand` in `Target::Act` |
| P14 Sense edge | done — `Targets::Tick` builds `Sense` from camera + live shots |
| P15 Tournament GA | done — 3-way tournament, elites, 2 random immigrants, mean/diversity |
| P16 Persist generation | done — `evo/pool.txt` header `# generation N ...` |
| P17 Isolated sim | done — `World("/tmp/...")`, GA operator tests, dodge/seek tests |

## 2d. Executed slice (v0.5) — behaviour, fitness, selection

| TAS | Status |
|---|---|
| P18 Predictive steer | done — `strafe` / `range` / `lead`, closing-shot Sense, wall bounce |
| P19 Fitness signal | done — near-miss + threat in survive/kill scores; hit radius from `size` |
| P20 Selection | done — truncation k=4, crowding, 2 clones + 2 mutant elites, adaptive mut, elite spawn |

## 2e. Executed slice (v0.6) — self-run pilot

| TAS | Status |
|---|---|
| P21 CLI | done — `--self-run`, `--headless`, `--ticks`, `--evo-dir`, `--help` |
| P22 Pilot genome | done — `src/evo/Pilot.hpp` (lead, smooth, trigger, target prefs) |
| P23 PilotMind | done — aim/fire policy, 9s episodes, fitness from kills/accuracy/misses |
| P24 Persist pilot | done — `evo/pilot.txt` + `evo/pilot.jsonl` |
| P25 Pilot intercept | done — sticky target, muzzle-origin aim, vel intercept, ray fire gate |

## 2f. Executed slice (v0.7) — improved PilotMind

| TAS | Status |
|---|---|
| P26 Closed-form intercept | done — quadratic time-of-flight solution + `lead` scale |
| P27 Wall bounce reflection | done — continuous 1D folding inside tunnel boundaries `[-8.85, 8.85]` |
| P28 True barrel ray gate | done — checks ray distance from current smoothed aim vector `(aim_x, aim_y)` |
| P29 Frame-rate independent slew | done — exponential smoothing `1 - exp(-rate * dt)` |
| P30 PilotPop niching & diversity | done — fitness sharing crowd discount + adaptive mutation rate |
| P31 Enhanced fitness reward | done — long-range intercept reward, inaction penalty, accuracy scaling |
| P32 Headless & windowed parity | done — consistent camera motion, rotation, and `mind->fireDir()` across modes |

## 3. Invariants

- `World::Tick` is the only mutation edge for creatures, shots, evolution.
- `World::recordFitness` is the only fitness recording edge.
- Body topology is a pure function of the genome (same genome ⇒ same segment count).
- Creature motion is a function of genome + `Sense` + per-genome RNG (same genome + same sense stream ⇒ same trajectory).
- `RenderFrame` reads state only.
- Autopilot aim-wander stays off (`#if 0` removed, not re-enabled).
- `CheckShoot` auto-fire remains as existing assist when `--self-run` is off.
- `--self-run` is the only evolving player-aim path; it does not use `CheckShoot`.

## 4. Three-Agent-Core

| Agent | Role |
|---|---|
| KickForge | Genome, Population, Archive, Target morph / limb topology |
| KickFlow | SimClock absorb + TAS order |
| KickGuard | no silent autopilot-default, archive local-only, heritable body |
