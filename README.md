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
Lorem ipsum dolor sit amet, consectetur adipiscing elit.
---
+- sidearrow
#$ callout example 2
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
consectetur adipiscing elit.
----------------------------
╰ sidearrow
┏---┳----------------------┓
| $ | callout example      |
┗---┻----------------------┛
</code>
</pre>
</td>
</tr>

  </tbody>
</table>

> **⚠️ Currently not working properly but input and output should look something like this.**
## What & Why
*A terminal can not display a markdown file correctly for obvious reasons*. What I have wanted for a while is a stylized file "format" made to make simple plain-text files a bit prettier. The main idea from the beginning being the callout (inspired by notions callouts) which would be a pain to create without this language. It was also important from the start to keep everything minimal and simple.

*My idea to keep the files tidy is to introduce a width*. A text file is pretty much infinite in width (without word-wrap that is) so by capping the with i could get more structured text files. (width can be seen on the divider in the example above)
   
## Install & Use
1. Compile termarkup (use `make.sh` or just compile with your c compilor because of no external libraries)
   ```bash
   $ ./make.sh
   ```
2. Create input file (eg: `example_input.txt`)
3. Run termarkup with specified **input**, **output** and your chosen **width**.
   ```bash
   $ ./termarkup example_input.txt example_output.txt 28
   ```
