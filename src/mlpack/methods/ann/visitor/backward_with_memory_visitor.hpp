/**
 * @file backward_with_memory_visitor.hpp
 * @author Sumedh Ghaisas
 *
 * This file provides an abstraction for the BackwardWithMemory() function.
 * The function is used in layers which requires 'memory' as input.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_VISITOR_BACKWARD_WITH_MEMORY_VISITOR_HPP
#define MLPACK_METHODS_ANN_VISITOR_BACKWARD_WITH_MEMORY_VISITOR_HPP

#include <mlpack/methods/ann/layer/layer_traits.hpp>
#include <mlpack/methods/ann/layer/layer_types.hpp>

#include <boost/variant.hpp>

namespace mlpack {
namespace ann {

/**
 * BackwardWithMemoryVisitor executes the BackwardWithMemory() function given
 * the input, memory and output parameter (if the function exists).
 */
class BackwardWithMemoryVisitor : public boost::static_visitor<void>
{
 public:
  /**
   * Execute the BackwardWithMemory() function given the input, current memory
   * and output parameter.
   */
  BackwardWithMemoryVisitor(arma::mat&& output,
                            const arma::mat&& memory,
                            arma::mat&& error,
                            arma::mat&& delta,
                            arma::mat&& dM);

  //! Execute the BackwardWithMemory() function.
  template<typename LayerType>
  void operator()(LayerType* layer) const;

 private:
  //! Execute the BackwardWithMemory() function with memory for a module which
  //! implements BackwardWithMemory() function
  template<typename T>
  typename std::enable_if<
      HasBackwardWithMemoryCheck<T, void(T::*)(const arma::mat&&,
      const arma::mat&&, arma::mat&&, arma::mat&&,
      arma::mat&&)>::value, void>::type
  BackwardWithMemory(T* layer) const;

  //! Do not execute the BackwardWithMemory() function for a module which
  //! doesn't implement ForwardWithMemory() function.
  template<typename T>
  typename std::enable_if<
      !HasBackwardWithMemoryCheck<T, void(T::*)(const arma::mat&&,
      const arma::mat&&, arma::mat&&, arma::mat&&,
      arma::mat&&)>::value, void>::type
  BackwardWithMemory(T* layer) const;

  //! The output parameter set.
  arma::mat&& output;

  //! Content of memory.
  const arma::mat&& memory;

  //! The error parameter.
  arma::mat&& error;

  //! The delta parameter.
  arma::mat&& delta;

  //! The delta of memory
  arma::mat& dM;
};

} // namespace ann
} // namespace mlpack

// Include implementation.
#include "backward_with_memory_visitor_impl.hpp"

#endif
