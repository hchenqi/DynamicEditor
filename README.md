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

> T

A function interprets data in a certain type. Which type of data it will interpret is assigned beforehand, but the actual content of the data is not known until it reads the data at runtime.

> F(T)

At runtime a function is being run with its input data, which then outputs a new function and new data for it to interpret. This process continues.

> T, F(T) -> T', F'(T') -> ...

For a fixed function, data is variable. For the runtime, functions and data are variable.

A function is of fixed size and contains enumerable content. Runtime can be of arbitrary size, but at a fixed time point, it also contains enumerable content.

Examples:

- Data with union type T of sub-types <T1, T2, ..., Tn> is stored as { tag, payload } with tag being an integer index of the payload type (1, 2, ..., n) and payload being data in that type. Its interpreter function F(T) is a mapping between integer indices and sub-interpreters accepting the corresponding sub-types { 1: F1(T1), 2: F2(T2), ..., n: Fn(Tn) }. The runtime takes data { tag, payload } with type T and function F(T), reads the data tag i, and produces { payload } with type Ti and function Fi.

> T <T1, T2, ..., Tn> { tag, payload }, F { 1: F1(T1), 2: F2(T2), ..., n: Fn(Tn) } -> Ti { payload }, Fi(Ti)

- Interpreter function F for data with empty type T = {} gives output of data with a certain type T' and function F'. We can say that F contains T' and F'(T').

> {}, F({}) -> T', F'(T')

- Data with union type T of sub-types <T1, T2, ..., Tn>, where each Ti is an empty type, has empty payload and is stored just as { tag } with tag being the integer index. Its interpreter function F(T) maps the tag to the empty type and the corresponding sub-function.

> T <T1 {}, T2 {}, ..., Tn {}> { tag }, F { 1: F1({}), 2: F2({}), ..., n: Fn({}) } -> {}, Fi({})



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
