# DynamicEditor

An editor with dynamic schemas

## Demo

### Initializing

### Converting to Tuple

### Appending Child

### Wrapping as Block

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

All item blocks including the root item block have *descriptor any* type.

#### Item Type

The inline item types are: (interpreter indexed by `interpreter_ref`)
- `StringRef`: `block<std::u16string>`
- `ItemBlockRef`: `block_ref` (of an item block)

Updating inline item data:
- `StringRef`: updating the text string and referencing to the new string block in the string table
- `ItemBlockRef`: referencing another item block

The item block type is:
- `DescriptorAny`: `descriptor_ref, ...`

The descriptor types are:
- `BasicDescriptor`: `interpreter_ref`
  - `StringRef`
  - `ItemBlockRef`
- `TupleDescriptor`: `std::vector<descriptor_ref>`
- `DynamicLengthArrayDescriptor`: `descriptor_ref`

Updating descriptor data:
- `BasicDescriptor`: updating inline item data (propagates to parent descriptor)
- `DynamicLengthArrayDescriptor`: adding/removing child

Converting descriptor type:
- `BasicDescriptor` | `TupleDescriptor` | `DynamicLengthArrayDescriptor` <-> `TupleDescriptor` | `DynamicLengthArrayDescriptor` (natural: single child)
- `TupleDescriptor` <-> `TupleDescriptor` (adding/updating/removing child)
- `DynamicLengthArrayDescriptor` <-> `TupleDescriptor` (natural: distribution; restricted(children with the same descriptor): reverse-distribution)
- `BasicDescriptor` | `TupleDescriptor` | `DynamicLengthArrayDescriptor` <-> `BasicDescriptor` for `ItemBlockRef` (wrap; unwrap)

#### Example

- The root item block initialized as `BasicDescriptor` for `StringRef` referencing an empty string:

```
ItemBlock (root item block)
(OnChildConvert(child, callback: Descriptor -> Descriptor): update descriptor reference, propagate data change)
- BasicDescriptor : Descriptor
  (OnConvert: create new descriptor from self)
  - StringRef : InlineItem
    (OnStringUpdate: lookup/create entry in the string table, update block reference, propagate data change)
    - block<std::u16string> (u"")
```

- `BasicDescriptor` converted to `TupleDescriptor`:

```
ItemBlock (root item block)
- TupleDescriptor : Descriptor
  (OnAppend: append a child descriptor)
  (OnInsertBefore: insert a child descriptor before another)
  - BasicDescriptor : Descriptor
    - StringRef : InlineItem
      - block<std::u16string> (u"")
```

- `TupleDescriptor` added a child of `TupleDescriptor`:

```
ItemBlock (root item block)
- TupleDescriptor : Descriptor
  - BasicDescriptor : Descriptor
    - StringRef : InlineItem
      - block<std::u16string> (u"")
  - TupleDescriptor : Descriptor
```

- The child `TupleDescriptor` added two children of `BasicDescriptor` for `StringRef` referencing the empty string:

```
ItemBlock (root item block)
- TupleDescriptor : Descriptor
  - BasicDescriptor : Descriptor
    - StringRef : InlineItem
      - block<std::u16string> (u"")
  - TupleDescriptor : Descriptor
    (OnWrap: create an item block initialized with self, convert self to `BasicDescriptor` for `ItemBlockRef` referencing the block)
    - BasicDescriptor : Descriptor
      - StringRef : InlineItem
        - block<std::u16string> (u"")
    - BasicDescriptor : Descriptor
      - StringRef : InlineItem
        - block<std::u16string> (u"")
```

- The child `TupleDescriptor` wrapped to `BasicDescriptor` for `ItemBlockRef`:

```
ItemBlock (root item block)
- TupleDescriptor : Descriptor
  - BasicDescriptor : Descriptor
    - StringRef : InlineItem
      - block<std::u16string> (u"")
  - BasicDescriptor : Descriptor
    - ItemBlockRef : InlineItem
      - block_ref (ItemBlock)
        - TupleDescriptor : Descriptor
          - BasicDescriptor : Descriptor
            - StringRef : InlineItem
              - block<std::u16string> (u"")
          - BasicDescriptor : Descriptor
            - StringRef : InlineItem
              - block<std::u16string> (u"")
```

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
