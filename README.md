# DynamicEditor

An editor for dynamic schema

## Demo

### Initializing

### Converting to Tuple

### Appending Child

### Extracting as Block

## Build

This project can be built with CMake on Windows. It depends on two other major projects of mine, *BlockStore* and *ViewDesign*.

## Design

The implementation is based on and simplified from the example described in [Preparation](docs/preparation.md).

### Data

The metadata and all item data are stored as blocks with *BlockStore*.

#### Metadata

as the global root block, storing a tuple of:
- reference of `StringTable`
- reference of `DescriptorRegistry`
- reference of root `ItemBlock`

#### StringTable

an *ordered reference set* of all `String` (`std::u16string`) indexed and ordered by the strings themselves

#### DescriptorRegistry

an *ordered reference set* of all `Descriptor` indexed and ordered by the descriptors themselves

#### ItemBlock

type:
- `DescriptorAny`: `descriptor_ref, ...`

descriptor types: (descriptor: collected in `DescriptorRegistry`, indexed by itself, referenced by `descriptor_ref`)
- `BasicItemDescriptor`: `interpreter_ref`
  - `StringRef`
  - `ItemBlockRef`
- `TupleDescriptor`: `std::vector<descriptor_ref>`
- `DynamicLengthArrayDescriptor`: `descriptor_ref`

basic item types inheriting `BasicItem`: (interpreter: compile-time constant, indexed by `interpreter_ref`)
- `StringRef`: `block<String>`
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

##### Example

- root `ItemBlock` initialized as `BasicItemDescriptor` for `StringRef` referencing `u""`:

```
ItemBlock (#0 root)
(OnChildConvert(child, callback: Descriptor -> Descriptor): update descriptor reference, propagate data change)
- BasicItemDescriptor : Descriptor
  (OnConvert: create new descriptor from self)
  - StringRef : BasicItem
    (OnStringUpdate: lookup/create entry in `StringTable`, update block reference, propagate data change)
    - block<String> (u"")
```

- `BasicItemDescriptor` converted to `TupleDescriptor`:

```
ItemBlock (#0 root)
- TupleDescriptor : Descriptor
  (OnAppend: append a child descriptor)
  (OnInsertBefore: insert a child descriptor before another)
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<String> (u"")
```

- `TupleDescriptor` added a child of `TupleDescriptor` with no child descriptor:

```
ItemBlock (#0 root)
- TupleDescriptor : Descriptor
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<String> (u"")
  - TupleDescriptor : Descriptor
```

- the child `TupleDescriptor` added two child descriptors of `BasicItemDescriptor` for `StringRef` referencing `u""`:

```
ItemBlock (#0 root)
- TupleDescriptor : Descriptor
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<String> (u"")
  - TupleDescriptor : Descriptor
    (OnExtract: create `ItemBlock` initialized with self, convert self to `BasicItemDescriptor` for `ItemBlockRef` referencing the `ItemBlock`)
    - BasicItemDescriptor : Descriptor
      - StringRef : BasicItem
        - block<String> (u"")
    - BasicItemDescriptor : Descriptor
      - StringRef : BasicItem
        - block<String> (u"")
```

- the child `TupleDescriptor` extracted as block referenced by `ItemBlockRef` as `BasicItemDescriptor`:

```
ItemBlock (#0 root)
- TupleDescriptor : Descriptor
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<String> (u"")
  - BasicItemDescriptor : Descriptor
    - ItemBlockRef : BasicItem
      - item_block_ref (#1)

ItemBlock (#1)
- TupleDescriptor : Descriptor
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<String> (u"")
  - BasicItemDescriptor : Descriptor
    - StringRef : BasicItem
      - block<String> (u"")
```

#### ItemBlockCache

caches `ItemBlock` and synchronizes data for `ItemBlockView`

### View

All view components and helper controls are built with *ViewDesign*.

#### MainWindow

provides context:
- (state) `bool verbose`
- (state) `bool schema_mode`
- `StringTable&`
- `DescriptorRegistry&`
- `OpenTab(item_block_ref)`
- `Clipboard&`

displays:
- `Select`: update state `verbose`
- `Select`: update state `schema_mode`
- fixed tab: `Clipboard`
- fixed tab: root `ItemBlockView`
- tabs: `ItemBlockView`

#### Clipboard

type:
- `String`
- `item_block_ref`
- `DescriptorView::ConstReference`

provides:
- `PasteAsDescriptorView()`:
  - `String`: create `BasicItemDescriptor` for `StringRef` referencing the string
  - `item_block_ref`: create `BasicItemDescriptor` for `ItemBlockRef` storing the reference
  - `DescriptorView::ConstReference`: create `DescriptorView` copied from the reference

#### ItemBlockView

can be displayed:
- as a tab in `MainWindow`
- inline in `ItemBlockRefView`

consumes context:
- `DescriptorRegistry&` (passed to `DescriptorView`)
- `Clipboard&`

displays:
- `Button`: copy self as `item_block_ref` to `Clipboard`
- top-level `DescriptorView` of the `ItemBlock`

#### DescriptorView

inherited by:
- `BasicItemDescriptorView`
- `TupleDescriptorView`
- `DynamicLengthArrayDescriptorView`

consumes context:
- (state) `bool schema_mode`

provides:
- `Copy()`: create `DescriptorView` copying self
- `OnChildReplace(DescriptorView& child, std::unique_ptr<DescriptorView>)`: replace child with a new one
- `Undo()` / `Redo()`

displays:
- (`schema_mode`)
  - `Button`: create `Tuple` copying self as child and replace self
  - `Button`: create `DynamicLengthArray` copying self as child and replace self
  - `Button`: create `BasicItem` for `ItemBlockRef` referencing block initialized with copy of self and replace self
- `Button`: select self and set focus (update transient state `selected` which resets at `LoseFocus`)

shortcut:
- (`selected`)
  - ctrl+C: copy self as `DescriptorView::ConstReference` to `Clipboard`
  - ctrl+V: replace self with `Clipboard::PasteAsDescriptorView()`
  - delete: delete self

##### BasicItemDescriptorView

displays:
- `BasicItemView` inherited by:
  - `StringRefView`
  - `ItemBlockRefView`

###### StringRefView

consumes context:
- (state) `bool verbose`
- `StringTable&`

displays:
- (`verbose`):
  - block reference
  - `TextView`: unmodified string
- `TextEditor` (focusable): edit the current string (mark if it is modified)

###### ItemBlockRefView

consumes context:
- (state) `bool verbose`
- `OpenTab(item_block_ref)`

displays:
- (`verbose`):
  - block reference
- `Select`: display/hide inline `ItemBlockView` (updating state `selected`)
- `Button`: open tab `ItemBlockView`
- (`selected`):
  - the inline `ItemBlockView`

##### TupleDescriptorView

consumes context:
- (state) `bool schema_mode`

displays:
- (`schema_mode`):
  - `Button`: add/remove child descriptor
  - `Button`: convert to `DynamicLengthArray` (enabled if all children share the same descriptor)
- a list of `DescriptorView`

##### DynamicLengthArrayDescriptorView

consumes context:
- (state) `bool schema_mode`

displays:
- (`schema_mode`):
  - `Button`: create `Tuple` copied from self
- a list of `DescriptorView` sharing the same descriptor
- `Button`: add/remove child descriptor

## License

All contents of this repository are provided for viewing purposes only.

All rights reserved.

Issues or feedbacks are welcome.
