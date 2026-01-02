#!/usr/bin/env bash
set -euo pipefail

mkdir -p results/raw

CSV="results/raw/seq.csv"
ITERS=1000
REPS=5

# Cabecera
echo "version,nx,ny,iters,threads_or_procs,time_s,checksum" > "$CSV"

# Ejecuta tamaños
for N in 100 500 1000; do
  ./main_seq "$N" "$N" "$ITERS" "$REPS" >> "$CSV"
done

echo "OK -> generado $CSV"
