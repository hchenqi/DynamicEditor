# DynamicEditor

An editor supporting dynamic types and data structures.

## Build Instructions

This project can be built with CMake on Windows. It depends on two other major projects of mine, `WndDesign` and `BlockStore`.

## Concepts

### Item / Block

Item is a logical unit of data in a hierarchy. An item can contain sub-items, and can be contained in a parent item.

Block is the storage unit. Each block has an upper size limit, and exposes an reference. Items are stored in blocks, and block boundary is naturally item boundary. Items larger than blocks must be stored in multiple blocks as sub-items linked by their references.

### Data Type / View Type

Data type describes the layout of the item data. View type describes how data in a certain data type is visually presented. Multiple view types can share the same data type.

> For example, boolean typed item can be displayed as `true` `false` strings, a checkbox, or a select with two options; integer typed item can be interpreted as a decimal string, hexadecimal string, background color, or the position of a slider, etc.

> Sometimes the view that allows user to edit item data also need to ensure the data is valid. For example, an integer type with possible values within 0 - 99 shouldn't take other values, though the storage bits can actually encode more values.

### Static Type / Dynamic Type

Item with static type doesn't store type information as the interpreter already knows its type and directly reads its data. Item with dynamic type stores type information along with its data for the interpreter to know how to interpret the data.

> If the item need to be static or dynamic is relative to the program that interprets it. For example, if a program that reads a string knows how long the string should be, then it's not necessary to also store the length of the string. On the contrary, if the program handles strings with dynamic lengths, the length need to be stored along with the string. And in an extreme case, when the program already knows what the string exactly is, then the string doesn't need to be stored in item data at all.

> One could also say, that every item has static type, because every item has to be interpreted by a certain interpreter and not by the others. Items with dynamic type are just to be interpreted by the dynamic type interpreter, regardless of how the interpreter internally work.

If the types an item can possibly have can be enumerated, then the type information for the item can be stored as an integer index indicating which type the item has, and the item actually has union type. The interpreter for a union type contains interpreters for each type of the union. It reads the index and calls the corresponding interpreter for the type.

If the types an item can possibly have can't be enumerated, then the item has true dynamic type. The interpreter must first construct a specific interpreter for this type at runtime based on the type information of the item which then interprets the item data.

> We can consider a string with arbitrary length has true dynamic type because the length of a string can't be simply enumerated if we ignore a size limit. Similarly, we can think that an integer also has true dynamic type because we can't enumerate all integers.

### Data / Function / Runtime

Data has a certain type.

> t: T

A function interprets data in a certain type. Which type of data it will interpret is assigned beforehand, but the actual content of the data is not known until it reads the data at runtime.

> F(T)

The runtime calls a function with its input data and then outputs a new function and new data for the output function to interpret.

> t: T, F(T) -> t': T', F'(T')

This process continues.

> t: T, F(T) -> t': T', F'(T') -> t": T", F"(T") -> ...

For a fixed function, data is variable. For the runtime, functions and data are variable.

A function is of fixed size and contains enumerable content. The runtime can be of arbitrary size, but at a fixed time point, it is also of fixed size and contains enumerable content.

Examples:

- Data with union type T of sub-types <T1, T2, ..., Tn> is stored as a tuple { tag, payload } with tag being an integer index of the payload type (1, 2, ..., n) and payload being data in that type. Its interpreter function F(T) is a mapping between integer indices and sub-interpreters accepting the corresponding sub-types { 1 ~ F1(T1), 2 ~ F2(T2), ..., n ~ Fn(Tn) }. The runtime takes data { tag, payload } with type T and function F(T), reads the data tag i, and produces { payload } with type Ti and function Fi.

  > { tag, payload }: T <T1, T2, ..., Tn>, F { 1 ~ F1(T1), 2 ~ F2(T2), ..., n ~ Fn(Tn) } -> payload: Ti, Fi(Ti)

- Interpreter function F for data with empty type T = {} gives output of constant data with a certain type T' and function F'. We can also say that F is a constant function constructed by combining T' and F'(T').

  > {}, F({}) -> t': T', F'(T')

- Data with union type T of sub-types <T1, T2, ..., Tn>, where each sub-type Ti is an empty type, has empty payload and is stored just as { tag } with tag being the integer index. Its interpreter function F(T) maps the tag to the empty type and the corresponding sub-interpreter function.

  > { tag }: T <T1 {}, T2 {}, ..., Tn {}>, F { 1 ~ F1({}), 2 ~ F2({}), ..., n ~ Fn({}) } -> {}, Fi({})

- Exist functions that take a certain type of data as input but output data with empty type and a constant function.

  > t: T, F(T) -> {}, F'({})

- Specially, exists a constant function that takes empty data as input and outputs empty data and the same constant function. This could be seen as the ending state of the runtime.

  > {}, F({}) -> {}, F({})

- Data with tuple type T of sub-types T1, T2, ..., Tn is stored as { T1, T2, ..., Tn }. The interpreter function F(T) can be constructed from the functions Fi(Ti) each interpreting the corresponding part of data with type Ti.

  > { t1: T1, t2: T2, ..., tn: Tn }: T, F(T) { F1(T1), F2(T2), ..., Fn(Tn) } -> { t1': T1', t2': T2', ..., tn': Tn' }: T', F'(T') { F1'(T1'), F2'(T2'), ..., Fn'(Tn') }

- Data with dynamic length array type T[] is stored as { n, T, T, ..., T } (with n times T). The interpreter function stores only one copy of F0(T) interpreting T. The runtime reads the length n and outputs tuple T[n] = { T, T, ..., T } (n times T) and the interpreter function for the tuple. This output function only exists at runtime, because it is of arbitrary size depending on the data.

  > { n, t1: T, t2: T, ..., tn: T }: T[], F(T[]) { F0(T) } -> { t1: T, t2: T, ..., tn: T }: T[n], F'(T[n]) { F0(T), F0(T), ..., F0(T) }

- With a fixed length n = k, the output function above can be stored for interpreting data with constant length array type T[k], which is the tuple { T, T, ..., T } (k times T).

  > { t1: T, t2: T, ..., tk: T }: T[k], F(T[k]) -> { t1': T', t2': T', ..., tk': T' }: T'[k], F'(T'[k])

### Function Construction

Function interpreting data at runtime is when data guides the transformation of the function. Information flows from data to function.

Function can be constructed by following principles:

- Given functions F1(T1), F2(T2), ..., Fn(Tn) in order, a union dispatcher function F(T) can be constructed with T being the tagged union type of T1, T2, ... Tn.

- Given a union dispatcher function F(T) with T being a tagged union type of T1, T2, ... Tn and an integer index i among 1, 2, ..., n, the interpreter function for Ti can be constructed by extracting the ith interpreter function from F(T).

- Given functions F1(T1), F2(T2), ..., Fn(Tn) in order, a tuple interpreter function F(T) can be constructed with T being the tuple of T1, T2, ..., Tn.

Data stores the construction guide for a function at runtime. In this sense, data seems to be the function and function be the data.

### Type Template



## Components

### ItemView

`ItemView` displays an item in a specific view type.

### BlockView

`BlockView` displays block boundaries. It can contain a tree of `ItemView`, and can itself be contained in an `ItemView` as inline expansion of a `Ref` typed item.

### TypeSelect

`TypeSelect` allows user to change the data type or view type of an item.

`ItemView` is reconstructed on the same item data when view type changes.

If the data type of an item is changed, type conversion on the item data is performed.

`TypeRegistry` stores all data types and their possible view types for user to select.
