# Roma

A modular routing engine in C++. Early stage, focused on a clean core and domain-agnostic design.

## Why
Most routing stacks are locked to one domain. Roma aims to separate a small kernel (graph + search + lifecycle) from pluggable modules (costs, constraints, preprocessors, data adapters) so the same engine can serve maps, networks, and aerospace.

## Status
- Early development; core graph + pathfinding scaffold WIP
- Maps ingest via GeoJSON targeted first
- Tests and CMake build in place

## Design pillars
- **Domain-agnostic kernel** — no domain fields in core types
- **Pluggable modules** — costs, constraints, preprocessors, adapters
- **Determinism & traceability** — reproducible routes, query traces
- **Performance awareness** — memory locality and update strategy matter

## Layout
- `src/`   core implementation
- `include/` public headers
- `tests/` unit & scenario tests
- `apps/`  small demos / runners
- `DEVELOPMENT_NOTES.md` notes/roadmap

## Build & test (minimal)
- CMake project; standard out-of-source build
- Tests live under `tests/` (run via your test runner or CTest)

## Near-term
- Map profile MVP: GeoJSON → query → route trace
- Basic tracing/metrics (nodes expanded, timings)
- Profile config for objectives/constraints
- CI that builds and runs tests on push

## Longer-term (profiles)
- **Automotive/Maps:** time-dependent costs, turn rules, CH/MLD
- **Packet Networks:** latency/jitter, SRLG constraints, partitions
- **Aerospace/Robotics:** altitude bands, corridors, wind/fuel overlays

## License
TBD.
