#include <Kokkos_Core.hpp>
#include <fstream>

using Kokkos::MDRangePolicy;
using Kokkos::parallel_for;
using Kokkos::printf;
using Kokkos::View;

// gcc version too old because system is too old?
// use ubunty instead of conda image, but we need to install python properly
void save(const View<double**>& u, const View<double**>& mesh_x,
          const View<double**>& mesh_y, int step) {
    std::fstream fs;
    fs.open(std::format("data/step_{:03d}.csv", step), std::fstream::out);
    fs << "x, y, u\n";
    for (int i = 0; i < u.extent(0); ++i) {
        for (int j = 0; j < u.extent(1); ++j) {
            fs << std::format("{}, {}, {}\n", mesh_x(i, j), mesh_y(i, j),
                              u(i, j));
        }
    }
}

int main(int argc, char* argv[]) {
    Kokkos::ScopeGuard guard(argc, argv);
    // prepare mesh
    int nx = 32, ny = 32;
    std::array<double, 2> x_range{0, 2 * Kokkos::numbers::pi};
    std::array<double, 2> y_range{0, 2 * Kokkos::numbers::pi};
    View<double**> mesh_x("mesh_x", nx, ny);
    View<double**> mesh_y("mesh_y", nx, ny);
    parallel_for(
        MDRangePolicy({0, 0}, {nx, ny}), KOKKOS_LAMBDA(int i, int j) {
            mesh_x(i, j) =
                x_range[0] + i * (x_range[1] - x_range[0]) / (nx - 1);
            mesh_y(i, j) =
                y_range[0] + j * (y_range[1] - y_range[0]) / (ny - 1);
        });

    // prepare fields
    // u = [rho, rho*u, rho*v, E, Bx, By]
    View<double***> u("u", nx, ny, 6);

    // initialize fields
    double gamma = 5.0 / 3.0;
    parallel_for(
        MDRangePolicy({0, 0}, {nx, ny}), KOKKOS_LAMBDA(int i, int j) {
            u(i, j, 0) = gamma * gamma;
            u(i, j, 1) = -Kokkos::sin(mesh_y(i, j));
            u(i, j, 2) = Kokkos::sin(mesh_x(i, j));
            u(i, j, 3) = -Kokkos::sin(mesh_y(i, j));
            u(i, j, 4) = Kokkos::sin(2 * mesh_x(i, j));
            u(i, j, 5) = gamma;
        });
    return 0;
}
