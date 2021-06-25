#pragma once

#include "type_traits"
#include "vector"

template<size_t Size, class Head, class... Tail>
struct Bamboo;

namespace myTypeTraits {

    template<class Head, class... Tail> const static size_t counter_v = counter_v < Tail
    ...> + 1;
    template<class Head> const static size_t counter_v<Head> = 1;

    template<size_t N, class Head, class... Tail>
    struct Cut {
        using Val = typename Cut<N - 1, Tail...>::Val;
        using Type = typename Cut<N - 1, Tail...>::Type;
    };

    template<class Head, class... Tail>
    struct Cut<0, Head, Tail...> {
        using Val = Bamboo<counter_v<Head, Tail...>, Head, Tail...>;
        using Type = Head;
    };

    template<class T, class Head, class... Tail>
    struct FindType {
        using Bamb = typename FindType<T, Tail...>::Bamb;
    };

    template<class T, class... Tail>
    struct FindType<T, T, Tail...> {
        using Bamb = Bamboo<counter_v<T, Tail...>, T, Tail...>;
    };

    template<int Num, class T, class Head, class... Tail>
    struct CheckLessT {
        using Yes = typename CheckLessT<Num, T, Tail...>::Yes;
    };

    template<class T, class Head, class... Tail>
    struct CheckLessT<-1, T, Head, Tail...> {
    };

    template<int Num, class T, class... Tail>
    struct CheckLessT<Num, T, T, Tail...> {
        using Yes = typename CheckLessT<Num - 1, T, Tail...>::Yes;
    };

    template<int Num, class T, class Head>
    struct CheckLessT<Num, T, Head> {
        using Yes = int;
    };

    template<int Num, class T>
    struct CheckLessT<Num, T, T> {
        using Yes = int;
    };

    template<class T>
    struct CheckLessT<0, T, T> {
    };

    template<class TupleHead, class ...Tuples>
    struct Linker;

    template<class TupleHead, class ...Tuples>
    struct Linker {
        using Type = typename std::remove_reference_t<TupleHead>::template Glue<typename std::remove_reference_t<typename Linker<Tuples...>::Type>>::Concat;
    };

    template<class TupleHead>
    struct Linker<TupleHead> {
        using Type = TupleHead;
    };
}
using namespace myTypeTraits;



template<class MarkerClass>
struct Marker {
};

template<class Head, class... Tail>
class TupleManager : public TupleManager<Tail...> {
    Head data;

public:
    template<typename HeadT, typename ...TailT>
    TupleManager(HeadT &&head, TailT &&... tail) : TupleManager<Tail...>(std::forward<TailT>(tail)...),
                                                   data(std::forward<HeadT>(head)) {};

    template<size_t N>
    auto &get() {
        if constexpr (N >= std::remove_reference_t<Head>::size) {
            return static_cast<TupleManager<Tail...> &>(*this).template get<N - std::remove_reference_t<Head>::size>();
        } else {
            return data.template get<N>();
        }
    };

    template<class... Types>
    auto &get() {
        return get<size - counter_v<Types...>>();
    };
    const static size_t size = std::remove_reference_t<Head>::size + TupleManager<Tail...>::size;
};


template<class Head>
class TupleManager<Head> {
    Head data;
public:
    template<typename HeadT>
    TupleManager(HeadT &&head) : data(std::forward<HeadT>(head)) {}

    template<size_t N>
    auto &get() {
        return data.template get<N>();
    }

    template<class... Types>
    auto &get() {
        return get<size - counter_v<Types...>>();
    };

    const static size_t size = std::remove_reference_t<Head>::size;
};


template<size_t Size, class Head, class... Tail>
struct Bamboo : public Bamboo<Size - 1, Tail...> {
    Head data;

    Bamboo() = default;

    Bamboo(const Bamboo<Size, Head, Tail...> &other) = default;

    Bamboo(Bamboo<Size, Head, Tail...> &other) = default;

    Bamboo(Bamboo<Size, Head, Tail...> &&other) = default;


    template<class InitH, class ...InitT>
    Bamboo(InitH &&head, InitT &&...tail): Bamboo<Size - 1, Tail...>(std::forward<InitT>(tail)...),
                                           data(std::forward<InitH>(head)) {}

    template<class marker1, class marker2, class... InitT>
    Bamboo(Marker<marker1>, Marker<marker2>, TupleManager<InitT...> &&manager): Bamboo<Size - 1, Tail...>(
            Marker<marker1>(), Marker<marker2>(), std::forward<TupleManager<InitT...>>(manager)),
                                                                                data(std::move(
                                                                                        manager.template get<Head, Tail...>())) {}

    template<class InitH, class... InitT>
    Bamboo &operator=(const Bamboo<Size, InitH, InitT...> &other) {
        static_cast<Bamboo<Size - 1, Tail...> &>(*this) = static_cast<const Bamboo<Size - 1, InitT...> &>(other);
        data = other.data;
        return *this;
    }
    template<class InitH, class... InitT>
    Bamboo &operator=(Bamboo<Size, InitH, InitT...> &&other) {
        static_cast<Bamboo<Size - 1, Tail...> &>(*this) = std::move(static_cast<Bamboo<Size - 1, InitT...> &>(other));
        data = std::move(other.data);
        return *this;
    }

    void swap(Bamboo &other) {
        std::swap(*this, other);
        static_cast<Bamboo<Size - 1, Tail...> &>(*this).swap(static_cast<Bamboo<Size - 1, Tail...> &>(other));
    }

    template<class HeadT, class... TailT>
    bool operator<(const Bamboo<Size, HeadT, TailT...> &other) const {
        if (data < other.data) {
            return true;
        }
        if (other.data < data) {
            return false;
        }

        return static_cast<const Bamboo<Size - 1, Tail...> &>(*this) <
               static_cast<const Bamboo<Size - 1, TailT...> &>(other);

    }

    template<class HeadT, class... TailT>
    bool operator==(const Bamboo<Size, HeadT, TailT...> &other) const {
        return data == other.data && static_cast<const Bamboo<Size - 1, Tail...> &>(*this) ==
                                     static_cast<const Bamboo<Size - 1, TailT...> &>(other);
    }
};



template<class Head>
struct Bamboo<1, Head> {
    Head data;

    Bamboo() = default;

    Bamboo(const Bamboo<1, Head> &other) = default;

    Bamboo(Bamboo<1, Head> &other) = default;

    Bamboo(Bamboo<1, Head> &&other) = default;

    template<class Init>
    Bamboo(Init &&init): data(std::forward<Init>(init)) {}


    template<class InitH>
    Bamboo &operator=(const Bamboo<1, InitH> &other) {
        data = other.data;
        return *this;
    }

    template<class InitH>
    Bamboo &operator=(Bamboo<1, InitH> &&other) {
        data = std::move(other.data);
        return *this;
    }

    void swap(Bamboo &other) {
        std::swap(*this, other);
    }

    template<class marker1, class marker2, class... Tuples>
    Bamboo(Marker<marker1>, Marker<marker2>, TupleManager<Tuples...> &&manager): data(
            std::move(manager.template get<Head>())) {}

    template<class HeadT>
    bool operator<(const Bamboo<1, HeadT> &other) const {
        if (data < other.data) {
            return true;
        }
        return false;
    }

    template<class HeadT>
    bool operator==(const Bamboo<1, HeadT> &other) const {
        return data == other.data;
    }
};


template<class... Types>
class Tuple : public Bamboo<counter_v<Types...>, Types...> {
    typedef Bamboo<counter_v<Types...>, Types...> Parent;


    template<class... GTypes>
    struct Glue {
    };
    template<class... GTypes>
    struct Glue<Tuple<GTypes...>> {
        using Concat = Tuple<Types..., GTypes...>;
    };

    template<class TupleHead, class ...Tuples>
    friend
    struct Linker;

public:

    Tuple() = default;

    //Tuple(Types &... types) : Bamboo<counter_v<Types...>, Types...>(types...) {}

    Tuple(Tuple<Types...> &other) = default;
    Tuple(const Tuple<Types...> &other) = default;

    Tuple(Tuple<Types...> &&other) = default;

    template<class... InitT>
    Tuple &operator=(const Tuple<InitT...> &other) {
        static_cast<Parent &>(*this) = static_cast<const Bamboo<counter_v<InitT...>, InitT...> &>(other);
        return *this;
    }

    template<class... InitT>
    Tuple &operator=(Tuple<InitT...> &&other) {
        static_cast<Parent &>(*this) = std::move(static_cast<Bamboo<counter_v<InitT...>, InitT...> &>(other));
        return *this;
    }


    template<class... InitT>
    Tuple(InitT &&...init): Bamboo<counter_v<Types...>, Types...>(std::forward<InitT>(init)...) {}



    template<class marker1, class marker2, class... Tuples>
    Tuple(Marker<marker1>, Marker<marker2>, TupleManager<Tuples...> &&manager) : Bamboo<counter_v<Types...>, Types...>(
            Marker<marker1>(), Marker<marker2>(), std::move(manager)) {}


    template<size_t N>
    typename Cut<N, Types...>::Type &get() &{
        return static_cast<typename Cut<N, Types...>::Val &>(*this).data;
    }

    template<size_t N>
    typename Cut<N, Types...>::Type get() &&{
        return std::move(static_cast<typename Cut<N, Types...>::Val &>(*this).data);
    }

    template<size_t N>
    [[nodiscard]] const typename Cut<N, Types...>::Type &get() const {
        return static_cast<const typename Cut<N, Types...>::Val &>(*this).data;
    }

    template<class T>
    T &get() {
        static_assert(std::is_same_v<typename CheckLessT<1, T, Types...>::Yes, int>);
        return static_cast<typename FindType<T, Types...>::Bamb &>(*this).data;
    }

    void swap(Tuple &other) {
        static_cast<Parent &>(*this).swap(static_cast<Parent &>(other));
    }

    template<class ...TypesT>
    bool operator<(const Tuple<TypesT...> &other) const {
        return static_cast< const Parent &>(*this) <
               static_cast<const Bamboo<counter_v<TypesT...>, TypesT...> &>(other);
    }

    template<class ...TypesT>
    bool operator>(const Tuple<TypesT...> &other) const {
        return static_cast<const Bamboo<counter_v<TypesT...>, TypesT...> &>(other) <
               static_cast< const Parent &>(*this);
    }

    template<class ...TypesT>
    bool operator<=(const Tuple<TypesT...> &other) const {
        return !(*this > other);
    }

    template<class ...TypesT>
    bool operator>=(const Tuple<TypesT...> &other) const {
        return !(*this < other);
    }



    template<class ...TypesT>
    bool operator!=(const Tuple<TypesT...> &other) const {
        return !(*this == other);
    }

    template<class ...TypesT>
    bool operator==(const Tuple<TypesT...> &other) const {
        return static_cast<const Bamboo<counter_v<TypesT...>, TypesT...> &>(other) ==
               static_cast< const Parent &>(*this);
    }

    template<class T>
    const T &get() const {
        static_assert(std::is_same_v<typename CheckLessT<1, T, Types...>::Yes, int>);
        return static_cast<const typename FindType<T, Types...>::Bamb &>(*this).data;
    }

    const static size_t size = counter_v<Types...>;

    template<size_t N>
    using element_type_t = typename Cut<N, Types...>::Type;
};


template<class ...Tuples>
auto tupleCat(Tuples &&... tuples) {
    return typename Linker<Tuples...>::Type(Marker<int>(), Marker<char>(),
                                            TupleManager<std::remove_reference_t<Tuples>...>(
                                                    std::forward<Tuples>(tuples)...));
}


template<size_t place, class ...Types>
decltype(auto) get(Tuple<Types...> &tuple) {
    return tuple.template get<place>();
}

template<size_t place, class ...Types>
decltype(auto) get(const Tuple<Types...> &tuple) {
    return tuple.template get<place>();
}

template<size_t place, class ...Types>
auto get(Tuple<Types...> &&tuple) {
    return std::move(tuple.template get<place>());
}

template<typename T, class ...Types>
decltype(auto) get(Tuple<Types...> &tuple) {
    return tuple.template get<T>();
}

template<typename T, class ...Types>
decltype(auto) get(const Tuple<Types...> &tuple) {
    return tuple.template get<T>();
}

template<typename T, class ...Types>
auto get(Tuple<Types...> &&tuple) {
    return std::move(tuple.template get<T>());
}

template<typename... T>
struct error {
    };

template<>
struct error<int> {

    const static int yes = 1;
};


template<typename ...Types>
auto makeTuple(Types&&... vals) {
    //error<Types...>::yes;
    return Tuple<std::decay_t<Types>...>(std::forward<Types>(vals)...);
}