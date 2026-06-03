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

#### String Table

The string table is an *unordered reference set* of all strings (`std::u16string`).

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
- `DynamicLengthArrayDescriptor`: `descriptor_ref`

Item update:
- `StringRefView`: updating the text string and referencing to the new string block in the string table
- `DescriptorAnyRefView`: referencing another item with *descriptor any* type
- `DescriptorAnyView`: updating item data, or converting descriptor

Descriptor update:
- `BasicDescriptor`:
  - updating the item with its interpreter
- `BasicDescriptor` | `TupleDescriptor` | `DynamicLengthArrayDescriptor` (natural)<-> `TupleDescriptor` | `DynamicLengthArrayDescriptor`
- `TupleDescriptor`:
  - adding a child descriptor
  - updating a child descriptor
  - removing a child descriptor
- `DynamicLengthArrayDescriptor`:
  - updating the descriptor

Descriptor wrap/unwrap:
- `DescriptorAnyView` (wrap)-> `DescriptorAnyRefView`
- `DescriptorAnyRefView` (unwrap) -> `DescriptorAnyView`

#### Example

- The root item with *descriptor any* type initialized as `BasicDescriptor` for `StringRefView` referencing an empty string:

```
BlockView (root item block)
- DescriptorAnyView : ItemView
  (OnChildConvert(child, callback: DescriptorView -> DescriptorView))
  - BasicDescriptorView : DescriptorView
    (OnConvert: create descriptor view, update descriptor reference, propagate data change)
    - StringRefView : ItemView
      (OnStringUpdate: lookup/create entry in the string table, update block reference, propagate data change)
      - block<std::u16string>
        - u""
```

- `BasicDescriptorView` converted to `TupleDescriptorView`:

```
BlockView (root item block)
- DescriptorAnyView : ItemView
  - TupleDescriptorView : DescriptorView
    - BasicDescriptorView : DescriptorView
      - StringRefView : ItemView
        - block<std::u16string>
          - u""
```

- `TupleDescriptorView` added a child:



### GUI

All item views and helper controls are displayed as view components with *ViewDesign*.

#### Main Window

The main window displays:
- the current block view (initial as root block view)

#### BlockView

`BlockView` displays the current item block with *descriptor any* type.

It can be displayed inline in a `DescriptorAnyRefView`, or displayed in an individual tab.

A `BlockView` contains the root `ItemView`.

#### ItemView

`ItemView` is inherited by:
- `StringRefView`
- `DescriptorAnyRefView`
- `DescriptorAnyView`

#### DescriptorAnyView

#### DescriptorView

`DescriptorView` is inherited by:
- `BasicDescriptorView`: contains `ItemView`
- `TupleDescriptorView`: contains a list of `DescriptorView`
- `DynamicLengthArrayDescriptorView`: contains a list of `DescriptorView` sharing the same descriptor
