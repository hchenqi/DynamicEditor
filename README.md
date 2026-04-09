# DynamicEditor

An editor supporting dynamic types and data structures.

## Build Instructions

This project can be built with CMake on Windows. It depends on two other major projects of mine, `WndDesign` and `BlockStore`.

## Concepts

### Item / Block

Item is a logical unit of data in a hierarchy. An item can contain sub-items, and can be contained in a parent item.

Block is the storage unit. Each block has an upper size limit, and exposes an reference. Items are stored in blocks, and block boundary is naturally item boundary. An item larger than a block must be divided into sub-items and stored in multiple blocks linked by their references.

### Data Type / View Type

Data type describes the layout of the item data. View type describes how data in a certain data type is visually presented. Multiple view types can share the same data type.

> For example, boolean typed item can be displayed as `true` `false` strings, a checkbox, or a select with two options; integer typed item can be interpreted as a decimal string, hexadecimal string, background color, or the position of a slider, etc.

> Sometimes the view that allows user to edit item data also need to ensure the data is valid. For example, an integer type with possible values within 0 - 99 shouldn't take other values, though the storage bits can actually encode more values.

### Static Type / Dynamic Type

Item with static type doesn't store type information as the interpreter already knows its type and directly reads its data. Item with dynamic type stores type information along with its data for the interpreter to know how to interpret the data.

> If the item need to be static or dynamic is relative to the program that interprets it. For example, if a program that reads a string knows how long the string should be, then it's not necessary to also store the length of the string. If the program handles strings with dynamic lengths, the length need to be stored along with the string. And when the program already knows what the string exactly is, then the string doesn't need to be stored in item data at all.

> One could also say, that every item has static type, because every item has to be interpreted by a certain interpreter and not by the others. Items with dynamic type are just to be interpreted by the dynamic type interpreter, regardless of how the interpreter internally work.

If the types an item can possibly have can be enumerated, then the type information for the item can be stored as an integer index indicating which type the item has, and the item actually has union type. The interpreter for a union type contains interpreters for each type of the union. It reads the index and calls the corresponding interpreter for the type.

If the types an item can possibly have can't be enumerated, then the item has true dynamic type. The interpreter must first construct a specific interpreter for this type at runtime based on the type information of the item which then interprets the item data.

> We can consider a string with arbitrary length has true dynamic type because the length of a string can't be simply enumerated if we ignore a size limit. Similarly, we can think that an integer also has true dynamic type because we can't enumerate all integers.

### Item / Type / Interpreter Function / Runtime

Item t has a certain type T.

> t: T

An interpreter function F(T) interprets item with type T. The item type is fixed for an interpreter function, but the actual data of the item is not known until it is provided at runtime.

> F(T)

At runtime an interpreter function receives an input item and then outputs a new interpreter function and a new item for the output interpreter function to interpret.

> t: T, F(T) -> t': T', F'(T')

For example:

- An item with union type T of sub-types < T1, T2, ..., Tn > is stored as a tuple `{ i, payload }` with i being an integer index of the payload type in (1, 2, ..., n) and payload being sub-item in that type. Its interpreter function F(T) is a mapping between integer indices and sub- interpreter functions accepting the corresponding sub-types { 1 ~ F1(T1), 2 ~ F2(T2), ..., n ~ Fn(Tn) }. At runtime it receives the item, reads i, and produces an item just as `payload` with type Ti and interpreter function Fi.

  > { i, payload: Ti }: T < T1, T2, ..., Tn >, F(T) { 1 ~ F1(T1), 2 ~ F2(T2), ..., n ~ Fn(Tn) } -> payload: Ti, Fi(Ti)

- An interpreter function F for items with empty type {} gives constant output of an item with a certain type T and an interpreter function F'. We can also say that F is a constant function constructed by combining an item t with type T and F'(T).

  > {}, F({}) -> t: T, F'(T)

- An item with union type T of sub-types < T1, T2, ..., Tn >, where each sub-type Ti is an empty type, has empty payload and is stored just as the integer index `i`. Its interpreter function F(T) maps the indices to the corresponding sub- interpreter functions which are constant functions.

  > i: T < T1 {}, T2 {}, ..., Tn {} >, F(T) { 1 ~ F1({}), 2 ~ F2({}), ..., n ~ Fn({}) } -> {}, Fi({})

- Exist functions that receive a certain input type and output items with empty type and constant functions.

  > t: T, F(T) -> {}, F'({})

- An item with tuple type T of sub-types { T1, T2, ..., Tn } is stored as a tuple `{ t1: T1, t2: T2, ..., tn: Tn }`. The interpreter function F(T) can be constructed from the interpreter functions Fi(Ti) each interpreting the corresponding sub-item with type Ti.

  > { t1: T1, t2: T2, ..., tn: Tn }: T, F(T) { F1(T1), F2(T2), ..., Fn(Tn) } -> { t1': T1', t2': T2', ..., tn': Tn' }: T', F'(T') { F1'(T1'), F2'(T2'), ..., Fn'(Tn') }

- An item with dynamic length array type T[] is stored as a tuple `{ l, T, T, ..., T }` (with l times T). The interpreter function stores only one copy of F0(T) that interprets items with type T. The runtime reads the length l and outputs an item with tuple type T[l] = { T, T, ..., T } (l times T) and the interpreter function for this tuple. This output interpreter function only exists at runtime, because it is of arbitrary size depending on the input item.

  > { l, t1: T, t2: T, ..., tn: T }: T[], F(T[]) { F0(T) } -> { t1: T, t2: T, ..., tn: T }: T[l], F'(T[l]) { F0(T), F0(T), ..., F0(T) }

- With a fixed length l = k, the output interpreter function above can be stored for interpreting items with constant length array type T[k] as tuple { T, T, ..., T } (k times T).

  > { t1: T, t2: T, ..., tk: T }: T[k], F(T[k]) -> { t1': T', t2': T', ..., tk': T' }: T'[k], F'(T'[k])

### Construction of Interpreter Function

Interpreter functions interpreting items at runtime is when items guide the construction of the interpreter functions.

Interpreter functions can be constructed by following principles:

- Given interpreter functions F1(T1), F2(T2), ..., Fn(Tn), a union interpreter function F(T) can be constructed with T being the union type of sub-types < T1, T2, ..., Tn >.

- Given a union interpreter function F(T) with T being the union type of sub-types < T1, T2, ..., Tn > and an integer index i in (1, 2, ..., n), the interpreter function for Ti can be constructed by copying the ith sub- interpreter function Fi(Ti) from F(T).

- Given interpreter functions F1(T1), F2(T2), ..., Fn(Tn), a tuple interpreter function F(T) can be constructed with T being the tuple type of sub-types { T1, T2, ..., Tn }.

### Constructor Function / Interpreter Registry

An item with dynamic type stores the construction guide for the interpreter function of its data. The interpreter function for the item can also be called a constructor function for the output interpreter function.

A union interpreter function is a constructor function with the integer index stored in the item being the construction guide.

There are also constructor functions for constructing union and tuple interpreter functions at runtime.

Basic interpreter functions F1(TF1), F2(TF2), ..., Fm(TFm) where TF1, TF2, ..., TFm are derived from basic types T1, T2, ..., Tn can be kept in an `interpreter registry`, each with a unique reference. Other interpreter functions can be constructed with them by certain constructor functions. Multiple interpreter functions can share the same type for the input item but interpret in different ways.

For example:

- An item with arbitrary tuple type stores a list of references to interpreter functions in the interpreter registry { rF1, rF2, ..., rFk } along with the payload tuple. The interpreter function for this item, the tuple constructor function, reads the list of references and constructs a tuple interpreter function by looking up the corresponding interpreter functions in the interpreter registry R = { rF1 ~ F1(TF1), rF2 ~ F2(TF2), ..., rFm ~ Fm(TFm) }.

  > { { k, rF1, rF2, ..., rFk }, { t1: T1, t2: T2, ..., tk: Tk } }: T, F(T) { R } -> { t1: T1, t2: T2, ..., tk: Tk }: T', F'(T') { F1(T1), F2(T2), ..., Fk(Tk) }

- Similarly, an item with arbitrary union type stores a list of references to interpreter functions in the interpreter registry along with the payload union. The union constructor function reads the list of references and constructs a union interpreter function.

  > { { k, rF1, rF2, ..., rFk }, { i, payload } }: T, F(T) { R } -> { i, payload }: T', F'(T') { 1 ~ F1(T1), 2 ~ F2(T2), ..., k ~ Fk(Tk) }

- An item with dynamic length array type of an arbitrary type stores the reference to the interpreter function for a single sub-item in the interpreter registry and the length of the array along with the payload array. The array constructor function reads the reference and the length and constructs a constant length array interpreter function.

  > { { rF0, l }, t: T'[l] }: T, F(T) { R } -> t: T'[l], F'(T'[l]) { F0(T'), F0(T'), ..., F0(T') }

Moreover, constructor functions as interpreter functions themselves can be put in the interpreter registry. This allows for interpreting items with any type.

- An item with any type stores the reference to an interpreter function along with the payload data. The any constructor function reads the reference and constructs the interpreter function for the payload. This resembles to how an item with union type is interpreted.

  > { rF', payload: T' }: T, F(T) { R } -> payload: T', F'(T')

### Construction of Item

An item is constructed according to the construction of its interpreter function. This is the reverse process of interpreting an item.

For example:

- Given a sub-item ti with type Ti, an item with union type T of sub-types < T1, T2, ..., Tn > where i is in (1, 2, ..., n) can be constructed by attaching the index i to the sub-item.

  > ti: Ti --> { i, ti: Ti }: T < T1, T2, ..., Tn >

- Given sub-items t1, t2, ..., tn with type T1, T2, ..., Tn respectively, an item with tuple type T { T1, T2, ..., Tn } can be constructed by putting the sub-items together.

  > t1: T1, t2: T2, ..., tn: Tn --> { t1: T1, t2: T2, ..., tn: Tn }: T { T1, T2, ..., Tn }

- Given an item t with type T and the reference of its interpreter function rF in the interpreter registry, an item with any type can be constructed by attaching the reference to the original item.

  > t: T --> { rF, t: T }: T'

- Given an item with tuple type T { T1, T2, ..., Tn } and the references of interpreter functions { rF1, rF2, ..., rFn } for each of the sub-item in the interpreter registry, an item with arbitrary tuple type T' can be constructed by attaching the reference list to the original item.

  > { t1: T1, t2: T2, ..., tn: Tn }: T --> { { n, rF1, rF2, ..., rFn }, { t1: T1, t2: T2, ..., tn: Tn }: T }: T'

  By attaching the references of interpreter functions to each of the sub-item of the tuple, we can also get an item with dynamic length array type of any type.

  > { { n, rF1, rF2, ..., rFn }, { t1: T1, t2: T2, ..., tn: Tn } } => { n, { rF1, t1: T1 }, { rF2, t2: T2 }, ..., { rFn, tn: Tn } }

- Given an item with union type T of sub-types < T1, T2, ..., Tn > and the references of interpreter functions { rF1, rF2, ..., rFn } for each of the sub-type in the interpreter registry, an item with arbitrary union type T' can be constructed by attaching the reference list to the original item.

  > { i, ti: Ti }: T < T1, T2, ..., Tn > --> { { n, rF1, rF2, ..., rFn }, { i, ti: Ti }: T }: T'

  This could possibly be simplified to an item with any type by just keeping the ith interpreter function reference:

  > { { n, rF1, rF2, ..., rFn }, { i, ti: Ti } } => { rFi, ti: Ti }

- Given sub-items t1, t2, ..., tn with the same type T, an item with fixed length array type T[n] can be constructed.

  > t1: T, t2: T, ..., tn: T --> { t1: T, t2: T, ..., tn: T }: T[n]

  It can be converted to dynamic length array type.

  > { t1: T, t2: T, ..., tn: T } => { n, t1: T, t2: T, ..., tn: T }

  With the reference of the interpreter function for a single sub-item, it can be converted to fixed length array type of an arbitrary type, or dynamic length array type of an arbitrary type.

  > { t1: T, t2: T, ..., tn: T } => { rF0, { t1: T, t2: T, ..., tn: T } }

  > { t1: T, t2: T, ..., tn: T } => { { rF0, n }, { t1: T, t2: T, ..., tn: T } }

  They can be further converted to fixed or dynamic length array type of any type.

  > { rF0, { t1: T, t2: T, ..., tn: T } } => { { rF0, t1: T }, { rF0, t2: T }, ..., { rF0, tn: T } }

  > { { rF0, n }, { t1: T, t2: T, ..., tn: T } } => { n, { rF0, t1: T }, { rF0, t2: T }, ..., { rF0, tn: T } }

### Construction Guide / Descriptor Registry

The construction guide stored in each item with the same type could be shared and stored in one copy. This way, a constructed interpreter function is represented as construction guide to a certain constructor function, which allows it to be identified and reused.

These construction guides can be called descriptors and dynamically added in a `descriptor registry`, each unique and with a unique reference.

Like an item with any type storing the reference to the interpreter function for its payload data, an item with descriptor any type stores the reference to the descriptor that guides the construction of the interpreter function for its payload data.

For example:

- The descriptor of tuple interpreter function for an item with tuple type of sub-types { T1, T2, ..., Tk } can be added as { rFC, { k, rF1, rF2, ..., rFk } } with reference rDn+1 in the descriptor registry DR with existing descriptors { rD1 ~ D1, rD2 ~ D2, ..., rDn ~ Dn }, where rFC is the reference to the tuple constructor function FC. The item is attached with the single reference to the descriptor instead of the list of references to interpreter functions of its sub-items.

  > { { k, rF1, rF2, ..., rFk }, { t1: T1, t2: T2, ..., tk: Tk } }: T, FC(T) { R } => { rDn+1, { t1: T1, t2: T2, ..., tk: Tk } }: T', F'(T') { R, DR ∪ { rDn+1 ~ { rFC, { k, rF1, rF2, ..., rFk } } } }

- Similarly, an item with union type can be attached with the reference to a descriptor. It can no longer be simplified by just keeping the ith interpreter function reference but multiple items with the same union type can refer to the same descriptor.

  > { { k, rF1, rF2, ..., rFk }, { i, ti: Ti } }: T, FC(T) { R } => { rDn+1, { i, ti: Ti } }: T', F'(T') { R, DR ∪ { rDn+1 ~ { rFC, { k, rF1, rF2, ..., rFk } } } }

A descriptor can contain references to interpreter functions or other descriptors. This makes it possible to have circular references for descriptors.

## Implementation

### Dependency

#### BlockStore

This project uses `BlockStore` as the storage library. `BlockStore` supports block creation/read/write, serialization/deserialization, garbage collection and common item containers.

A block is allocated by `BlockManager`. `BlockManager` also keeps the metadata for the backend database file and controls garbage collection.

The reference of a block is `block_ref`. With `block_ref` a one can read and write the data of a block. A `block_ref` can be serialized to block data and deserialized from block data.

Serialization and deserialization between block data and objects are performed by `SerializeContext` and `DeserializeContext`.

#### WndDesign

This project uses `WndDesign` as the GUI library.

All view components are derived from the component base class `WndObject` and various template components that calculate layouts, draw contents and deliver messages by different ways.

### Core

#### ItemView

`ItemView` is the base view class for an item. It is inherited and implemented by specific view types. The class constructor of a class inheriting `ItemView` is a basic interpreter function for an item.

Each `ItemView` has a parent `ItemView`, except when it's the root item of a `BlockView`. The parent `ItemView` or the `BlockView` calls the constructor of an `ItemView` providing the `DeserializeContext` as the item for it to interpret.

An `ItemView` can modify its data and construct a new item. It notifies its parent `ItemView` about its data change which then updates the parent item until `BlockView`.

#### BlockView

`BlockView` marks block boundaries. It can contain a tree of `ItemView`, and can itself be contained in a `RefView` as inline expansion of an item with Ref type.

`BlockView` provides the initial `DeserializeContext` from the block for its root item and stores the updated root item to the block.

#### Interpreter

The basic item types are:
- Empty: `void`
- Boolean: `bool`
- Integer: `int`
- String: `std::string`
- Ref: `std::pair<interpreter_ref, block_ref>`

The constructed item types are:
- Any: `interpreter_ref, ...`
- Array: `std::pair<size_t, interpreter_ref>, ...`
- Tuple: `std::vector<interpreter_ref>, ...`
- Union: `std::vector<interpreter_ref>, size_t, ...`

The basic interpreter functions are provided by the view types inheriting `ItemView`:
- EmptyView
- BooleanView
- IntegerView
- StringView
- RefView
(constructor functions)
- AnyView
- ArrayView
- TupleView
- UnionView

The list of basic interpreter functions including constructor functions can be extended.

The reference type of basic interpreter functions is:
- interpreter_ref: `size_t`

Each basic interpreter function class is registered in the `InterpreterRegistry` with a fixed order and keeps its `interpreter_ref` statically initialized when registered.

#### Descriptor

`DescriptorAnyView` inheriting `ItemView` provides the interpreter function for items with descriptor any type:
- DescriptorAny: `descriptor_ref, ...`

A descriptor can be registered in `DescriptorRegistry` returning its `descriptor_ref`. Each descriptor registered in `DescriptorRegistry` is unique. The same `descriptor_ref` will be returned if a descriptor is already registered. The descriptor types are:
- BasicDescriptor: `interpreter_ref`
- ArrayDescriptor: `std::pair<size_t, descriptor_ref>`
- TupleDescriptor: `std::vector<descriptor_ref>`
- UnionDescriptor: `std::vector<descriptor_ref>`

A `descriptor_ref` is a `block_ref` for the union of the descriptor types:
- descriptor_ref: `block_ref` (`block<std::variant<BasicDescriptor, ArrayDescriptor, TupleDescriptor, UnionDescriptor>>`)

From a `descriptor_ref` the corresponding descriptor can be retrieved and a `DescriptorView` can be constructed.

`DescriptorView` is the base class for a constructor that constructs an interpreter function by a descriptor of a certain descriptor type. It is inherited and implemented by the descriptor view types:
- BasicDescriptorView
- ArrayDescriptorView
- TupleDescriptorView
- UnionDescriptorView

Type conversion is performed when the type of an item is changed.
