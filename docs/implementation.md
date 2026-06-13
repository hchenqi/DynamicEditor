# Implementation

> The implementation is based on [Design](design.md) with a modified structure and an emphasis on `History` and operations.

## ItemRef

copy-on-write reference for `Item`

specialization of `COWRef<T>` (utility) which either holds a const reference or an owning `std::unique_ptr<const T>`

used to store items with `History` stacks, where references to objects in previous stack entries are ensured to be alive

(the const reference and the `std::unique_ptr<const T>` can be replaced with a single `std::shared_ptr<const T>` to support dropping entries in the stacks and automatically transferring the ownership to the later entries)

## Context

application-level singletons

### DescriptorRegistry

a persistent *ordered reference set* of all `Descriptor`, ordered by and indexed with the descriptors themselves

### StringTable

a persistent *ordered reference set* of all `String` (`std::u16string`) ordered by and indexed with the strings themselves

### ItemBlockCache

caches `ItemBlock` indexed by `item_block_ref` and synchronizes data for `ItemBlockView`

### History

offers undo/redo of operations on `Item`

#### EditState

the current edit state:
- a `COWRef` to focused `ItemBlock` and `Item`
- possibly the caret/selection range of the focused `Item`

#### StackEntry

stores a list of modified `ItemBlock` as `item_block_ref` with a `COWRef` to each's root item and `EditState` after one operation

`UndoStack` and `RedoStack` each stores a list of `StackEntry` for operations

an `ItemBlock` itself stores and displays its current items as a tree, while it's previous or reverted data stored in `UndoStack` or `RedoStack`:

```
UndoStack - item - RedoStack
```

the initial item data and `ItemState` is pushed to `UndoStack` when the application starts

#### Operation

an operation begins at `Begin()` and finishes at `Cancel()` or `End()`

an item can only be modified during an operation

- `Begin()`: create a temporary `StackEntry` and save the current edit state

when a new item is to be modified, its data is copied with a new reference, and the reference update propagates to its parent item until the root item of the `ItemBlock`

- `Cancel()`: point the root item reference of the `ItemBlock` to the last entry on `UndoStack` the data of the items in the entry, discard the entry

- `End()`: push the entry and the current edit state in `UndoStack`, clear `RedoStack`

between operations, `Undo()` or `Redo()` can be called:

- `Undo()`: pop last entry in `UndoStack` and push it to `RedoStack`, 

- `Redo()` 

## Metadata

as the global root block, storing a tuple of:
- reference of `DescriptorRegistry`
- reference of `StringTable`
- reference of root `ItemBlock`

## ItemBlock

## StringBlock

## Item

owned or referenced by `ItemRef`

provides:
- `UpdateSelf(std::unique_ptr<const Item>)`, `virtual OnChildUpdate(Item&, std::unique_ptr<const Item>)`: called within an `Operation` when self is to be modified, updates the entire branch on the tree

### StringRef

provides:


### ItemBlockRef

### Descriptor

#### BasicDescriptor

#### TupleDescriptor

#### DynamicLengthArrayDescriptor

## ItemView

owned by an `Item` 
