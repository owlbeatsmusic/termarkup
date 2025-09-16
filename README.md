<h1 align="center">
Termarkup
</h1>
<p align="center">
A "Terminal" Plain Text to Plain Text Markup Language.
</p>

![screenshot of skrot-nav from 2025-02-07](docs/images/example1.png?raw=true)
<i>Example 1. Input on the left, output on the right. Displayed with vim.</i>
</br>
<!-- prettier-ignore -->
<center>
<table>
  <thead>
    <tr>
      <th width="500px"> INPUT</th>
      <th width="500px">OUTPUT</th>
    </tr>
  </thead>
  <tbody>
  <tr width="600px">
<td>
<pre>
<code>
*-heading 1
**-heading 2
***-heading 3
Lorem ipsum dolor sit amet, consectetur.
---
+- sidearrow
#$ callout example 2. this is a test.
 <br>
 <br>
</code>
</pre>
</td>
<td>
<pre>
<code>
*- heading 1 -*
**- heading 2 -**
***- heading 3 -***
Lorem ipsum dolor sit amet, 
consectetur.
----------------------------
╰ sidearrow
┏---┳----------------------┓
| $ | callout example. thi |
|   | s is a test.         | 
┗---┻----------------------┛
</code>
</pre>
</td>
</tr>

  </tbody>
</table>
</center>

> [!WARNING]
> Use release for functional code. Current code is under development.

## What Is This?
*A terminal can not display a markdown file correctly*. What I have wanted for a while is a stylized file "format" to make simple plain-text files a bit prettier. The main idea from the beginning being the callout (inspired by notions callouts) which would be a pain to create without this language. It was also important from the start to keep everything minimal and simple.

*My idea to keep the files tidy is to introduce a width*. A text file is pretty much infinite in width (without word-wrap that is) so by limiting the width I could get more structured text files. (the width can be seen on the divider in the example above)
   
### Install & Use
(*only compatible for unix systems for now**)
1. Compile termarkup (run `./make.sh` or just compile with your c compiler because there are no external libraries)
   ```bash
   $ ./make.sh
   ```
2. Create the input file (eg: `example_input.txt`)
3. Run termarkup with specified **input**, **output**, your chosen **width** and optionally **theme**. Read more here: [Command Line Interface](#command-line-interface)
   ```bash
   $ ./termarkup example_input.txt example_output.txt 28 themes/normal-theme.txt
   ```
4. The output is now written to your specified output file.
</br>

## Documentation
Table of contents
1. [Command Line Interface](#command-line-interface)
2. [All Formatting](#all-formatting)
3. [Modifiers](#modifiers)	
4. [Themes](#themes)	

### Command Line Interface
When running termakrup in your terminal these are the parameters in order.
1. **`Input file path`**. [_REQUIRED_]. Path to the file that will be read. It will be read as a plain text file so the suggested file extension is `.txt` but any will work.
2. **`Output file path`**. [_REQUIRED_]. Path to the file that the output will be written to. It will be written as a plain text file.
3. **`Width`**. [_REQUIRED_]. Whole number between 10 and 512. The width limits the length of each line/row and separates the text into multiple lines/rows neatly.
4. **`Theme`**. [_OPTIONAL_]. Path to the theme file. Customize the output to your liking. Read more here: [Themes](#themes)	

### All Formatting
With a width of `28` and using `themes/normal-theme.txt`
<table><thead><tr>
      <th width="500px"> INPUT</th>
      <th width="500px">OUTPUT</th>
    </tr></thead>
<tbody><tr width="600px"><td><pre>
<code>*-heading 1
<br><br></code></pre></td>
<td><pre>
<code>*- heading 1 -*
</code></pre></td></tr></tbody>

<tbody><tr width="600px"><td><pre>
<code>**-heading 2
<br><br></code></pre></td>
<td><pre>
<code>**- heading 2 -**
</code></pre></td></tr></tbody>

<tbody><tr width="600px"><td><pre>
<code>***-heading 3
<br><br></code></pre></td>
<td><pre>
<code>***- heading 3 -***
</code></pre></td></tr></tbody>

<tbody><tr width="600px"><td><pre>
<code>---
<br><br></code></pre></td>
<td><pre>
<code>----------------------------
</code></pre></td></tr></tbody>

<tbody><tr width="600px"><td><pre>
<code>+-sidearrow
<br><br></code></pre></td>
<td><pre>
<code>╰ sidearrow
</code></pre></td></tr></tbody>

<tbody><tr width="600px"><td><pre>
<code>#$ callout example 2. this is a test.
<br><br></code></pre></td>
<td><pre>
<code>┏---┳----------------------┓
| $ | callout example. thi |
|   | s is a test.         | 
┗---┻----------------------┛
</code></pre></td></tr></tbody>

<tbody><tr width="600px"><td><pre>
<code>this is just plain text. this is just plain text.
<br><br></code></pre></td>
<td><pre>
<code>this is just plain text. thi
s is just plain text.
</code></pre></td></tr></tbody>
</table>


### Modifiers
You can place a modifier at the start of the line to modify the formatting. The modifiers are 
 - `%c` - **CENTER**. which centers the formatting(eg: heading 1). This works on everything except callouts and dividers.

### Themes
You can customzie your output using themes. Theme-files are located in the `themes/` folder but does not have to be there to be used. You can either use one of the themes that already exsist, modify it or create your own! 

For all formatting **except** the `divider` and `callout` the theme file is structured like this (eg: heading 1):
```
heading_1 = ["*- ", " -*", 3, 3]
```
The first two strings are the **before** and **after**. Before is the string that comes before the heading 1 and after is the string that comes after. The two digits are the **before length** and **after length** of their corresponding strings (including white space). This is due to some special charcters being counted as multiple charcters. 
> [!IMPORTANT]  
> Remember to change the before and after length when chaning the before and after strings.

**Callout**</br>
For the border there is "true"/"false" (remember quotations) for if to show border, x-padding & y-padding, and a form of sheet that you modify. You can only **one** symbol per value (although it can be a symbol with length of multiple characters such as a unicode character).
```
border = ["true", 5, 2, "┏", "┓", "┗", "┛"]
```


**Divider**</br>
For the divider you do not have to specify the length but in you can only have **one** symbol (although it can be a symbol with length of multiple characters such as a unicode character).
```
divider = ["-"]
```

**Callout**</br>
For callouts there is a form of sheet that you modify. As with dividers you can only **one** symbol per value (here to it can be a symbol with length of multiple characters such as a unicode character).
```
callout    = ["━", "┃", "┏", "┳", "┓", "┗", "┻", "┛"]
```
