#ifndef LAYER_H_
#define LAYER_H_

#include "layer_cache.h"
#include "transformation.h"
#include <string>
#include <vector>

class LayerCache;
class Transformation;
class LinearTransformation;
class ActivationTransformation;

class Layer {
    private:
        // only protected to make access to layer cache easier
        std::unique_ptr<LayerCache> _layer_cache; 

    public: 
        // destructor 
        virtual ~Layer() {};

        // setter
        void SetLayerCache(std::unique_ptr<LayerCache>);

        // getter
        std::unique_ptr<LayerCache>& GetLayerCache() { return _layer_cache; }

        // forward pass
        void Input(std::vector<double>); // TODO: do we copy too often here?
        virtual void Forward() = 0;
        std::vector<double> Output(); // TODO: better to return const ref?

        // backward pass
        // virtual void Backward() = 0;

        // summary
        virtual std::string Summary() = 0;
};

/**********************
 * LINEAR LAYER CLASS *
 **********************/
 
// TODO: need to refactor, e.g. with an interface Layer or a factory method?
// issues with interface Layer: Transformation class and downcasting

class LinearLayer: public Layer {
    private:
        std::unique_ptr<LinearTransformation> _transformation;

    public:
        // constructor
        LinearLayer(int rows, int cols); 

        // destructor
        // TODO: always needed?
        ~LinearLayer() {};

        // initialize
        void Initialize(std::string initialization_type);

        // forward pass
        void Forward();
        // backward pass AND updating weights
        //void Backward() {}

        // summary
        std::string Summary();
};


/********************
 * ACTIVATION LAYER *
 ********************/

class ActivationLayer: public Layer {
    private:
        std::unique_ptr<ActivationTransformation> _transformation;
        std::string _activation;

    public:
        // default constructor
        ActivationLayer(int m, std::string activation_fct);
           
        // destructor
        // TODO: always needed?
        ~ActivationLayer() {};

        // setters/getters
        std::string GetActivationString() { return _activation; }

        // forward pass
        void Forward();
        // backward pass AND updating weights
        //void Backward() {}

        // summary
        std::string Summary();
};



// flatten layer --> extra layer class
        //static std::vector<double> Flatten(const std::vector<std::vector<double> >& matrix);



#endif // LAYER_H_