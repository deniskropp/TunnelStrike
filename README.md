# TunnelStrike

3D Shooter, moving through endless tunnel with creatures (enemies).

![Screenshot](https://github.com/deniskropp/TunnelStrike/raw/main/screenshots/TunnelStrike1.png)

## Features

### Minimum requirements
    Mouse movements for aiming
    Click or tap to Shoot

### Advanced features
    Sound effects
    Self-running mode (`--self-run`) with evolving pilot aim/fire policy
    Fixed simulation clock (30 ms tick, display decoupled)
    GA-controlled creature motion and body structure
    Motion genes for player seek / shot dodge / strafe / range / lead
    Survival fitness from time, near-misses, and threat; size-scaled hit radius
    Persistent evolution archive under `evo/` (generation, mean, diversity)

## Planned Features

### Evolving creatures
    GA based logic controlling motion depending on position relative to player, bullets (`seek`, `dodge`, `reaction`, `strafe`, `range`, `lead`)
    Appearance / limb structure evolving from survival fitness (heritable `limbs`, `limb_len`, `fork`, `twist`)
    Optional human assessment overlay: `evo/assess.txt` (`* <bonus>` or `<bonus> <genome line>`)

### Simulation decoupled from display
    Even with low FPS the bullets and creatures do well behave

### Other features
    Permanent storage of information from evolution

## Build

    make
    ./TunnelStrike

    ./TunnelStrike --self-run

Headless simulation / archive replay seed:

    make sim
    ./TunnelStrike-sim

    ./TunnelStrike --evo-verify /tmp/ts-game-evo 18000
    ./TunnelStrike --self-run --headless --evo-dir /tmp/ts-self-run 18000

Headless sim uses an isolated archive under `/tmp` and checks genome roundtrip, GA operators, sensed motion, then 18000 ticks.
`./TunnelStrike --evo-verify` runs the same World tick/evolve path as the game (no window) against a chosen `evo/` directory.
`--self-run` replaces mouse aim with an evolving pilot (intercept lead, sticky target, ray fire gate). Policy is stored in `evo/pilot.txt` and `evo/pilot.jsonl`. `--self-run --headless` verifies that the pilot generation advances.

Evolution state is written to `evo/generation.jsonl`, `evo/pool.txt` and `evo/snapshot.txt`.
A pool file is loaded automatically on the next start.

## OCS

Protocol document: [`docs/ocs-protocol.md`](docs/ocs-protocol.md)
