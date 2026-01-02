#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "grid.h"
#include "config.h"
#include "heat.h"

static double now_sec(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

// Uso: ./main_seq nx ny iters reps
// Defaults: nx=100 ny=100 iters=1000 reps=5
int main(int argc, char **argv) {
    int nx    = (argc > 1) ? atoi(argv[1]) : 100;
    int ny    = (argc > 2) ? atoi(argv[2]) : 100;
    int iters = (argc > 3) ? atoi(argv[3]) : 1000;
    int reps  = (argc > 4) ? atoi(argv[4]) : 5;

    if (nx < 3 || ny < 3 || iters <= 0 || reps <= 0) {
        fprintf(stderr, "Uso: %s nx ny iters reps (nx,ny>=3)\n", argv[0]);
        return 1;
    }

    const real_t hot  = (real_t)100.0;
    const real_t cold = (real_t)0.0;

    grid_t g;
    if (grid_alloc(&g, nx, ny) != 0) {
        fprintf(stderr, "Error reservando malla\n");
        return 1;
    }

    double sum = 0.0;
    double best = 1e300;
    real_t checksum_last = 0;

    for (int r = 0; r < reps; ++r) {
        // Estado inicial idéntico en cada repetición
        grid_init_bc(&g, hot, cold);

        // Medimos SOLO la fase iterativa
        double t0 = now_sec();
        int rc = heat_iterate_seq(&g, iters);
        double t1 = now_sec();

        if (rc != 0) {
            fprintf(stderr, "Error en heat_iterate (rc=%d)\n", rc);
            grid_free(&g);
            return 1;
        }

        double dt = t1 - t0;
        sum += dt;
        if (dt < best) best = dt;

        // Validación (fuera del tiempo medido)
        checksum_last = grid_checksum(&g);
    }

    double avg = sum / reps;

    // CSV: version,nx,ny,iters,threads_or_procs,time_s,checksum
    printf("seq,%d,%d,%d,%d,%.9f,%.6f\n", nx, ny, iters, 1, avg, (double)checksum_last);

    // Info humana por stderr (no se mete en el CSV si rediriges stdout)
    fprintf(stderr, "SEQ nx=%d ny=%d iters=%d reps=%d avg=%.6fs best=%.6fs checksum=%.2f\n",
            nx, ny, iters, reps, avg, best, (double)checksum_last);

    grid_free(&g);
    return 0;
}
