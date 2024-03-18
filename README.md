# Termarkup
A "Terminal" Plain Text to Plain Text Markup Language. To create/view prettier documents in terminal.

> **⚠️ Currently not working properly but input and output will look something like this.**

**Table of Contents**
 - [How To](#how-to)
 - [Syntax](#syntax)
 - [Modifiers](#modifiers)

## How to
1. Compile termarkup
   ```bash
   $ ./make.sh
   ```
2. Create input file (eg: `example.txt`)
3. Run termarkup with specified **input**, **output** and your chosen **width**.
   ```bash
   $ ./termarkup example.txt output.txt 40
   ```

## Syntax
*input - output*

**Heading 1**
```
*!heading
```
```
*- heading -*
```
</br>

**Heading 2**
```
*!!heading
```
```
**- heading -**
```
</br>

**Heading 3**
```
*!!!heading
```
```
***- heading -***
```
</br>

**Sidearrow**
```
+- sidearrow
```
```
╰ sidearrow
```
</br>

**Divider**
```
---
```
```
⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯
```
</br>

**Callout**
```
#2 thisisacallout
```
```
┌–––┬–––––––––––––––––┒
│ # │ thisisacallout. │
└———┴————————––———————┘
```

## Modifiers
**Center Modifier** - centers object according to width
```
%c *!header1
```

