# BGP Simulator

This project is an implementation of a simplified Border Gateway Protocol (BGP) simulator for CSE 3150.
In plain terms, it:
Loads AS (Autonomous System) relationship data (from CAIDA)
Builds a graph of how networks connect
Simulates how routing announcements spread
Optionally applies Route Origin Validation (ROV)
Outputs each AS’s routing table (RIB) as a CSV file

## Build and run

```
make                # builds build/bgp_sim
make test           # builds and runs the GoogleTest suite
make bench-prefix   # runs end-to-end against bench/prefix and diffs output
make bench-subprefix
make bench-many     # the 100k-node, 172MB-output benchmark
```

The first time you run make test, it will download GoogleTest.
If you already have GoogleTest installed (e.g. via Homebrew), you can skip that step:

```
make test GTEST_PREFIX=$(brew --prefix googletest)
```

CLI usage of the binary:

```
build/bgp_sim --caida  <as-rel2.txt>      \
              --anns   <anns.csv>         \
              --rov    <rov_asns.csv>     \
              --output <ribs.csv>
```

Exit codes: 
0 → success
1 → invalid input or arguments
2 → provider/customer cycle detected (this is an error in the input graph)

## File layout

```
include/sim/        Public headers (Announcement, Policy, BGP, ROV, AS,
                    AsGraph, Simulator, csv_io, types).
src/                Implementation .cpp files plus main.cpp.
tests/              GoogleTest suites: parser, cycle, ranks, conflict,
                    rov, propagation, csv.
bench/              System-test data and compare_output.sh.
Makefile            Builds the binary and (on demand) the test runner.
```

## Architecture and design decisions

### Graph structure
Each AS (Autonomous System) is a node
Relationships:
Providers
Customers
Peers
Important design choice:
The graph stores AS nodes using smart pointers
But edges are just ASN numbers (ints) instead of pointers
Why?
Simpler memory management
Faster comparisons
Avoids pointer complexity

### Parsing input
AsGraph::load_caida:
Reads the CAIDA file efficiently
Ignores:
Comment lines (#)
Bad/malformed rows
Interprets relationships:
-1 → provider → customer
0 → peer ↔ peer

### Cycle detection
Before simulation, we check:
“Does the provider/customer graph contain a cycle?”
Uses DFS (depth-first search)
Ignores peer edges (cycles there are allowed)
If a cycle is found → program exits with code 2

### Ranking nodes
We assign each AS a rank based on its position:
Leaves (no customers) → rank 0
Higher nodes → higher rank
This ensures:
Data flows correctly upward first
Then across
Then downward

### Propagation (core simulation)
The simulation runs in three phases:
Phase 1: Up
AS → providers
Lower ranks send first
Phase 2: Across
AS → peers
Only one hop allowed
Prevents invalid routing patterns (valley-free routing)
Phase 3: Down
AS → customers
Reverse of the “up” phase

Important rules enforced
Loop prevention
An AS won’t accept a route if it already appears in the path
Origin routes are protected
An AS always prefers its own announcement

### CChoosing the best route
When multiple routes exist, we pick the best one:
Relationship priority:
  origin > customer > peer > provider
Shorter AS path
Smaller next-hop ASN (tie-breaker)
### ROV

Some ASes can be configured to use ROV
These ASes:
Drop invalid announcements immediately
Never store or propagate them

### Output format

The output CSV looks like:
  asn,prefix,as_path

Ex.
    1,10.0.0.0/24,"(1, 11537, 27)"
    7,192.168.0.0/16,"(7,)"

Sorted by ASN, then prefix
AS paths are formatted like Python tuples

## Test coverage

`make test` runs 40 tests across seven suites:

- **ParserTest** — comments, edge construction, peer symmetry, malformed
  lines, empty input.
- **CycleTest** — acyclic vs cyclic provider chains, self-loops, peer
  cycles allowed, mixed peer/provider graphs.
- **RankTest** — single node, linear chain, diamonds (longest path
  decides rank), peers don't affect rank.
- **ConflictTest** — every rule of `BGP::prefer`, including identity,
  with the tests targeting the comparator directly.
- **ROVTest** — invalid drops, valid passes through, mixed at the same
  prefix.
- **PropagationTest** — full simulator on the bgpsimulator.com example
  graph, covering origin retention, up phase, across phase, down phase,
  shared-provider tie-break, and ROV-based hijack drop.
- **CsvTest** — announcement parsing, ROV ASN parsing, blank-line and
  header tolerance, missing-file handling.

## System tests

The three `bench/*` datasets in this repo are validated end-to-end:

| Dataset       | Lines in     | Output rows | Status |
|---------------|--------------|-------------|--------|
| bench/prefix  | 489k CAIDA   | 78k         | ✓      |
| bench/subprefix | 489k CAIDA | 78k×2 prefixes | ✓ |
| bench/many    | 489k CAIDA   | ~1.5M       | ✓      |

bench/many runs in ~9 seconds with ~900MB peak RSS on a single core, well
under the 2-CPU / 8GB envelope from the spec.

## Known limitations / TODOs

- IPv6 prefixes are passed through verbatim as strings; we don't actually
  parse or canonicalise them. The spec only requires per-prefix routing,
  not aggregation, so this is fine for v1.
- The graph is single-threaded; the per-rank propagation loop is the
  obvious place to introduce a thread pool if a future speed-up is needed.
- Output ordering matches the expected sorted form; we don't preserve any
  natural insertion order from the input CSV.
