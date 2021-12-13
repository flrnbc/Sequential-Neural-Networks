#include <cmath>
#include <stdexcept>

#include "activation.h"

double heaviside(double x) {
    if (x < 0) { return 0; }
    return 1;
}

double identity(double x) {
    return x;
}

double prelu(double x, double a) {
    if (x <= 0) { return a*x; }
    return x;
}

double relu(double x) {
    return prelu(x, 0);
}

double sigmoid(double x) {
    return 1/(1 + exp(-x));
}


ActivationFct::ActivationFct(std::string fct_name) {
    if (fct_name == "heaviside") {
        activation_fct = &heaviside;
    }
    else if (fct_name == "identity") {
        activation_fct = &identity;
    }
    // TODO #B: to simplify, only offer relu and not prelu at the moment
    else if (fct_name == "relu") {
        activation_fct = &relu;
    }
    else if (fct_name == "sigmoid") {
        activation_fct = &sigmoid;
    }
    else if (fct_name == "softmax") {
        activation_fct = &exp; // note that we post-process for "softmax" (see below)
    }
    else if (fct_name == "tanh") {
        activation_fct = &tanh;
    }
    else throw std::invalid_argument("Not a valid activation function.");

    // finally set the name
    this->SetName(fct_name);
}


std::vector<double> ActivationFct::Evaluate(std::vector<double> v) {
    // TODO #A: optimize vectorization?! (OpenMP?)
    for (double& d: v) {
        d = activation_fct(d); // NOTE: range-based did not modify the values (even when using &)?!
    }

    // TODO: that's not ideal but ok for now...
    if (_name == "softmax") {
        double sum = 0.0;

        for (double& d: v) {
            sum += d; // note the definition of softmax above
        }

        for (double& d: v) {
            d /= sum; // sum != 0
        }
    }

    return v;
}
