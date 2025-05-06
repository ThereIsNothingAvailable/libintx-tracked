#include "parse_basis_set_v2.h"
#include "libintx/gpu/api/api.h"
#include "libintx/gpu/md/engine.h"
#include <unsupported/Eigen/CXX11/Tensor>

#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <tuple>

using namespace libintx;
namespace gpu = libintx::gpu;

int main() {
    std::vector<Atom> molecule = parseBasisSet("sto-3g.txt");

    std::vector<std::tuple<Gaussian, libintx::array<double, 3>>> basis;

    for (const auto& atom : molecule) {
        libintx::array<double, 3> center = { atom.position[0], atom.position[1], atom.position[2] };
        for (const auto& bf : atom.basisFunctions) {
            int L = bf.angularMomentum[0];
            std::vector<Gaussian::Primitive> primitives;
            for (const auto& prim : bf.primitives) {
                primitives.push_back({prim.exponent, prim.coefficient});
            }
            basis.emplace_back(Gaussian(L, primitives, true), center);
        }
    }

    std::cout << "Parsed and built Basis<Gaussian> from STO-3G.\n";

    std::vector<pair<int, int>> ijs, kls;
    for (size_t i = 0; i < basis.size(); ++i)
        for (size_t j = 0; j < basis.size(); ++j)
            ijs.emplace_back(i, j);
    for (size_t k = 0; k < basis.size(); ++k)
        for (size_t l = 0; l < basis.size(); ++l)
            kls.emplace_back(k, l);

    // Subset of shell pairs for testing
    std::vector<pair<int, int>> ijs_small(ijs.begin(), ijs.begin() + 10);
    std::vector<pair<int, int>> kls_small(kls.begin(), kls.begin() + 10);

    Eigen::Tensor<double, 2> result(10, 10);
    gpu::host::register_pointer(result.data(), result.size());

    // Declare and use stream + md properly
    gpuStream_t stream = 0;
    auto md = gpu::md::eri<4>(basis, basis, stream);
    md->compute(ijs_small, kls_small, result.data(), {10, 10});
    gpu::stream::synchronize(stream);

    std::ofstream out("eri_output_gpu.txt");
    for (size_t ij = 0; ij < 10; ++ij) {
        for (size_t kl = 0; kl < 10; ++kl) {
            double val = result(ij, kl);
            auto [i, j] = ijs_small[ij];
            auto [k, l] = kls_small[kl];
            out << i << " " << j << " " << k << " " << l << " " << val << "\n";
        }
    }
    out.close();

    gpu::host::unregister_pointer(result.data());
    std::cout << "ERIs written to eri_output_gpu.txt\n";

    return 0;
}

