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

### Item / Interpreter Function / Runtime

Item `t` has a certain type T.

> t: T

An interpreter function F(T) interprets item with type T. The item type is fixed for an interpreter function, but the actual data of the item is not known until it is provided at runtime.

> F(T)

The runtime calls an interpreter function with input item and then outputs a new interpreter function and a new item for the output interpreter function to interpret.

> t: T, F(T) -> t': T', F'(T')

For a fixed interpreter function, item is variable. For the runtime, interpreter functions and items are variable.

An interpreter function is of fixed size and contains enumerable content. The runtime can be of arbitrary size, but at a fixed time point, it is also of fixed size and contains enumerable content.

Examples:

- An item with union type T of sub-types <T1, T2, ..., Tn> is stored as a tuple `{ tag, payload }` with tag being an integer index of the payload type in (1, 2, ..., n) and payload being sub-item in that type. Its interpreter function F(T) is a mapping between integer indices and sub- interpreter functions accepting the corresponding sub-types { 1 ~ F1(T1), 2 ~ F2(T2), ..., n ~ Fn(Tn) }. The runtime takes item `{ tag, payload }` with type T and interpreter function F(T), reads the tag i, and produces item `payload` with type Ti and interpreter function Fi.

  > { tag, payload }: T <T1, T2, ..., Tn>, F(T) { 1 ~ F1(T1), 2 ~ F2(T2), ..., n ~ Fn(Tn) } -> payload: Ti, Fi(Ti)

- Interpreter function F for items with empty type `{}` gives constant output of item with a certain type T' and interpreter function F'. We can also say that F is a constant function constructed by combining an item t' with type T' and F'(T').

  > {}, F({}) -> t': T', F'(T')

- An item with union type T of sub-types <T1, T2, ..., Tn>, where each sub-type Ti is an empty type, has empty payload and is stored just as `tag` with tag being the integer index. Its interpreter function F(T) maps the indices to the corresponding sub- interpreter functions which are constant functions.

  > tag: T <T1 {}, T2 {}, ..., Tn {}>, F(T) { 1 ~ F1({}), 2 ~ F2({}), ..., n ~ Fn({}) } -> {}, Fi({})

- Exist functions for a certain input type that output item with empty type and a constant function.

  > t: T, F(T) -> {}, F'({})

- An item with tuple type T of sub-types <T1, T2, ..., Tn> is stored as a tuple `{ t1: T1, t2: T2, ..., tn: Tn }`. The interpreter function F(T) can be constructed from the interpreter functions Fi(Ti) each interpreting the corresponding sub-item with type Ti.

  > { t1: T1, t2: T2, ..., tn: Tn }: T, F(T) { F1(T1), F2(T2), ..., Fn(Tn) } -> { t1': T1', t2': T2', ..., tn': Tn' }: T', F'(T') { F1'(T1'), F2'(T2'), ..., Fn'(Tn') }

- An item with dynamic length array type T[] is stored as a tuple `{ l, T, T, ..., T }` (with l times T). The interpreter function stores only one copy of F0(T) that interprets items with type T. The runtime reads the length l and outputs an item with tuple type T[l] = { T, T, ..., T } (l times T) and the interpreter function for this tuple. This output interpreter function only exists at runtime, because it is of arbitrary size depending on the input item.

  > { l, t1: T, t2: T, ..., tn: T }: T[], F(T[]) { F0(T) } -> { t1: T, t2: T, ..., tn: T }: T[l], F'(T[l]) { F0(T), F0(T), ..., F0(T) }

- With a fixed length l = k, the output interpreter function above can be stored for interpreting items with constant length array type T[k] as tuple { T, T, ..., T } (k times T).

  > { t1: T, t2: T, ..., tk: T }: T[k], F(T[k]) -> { t1': T', t2': T', ..., tk': T' }: T'[k], F'(T'[k])

### Construction of Interpreter Function

Interpreter functions interpreting items at runtime is when items guide the construction of the interpreter functions.

Interpreter functions can be constructed by following principles:

- Given interpreter functions F1(T1), F2(T2), ..., Fn(Tn) in order, a union interpreter function F(T) can be constructed with T being the union type of sub-types <T1, T2, ..., Tn>.

- Given a union interpreter function F(T) with T being the union type of sub-types <T1, T2, ..., Tn> and an integer index i in (1, 2, ..., n), the interpreter function for Ti can be constructed by copying the ith sub- interpreter function Fi(Ti) from F(T).

- Given interpreter functions F1(T1), F2(T2), ..., Fn(Tn) in order, a tuple interpreter function F(T) can be constructed with T being the tuple type of sub-types <T1, T2, ..., Tn>.

### Constructor Function and Type Registry

An item with dynamic type stores the construction guide for the interpreter function of its data. The interpreter function for the item can also be called a constructor function.

A union interpreter function is a constructor function with the integer index stored in the item being the construction guide.

There are also constructor functions for constructing union and tuple interpreter functions at runtime.

Basic interpreter functions F1(T1), F2(T2), ..., Fn(Tn) where T1, T2, ..., Tn are basic types can be kept in a type registry, each with a unique reference. Other interpreter functions can be constructed with them by certain constructor functions.

For example:

- An item with arbitrary tuple type stores a list of references to interpreter functions in the type registry { rF1, rF2, ..., rFk } along with the payload data. The interpreter function for this item, the tuple constructor function C, reads the list of references and constructs a tuple interpreter function by looking up the corresponding interpreter functions in the type registry R = { rF1 ~ F1(T1), rF2 ~ F2(T2), ..., rFn ~ Fn(Tn) }.

  > { { k, rF1, rF2, ..., rFk }, payload }: T, C(T) { R } -> payload: T' { T1, T2, ..., Tk }, F(T') { F1(T1), F2(T2), ..., Fk(Tk) }

- Similarly, an item with arbitrary union type stores a list of references to interpreter functions in the type registry along with the payload data. The union constructor function C reads the list of references and constructs a union interpreter function.

  > { { k, rF1, rF2, ..., rFk }, payload }: T, C(T) { R } -> payload { tag, payload' }: T', F(T') { 1 ~ F1(T1), 2 ~ F2(T2), ..., k ~ Fk(Tk) }

- An item with dynamic length arbitrary array type stores the reference to the interpreter function for a single sub-item in the type registry and the length of the array along with the payload data. The array constructor function reads the reference and the length and constructs a constant length array interpreter function.

  > { { rF, l }, payload }: T, C(T) { R } -> payload: T'[l], F(T'[l]) { F(T'), F(T'), ..., F(T') }

Moreover, the constructor functions themselves can be put in the type registry. This allows for interpreting items with any type.

- An item with any type stores the reference to an interpreter function along with the payload data. The any constructor function reads the reference and constructs the interpreter function for the payload. This resembles to how an item with union type is interpreted.

  > { rF, payload }: T, C(T) { R } -> payload: T', F(T')

The constructed interpreter functions can be put back in the type registry having their own references. They can be stored in the constructed form, or stored as the construction guide with a fixed constructor function.

For example:

- The constructed tuple interpreter function can be stored as `{ rC, { k, rF1, rF2, ..., rFk } }` in the type registry where rC is the reference to the tuple constructor function.

  > { rF, payload }: T, C(T) { ...R, rF ~ { rC', { k, rF1, rF2, ..., rFk } } } -> { { k, rF1, rF2, ..., rFk }, payload }: T', C'(T') { R }

Thus, functions are also data. This allows for them to be identified and reused.



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
