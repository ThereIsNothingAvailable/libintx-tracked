#ifndef PARSE_BASIS_SET_V2_H
#define PARSE_BASIS_SET_V2_H

#include <vector>
#include <string>

// Struct for Gaussian primitives
struct GaussianPrimitive {
    double exponent;
    double coefficient;
    double pCoefficient = 0.0; // P-type coefficient for SP basis
};

// Struct for Basis Function (S, P)
struct BasisFunction {
    std::string type;
    std::vector<int> angularMomentum; // (l_x, l_y, l_z)
    std::vector<GaussianPrimitive> primitives;
    // double position[3];

    BasisFunction() : angularMomentum(3, 0) {}
};

// Struct for an Atom
struct Atom {
    std::string element;
    std::vector<BasisFunction> basisFunctions;
    double position[3]; // (x, y, z) coordinates

    Atom() {
        position[0] = position[1] = position[2] = 0.0;  // Default to (0,0,0)
    }
};;

// Function to parse the STO-3G basis set file
std::vector<Atom> parseBasisSet(const std::string& filePath);

#endif // PARSE_BASIS_SET_V2_H
