/*
 * TypeWrapper.h
 *
 *  Created on: 17.10.2015
 *      Author: domenicjenz
 */

#pragma once
#include <tuple>


template <int... Is>
struct IntSequence {};

template <int N, int... Is>
struct GenerateSequence : GenerateSequence<N - 1, N - 1, Is...> {};

template <int... Is>
struct GenerateSequence<0, Is...> : IntSequence<Is...> {};


void printSequence(IntSequence<>) {
  std::cout << std::endl;
}

template <int N, int...Is>
void printSequence(IntSequence<N, Is...>) {
  std::cout << N << " ";
  printSequence(IntSequence<Is...>{});
}

template<typename BaseType>
class TypeWrapperBase
{
public:
  virtual ~TypeWrapperBase() = default;
  virtual BaseType* createNew() = 0;
};

template<typename BaseType, typename DerivedType, typename... ArgTypes>
class TypeWrapper : public TypeWrapperBase<BaseType>
{
public:
  TypeWrapper(ArgTypes&&... args) :
    _args(std::forward<ArgTypes>(args)...) {
  }

  template <int... Is>
  BaseType* createNew(IntSequence<Is...>)
  {
    return new DerivedType(std::get<Is>(_args)...);
  }

  BaseType* createNew() {
    return createNew(GenerateSequence<sizeof...(ArgTypes)>{});
  }

private:
  std::tuple<ArgTypes...> _args;
};

template<typename BaseType, typename DerivedType, typename... ArgTypes>
TypeWrapper<BaseType, DerivedType, ArgTypes...>* makeWrapper(ArgTypes... args)
{
  return new TypeWrapper<BaseType, DerivedType, ArgTypes...>(std::forward<ArgTypes>(args)...);
}


