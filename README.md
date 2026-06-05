# DynamicEditor

An editor with dynamic schemas

## Demo

### Initializing

### Converting to Tuple

### Appending Child

### Extracting as Block

## Build Instructions

This project can be built with CMake on Windows. It depends on two other major projects of mine, *BlockStore* and *ViewDesign*.

## Implementation

The implementation is based on and simplified from the example described in [Preparation](docs/preparation.md).

### Data

The metadata and all item data are stored as blocks with *BlockStore*.

#### Metadata

the global root block storing a tuple of:
- reference of `StringTable`
- reference of `DescriptorRegistry`
- reference of root `ItemBlock`

#### StringTable

an *unordered reference set* of all strings (`std::u16string`).

#### DescriptorRegistry

an *unordered reference set* of all descriptors.

#### ItemBlock

has type:
- `DescriptorAny`: `descriptor_ref, ...`

descriptor types: (descriptor: collected in `DescriptorRegistry`, indexed by itself, referenced by `descriptor_ref`)
- `BasicItemDescriptor`: `interpreter_ref`
  - `StringRef`
  - `ItemBlockRef`
- `TupleDescriptor`: `std::vector<descriptor_ref>`
- `DynamicLengthArrayDescriptor`: `descriptor_ref`

basic item types inheriting `BasicItem`: (interpreter: compile-time constant, indexed by `interpreter_ref`)
- `StringRef`: `block<std::u16string>` (string: collected in `StringTable`, indexed by itself, referenced by `block<std::u16string>`)
- `ItemBlockRef`: `item_block_ref`

basic item data update: (propagates to parent descriptor)
- `StringRef`: updating the string and referencing the new string block in `StringTable`
- `ItemBlockRef`: referencing another `ItemBlock`

descriptor data update:
- `BasicItemDescriptor`: updating basic item data
- `TupleDescriptor`: updating child descriptor data
- `DynamicLengthArrayDescriptor`: adding/removing child, updating child descriptor data

descriptor type conversion:
- `BasicItemDescriptor` | `TupleDescriptor` | `DynamicLengthArrayDescriptor` <-> `TupleDescriptor` | `DynamicLengthArrayDescriptor` (natural: single child)
- `TupleDescriptor` <-> `TupleDescriptor` (adding/updating/removing child)
- `DynamicLengthArrayDescriptor` <-> `TupleDescriptor` (natural: distribution; restricted(children with the same descriptor): reverse-distribution)
- `BasicItemDescriptor` | `TupleDescriptor` | `DynamicLengthArrayDescriptor` -> `BasicItemDescriptor` for `ItemBlockRef` (extract)

#### Example

- The root `ItemBlock` initialized as `BasicItemDescriptor` for `StringRef` referencing `u""`:

```
ItemBlock (#0 root)
(OnChildConvert(child, callback: Descriptor -> Descriptor): update descriptor reference, propagate data change)
- BasicItemDescriptor : Descriptor
  (OnConvert: create new descriptor from self)
  - StringRef : BasicItem
    (OnStringUpdate: lookup/create entry in `StringTable`, update block reference, propagate data change)
    - block<std::u16string> (u"")
```

- `BasicItemDescriptor` converted to `TupleDescriptor`:

```
ItemBlock (#0 root)
- TupleDescriptor : Descriptor
  (OnAppend: append a child descriptor)
  (OnInsertBefore: insert a child descriptor before another)
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<std::u16string> (u"")
```

- `TupleDescriptor` added a child of `TupleDescriptor` with no child descriptor:

```
ItemBlock (#0 root)
- TupleDescriptor : Descriptor
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<std::u16string> (u"")
  - TupleDescriptor : Descriptor
```

- The child `TupleDescriptor` added two child descriptors of `BasicItemDescriptor` for `StringRef` referencing `u""`:

```
ItemBlock (#0 root)
- TupleDescriptor : Descriptor
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<std::u16string> (u"")
  - TupleDescriptor : Descriptor
    (OnExtract: create `ItemBlock` initialized with self, convert self to `BasicItemDescriptor` for `ItemBlockRef` referencing the `ItemBlock`)
    - BasicItemDescriptor : Descriptor
      - StringRef : BasicItem
        - block<std::u16string> (u"")
    - BasicItemDescriptor : Descriptor
      - StringRef : BasicItem
        - block<std::u16string> (u"")
```

- The child `TupleDescriptor` extracted to a new block referenced by `ItemBlockRef` as `BasicItemDescriptor`:

```
ItemBlock (#0 root)
- TupleDescriptor : Descriptor
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<std::u16string> (u"")
  - BasicItemDescriptor : Descriptor
    - ItemBlockRef : BasicItem
      - item_block_ref (#1)

ItemBlock (#1)
- TupleDescriptor : Descriptor
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<std::u16string> (u"")
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<std::u16string> (u"")
```

### UI

All view components and helper controls are built with *ViewDesign*.

#### MainWindow

displays:
- one fixed tab for root `ItemBlockView`
- additional tabs for `ItemBlockView`

provides context:
- `StringTable&`
- `DescriptorRegistry&`
- `OpenTab(item_block_ref)`

#### ItemBlockView

can be displayed in a tab in `MainWindow` or inline in `ItemBlockRefView`

displays the current `ItemBlock` holding the top-level `DescriptorView`

consumes context `DescriptorRegistry&` and passes it to `DescriptorView`

#### DescriptorView

inherited by:
- `BasicItemDescriptorView`
- `TupleDescriptorView`
- `DynamicLengthArrayDescriptorView`

(verbose) displays conversion options:
- convert to `Tuple` as child
- convert to `DynamicLengthArray` as child
- extract as block

##### BasicItemDescriptorView

contains `BasicItemView`

##### TupleDescriptorView

contains a list of `DescriptorView`

(verbose) displays additional conversion options:
- add/update/remove a child descriptor
- convert to `DynamicLengthArray` (enabled if all children share the same descriptor)

##### DynamicLengthArrayDescriptorView

contains a list of `DescriptorView` sharing the same descriptor

(verbose) displays additional conversion options:
- convert to `Tuple`

#### BasicItemView

inherited by:
- `StringRefView`
- `ItemBlockRefView`

##### StringRefView

consumes context `StringTable&`

##### ItemBlockRefView

consumes context `OpenTab(item_block_ref)`

opens a tab for the `ItemBlockView` or displays it inline
