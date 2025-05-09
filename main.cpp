#include <Kokkos_Core.hpp>
#include <fstream>
#include <gridformat/gridformat.hpp>

using Kokkos::MDRangePolicy;
using Kokkos::parallel_for;
using Kokkos::printf;
using Kokkos::View;
const double pi = Kokkos::numbers::pi;

void save(const View<double***>& u, const View<double**>& mesh_x,
          const View<double**>& mesh_y, int step) {
    std::fstream fs;
    fs.open(std::format("data/step_{:03d}.csv", step), std::fstream::out);
    fs << "x, y, u\n";
    for (int i = 0; i < u.extent(0); ++i) {
        for (int j = 0; j < u.extent(1); ++j) {
            printf("u(%d,%d, 1)=%f\n", i, j, u(i, j, 1));
            fs << std::format("{}, {}, {}\n", mesh_x(i, j), mesh_y(i, j),
                              u(i, j, 1));
        }
    }
}

int main(int argc, char* argv[]) {
    Kokkos::ScopeGuard guard(argc, argv);
    // prepare mesh
    int nx = 32, ny = 32;
    std::array<double, 2> x_range{0, 2 * pi};
    std::array<double, 2> y_range{0, 2 * pi};
    View<double**> mesh_x("mesh_x", nx, ny);
    View<double**> mesh_y("mesh_y", nx, ny);

    GridFormat::ImageGrid<2, double> grid{
        {2 * pi, 2 * pi}, // domain size
        {32, 32}          // number of cells (pixels) in each direction
    };
    GridFormat::VTKHDFTimeSeriesWriter writer{grid, "data"};
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

    // save initial condition
    // save(u, mesh_x, mesh_y, 0);
    writer.set_cell_field("mx", [&](const auto cell) {
        return u(cell.location[0], cell.location[1], 1);
    });
    writer.write(0.0);

    writer.set_cell_field("mx", [&](const auto cell) {
        return u(cell.location[0], cell.location[1], 1);
    });
    writer.write(0.1);
}
