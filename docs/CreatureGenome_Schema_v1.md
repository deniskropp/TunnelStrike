# CreatureGenome Schema v1 + GA Engine Prototype

**Forge ID**: unified-metaforge-TunnelStrike-2026-06-27
**Meta-Iteration**: 2
**Status**: Designed & Pushed

## CreatureGenome (KickLang native + JSON serializable)

```kicklang
genome CreatureGenome {
    motion: {
        forward_speed: float [0.5..4.0]
        turn_rate: float [0.0..2.5]
        strafe_bias: float [-1.0..1.0]
        dodge_chance: float [0.0..0.8]
        vertical_bias: float [-0.5..0.5]
        reaction_delay: int [1..8]
    }
    appearance: {
        scale: float [0.6..2.2]
        segment_count: int [3..12]
        hue: float [0.0..1.0]
        deformation: float [0.0..0.7]
        glow_intensity: float [0.0..1.0]
    }
    personality_seed: uint32
}
```

## Fitness Model
- base_fitness = survival_seconds
- novelty_bonus = human_rating (0-10) * 0.3 (optional)
- final_fitness = base + novelty + survival_pressure
- CoherenceMonitorBridge tracks flux/drift/valence per creature

## GA Engine Prototype (7 TAS-GA blocks)
1. InitializePopulation(N=24)
2. EvaluatePopulation() in decoupled sim
3. AggregateFitness() + CoherenceMonitorBridge
4. Selection (top_k=8, elitism)
5. Variation: gaussian mutate + uniform crossover
6. PersistGeneration() → evolution_log.jsonl / SQLite
7. SpawnNextWave() → Targets::spawn_from_genome()

## C++ Integration Surfaces
- Target::loadGenome(CreatureGenome) or JSON loader
- Targets::spawn_from_genome(...)
- World::Tick already calls per-target update → ready for GA motion swap-in

**Next**: Implement prototype in meta-iteration 3.
