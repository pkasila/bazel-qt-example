# Pantry Keeper

Qt C++ Widgets application for Laboratory Work #1, task 2.

## Theme
A pantry inventory manager. You keep track of groceries on a shelf, add new items, edit the selected item, and watch which products are low on stock.

## Widgets used
Basic widgets:
- `QLabel` — title and summary text.
- `QLineEdit` — separate input for adding a new item and another one for editing the selected item.
- `QPushButton` — add a new item / save the selected item.
- `QCheckBox` — show only low-stock items.
- `QComboBox` — choose the item's category.

Other widgets:
- `QSpinBox` — set item quantity.
- `QTableWidget` — show the pantry list and item state.
- `QProgressBar` — show the percentage of well-stocked items.

## Interaction
- Type a new item name in the **Add new item** field and press **Enter** or click **Add item**.
- Select a row in the table to edit that item in the **Edit selected item** section.
- Use the edit name field, category combo box, and quantity spin box to change the selected item, then press **Enter** or click **Save selected item**.
- Turn on **Show only low-stock items** to filter the table.
- Double-click a table row to use one unit of that item.
- Click the title to reload the sample pantry.

## Build
This project is wired for Bazel and expects a local Qt 5 Widgets development setup.

```bash
bazel run //:pantry_keeper
```

If your Qt headers or libraries are installed in a different location, edit the `-I...` include paths and `linkopts` inside `BUILD.bazel`.
