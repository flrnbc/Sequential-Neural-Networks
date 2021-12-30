#include "transformation.h"
#include <Eigen/Dense>

#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>


// helper function for random numbers
// TODO: is there a performance issue? (since we create a generator etc. each time)?
double RandomNumberUniform(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);

    return dis(gen);
}

double RandomNumberNormal(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(min, max);

    return dis(gen);
}


/******************
 * TRANSFORMATION *
 ******************/

// Transformation::Transformation() {
//     // not meant to be called directly
//     SetCols(0);
//     SetRows(0);
// }

// std::vector<double> Transformation::Transform(std::vector<double> input) {
//     // just the identity
//     return input;
// }

// std::string Transformation::Summary() {
//     return "Identity transformation.";
// }


/*************************
 * LINEAR TRANSFORMATION *
 *************************/

// random initialization (used for different activation functions)
// either He or normalized Xavier initialization
void LinearTransformation::Initialize(std::string initialization_type) {
    if (initialization_type != "He" && initialization_type != "Xavier") {
        // TODO #A: need to catch it somewhere?
        throw std::domain_error("Initialization type is unknown.");
    }

    int rows = Rows();
    int cols = Cols();

    // actual initialization of weights
    if (initialization_type == "Xavier") {      
        // use normalized Xavier weight initialization
        int inputPlusOutputSize = cols + rows;
        // NOTE: uniform_real_distribution(a, b) generates for [a, b) (half-open interval)
        double min = -(sqrt(6)/sqrt(inputPlusOutputSize));
        double max = sqrt(6)/sqrt(inputPlusOutputSize);

        // randomly initialize weights
        // NOTE: be careful with cache-friendliness (outer loop over rows)
        // TODO #A: would be nicer to put this for-loop after the else-block (but then would have to declare dis before; but as what?)
        for (int i=0; i<rows; i++) {
            for (int j=0; j<cols; j++) {
                _weights(i, j) = RandomNumberUniform(min, max); 
            }
        }
    } 
    else {
        // use He weight initialization
        double min = 0.0;
        double max = sqrt(2.0/cols);

        // randomly initialize weights
        for (int i=0; i<rows; i++) {
            for (int j=0; j<cols; j++) {
                _weights(i, j) = RandomNumberNormal(min, max); 
            }
        }
    }

}

// transform method
Eigen::VectorXd LinearTransformation::Transform(Eigen::VectorXd inputVector) {
    if (inputVector.rows() != Cols()) {
        // TODO #A: catch exception somewhere?
        throw std::domain_error("Vector cannot be evaluated.");
    } else {
        return _weights*inputVector;
    }
}

// Summary
std::string LinearTransformation::Summary() {
    std::string type = Type() + "\n";
    std::string rows = std::to_string(Rows());
    std::string cols = std::to_string(Cols());
    std::string shape = "Shape: (" + rows + ", " + cols + ")" + "\n";

    std::string weights_string = "Weights:\n";
    // convert matrix to string
    std::stringstream ss_weights;
    ss_weights << _weights;
    weights_string += ss_weights.str();

    std::string bias_string = "Bias:\n";
    // convert vector to string
    std::stringstream ss_bias;
    ss_bias << _bias;
    bias_string += ss_bias.str();

    std::string derivative_string = "Derivative:\n";
    // convert matrix to string
    std::stringstream ss_derivative;
    ss_derivative << *_derivative;
    derivative_string += ss_derivative.str();

    return type + shape + weights_string + "\n" + bias_string + derivative_string;
}


/*****************************
 * ACTIVATION TRANSFORMATION *
 *****************************/

// transform methods for ActivationTransformation
Eigen::VectorXd ActivationTransformation::Transform(Eigen::VectorXd inputVector) {
    // TODO: optimize with vectorization? (OpenMP?)

    for (int i = 0; i < inputVector.rows(); i++) {
        inputVector(i) = _function(inputVector(i)); // TODO: range-based did not modify the values (even when using &)?!
    }

    if (_type == "softmax") {
        double vectorSum = inputVector.sum();
        for (auto &d: inputVector) {
            d /= vectorSum; // Note: this is safe because _function = exp for softmax
        }
    }

    return inputVector;
}

// update derivative
void ActivationTransformation::UpdateDerivative(Eigen::VectorXd vector) {
    // TODO: this has to change if we introduce more activation functions which are not vectorizations
    if (vector.size() != Cols()) {
        throw std::domain_error("Vector size does not match with the derivative.");
    }

    if (_type != "softmax") {
        for (int j=0; j<Cols(); j++) {
            // safe because we initialize to a zero matrix
            (*_derivative)(j, j) = _function.Derivative(vector(j));
        }
    }

    if (_type == "softmax") {
        /* 
        NOTE: Here we assume that the vector is the output of softmax, i.e.
        vector = softmax(x_1, ... , x_N) = (s_1, ... , s_N). Then the jacobian J is given by 

            J(i, i) = -s_i*(1-s_i)       
            J(i, j) = -s_i*s_j          if i != j.

        */
        for (int i=0; i<Rows(); i++) {
            for (int j=0; j<Cols(); j++) {
                if (i == j) {
                    (*_derivative)(i, j) = -vector(i)*(1-vector(i));
                }
                else {
                    (*_derivative)(i, j) = -vector(i)*vector(j);
                }
            }
        }
    }
}

std::string ActivationTransformation::Summary() {
    std::string rows = std::to_string(Rows());
    std::string cols = std::to_string(Cols());
    std::string shape = "Shape: (" + rows + ", " + cols + ")" + "\n";

    std::string derivative_string = "Derivative:\n";
    // convert matrix to string
    std::stringstream ss_derivative;
    ss_derivative << *_derivative;
    derivative_string += ss_derivative.str();

    return shape + "Function name: " + Type() + "\n" + derivative_string;
}