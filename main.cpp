#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

class Monom {
public:
    double c;
    int q;

    Monom(double coef = 0, int pow = 0) : c(coef), q(pow) {}

    Monom operator+(const Monom &other) {
        if (q != other.q) {
            throw std::invalid_argument("power mismatch");
        }
        return Monom(c + other.c, q);
    }

    Monom operator-(const Monom &other) {
        if (q != other.q) {
            throw std::invalid_argument("power mismatch");
        }
        return Monom(c - other.c, q);
    }

    Monom operator*(const Monom &other) const {
        return Monom(c * other.c, q + other.q);
    }

    Monom operator/(const Monom &other) const {
        if (other.c == 0) {
            throw std::invalid_argument("cant divide by 0");
        }
        return Monom(c / other.c, q - other.q);
    }

    friend std::ostream &operator<<(std::ostream &os, const Monom &m) {
        os << m.c << "x^" << m.q;
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Monom &m) {
        is >> m.c >> m.q;
        return is;
    }
};

class Polynomial {
private:
    std::vector<Monom> monoms;

public:
    void addMonom(const Monom &newm) {
        for (Monom &monom : monoms) {
            if (monom.q == newm.q) {
                monom = monom + newm;
                return;
            }
        }
        monoms.push_back(newm);
    }

    Polynomial &operator=(const Polynomial &other) {
        if (this != &other) {
            monoms.clear();
            for (const Monom &m : other.monoms) {
                addMonom(m);
            }
        }
        return *this;
    }

    Polynomial operator+(const Polynomial &other) {
        Polynomial result = *this;
        for (const Monom &m : other.monoms) {
            result.addMonom(m);
        }
        result.sort();
        return result;
    }

    Polynomial operator-(const Polynomial &other) {
        Polynomial result = *this;
        for (const Monom &m : other.monoms) {
            result.addMonom(Monom(-m.c, m.q));
        }
        result.sort();
        return result;
    }

    Polynomial operator*(const Polynomial &other) {
        Polynomial result;
        for (const Monom &m : monoms) {
            for (const Monom &om : other.monoms) {
                result.addMonom(m * om);
            }
        }
        result.sort();
        return result;
    }

    std::pair<Polynomial, Polynomial> operator/(const Polynomial &other) const {
        if (other.monoms.empty() || (other.monoms.size() == 1 && other.monoms[0].c == 0)) {
            throw std::invalid_argument("zero polynomial division");
        }

        Polynomial result; // Частное
        Polynomial divisible = *this; // Делимое, он же - остаток

        // До опустошения делимого многочлена или до тех пор, пока старшая степень делимого больше старшей степени делителя
        while (!divisible.monoms.empty() && divisible.monoms[0].q >= other.monoms[0].q) {
            Monom rmonom = divisible.monoms[0] / other.monoms[0]; // x^5-3x^4+2x^3+7x^2-3x+5 | x^2-x+1 
            result.addMonom(rmonom);                              //                         |‾x^3‾‾‾‾ <--

            Polynomial temp = other * rmonom; //  x^5-3x^4+2x^3+7x^2-3x+5 | x^2-x+1 
                                              //  x^5-x^4+x^3 <--         |‾x^3‾‾‾‾

            divisible = divisible - temp;     //  x^5-3x^4+2x^3+7x^2-3x+5 | x^2-x+1 
                                              // ‾x̲^̲5̲-̲x̲^̲4̲+̲x̲^̲3̲____________ |‾x^3‾‾‾‾
                                              //     -2x^4+x^3+7x^2-3x+5 <--
            divisible.sort();
        }

        result.sort();
        return {result, divisible};
    }

    Polynomial operator/(const Monom &divm) const {
        Polynomial result;
        for (const Monom &m : monoms) {
            result.addMonom(m / divm);
        }
        result.sort();
        return result;
    }

    Polynomial operator*(const Monom &mulm) const {
        Polynomial result;
        for (const Monom &m : monoms) {
            result.addMonom(m * mulm);
        }
        result.sort();
        return result;
    }

    Polynomial operator*(const int muli) const {
        Polynomial result;
        for (const Monom &m : monoms) {
            result.addMonom(m * muli);
        }
        result.sort();
        return result;
    }

    friend std::ostream &operator<<(std::ostream &os, Polynomial &p) {
        p.sort();
        if (p.monoms.size() == 0) {
            os << "0";
            return os;
        }

        for (int i = 0; i < p.monoms.size(); i++) {
            if (p.monoms[i].c < 0) {
                os << "-";
            } else if (i != 0) {
                os << "+";
            }

            if (i != 0) {
                os << " ";
            }

            if (p.monoms[i].c == 1 && p.monoms[i].q == 0) {
                os << p.monoms[i].c;
            } else if (p.monoms[i].c != 1) {
                os << std::abs(p.monoms[i].c);
            }

            if (p.monoms[i].q == 1) {
                os << "x";
            } else if (p.monoms[i].q != 0) {
                os << "x^" << p.monoms[i].q;
            }
            
            if (i < p.monoms.size() - 1) {
                os << " ";
            }
        }
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Polynomial &p) {
        std::string line;
        std::getline(is, line);
        if (line.empty())
            return is;

        std::istringstream iss(line);
        double coef;
        int power;

        while (iss >> coef >> power) {
            p.addMonom(Monom(coef, power));
        }

        p.sort();
        return is;
    }

    void sort() {
        std::sort(monoms.begin(), monoms.end(),
                  [](const Monom &a, const Monom &b) { return a.q > b.q; });
        for (int i = 0; i < monoms.size(); i++) {
            if (monoms[i].c == 0) {
                monoms.erase(monoms.begin() + i);
            }
        }
    }
};

int main() {
    // 1 5 -3 4 2 3 7 2 -3 1 5 0
    // 1 2 -1 1 1 0
    Polynomial p1, p2;
    std::cout << "Enter 1st polynomial's monoms separating all variables by space (e.g. 1 2 3 4 => x^2 + 3x^4):\n";
    std::cin >> p1;

    std::cout << "Enter 2nd polynomial's monoms:\n";
    std::cin >> p2;

    std::cout << "First polynomial: " << p1 << std::endl;
    std::cout << "Second polynomial: " << p2 << std::endl;

    Polynomial sum = p1 + p2;
    Polynomial diff = p1 - p2;
    Polynomial mult = p1 * p2;
    Polynomial div = (p1 / p2).first;
    Polynomial rem = (p1 / p2).second;
    std::cout << "p1 + p2: " << sum << std::endl;
    std::cout << "p1 - p2: " << diff << std::endl;
    std::cout << "p1 * p2: " << mult << std::endl;
    std::cout << "p1 / p2: (" << p2 << ")*(" << div << ") + " << rem << std::endl;

    std::cout << std::endl;
    Monom m(2, 6);
    Polynomial divm = p1 / m;
    Polynomial multm = p1 * m;
    std::cout << "p1 * 2x^6: " << multm << std::endl;
    std::cout << "p1 / 2x^6: " << divm << std::endl;

    return 0;
}
