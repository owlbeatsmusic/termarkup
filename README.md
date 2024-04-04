<h1 align="center">
Termarkup
</h1>
<p align="center">
A "Terminal" Plain Text to Plain Text Markup Language.
</p>
<!-- prettier-ignore -->
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

> [!WARNING]
> This is the development branch, things are unstable.

## What Is This?
*A terminal can not display a markdown file correctly*. What I have wanted for a while is a stylized file "format" made to make simple plain-text files a bit prettier. The main idea from the beginning being the callout (inspired by notions callouts) which would be a pain to create without this language. It was also important from the start to keep everything minimal and simple.

*My idea to keep the files tidy is to introduce a width*. A text file is pretty much infinite in width (without word-wrap that is) so by capping the with i could get more structured text files. (width can be seen on the divider in the example above)
   
### Install & Use
1. Compile termarkup (use `make.sh` or just compile with your c compilor because of no external libraries)
   ```bash
   $ ./make.sh
   ```
2. Create input file (eg: `example_input.txt`)
3. Run termarkup with specified **input**, **output**, your chosen **width** and optionally **theme**.
   ```bash
   $ ./termarkup example_input.txt example_output.txt 28 themes/normal-theme.txt
   ```
</br>

## Documentation
Table of contents
1. [All Formatting](#all-formatting)
2. [Modifiers](#modifiers)	
3. [Themes](#themes)	

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
