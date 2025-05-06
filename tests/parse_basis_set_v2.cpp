#include "parse_basis_set_v2.h"
#include <iostream>
#include <vector>

// ✅ Function to Hardcode H₂O Molecule with STO-3G Basis Set using PySCF Geometry
std::vector<Atom> parseBasisSet(const std::string& filePath) {
    std::vector<Atom> molecule;

    std::cout << "🔹 Hardcoding H₂O Molecule with STO-3G Basis Set (Updated Geometry)...\n";

    // ✅ Hardcode Hydrogen (H1) - PySCF cc-pVDZ Geometry
    Atom H1;
    H1.element = "H";
    H1.position[0] = 0.000000;
    H1.position[1] = 0.755453;
    H1.position[2] = -0.471161;

    BasisFunction H1_S;
    H1_S.type = "S";
    H1_S.angularMomentum = {0, 0, 0};  // S orbital
    H1_S.primitives = {
        {3.42525, 0.154328},
        {0.623914, 0.535328},
        {0.168855, 0.444635}
    };
    H1.basisFunctions.push_back(H1_S);
    molecule.push_back(H1);

    std::cout << "✅ Stored Hydrogen 1 at (" << H1.position[0] << ", " << H1.position[1] << ", " << H1.position[2] << ")\n";

    // ✅ Hardcode Hydrogen (H2)
    Atom H2;
    H2.element = "H";
    H2.position[0] = 0.000000;
    H2.position[1] = -0.755453;
    H2.position[2] = -0.471161;

    BasisFunction H2_S;
    H2_S.type = "S";
    H2_S.angularMomentum = {0, 0, 0};  // S orbital
    H2_S.primitives = {
        {3.42525, 0.154328},
        {0.623914, 0.535328},
        {0.168855, 0.444635}
    };
    H2.basisFunctions.push_back(H2_S);
    molecule.push_back(H2);

    std::cout << "✅ Stored Hydrogen 2 at (" << H2.position[0] << ", " << H2.position[1] << ", " << H2.position[2] << ")\n";

    // ✅ Hardcode Oxygen (O)
    Atom O;
    O.element = "O";
    O.position[0] = 0.000000;
    O.position[1] = 0.000000;
    O.position[2] = 0.117790;

    // Oxygen S orbital
    BasisFunction O_S;
    O_S.type = "S";
    O_S.angularMomentum = {0, 0, 0};  // S orbital
    O_S.primitives = {
        {130.709, 0.154328},
        {23.8089, 0.535328},
        {6.44361, 0.444635}
    };
    O.basisFunctions.push_back(O_S);

    // Oxygen SP orbitals (split into S and 3 P orbitals)
    BasisFunction O_SP;
    O_SP.type = "SP";
    O_SP.angularMomentum = {0, 0, 0};  // Placeholder
    O_SP.primitives = {
        {5.03315, -0.0999672, 0.155916},
        {1.1696, 0.399513, 0.607684},
        {0.380389, 0.700115, 0.391957}
    };

    // ✅ Split SP into 1 S and 3 P orbitals
    BasisFunction O_SP_S = O_SP;
    O_SP_S.type = "S";
    for (auto& prim : O_SP_S.primitives) {
        prim.pCoefficient = 0.0;
    }
    O.basisFunctions.push_back(O_SP_S);

    for (int i = 0; i < 3; ++i) {
        BasisFunction O_SP_P = O_SP;
        O_SP_P.type = "P";
        if (i == 0) O_SP_P.angularMomentum = {1, 0, 0};  // Px
        if (i == 1) O_SP_P.angularMomentum = {0, 1, 0};  // Py
        if (i == 2) O_SP_P.angularMomentum = {0, 0, 1};  // Pz

        for (auto& prim : O_SP_P.primitives) {
            prim.coefficient = prim.pCoefficient;
            prim.pCoefficient = 0.0;
        }
        O.basisFunctions.push_back(O_SP_P);
    }

    molecule.push_back(O);

    std::cout << "✅ Stored Oxygen at (" << O.position[0] << ", " << O.position[1] << ", " << O.position[2] << ")\n";

    return molecule;
}
