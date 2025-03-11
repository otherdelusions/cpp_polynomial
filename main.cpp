#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

class Monom {
public:
    double c;
    int q;

    Monom(double coef = 0, int pow = 0) : c(coef), q(pow) {}

    Monom operator+(const Monom& other) {
        if (q != other.q) {
            throw std::invalid_argument("power mismatch");
        }
        return Monom(c + other.c, q);   
    }

    Monom operator-(const Monom& other) {
        if (q != other.q) {
            throw std::invalid_argument("power mismatch");
        }
        return Monom(c - other.c, q);
    }

    Monom operator*(const Monom& other) {
        return Monom(c * other.c, q + other.q);
    }

    Monom operator/(const Monom& other) {
        if (other.c == 0) {
            throw std::invalid_argument("cant divide by 0");
        }
        return Monom(c / other.c, q - other.q);
    }

    friend std::ostream& operator<<(std::ostream& os, const Monom& m) {
        os << m.c << "x^" << m.q;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Monom& m) {
        is >> m.c >> m.q;
        return is;
    }
};

class Polynomial {
private:
    std::vector<Monom> monoms;

public:
    void addMonom(const Monom& newm) {
        for (auto& monom : monoms) {
            if (monom.q == newm.q) {
                monom = monom + newm;
                return;
            }
        }
        monoms.push_back(newm);
    }

    Polynomial operator+(const Polynomial& other) {
        Polynomial result = *this;
        for (const auto& m : other.monoms) {
            result.addMonom(m);
        }
        return result;
    }

    Polynomial operator-(const Polynomial& other) {
        Polynomial result = *this;
        for (const auto& m : other.monoms) {
            result.addMonom(Monom(-m.c, m.q));
        }
        return result;
    }

    Polynomial operator*(const Polynomial& other) {
        Polynomial result;
        for (int i = 0; i < monoms.size(); i++) {
            for (int j = 0; j < other.monoms.size(); j++) {
                result.addMonom(monoms[i] * other.monoms[j]);
            }
        }
        return result;
    }

    Polynomial operator/(const Polynomial& other) const {
        if (other.monoms.empty()) {
            throw std::invalid_argument("cant divide by 0");
        }
    
        Polynomial result;
        Polynomial remainder = *this;
    
        while (!remainder.monoms.empty()) {
            Monom leadingMonom = remainder.monoms.back();
            Monom divisorMonom = other.monoms.back();
    
            if (leadingMonom.q < divisorMonom.q) {
                break; // Cannot divide further
            }
    
            Monom term = leadingMonom / divisorMonom;
            result.addMonom(term);
    
            Polynomial termPoly;
            termPoly.addMonom(term);
    
            remainder = remainder - (termPoly * other);
        }
    
        return result;
    }

    Polynomial operator/(const Monom& m) {
        Polynomial result;
        for (int i = 0; i < monoms.size(); i++) {
            result.addMonom(monoms[i] / m);
        }
        return result;
    }

    Polynomial operator*(const Monom& m) {
        Polynomial result;
        for (int i = 0; i < monoms.size(); i++) {
            result.addMonom(monoms[i] * m);
        }
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, Polynomial& p) {
        std::sort(p.monoms.begin(), p.monoms.end(), [](const Monom& a, const Monom& b) {
            return a.q < b.q;
        });
        for (int i = 0; i < p.monoms.size(); i++) {\
            // TODO: Переделать этот стыд
            if (p.monoms[i].c == 0) {
                os << "0";

                if (i < p.monoms.size() - 1) {
                    os << " + ";
                }
                continue;
            }
            
            if (p.monoms[i].c != 1 && p.monoms[i].c != -1) {
                os << p.monoms[i].c;
            }

            if (p.monoms[i].c == -1) {
                os << "-";
            }

            if (p.monoms[i].q != 0 && p.monoms[i].q != 1) {
                os << "x^" << p.monoms[i].q;
            }

            if (p.monoms[i].q == 1) {
                os << "x";
            }
            
            if (i < p.monoms.size() - 1) {
                os << " + ";
            }
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Polynomial& p) {
        std::string line;
        std::getline(is, line);
        if (line.empty()) return is;

        std::istringstream iss(line);
        double coef;
        int power;
        
        while (iss >> coef >> power) {
            p.addMonom(Monom(coef, power));
        }
        
        return is;
    }
};

int main() {
    // 5x^6 + 3x^6 + 2x^0 + 48x^3 + 9x^9 + 6x^12
    // 5x^5 + 50x^50 + 20x^0 + 2x^6 
    Polynomial p1, p2;
    std::cout << "Enter first polynomial monoms separated by space:\n";
    std::cin >> p1;

    std::cout << "Enter second polynomial monoms separated by space:\n";
    std::cin >> p2;

    std::cout << "First polynomial: " << p1 << std::endl;
    std::cout << "Second polynomial: " << p2 << std::endl;

    Polynomial sum = p1 + p2;
    Polynomial diff = p1 - p2;
    Polynomial mult = p1 * p2;
    //Polynomial div = p1 / p2;
    std::cout << "p1 + p2: " << sum << std::endl;
    std::cout << "p1 - p2: " << diff << std::endl;
    std::cout << "p1 * p2: " << mult << std::endl;
    //std::cout << "p1 / p2: " << div << std::endl;

    std::cout << std::endl;
    Monom m(2, 6);
    Polynomial divm = p1 / m;
    Polynomial multm = p1 * m;
    std::cout << "p1 * 2x^6: " << multm << std::endl;
    std::cout << "p1 / 2x^6: " << divm << std::endl;

    return 0;
}