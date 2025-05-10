#include <Kokkos_Core.hpp>
#include <gridformat/grid/image_grid.hpp>
#include <gridformat/gridformat.hpp>

using Kokkos::MDRangePolicy;
using Kokkos::parallel_for;
using Kokkos::printf;
using Kokkos::RangePolicy;
using Kokkos::View;

namespace Const {
const double pi = Kokkos::numbers::pi;
const double gamma = 5.0 / 3.0;
} // namespace Const

void save_fields(View<double***>& u, auto& writer, const double time) {
    // save fields
    writer.set_cell_field("rho", [&](const auto cell) {
        return u(cell.location[0], cell.location[1], 0);
    });
    writer.set_cell_field("mx", [&](const auto cell) {
        return u(cell.location[0], cell.location[1], 1);
    });
    writer.set_cell_field("my", [&](const auto cell) {
        return u(cell.location[0], cell.location[1], 2);
    });
    writer.set_cell_field("E", [&](const auto cell) {
        return u(cell.location[0], cell.location[1], 3);
    });
    writer.set_cell_field("Bx", [&](const auto cell) {
        return u(cell.location[0], cell.location[1], 4);
    });
    writer.set_cell_field("By", [&](const auto cell) {
        return u(cell.location[0], cell.location[1], 5);
    });
    writer.write(time);
}

int main(int argc, char* argv[]) {
    Kokkos::ScopeGuard guard(argc, argv);
    // prepare mesh
    const int nx = 32, ny = 32;
    GridFormat::ImageGrid<2, double> grid{
        {2 * Const::pi, 2 * Const::pi}, // domain size
        {nx, ny} // number of cells (pixels) in each direction
    };
    GridFormat::VTKHDFTimeSeriesWriter writer{grid, "data"};

    // prepare fields
    // u = [rho, rho*u, rho*v, E, Bx, By]
    View<double***> u("u", nx, ny, 6);
    const auto cells_range = grid.cells();
    std::vector cells(cells_range.begin(), cells_range.end());
    // initialize fields
    parallel_for(
        cells.size(), KOKKOS_LAMBDA(const int k) {
            auto cell = cells[k];
            auto [i, j] = cell.location;
            auto [x, y] = grid.center(cell);

            u(i, j, 0) = Const::gamma * Const::gamma;
            u(i, j, 1) = -Kokkos::sin(y);
            u(i, j, 2) = Kokkos::sin(x);
            u(i, j, 3) = -Kokkos::sin(y);
            u(i, j, 4) = Kokkos::sin(2 * x);
            u(i, j, 5) = Const::gamma;
        });

    // save initial condition
    save_fields(u, writer, 0.0);

    // time loop
    const double dt = 0.01;
    const double tmax = 1.0;
    for (double t = dt; t < tmax; t += dt) {
        // update fields
        parallel_for(
            cells.size(), KOKKOS_LAMBDA(const int k) {
                auto cell = cells[k];
                auto [i, j] = cell.location;
                auto [x, y] = grid.center(cell);
            });
        // save fields
        save_fields(u, writer, t);
    }

    return 0;
}
