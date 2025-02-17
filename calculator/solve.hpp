#pragma once
#include "tree.hpp"

double doFunction(std::string name, double value = 0) {
    static double _logbase{ 2 };
    static double _modbase{ 3 };
    // No NTHROOT function because you can do that with x^(1/y)
    // Trigonometry
    if      (name == "SIN")   return std::sin(value);
    else if (name == "ASIN")  return std::asin(value);
    else if (name == "COS")   return std::cos(value);
    else if (name == "ACOS")  return std::acos(value);
    else if (name == "TAN")   return std::tan(value);
    else if (name == "ATAN")  return std::atan(value);
    // Roots
    else if (name == "SQRT")  return std::sqrt(value);
    else if (name == "CBRT")  return std::pow(value, (1/3));
    // Logarithms
    else if (name == "SETN")  {
        _logbase = value; // Because functions can't take two arguments, 'n'
        return 0;         // must be set before using LOGN
    }
    else if (name == "LOG")   return std::log10(value);
    else if (name == "LOGE")  return std::log(value);
    else if (name == "LOGN")  return std::log(value) / std::log(_logbase);
    // Etc
    else if (name == "ABS" && value >= 0)  return value;
    else if (name == "ABS" && value < 0)   return -value;
    else if (name == "SIGN" && value > 0)  return 1;
    else if (name == "SIGN" && value == 0) return 0;
    else if (name == "SIGN" && value < 0)  return -1;
    // Modulo
    else if (name == "EVEN")  return std::fmod(value, 2);
    else if (name == "SETM")  {
        _modbase = value; // Because functions can't take two arguments, 'n'
        return 0;         // must be set before using LOGN
    }
    else if (name == "MOD")  return std::fmod(value, _modbase);
    // Math constants
    else if (name == "PI" && value != 0)  return std::numbers::pi * value;
    else if (name == "PI")    return std::numbers::pi;
    else if (name == "E" && value != 0)  return std::numbers::e * value;
    else if (name == "E")     return std::numbers::e;
    else {
        std::cout << "<?> Unknown function " << name << "\n";
        return 0;
    }
}

double solveOperation(o oper, double left, double right, std::string function = "") {
    switch(oper) {
        case o::add: return left + right;
        case o::subtract: return left - right;
        case o::negate: return -right;
        case o::multiply: return left * right;
        case o::divide: return left / right;
        case o::exponent: return std::pow(left, right);
        case o::function: return doFunction(function, right);
        case o::none:
        default: std::cout << "<?> [solveOperation:0] Unknown operation " << oAsString(oper) << ".\n"; return right;
    }
}

double solveTree(const TreeItem& item, std::map<char, double> variables) {
    double left{ 0 };
    double right{ 0 };
    // {0, 1} so right happens first and
    // Eg LOGN(SETN(4)+ x*LOGN(SETN(5)+ y))
    // would set N to 5 AFTER 4 and the original log would use 4.
    // If the left is evaluated last, LOGN(SETN(x)+ ... will always 
    // be log base x
    for (const int i : {1, 0}) {
        double* v{ &left };
        TreeItem* side{ item.left };
        if (i == 1) {
            v = &right;
            side = item.right;
        }
        if (side != nullptr) {
            if (side->solved) {
                *v = side->value;
            } else if (side->isVariable && variables.contains(side->variable)) {
                *v = variables.find(side->variable)->second;
            } else if (side->operation != o::none) {
                *v = solveTree(*side, variables);
            }
        }
    }
    return solveOperation(item.operation, left, right, item.function);
}
