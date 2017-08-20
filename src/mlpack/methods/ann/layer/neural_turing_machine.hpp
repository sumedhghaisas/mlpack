/**
 * @file neural_turing_machine.hpp
 * @author Sumedh Ghaisas
 *
 * Definition of the Neural Turing Machine class.
 *
 * //TODO add reference
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LAYER_NTM_HPP
#define MLPACK_METHODS_ANN_LAYER_NTM_HPP

#include <mlpack/prereqs.hpp>

#include <boost/ptr_container/ptr_vector.hpp>

#include "../visitor/delta_visitor.hpp"
#include "../visitor/output_parameter_visitor.hpp"
#include "../visitor/forward_with_memory_visitor.hpp"
#include "../activation_functions/softplus_function.hpp"

#include "layer_types.hpp"
#include "add_merge.hpp"
#include "sequential.hpp"

#include "memory_head.hpp"
#include "write_memory.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

/**
 * An implementation of Neural Turing Machine Layer.
 *
 * This class allows specification of the type of the activation functions used
 * for the gates and cells and also of the type of the function used to
 * initialize and update the peephole weights.
 *
 * @tparam InputDataType Type of the input data (arma::colvec, arma::mat,
 *         arma::sp_mat or arma::cube).
 * @tparam OutputDataType Type of the output data (arma::colvec, arma::mat,
 *         arma::sp_mat or arma::cube).
 */
template <
    typename InputDataType = arma::mat,
    typename OutputDataType = arma::mat
>
class NeuralTuringMachine
{
 public:
  /**
   * Create the Neural Turing Machine layer object using the specified
   * parameters.
   *
   * @param inSize The number of input units.
   * @param outSize The number of output units.
   * @param numMem Number of memory locations to use.
   * @param memSize Size of each memory location.
   * @param shiftSize Circular shift rotation size.
   * @param controller The controller network to use.
   */
  NeuralTuringMachine(const size_t inSize,
                      const size_t outSize,
                      const size_t numMem,
                      const size_t memSize,
                      const size_t shiftSize,
                      LayerTypes controller);

  /**
   * Ordinary feed forward pass of a neural network, evaluating the function
   * f(x) by propagating the activity forward through f.
   *
   * @param input Input data used for evaluating the specified function.
   * @param output Resulting output activation.
   */
  template<typename eT>
  void Forward(arma::Mat<eT>&& input, arma::Mat<eT>&& output);

  /**
   * Ordinary feed backward pass of a neural network, calculating the function
   * f(x) by propagating x backwards trough f. Using the results from the feed
   * forward pass.
   *
   * @param input The propagated input activation.
   * @param gy The backpropagated error.
   * @param g The calculated gradient.
   */
  template<typename eT>
  void Backward(const arma::Mat<eT>&& /* input */,
                arma::Mat<eT>&& gy,
                arma::Mat<eT>&& g);

  /*
   * Calculate the gradient using the output delta and the input activation.
   *
   * @param input The input parameter used for calculating the gradient.
   * @param error The calculated error.
   * @param gradient The calculated gradient.
   */
  template<typename eT>
  void Gradient(arma::Mat<eT>&& input,
                arma::Mat<eT>&& /* error */,
                arma::Mat<eT>&& /* gradient */);

  /*
   * Resets the cell to accept a new input.
   * This breaks the BPTT chain starts a new one.
   */
  void ResetCell();

  //! The value of the deterministic parameter.
  bool Deterministic() const { return deterministic; }
  //! Modify the value of the deterministic parameter.
  bool& Deterministic() { return deterministic; }

  //! Get the parameters.
  OutputDataType const& Parameters() const { return weights; }
  //! Modify the parameters.
  OutputDataType& Parameters() { return weights; }

  //! Get the input parameter.
  InputDataType const& InputParameter() const { return inputParameter; }
  //! Modify the input parameter.
  InputDataType& InputParameter() { return inputParameter; }

  //! Get the output parameter.
  OutputDataType const& OutputParameter() const { return outputParameter; }
  //! Modify the output parameter.
  OutputDataType& OutputParameter() { return outputParameter; }

  //! Get the delta.
  OutputDataType const& Delta() const { return delta; }
  //! Modify the delta.
  OutputDataType& Delta() { return delta; }

  //! Get the gradient.
  OutputDataType const& Gradient() const { return gradient; }
  //! Modify the gradient.
  OutputDataType& Gradient() { return gradient; }

  //! Get the model modules.
  std::vector<LayerTypes>& Model() { return network; }

  /**
   * Serialize the layer
   */
  template<typename Archive>
  void Serialize(Archive& ar, const unsigned int /* version */);

 private:
  //! Locally-stored number of input units.
  size_t inSize;

  //! Locally-stored number of output units.
  size_t outSize;

  //! Number of memory locations.
  size_t numMem;

  //! Size of memory locations.
  size_t memSize;

  //! Circular shift rotation size.
  size_t shiftSize;

  //! Memory read head.
  LayerTypes readHead;

  //! Locally stored read head error.
  arma::mat dReadHead;

  //! Memory write layer.
  LayerTypes writeMem;

  //! Locally stored memory write error.
  arma::mat dWriteHead;

  //! Controller.
  LayerTypes controller;

  //! Locally stored memory content error.
  arma::mat dMem;

  //! Locally stored previous memory content error.
  arma::mat dMemPrev;

  //! All zeros vector for initializing read.
  arma::mat allZeros;

  //! All ones matrix for initializing memory.
  arma::mat allOnes;

  //! Storing all the memory contents for backward pass.
  std::list<arma::mat> memoryHistory;

  //! Backward pass iterator to stored memory content.
  std::list<arma::mat>::iterator bMemoryHistory;

  //! Storing all the memory reads for backward pass.
  std::list<arma::mat> lReads;

  //! Backward pass iterator to stored memory reads.
  std::list<arma::mat>::iterator gReads;

  //! Locally-stored weight object.
  OutputDataType weights;

  //! Locally-stored output parameter visitor.
  OutputParameterVisitor outputParameterVisitor;

  //! Locally-stored delta visitor.
  DeltaVisitor deltaVisitor;

  //! Locally-stored list of network modules.
  std::vector<LayerTypes> network;

  //! Locally-stored number of forward steps.
  size_t forwardStep;

  //! Locally-stored number of backward steps.
  size_t backwardStep;

  //! Locally-stored number of gradient steps.
  size_t gradientStep;

  //! Locally-stored previous error.
  arma::mat prevError;

  //! If true dropout and scaling is disabled, see notes above.
  bool deterministic;

  //! Locally-stored delta object.
  OutputDataType delta;

  //! Locally-stored gradient object.
  OutputDataType gradient;

  //! Locally-stored input parameter object.
  InputDataType inputParameter;

  //! Locally-stored output parameter object.
  OutputDataType outputParameter;
}; // class NeuralTuringMachine

} // namespace ann
} // namespace mlpack

// Include implementation.
#include "neural_turing_machine_impl.hpp"

#endif
