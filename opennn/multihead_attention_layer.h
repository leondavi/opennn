//   OpenNN: Open Neural Networks Library
//   www.opennn.net
//
//   M U L T I H E A D   A T T E N T I O N   L A Y E R   C L A S S   H E A D E R
//
//   Artificial Intelligence Techniques SL
//   artelnics@artelnics.com

#ifndef MULTIHEADATTENTIONLAYER_H
#define MULTIHEADATTENTIONLAYER_H

// System includes

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

// OpenNN includes

#include "config.h"
#include "layer.h"

#ifdef OPENNN_MKL
#include "../mkl/mkl.h"
#endif

namespace opennn
{

struct MultiheadAttentionLayerForwardPropagation;
struct MultiheadAttentionLayerBackPropagation;
struct MultiheadAttentionLayerBackPropagationLM;

#ifdef OPENNN_CUDA
#include "../../opennn-cuda/opennn-cuda/struct_perceptron_layer_cuda.h"
#endif


/// This class represents a layer of Multihead Attention.

/// MultiheadAttentionLayer has 2 types of input: Context and Input. Output has the shape of Input
/// The layer consists of 2 separate PerceptronLayers at the start (1 for Context and  1 for Input) and 1 at the end, all for projection purposes.
/// In between there is an attention computation.
///
/// Layers of Multihead Attention will be used to construct Transformer models .

class MultiheadAttentionLayer : public Layer

{

public:

    // Constructors

    explicit MultiheadAttentionLayer();

    explicit MultiheadAttentionLayer(const Index&, /// Input size
                                     const Index&, /// Context size
                                     const Index&, /// Embedding depth
                                     const Index& /// Number of attention heads
                                     );

    // Get methods

    bool is_empty() const;

    Index get_input_size() const;
    Index get_context_size() const;
    Index get_depth() const;
    Index get_number_of_heads() const;

    Tensor<Index, 1> get_inputs_dimensions() const final;
    Tensor<Index, 1> get_outputs_dimensions() const final;

    Tensor<type, 3> get_query_kernel() const;
    Tensor<type, 3> get_key_kernel() const;
    Tensor<type, 3> get_value_kernel() const;

    Tensor<type, 3> get_projection_kernel() const;

    Index get_parameters_number() const final;

    // Display messages

    const bool& get_display() const;

    // Set methods

    void set();
    void set(const Index&, const Index&, const Index&, const Index&);

    void set_default();
    void set_name(const string&);

    // Architecture

    void set_input_size(const Index&);
    void set_context_size(const Index&);
    void set_depth(const Index&);
    void set_number_of_heads(const Index&);

    void set_kernels();
    void set_parameters_random() final;

    void set_dropout_rate(const type&);

    // Display messages

    void set_display(const bool&);

    // Linear transformation & projection

    void calculate_query_transformation(const Tensor<type, 3>&, type*);
    void calculate_key_transformation(const Tensor<type, 3>&, type*);
    void calculate_value_transformation(const Tensor<type, 3>&, type*);

    void calculate_output_projection(const Tensor<type, 4>&, type*);

    // Attention computation

    void compute_attention_scores(type*, const Tensor<Index, 1>&, type*, const Tensor<Index, 1>&, type*); /// Softmax before saving

    void compute_attention_output(type*, const Tensor<Index, 1>&, type*, const Tensor<Index, 1>&, type*);

    // Multihead Attention layer outputs

    void forward_propagate(const Tensor<DynamicTensor<type>, 1>&,
                           LayerForwardPropagation*,
                           const bool&) final;

/*
    void forward_propagate(type*,
                           const Tensor<Index, 1>&,
                           Tensor<type, 1>&,
                           LayerForwardPropagation*) final;
*/
    // Expression methods

//    string write_expression(const Tensor<string, 1>&, const Tensor<string, 1>&) const final;

    string write_activation_function_expression() const;

    // Serialization methods
    /// @todo

//    void from_XML(const tinyxml2::XMLDocument&) final;
//    void write_XML(tinyxml2::XMLPrinter&) const final;

protected:

    // MEMBERS

    /// Input size

    Index input_size;

    /// Context size

    Index context_size;

    /// Embedding depth

    Index depth;

    /// Number of attention heads

    Index number_of_heads;

    /// Linear transformation kernels

    Tensor<type, 3> query_kernel;
    Tensor<type, 3> key_kernel;
    Tensor<type, 3> value_kernel;

    /// Linear projection kernel

    Tensor<type, 3> projection_kernel;

    /// Dropour rate

    type dropout_rate = type(0);

    /// Display messages to screen.

    bool display = true;

#ifdef OPENNN_CUDA
#include "../../opennn-cuda/opennn-cuda/perceptron_layer_cuda.h"
#else
    };
#endif

    struct MultiheadAttentionLayerForwardPropagation : LayerForwardPropagation
    {
        // Default constructor

        explicit MultiheadAttentionLayerForwardPropagation() : LayerForwardPropagation()
        {
        }

        virtual ~MultiheadAttentionLayerForwardPropagation()
        {
        }


        explicit MultiheadAttentionLayerForwardPropagation(const Index& new_batch_samples_number, Layer* new_layer_pointer)
            : LayerForwardPropagation()
        {
            set(new_batch_samples_number, new_layer_pointer);
        }

        void set(const Index& new_batch_samples_number, Layer* new_layer_pointer) final
        {
            MultiheadAttentionLayer* layer_pointer = static_cast<MultiheadAttentionLayer*>(new_layer_pointer);

            batch_samples_number = new_batch_samples_number;

            const Index input_size = layer_pointer->get_input_size();

            const Index context_size = layer_pointer->get_context_size();

            const Index depth = layer_pointer->get_depth();

            const Index number_of_heads = layer_pointer->get_number_of_heads();

            // Outputs

            outputs.resize(1);
            Tensor<Index, 1> output_dimensions(3);
            output_dimensions.setValues({batch_samples_number, input_size, depth});
            outputs(0).set_dimensions(output_dimensions);

            // Rest of quantities

            transformed_query.resize(new_batch_samples_number, input_size, depth, number_of_heads);
            transformed_key.resize(new_batch_samples_number, context_size, depth, number_of_heads);
            transformed_value.resize(new_batch_samples_number, context_size, depth, number_of_heads);

            attention_scores.resize(new_batch_samples_number, input_size, context_size, number_of_heads);
            attention_outputs.resize(new_batch_samples_number, input_size, depth, number_of_heads);
        }

        void print() const
        {
//            cout << "Attention scores:" << endl;
//            cout << attention_scores.dimensions() << endl;


//            cout << "Outputs dimensions:" << endl;
//            cout << outputs_dimensions << endl;

//            cout << "Outputs:" << endl;
//            cout << TensorMap<Tensor<type,3>>(outputs_data, outputs_dimensions(0), outputs_dimensions(1), outputs_dimensions(2)) << endl;

//            cout << "Attention scores:" << endl;
//            cout << attention_scores << endl;
        }

        type* get_transformed_query_data()
        {
            return transformed_query.data();
        }

        type* get_transformed_key_data()
        {
            return transformed_key.data();
        }

        type* get_transformed_value_data()
        {
            return transformed_value.data();
        }

        type* get_attention_scores_data()
        {
            return attention_scores.data();
        }

        type* get_attention_outputs_data()
        {
            return attention_outputs.data();
        }

        Tensor<type, 4> transformed_query;
        Tensor<type, 4> transformed_key;
        Tensor<type, 4> transformed_value;

        Tensor<type, 4> attention_scores;
        Tensor<type, 4> attention_outputs;
    };


    struct MultiheadAttentionLayerBackPropagationLM : LayerBackPropagationLM
    {
        // Default constructor

        explicit MultiheadAttentionLayerBackPropagationLM() : LayerBackPropagationLM()
        {

        }


        explicit MultiheadAttentionLayerBackPropagationLM(const Index& new_batch_samples_number, Layer* new_layer_pointer)
            : LayerBackPropagationLM()
        {
            set(new_batch_samples_number, new_layer_pointer);
        }


        virtual ~MultiheadAttentionLayerBackPropagationLM()
        {

        }


        void set(const Index& new_batch_samples_number, Layer* new_layer_pointer)
        {
            layer_pointer = new_layer_pointer;

            batch_samples_number = new_batch_samples_number;

            const Index neurons_number = layer_pointer->get_neurons_number();
            const Index parameters_number = layer_pointer->get_parameters_number();

            deltas.resize(batch_samples_number, neurons_number);

            squared_errors_Jacobian.resize(batch_samples_number, parameters_number);
        }

        void print() const
        {
            cout << "Deltas:" << endl;
            cout << deltas << endl;

            cout << "Squared errors Jacobian: " << endl;
            cout << squared_errors_Jacobian << endl;

        }

        Tensor<type, 2> squared_errors_Jacobian;
    };



    struct MultiheadAttentionLayerBackPropagation : LayerBackPropagation
    {
        // Default constructor

        explicit MultiheadAttentionLayerBackPropagation() : LayerBackPropagation()
        {

        }

        virtual ~MultiheadAttentionLayerBackPropagation()
        {
        }


        explicit MultiheadAttentionLayerBackPropagation(const Index& new_batch_samples_number, Layer* new_layer_pointer)
            : LayerBackPropagation()
        {
            set(new_batch_samples_number, new_layer_pointer);
        }


        void set(const Index& new_batch_samples_number, Layer* new_layer_pointer)
        {
            layer_pointer = new_layer_pointer;

            batch_samples_number = new_batch_samples_number;

            const Index neurons_number = layer_pointer->get_neurons_number();
            const Index inputs_number = layer_pointer->get_inputs_number();

            deltas_dimensions.resize(2);
            deltas_dimensions.setValues({batch_samples_number, neurons_number});

            deltas_data = (type*)malloc( static_cast<size_t>(batch_samples_number*neurons_number*sizeof(type)));

            biases_derivatives.resize(neurons_number);

            synaptic_weights_derivatives.resize(inputs_number, neurons_number);

            deltas_times_activations_derivatives.resize(batch_samples_number, neurons_number);
        }

        Tensor< TensorMap< Tensor<type, 1> >*, 1> get_layer_gradient()
        {
            Tensor< TensorMap< Tensor<type, 1> >*, 1> layer_gradient(2);

            const Index inputs_number = layer_pointer->get_inputs_number();
            const Index neurons_number = layer_pointer->get_neurons_number();

            layer_gradient(0) = new TensorMap<Tensor<type, 1>>(biases_derivatives.data(), neurons_number);
            layer_gradient(1) = new TensorMap<Tensor<type, 1>>(synaptic_weights_derivatives.data(), inputs_number*neurons_number);

            return layer_gradient;
        }


        void print() const
        {
            cout << "Deltas:" << endl;
            //cout << deltas << endl;

            cout << "Biases derivatives:" << endl;
            cout << biases_derivatives << endl;

            cout << "Synaptic weights derivatives:" << endl;
            cout << synaptic_weights_derivatives << endl;
        }

        Tensor<type, 1> biases_derivatives;
        Tensor<type, 2> synaptic_weights_derivatives;

        Tensor<type, 2> deltas_times_activations_derivatives;

    };

}

#endif // MULTIHEAD_ATTENTION_LAYER_H


// OpenNN: Open Neural Networks Library.
// Copyright(C) 2005-2023 Artificial Intelligence Techniques, SL.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software

// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
