# DynamicEditor

An editor supporting dynamic types and data structures.

## Build Instructions

This project can be built with CMake on Windows platform. It depends on two other major projects of mine, `WndDesign` and `BlockStore`.

## Concepts

### Block / Item

Block is the storage unit with an upper size limit. Each block can be referenced. Item is a logical unit. Block boundary is naturally item boundary but not necessarily vice versa. Multiple items can be stored in one block. Multiple blocks can be used for constructing one item in a certain data structure by referencing.

### Data Type / View Type

Data type describes the layout of the data of an item. View type describes how data in a certain data type is presented. Multiple view types can share the same data type.

> For example, boolean typed item can be displayed as `true` `false` strings, a checkbox, or a select with two options; integer typed item can be interpreted as a decimal string, hexadecimal string, background color, or the position of a slider, etc.

> The view that allows user to edit the data of an item also need to ensure the data is valid. For example, an integer type with possible values within 0 - 99 shouldn't take other values, though the storage bits can actually encode more values.

### Static Type / Dynamic Type

Item with static type doesn't store extra type information and can only be interpreted in one way. Item with dynamic type can take values of different types thus the type information also needs to be stored.

> If the item need to be static or dynamic is relative to the program that interprets it. For example, if a program that reads a string knows how long the string should be, then it's not necessary to also store the length of the string. On the contrary, if the program handles strings with dynamic lengths, the length need to be stored along with the string. And in an extreme case, when the program already knows what the string exactly is, then the string doesn't need to be stored in item data at all.

If the types the item can take is pre-determined, then the item has a union type. The type information for the item is an index of which type in the union is chosen. The item still behaves like with a static type if we consider the union type to be just the type of the item. The program that reads the item still knows all possible types the item can take.

If the types the item can take can't be enumerated beforehand, but can be constructed in a certain way, the item has true dynamic type. The program constructs the type based on the type information stored in the item data before it can interpret the item.

> We can consider a string with arbitrary length has true dynamic type because the length of a string can't be simply enumerated regardless of the size limit. Similarly, we can think that an integer has dynamic type over all possible integers because we can't enumerate all of them.
