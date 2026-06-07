# Design

The design is based on and simplified from the example described in [Preparation](preparation.md).

## Data

The metadata and all item data are stored as blocks with *BlockStore*.

### Metadata

as the global root block, storing a tuple of:
- reference of `StringTable`
- reference of `DescriptorRegistry`
- reference of root `ItemBlock`

### StringTable

an *ordered reference set* of all `String` (`std::u16string`) indexed and ordered by the strings themselves

### DescriptorRegistry

an *ordered reference set* of all `Descriptor` indexed and ordered by the descriptors themselves

### ItemBlock

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

#### Example

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

### ItemBlockCache

caches `ItemBlock` and synchronizes data for `ItemBlockView`

## View

All view components and helper controls are built with *ViewDesign*.

### MainWindow

provides context:
- (state) `bool verbose`
- (state) `bool schema_mode`
- `StringTable&`
- `ItemBlockCache&`
- `DescriptorRegistry&`
- `OpenTab(item_block_ref)`
- `Clipboard&`

displays:
- `Select`: update state `verbose`
- `Select`: update state `schema_mode`
- tab (fixed): `Clipboard`
- tab (fixed): root `ItemBlockView`
- tabs (closable): `ItemBlockView`

### Clipboard

type:
- `String` (possibly from external)
- `ItemBlockView::ConstReference`
- `DescriptorView::ConstReference`

provides:
- `Put(String)`
- `Put(ItemBlockView::ConstReference)`
- `Put(DescriptorView::ConstReference)`
- `std::unique_ptr<DescriptorView> GetDescriptorView()`:
  - `String`: create `BasicItemDescriptor` for `StringRef` referencing the string
  - `ItemBlockView::ConstReference`: create `BasicItemDescriptor` for `ItemBlockRef` storing the `item_block_ref` of the `ItemBlockView`
  - `DescriptorView::ConstReference`: create `DescriptorView` copied from the reference

displays:
- `ListLayout` as a stack of clipboard items with type and preview, the newest on top

### ItemBlockView

can be displayed:
- as a tab in `MainWindow`
- inline in `ItemBlockRefView`

consumes context:
- `ItemBlockCache&`
- `DescriptorRegistry&` (passed as argument to `DescriptorView`)
- `Clipboard&`

consumes argument:
- `item_block_ref`

provides:
- `GetReference()`: create `ItemBlockView::ConstReference` from self

displays:
- `Button`: put `GetReference()` to `Clipboard`
- top-level `DescriptorView` of the `ItemBlock` read from and synchronized by `ItemBlockCache&`

### DescriptorView

inherited by:
- `BasicItemDescriptorView`
- `TupleDescriptorView`
- `DynamicLengthArrayDescriptorView`

consumes context:
- (state) `bool schema_mode`
- `Clipboard&`

consumes argument:
- `DescriptorRegistry&` (passed as argument to child `DescriptorView`)

provides:
- `GetReference()`: create `DescriptorView::ConstReference` from self
- `DuplicateSelf()`: create `DescriptorView` copying self
- `ReplaceSelf(std::unique_ptr<DescriptorView>)`: replace self with another in parent `DescriptorView`
- `DeleteSelf()`: delete self in parent `DescriptorView`
- `virtual void OnChildReplace(DescriptorView& child, std::unique_ptr<DescriptorView>) {}`
- `virtual void OnChildDelete(DescriptorView& child) {}`

displays:
- (`schema_mode`)
  - `Button`: call `ReplaceSelf()` with `TupleDescriptorView` constructed with one child from `DuplicateSelf()`
  - `Button`: call `ReplaceSelf()` with `DynamicLengthArrayDescriptorView` constructed with one child from `DuplicateSelf()`
  - `Button`: call `ReplaceSelf()` with `BasicItemDescriptorView` constructed with `ItemBlockRef` referencing the `ItemBlock` constructed from `DuplicateSelf()` displaying inline `ItemBlockView`
- `Button`: select self and set focus (update transient state `selected` which resets at `LoseFocus`)

shortcut:
- (`selected`)
  - ctrl+C: put `GetReference()` to `Clipboard`
  - ctrl+V: call `ReplaceSelf()` with `Clipboard::GetDescriptorView()`
  - delete: call `DeleteSelf()`

#### BasicItemDescriptorView

displays:
- `BasicItemView` inherited by:
  - `StringRefView`
  - `ItemBlockRefView`

##### StringRefView

consumes context:
- (state) `bool verbose`
- `StringTable&`

displays:
- (`verbose`):
  - block reference
  - `TextView`: unmodified string
- `TextEditor` (focusable): edit the current string (mark if it is modified)
- `Button` (enabled if string modified): commit the string

##### ItemBlockRefView

consumes context:
- (state) `bool verbose`
- `OpenTab(item_block_ref)`

displays:
- (`verbose`):
  - block reference
- `Select`: display/hide inline `ItemBlockView` (updating state `show_inline`)
- `Button`: open tab `ItemBlockView`
- (`show_inline`):
  - the inline `ItemBlockView`

#### TupleDescriptorView

consumes context:
- (state) `bool schema_mode`

overrides:
- `virtual void OnChildReplace(DescriptorView& child, std::unique_ptr<DescriptorView>)`: replace child with another and update descriptor
- `virtual void OnChildDelete(DescriptorView& child)`: delete child and update descriptor

displays:
- (`schema_mode`):
  - `Button` (before each child and at the end): add/remove child descriptor
  - `Button` (enabled if all children share the same descriptor): call `ReplaceSelf()` with `DynamicLengthArrayDescriptorView` constructed with children copied from self
- a list of `DescriptorView`

#### DynamicLengthArrayDescriptorView

consumes context:
- (state) `bool schema_mode`

overrides:
- `virtual void OnChildReplace(DescriptorView& child, std::unique_ptr<DescriptorView>)`: if descriptor matches, replace child with another, else show error message
- `virtual void OnChildDelete(DescriptorView& child)`: delete child

displays:
- `Button` (before each child and at the end): add/remove child descriptor
- (`schema_mode`):
  - `Button`: call `ReplaceSelf()` with `TupleDescriptorView` constructed with children copied from self
- a list of child `DescriptorView` sharing the same descriptor
