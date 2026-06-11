# Implementation

> The implementation is based on [Design](design.md) with a modified structure and an emphasis on `History` and operations.

## Utility

### COWRef (Copy-On-Write Reference)

offers a wrapper that either holds a const reference pointing to another object, or holds a `std::unique_ptr<T>` owning the object that can be modified

used to implement copy-on-write `Item` tree for operations with `History` stacks, where references to objects in previous stack entries are ensured to be alive

(the const reference can be replaced with `std::shared_ptr<const T>` and the owner `std::shared_ptr<T>`, in case some entries need to be dropped so the later entry takes the ownership automatically)

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

### BasicDescriptor

#### StringRef

#### ItemBlockRef

### TupleDescriptor

### DynamicLengthArrayDescriptor
