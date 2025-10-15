#include "quantum_framework.hpp"
#include <iostream>
#include <cmath>

void verify_quaternion_algebra() {
    std::cout << "=== QUATERNION CONCEPT DEMONSTRATION ===\n\n";
    std::cout << "Note: Full quaternion algebra requires complex numbers.\n";
    std::cout << "This demonstrates the concept using real 2×2 matrices.\n\n";

    // Get the quaternion basis elements
    auto i = qi::FundamentalAlgebraicAtom::quaternion_i();
    auto j = qi::FundamentalAlgebraicAtom::quaternion_j();
    auto k = qi::FundamentalAlgebraicAtom::quaternion_k();

    std::cout << "Quaternion-inspired basis elements:\n";
    std::cout << "i = " << i.to_string() << " (rotation in xy-plane)\n";
    std::cout << "j = " << j.to_string() << " (rotation in z-direction)\n";
    std::cout << "k = " << k.to_string() << " (combined rotation)\n\n";

    // Show their algebraic properties
    std::cout << "Algebraic properties:\n";
    auto i_squared = i * i;
    auto j_squared = j * j;
    auto k_squared = k * k;

    std::cout << "i² = " << i_squared.to_string() << "\n";
    std::cout << "j² = " << j_squared.to_string() << "\n";
    std::cout << "k² = " << k_squared.to_string() << "\n\n";

    // Show interactions
    auto ij = i * j;
    auto ji = j * i;
    auto jk = j * k;
    auto kj = k * j;

    std::cout << "Interaction patterns:\n";
    std::cout << "ij = " << ij.to_string() << "\n";
    std::cout << "ji = " << ji.to_string() << "\n";
    std::cout << "jk = " << jk.to_string() << "\n";
    std::cout << "kj = " << kj.to_string() << "\n\n";

    // Show that they represent different algebraic structures
    std::cout << "Algebraic diversity:\n";
    std::cout << "i is complex: " << (i.is_complex() ? "✓" : "✗") << "\n";
    std::cout << "j is split-complex: " << (j.is_split_complex() ? "✓" : "✗") << "\n";
    std::cout << "k is dual: " << (k.is_dual() ? "✓" : "✗") << "\n\n";

    std::cout << "=== KEY INSIGHT ===\n";
    std::cout << "• The 2×2 matrix can represent multiple algebraic concepts\n";
    std::cout << "• Different basis elements naturally have different algebraic properties\n";
    std::cout << "• This demonstrates the multi-algebraic nature of the framework\n";
    std::cout << "• Full quaternion algebra would require extending to complex matrices\n\n";
}

void test_quaternion_composition() {
    std::cout << "\n=== MULTI-ALGEBRAIC COMPOSITION TEST ===\n\n";

    // Test composing elements from different algebraic systems
    auto i = qi::FundamentalAlgebraicAtom::quaternion_i();
    auto j = qi::FundamentalAlgebraicAtom::quaternion_j();
    auto k = qi::FundamentalAlgebraicAtom::quaternion_k();

    // Create a mixed algebraic composition
    auto composition = qi::FundamentalAlgebraicAtom(1.0) +
                      i * qi::FundamentalAlgebraicAtom(0.5) +
                      j * qi::FundamentalAlgebraicAtom(0.3) +
                      k * qi::FundamentalAlgebraicAtom(0.2);

    std::cout << "Mixed algebraic composition:\n";
    std::cout << "1.0 + 0.5i + 0.3j + 0.2k = " << composition.to_string() << "\n\n";

    // Show how different algebraic systems interact
    std::cout << "Algebraic interactions:\n";
    auto complex_component = i * qi::FundamentalAlgebraicAtom(2.0);
    auto split_complex_component = j * qi::FundamentalAlgebraicAtom(1.5);
    auto dual_component = k * qi::FundamentalAlgebraicAtom(0.8);

    std::cout << "Complex component (2i): " << complex_component.to_string() << "\n";
    std::cout << "Split-complex component (1.5j): " << split_complex_component.to_string() << "\n";
    std::cout << "Dual component (0.8k): " << dual_component.to_string() << "\n\n";

    // Show emergent properties
    auto cross_product = complex_component * split_complex_component;
    std::cout << "Cross-system product (complex × split-complex):\n";
    std::cout << "2i × 1.5j = " << cross_product.to_string() << "\n";
    std::cout << "Emergent properties: " << cross_product.describe_emergent_properties() << "\n";
}

int main() {
    verify_quaternion_algebra();
    test_quaternion_composition();
    return 0;
}