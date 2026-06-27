#!/usr/bin/env python3
"""
TunnelStrike GA Engine Prototype v1
MetaForge: unified-metaforge-TunnelStrike-2026-06-27 | Meta-Iteration 3

Self-contained reference implementation of the CreatureGenome + GA loop.
Demonstrates:
- Genome initialization within schema bounds
- Decoupled evaluation (simulated tunnel survival)
- Fitness with optional human novelty hook
- Selection, mutation, crossover
- Persistence to JSONL

Run: python ga_tunnelstrike_prototype.py
"""

import json
import random
import time
from dataclasses import dataclass, asdict
from typing import List, Dict, Optional

# ============================================================
# CreatureGenome Schema v1 (matches docs/CreatureGenome_Schema_v1.md)
# ============================================================
@dataclass
class CreatureGenome:
    # Motion genes
    forward_speed: float
    turn_rate: float
    strafe_bias: float
    dodge_chance: float
    vertical_bias: float
    reaction_delay: int

    # Appearance genes
    scale: float
    segment_count: int
    hue: float
    deformation: float
    glow_intensity: float

    personality_seed: int

    @classmethod
    def random(cls) -> "CreatureGenome":
        return cls(
            forward_speed=random.uniform(0.5, 4.0),
            turn_rate=random.uniform(0.0, 2.5),
            strafe_bias=random.uniform(-1.0, 1.0),
            dodge_chance=random.uniform(0.0, 0.8),
            vertical_bias=random.uniform(-0.5, 0.5),
            reaction_delay=random.randint(1, 8),
            scale=random.uniform(0.6, 2.2),
            segment_count=random.randint(3, 12),
            hue=random.uniform(0.0, 1.0),
            deformation=random.uniform(0.0, 0.7),
            glow_intensity=random.uniform(0.0, 1.0),
            personality_seed=random.randint(0, 2**32 - 1),
        )

    def mutate(self, rate: float = 0.15) -> "CreatureGenome":
        """Gaussian mutation with bounds clamping"""
        def clamp(val, lo, hi):
            return max(lo, min(hi, val))

        return CreatureGenome(
            forward_speed=clamp(self.forward_speed + random.gauss(0, 0.4) * rate, 0.5, 4.0),
            turn_rate=clamp(self.turn_rate + random.gauss(0, 0.3) * rate, 0.0, 2.5),
            strafe_bias=clamp(self.strafe_bias + random.gauss(0, 0.25) * rate, -1.0, 1.0),
            dodge_chance=clamp(self.dodge_chance + random.gauss(0, 0.15) * rate, 0.0, 0.8),
            vertical_bias=clamp(self.vertical_bias + random.gauss(0, 0.15) * rate, -0.5, 0.5),
            reaction_delay=max(1, min(8, self.reaction_delay + random.randint(-2, 2))),
            scale=clamp(self.scale + random.gauss(0, 0.2) * rate, 0.6, 2.2),
            segment_count=max(3, min(12, self.segment_count + random.randint(-2, 2))),
            hue=(self.hue + random.gauss(0, 0.1) * rate) % 1.0,
            deformation=clamp(self.deformation + random.gauss(0, 0.1) * rate, 0.0, 0.7),
            glow_intensity=clamp(self.glow_intensity + random.gauss(0, 0.15) * rate, 0.0, 1.0),
            personality_seed=self.personality_seed,
        )

    def to_dict(self) -> dict:
        return asdict(self)

# ============================================================
# Simple Decoupled Evaluation (simulates world.Tick survival)
# ============================================================
def evaluate_genome(genome: CreatureGenome, steps: int = 120) -> float:
    """
    Simulated survival time in the tunnel.
    Higher forward_speed and good dodge/turn help.
    This is a placeholder for real decoupled physics + collision.
    """
    survival = 30.0  # base seconds

    # Motion contribution (tuned for tunnel shooter feel)
    survival += genome.forward_speed * 12.0
    survival += (1.0 - abs(genome.strafe_bias)) * 8.0
    survival += genome.turn_rate * 4.0
    survival += genome.dodge_chance * 15.0
    survival -= genome.reaction_delay * 1.5
    survival += genome.vertical_bias * 3.0   # slight bonus for vertical play

    # Add noise + occasional "lucky dodge"
    survival += random.uniform(-8, 12)

    # Clamp to reasonable game length
    return max(5.0, min(180.0, survival))


def fitness(genome: CreatureGenome, human_rating: Optional[float] = None) -> float:
    base = evaluate_genome(genome)
    if human_rating is not None:
        base += human_rating * 4.0   # novelty / coolness bonus
    return base

# ============================================================
# GA Engine Core (TAS-GA blocks implementation)
# ============================================================
def initialize_population(n: int = 24) -> List[CreatureGenome]:
    return [CreatureGenome.random() for _ in range(n)]


def evaluate_population(pop: List[CreatureGenome]) -> List[float]:
    return [fitness(g) for g in pop]


def select_top(pop: List[CreatureGenome], fitnesses: List[float], k: int = 8) -> List[CreatureGenome]:
    # Elitism + tournament-ish
    sorted_pop = [g for _, g in sorted(zip(fitnesses, pop), key=lambda x: x[0], reverse=True)]
    return sorted_pop[:k]


def crossover(p1: CreatureGenome, p2: CreatureGenome) -> CreatureGenome:
    """Uniform crossover"""
    child = CreatureGenome(
        forward_speed=random.choice([p1.forward_speed, p2.forward_speed]),
        turn_rate=random.choice([p1.turn_rate, p2.turn_rate]),
        strafe_bias=random.choice([p1.strafe_bias, p2.strafe_bias]),
        dodge_chance=random.choice([p1.dodge_chance, p2.dodge_chance]),
        vertical_bias=random.choice([p1.vertical_bias, p2.vertical_bias]),
        reaction_delay=random.choice([p1.reaction_delay, p2.reaction_delay]),
        scale=random.choice([p1.scale, p2.scale]),
        segment_count=random.choice([p1.segment_count, p2.segment_count]),
        hue=random.choice([p1.hue, p2.hue]),
        deformation=random.choice([p1.deformation, p2.deformation]),
        glow_intensity=random.choice([p1.glow_intensity, p2.glow_intensity]),
        personality_seed=random.choice([p1.personality_seed, p2.personality_seed]),
    )
    return child


def mutate_population(pop: List[CreatureGenome], rate: float = 0.18) -> List[CreatureGenome]:
    return [g.mutate(rate) for g in pop]


def persist_generation(gen_num: int, pop: List[CreatureGenome], fitnesses: List[float], path: str = "evolution_log.jsonl"):
    with open(path, "a") as f:
        for g, fit in zip(pop, fitnesses):
            record = {
                "generation": gen_num,
                "timestamp": time.time(),
                "fitness": round(fit, 2),
                "genome": g.to_dict()
            }
            f.write(json.dumps(record) + "\n")


def run_ga(generations: int = 6, pop_size: int = 24, elite_k: int = 8):
    print("=== TunnelStrike GA Engine Prototype v1 ===")
    print(f"Running {generations} generations | pop={pop_size} | elite={elite_k}\n")

    population = initialize_population(pop_size)
    best_overall = None
    best_fitness = -1

    for gen in range(generations):
        fitnesses = evaluate_population(population)
        persist_generation(gen, population, fitnesses)

        # Find best this gen
        best_idx = max(range(len(fitnesses)), key=lambda i: fitnesses[i])
        best_this_gen = population[best_idx]
        best_fit = fitnesses[best_idx]

        if best_fit > best_fitness:
            best_fitness = best_fit
            best_overall = best_this_gen

        print(f"Gen {gen:02d} | Best fitness: {best_fit:6.1f} | "
              f"speed={best_this_gen.forward_speed:.2f} turn={best_this_gen.turn_rate:.2f} "
              f"dodge={best_this_gen.dodge_chance:.2f}")

        # Selection
        elites = select_top(population, fitnesses, elite_k)

        # Create next generation
        next_pop: List[CreatureGenome] = elites.copy()
        while len(next_pop) < pop_size:
            p1, p2 = random.sample(elites, 2)
            child = crossover(p1, p2)
            next_pop.append(child)

        # Mutation on the non-elite part
        for i in range(elite_k, len(next_pop)):
            if random.random() < 0.85:
                next_pop[i] = next_pop[i].mutate(0.18)

        population = next_pop

    print("\n=== Best creature overall ===")
    print(json.dumps(best_overall.to_dict(), indent=2))
    print(f"Final best fitness: {best_fitness:.1f}")
    print(f"\nEvolution log written to evolution_log.jsonl")
    return best_overall


if __name__ == "__main__":
    random.seed(42)  # reproducible for demo
    run_ga(generations=8, pop_size=20, elite_k=6)
