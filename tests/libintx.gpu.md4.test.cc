#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test.h"

#include "libintx/engine/md/reference.h"
#include "libintx/pure.transform.h"

#include "libintx/gpu/api/api.h"
#include "libintx/gpu/md/engine.h"

#include <unsupported/Eigen/CXX11/Tensor>

using namespace libintx;

void md_eri4_subcase(int A, int B, int C, int D, std::pair<int,int> K = {1,1}) {

  namespace gpu = libintx::gpu;

  printf("(%i%i|%i%i) K={%i,%i}\n", A, B, C, D, K.first, K.second);

  int M = 16+3;
  int N = 16+1;

  int NA = npure(A);
  int NB = npure(B);
  int NC = npure(C);
  int ND = npure(D);

  auto [bra,ijs] = test::basis2({A,B}, {K.first,1}, M);
  auto [ket,kls] = test::basis2({C,D}, {K.second,1}, N);

  const size_t batch_size_ij = 2;  // Small batch sizes
  const size_t batch_size_kl = 2;

  // Allocate only small temporary result tensor
  Eigen::Tensor<double,6> batch_result(batch_size_ij, NA, NB, NC, ND, batch_size_kl);
  gpu::host::register_pointer(batch_result.data(), batch_result.size());

  gpuStream_t stream = 0;
  auto md = gpu::md::eri<4>(bra, ket, stream);

  // Big CPU-side result tensor for validation
  Eigen::Tensor<double,6> full_result(M, NA, NB, NC, ND, N);
  full_result.setZero(); // Important to initialize

  for (size_t ij_start = 0; ij_start < ijs.size(); ij_start += batch_size_ij) {
    size_t ij_end = std::min(ij_start + batch_size_ij, ijs.size());

    for (size_t kl_start = 0; kl_start < kls.size(); kl_start += batch_size_kl) {
      size_t kl_end = std::min(kl_start + batch_size_kl, kls.size());

      // Prepare small batches
      std::vector<decltype(ijs)::value_type> ijs_sub(ijs.begin() + ij_start, ijs.begin() + ij_end);
      std::vector<decltype(kls)::value_type> kls_sub(kls.begin() + kl_start, kls.begin() + kl_end);

      // Compute batch into small tensor
      md->compute(ijs_sub, kls_sub, batch_result.data(), {batch_size_ij*NA*NB, NC*ND*batch_size_kl});
      gpu::stream::synchronize(stream);

      // Copy batch results into full result tensor
      for (size_t local_ij = 0; local_ij < ijs_sub.size(); ++local_ij) {
        for (size_t local_kl = 0; local_kl < kls_sub.size(); ++local_kl) {
          for (int a = 0; a < NA; ++a)
          for (int b = 0; b < NB; ++b)
          for (int c = 0; c < NC; ++c)
          for (int d = 0; d < ND; ++d)
          {
            full_result(ij_start + local_ij, a, b, c, d, kl_start + local_kl) = batch_result(local_ij, a, b, c, d, local_kl);
          }
        }
      }
    }
  }

  // Now validate
  for (size_t ij = 0; ij < ijs.size(); ++ij) {
    for (size_t kl = 0; kl < kls.size(); ++kl) {

      auto [i,j] = ijs[ij];
      auto [k,l] = kls[kl];
      Eigen::Tensor<double,4> ab_cd_ref(npure(A), npure(B), npure(C), npure(D));
      {
        Eigen::Tensor<double,4> ab_cd_cartesian(
          ncart(A), ncart(B), ncart(C), ncart(D)
        );
        ab_cd_cartesian.setZero();
        libintx::md::reference::compute(bra[i], bra[j], ket[k], ket[l], ab_cd_cartesian);
        libintx::pure::reference::transform(
          A, B, C, D,
          ab_cd_cartesian,
          ab_cd_ref
        );
      }
      test::check4(
        [&](const auto &ab_cd_ref, auto ... idx) {
          auto ab_cd = full_result(ij, idx..., kl);
          CHECK(ab_cd == ab_cd_ref);
        },
        ab_cd_ref
      );
    }
  }

  gpu::host::unregister_pointer(batch_result.data());

}

#define MD_ERI4_SUBCASE(A,B,C,D,Ks)                     \
  if (test::enabled(A,B,C,D)) {                         \
    SUBCASE(str("(AB|CD)=(",A,B,"|",C,D,")").c_str()) { \
      for (auto K : Ks) {                               \
        md_eri4_subcase(A,B,C,D,K);                     \
      }                                                 \
    }                                                   \
  }

TEST_CASE("gpu.md.eri4") {

  std::vector< std::pair<int,int> > Ks = {
    {1,1}, {1,5}, {3,5}
  };

  MD_ERI4_SUBCASE(1,0,0,0,Ks);
  MD_ERI4_SUBCASE(0,0,0,0,Ks);
  MD_ERI4_SUBCASE(1,0,0,0,Ks);
  MD_ERI4_SUBCASE(1,2,0,0,Ks);
  MD_ERI4_SUBCASE(2,0,0,0,Ks);
  MD_ERI4_SUBCASE(0,0,2,0,Ks);
  MD_ERI4_SUBCASE(1,1,0,0,Ks);
  MD_ERI4_SUBCASE(1,1,1,0,Ks);
  MD_ERI4_SUBCASE(1,1,2,0,Ks);
  MD_ERI4_SUBCASE(2,2,1,0,Ks);
  MD_ERI4_SUBCASE(1,0,2,2,Ks);
  MD_ERI4_SUBCASE(1,1,3,0,Ks);
  MD_ERI4_SUBCASE(3,3,1,0,Ks);
  MD_ERI4_SUBCASE(1,0,3,3,Ks);
  MD_ERI4_SUBCASE(1,1,1,1,Ks);
  MD_ERI4_SUBCASE(2,2,2,2,Ks);
  MD_ERI4_SUBCASE(3,3,3,3,Ks);
}
