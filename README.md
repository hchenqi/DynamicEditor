# DynamicEditor

An editor with dynamic schemas

## Demo

### Initialization

### 

## Build Instructions

This project can be built with CMake on Windows. It depends on two other major projects of mine, *ViewDesign* and *BlockStore*.

## Implementation

The implementation is based on and simplified from the example described in [Preparation](docs/preparation.md).

### Data

The metadata and all item data are stored as blocks with *BlockStore*.

#### Metadata

The metadata block as the global root block stores a tuple of:
- the reference to the descriptor registry
- the reference to the root item block

#### Descriptor Registry

The descriptor registry is an *unordered reference set* of all descriptors.

#### Root Item

The root item is stored in a block with *descriptor any* type, initialized as `BasicDescriptor` for `StringView` with an empty string.

#### Item Type

The interpreter functions and their corresponding item types are:
- `StringView`: `std::u16string`
- `RefView`: `std::pair<interpreter_ref, block_ref>`
- `DescriptorAnyView`: `descriptor_ref, ...`

The descriptor types are:
- `BasicDescriptor`: `interpreter_ref`
  - `StringView`
  - `RefView`
- `TupleDescriptor`: `std::vector<descriptor_ref>`

Item update:
- `StringView`: modifying the text string
- `RefView`: referencing another item block with its interpreter
- `DescriptorAnyView`: updating data, or updating data with a new descriptor

Item conversion:
- `StringView` | `DescriptorAnyView` -> `RefView`
- `RefView` (accordingly)-> `StringView` | `DescriptorAnyView`

Descriptor conversion:
- `BasicDescriptor`:
  - `StringView` -> `RefView`
  - `RefView` -> `StringView`
- `BasicDescriptor` -> `TupleDescriptor`
- `TupleDescriptor`:
  - adding a child descriptor
  - changing a child descriptor
  - removing a child descriptor
- `TupleDescriptor` (with one child)-> `BasicDescriptor`

### GUI

All item views and helper controls are displayed as view components with *ViewDesign*.

#### ItemView

#### DescriptorSelect
