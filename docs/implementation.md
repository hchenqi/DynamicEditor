# Implementation

> The implementation is based on [Design](design.md) with a modified structure and an additional description on `History` and `Operation`.

## Context

### DescriptorRegistry

### StringTable

### ItemBlockCache

## Item

### Type

#### Basic

##### StringRef

##### ItemBlockRef

#### Tuple

#### DynamicLengthArray

### History

(global) offers undo/redo of item-level operations

#### StackEntry

stores a list of modified items with their original data for one operation

`UndoStack` and `RedoStack` each stores a list of `StackEntry` for operations

an item itself stores and displays its current data, while it's previous or reverted data stored in `UndoStack` or `RedoStack`

```
UndoStack - item - RedoStack
```

#### Operation

an operation begins at `Begin()` and finishes at `Cancel()` or `End()`

an item can only be modified during an operation

- `Begin()`: create a temporary `StackEntry`

when a new item is modified, the original data of the item is stored in the entry

- `Cancel()`: restore the data of the items in the entry, discard the entry

- `End()`: push the entry in `UndoStack`

#### Undo

`Undo()` moves the pointer on the `Stack` backwards and 

#### Redo

## Block

### ItemBlock

### String
