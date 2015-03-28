//
// Example of how the List ADT may be implemented and used in C++.
//
// Author : Eric Brisco (eric.brisco@gmail.com)
// License: The MIT License (MIT)
//

#include <functional>
#include <memory>
#include <iostream>

//
// Implementation of the List algebraic data type. This is the implementation
// in Haskell, for reference.
//
//   data List a = Cons { head :: a, tail :: List a } | Nil
//
template<typename T>
class List
{

public:

  enum Tag { Cons, Nil };

  typedef typename std::shared_ptr<List<T> > ptr;

private:

  //
  // Stores which constructor was used.
  //
  enum Tag tag;

  //
  // Stores the head of the list for Cons constructor.
  //
  T head;

  //
  // Stores the tail of the list for Cons constructor.
  //
  typename List<T>::ptr tail;

  List() { }

public:

  ~List() { }

  //
  // Destruction (elimination) for List ADT.
  //
  template<typename R>
  R cases( std::function<R(T,List<T>::ptr)> cons_case
         , std::function<R()> nil_case
         ) {
    switch (tag) {
      case Tag::Cons: return cons_case(head, tail);
      case Tag::Nil : return nil_case();
    }
  }

  //
  // Cons construction (introduction) for List ADT.
  //
  static typename List<T>::ptr cons(T head, typename List<T>::ptr tail) {
    List<T> *xxs = new List<T>();
    xxs->tag  = Tag::Cons;
    xxs->head = head;
    xxs->tail = tail;
    return typename List<T>::ptr(xxs);
  }

  //
  // Nil construction (introduction) for List ADT.
  //
  static typename List<T>::ptr nil() {
    List<T> *xxs = new List<T>();
    xxs->tag = Tag::Nil;
    return typename List<T>::ptr(xxs);
  }

};


//
// Wrapper to improve type inference.
//
template<typename T>
typename List<T>::ptr cons(T head, typename List<T>::ptr tail) {
  return List<T>::cons(head, tail);
}

//
// Wrapper to improve type inference.
//
template<typename T>
typename List<T>::ptr nil() {
  return List<T>::nil();
}

//
// Overloaded right-associative operator for more convenient list construction.
//
template<typename T>
typename List<T>::ptr operator&=(T head, typename List<T>::ptr tail) {
  return List<T>::cons(head, tail);
}

//
// Right-associative fold, a powerful higher-order function on lists.
//
template<typename R, typename T>
R foldr(std::function<R(T,R)> f, R acc, typename List<T>::ptr list) {
  return (*list).template cases<R>(
    [f,acc](T head, typename List<T>::ptr tail) {
      return f(head, foldr(f, acc, tail));
    },
    [f,acc]() { return acc; }
  );
}

//
// Sum of a list, implemented using foldr.
//
template<typename T>
T sum(typename List<T>::ptr list) {
  return foldr<T,T>([](T i, T s) { return i + s; }, (T) 0, list);
}

//
// Mapping elements of a list, using foldr.
//
template<typename U, typename T>
typename List<U>::ptr map(std::function<U(T)> f, typename List<U>::ptr ts) {
  return foldr<typename List<U>::ptr,T>(
    [f](T x, typename List<U>::ptr us) { return f(x) &= us; },
    nil<U>(),
    ts
  );
}

//
// Filtering elements of a list, using foldr.
//
template<typename T>
typename List<T>::ptr filter(std::function<bool(T)> f, typename List<T>::ptr list) {
  return foldr<typename List<T>::ptr,T>(
    [f](T head, typename List<T>::ptr tail) {
      if (f(head)) return head &= filter<T>(f, tail);
      else return filter<T>(f, tail);
    },
    nil<T>(),
    list
  );
}

//
// This is so we can write std::cout << *myList
//
template<typename T>
std::ostream& operator<<(std::ostream& out, List<T> &list) {
    list.template cases<void>(
      [&out](T head, typename List<T>::ptr tail) {
        out << head << " &= " << *tail;
      },
      [&out]() {
        out << "nil";
      }
    );
    return out;
}

//
// Run the examples.
//
int main(int argc, char *argv[]) {

  // Construct the list [1,2,3]
  auto as = cons(1, cons(2, cons(3, nil<int>())));

  // Consturct the list [1,2,3] using a right-associative infix operator.
  auto bs = 1 &= 2 &= 3 &= nil<int>();

  // Elements doubled from the list bs.
  auto cs = map<int,int>([](int x) { return x * 2; }, bs);

  // Odd elements removed from the list bs.
  auto ds = filter<int>([](int x) { return x % 2 == 0; }, bs);

  // Print out lists as, bs, cs, ds, and their sums.
  std::cout << "sum(" << *as << ") = " << sum<int>(as) << std::endl;
  std::cout << "sum(" << *as << ") = " << sum<int>(bs) << std::endl;
  std::cout << "sum(" << *cs << ") = " << sum<int>(cs) << std::endl;
  std::cout << "sum(" << *ds << ") = " << sum<int>(ds) << std::endl;

  return 0;

}
