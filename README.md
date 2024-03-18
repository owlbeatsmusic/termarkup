# Termarkup
A "Terminal" Plain Text to Plain Text Markup Language. To create/view prettier documents in terminal.

Currently not working properly but source and output will look something like this.

Width is specified att execution.

**Table of Contents**
 - [Syntax](#syntax)
 - [Modifiers](#modifiers)

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

