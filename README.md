# DynamicEditor

An editor with dynamic schemas

## Demo

### Initialization

### 

## Build Instructions

This project can be built with CMake on Windows. It depends on two other major projects of mine, *BlockStore* and *ViewDesign*.

## Implementation

The implementation is based on and simplified from the example described in [Preparation](docs/preparation.md).

### Data

The metadata and all item data are stored as blocks with *BlockStore*.

#### Metadata

The metadata block as the global root block stores a tuple of:
- the reference to the descriptor registry
- the reference to the string table
- the reference to the root item block

#### Descriptor Registry

The descriptor registry is an *unordered reference set* of all descriptors.

#### Root Item

The root item is stored in a block with *descriptor any* type.

#### Item Type

The interpreter functions and their corresponding item types are:
- `StringRefView`: `block<std::u16string>`
- `DescriptorAnyRefView`: `block_ref` (to an item with *descriptor any* type)
- `DescriptorAnyView`: `descriptor_ref, ...`

The descriptor types are:
- `BasicDescriptor`: `interpreter_ref`
  - `StringRefView`
  - `DescriptorAnyRefView`
- `TupleDescriptor`: `std::vector<descriptor_ref>`

Item update:
- `StringRefView`: updating the text string and referencing to the new string block in the string table
- `DescriptorAnyRefView`: referencing another item with *descriptor any* type
- `DescriptorAnyView`: updating data, or converting descriptor

Descriptor update:
- `BasicDescriptor`:
  - updating the item with its interpreter
- `BasicDescriptor` | `TupleDescriptor` (natural)-> `TupleDescriptor`
- `TupleDescriptor` (with one child)-> `BasicDescriptor` | `TupleDescriptor`
- `TupleDescriptor`:
  - adding a child descriptor
  - updating a child descriptor
  - removing a child descriptor

Descriptor wrap/unwrap:
- `DescriptorAnyView` (wrap)-> `DescriptorAnyRefView`
- `DescriptorAnyRefView` (unwrap) -> `DescriptorAnyView`

#### Workflow

- The root item with *descriptor any* type is initialized as `BasicDescriptor` for `StringRefView` referencing an empty string:

```
BlockView (root item block)
- DescriptorAnyView : ItemView
  - BasicDescriptorView : DescriptorView
    - StringRefView : ItemView
      - block<std::u16string>
        - u""
```

- Modifying the string value only creates/reuses an entry in the string table and the `StringRefView` updates the reference to the entry.

#### Example

- The root item has schema:

```
TupleDescriptor
- BasicDescriptor
  - StringRef
- BasicDescriptor
  - DescriptorAnyRef
- BasicDescriptor
  - DescriptorAnyRef
```

### GUI

All item views and helper controls are displayed as view components with *ViewDesign*.

#### ItemView

`ItemView` is inherited by:
- `StringRefView`
- `DescriptorAnyRefView`
- `DescriptorAnyView`

#### BlockView

`BlockView` displays the current item block with *descriptor any* type.

It can be displayed inline in a `DescriptorAnyRefView`, or displayed in its own tab.

#### TreeView

`TreeView` displays the tree structure of the current `BlockView`. 

#### DescriptorSelect
