# TunnelStrike — OCS Protocol Document

```
⨯protocol/ocs: [node=OCS/Root]
Project: TunnelStrike
Mode: Hybrid
Surface: protocol document
Branch: main
Meta-DNA: tunnelstrike-ocs-proto-v0.2
Coherence-target: 0.88
```

## 1. Living Objective

TunnelStrike als gebundenes OCS-Protokoll-Dokument führen und den C++/SFML-Kern entlang der README-Roadmap fortschreiben:

1. Simulationsschritt vom Display-Takt trennen (`SimClock`, 30 ms).
2. Kreaturen mit GA-steuerbarer Motion + Körperstruktur.
3. Evolutionszustand persistent speichern (`evo/`).

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

## 3. Invariants

- `World::Tick` is the only mutation edge for creatures, shots, evolution.
- `RenderFrame` reads state only.
- Autopilot aim-wander stays off (`#if 0` removed, not re-enabled).
- `CheckShoot` auto-fire remains as existing self-running assist.

## 4. Three-Agent-Core

| Agent | Role |
|---|---|
| KickForge | Genome, Population, Archive, Target morph |
| KickFlow | SimClock absorb + TAS order |
| KickGuard | no silent autopilot-default, archive local-only |
