# DynamicEditor

An editor that supports dynamic types and data structures.

## Demo

### Initialization



## Build Instructions

This project can be built with CMake on Windows. It depends on two other major projects of mine, `ViewDesign` and `BlockStore`.

## Implementation

The implementation is based on and simplified from the example described in [Preparation](docs/preparation.md).

### Type



### Data

The metadata and all item data are stored as blocks with `BlockStore`.

#### Metadata

The metadata block as the global root block stores a tuple of:
- the reference to the descriptor registry
- the reference to the root item block

#### Descriptor Registry

The descriptor registry is an *unordered ref set* of all descriptors of the items.

#### Root Item

The root item is stored in a block which has *descriptor any* type.

It is initialized as `BasicDescriptor` for `StringView` with an empty string.

### GUI


