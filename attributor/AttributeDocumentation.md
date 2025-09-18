# Attribute Documentation / Requirements *(WORK IN PROGRESS)*

Attributor Documentation, containing the cases for each attribute and how they are placed into the xml


## Shortcuts
* [General Syntax](#general-syntax-1) - General Attribute Usage
* [Attribute Information](#attributes) - Base syntax per attribute

# General Syntax
## Placement

The syntax of the attributes will usually go by the following guidelines:
- Property of <code>expr</code>
- Property of <code>decl</code>
- Resides in the outer-most <code>expr</code> or <code>decl</code> element per line

(Some cases exist in which the guidelines are inaccurate.)

## Attribute Values

Attribute values consist of 3 identifiable parts per slice profile:
- Variable Name
- Declaration Line
- Filepath

C++ Code:

```cpp
#include <iostream>

int main() {
    int x = 20;
    x = 30;
    std::cout << x << std::endl;
}
```

Slice Profile JSON:

```json
{
"x-4-c2b72b167ef994ae4a51d54129fa80f322b323ad":{
    "file":"thing.cpp",
    "language":"C++",
    "namespace":[],
    "class":"",
    "function":"main",
    "type":"int",
    "name":"x",
    "dependence":[],
    "aliases":[],
    "calls":[],
    "use":[5,6],
    "definition":[4,5]
}
}
```

From the json file, the object name "x-4-c2b72b167ef994ae4a51d54129fa80f322b323ad" contains the name of the variable and the declaration line separated by hyphens, along with a hash afterwards. "thing.cpp" is in the filename value. The three of these are concatenated into a single string with no spaces, then passed through a SHA1 hash, which is used as the value of the attribute in the XML.

```
Variable Name:      "x"
Declaration Line:   "4"
Filepath:           "thing.cpp"

Pre-SHA1:           "x4thing.cpp"
Post-SHA1:          "f1227bbf5cb9bd19b255096e326ebdbe58395c60"
```

In order for this to be tracked throughout the XML file, every instance where a slice profile is either declared/defined/used, it will use the same hash with the attribute. 

Finalized XML:

```xml
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file>&lt;iostream&gt;</cpp:file></cpp:include>

<function><type><name>int</name></type> <name>main</name><parameter_list>()</parameter_list> <block>{<block_content>
    <decl_stmt><decl slice:decl="f1227bbf5cb9bd19b255096e326ebdbe58395c60"><type><name>int</name></type> <name>x</name> <init>= <expr><literal type="number">20</literal></expr></init></decl>;</decl_stmt>
    <expr_stmt><expr slice:def="f1227bbf5cb9bd19b255096e326ebdbe58395c60"><name>x</name> <operator>=</operator> <literal type="number">30</literal></expr>;</expr_stmt>
    <expr_stmt><expr slice:use="f1227bbf5cb9bd19b255096e326ebdbe58395c60"><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <name>x</name> <operator>&lt;&lt;</operator> <name><name>std</name><operator>::</operator><name>endl</name></name></expr>;</expr_stmt>
</block_content>}</block></function></unit>
```

# Attributes

## slice:decl
### <code>slice:decl</code> is the declaration of a variable with a slice profile.

Example: x's and y's

```cpp
int x = 10;
int y = 20;
y = x + 40;
```

Corresponding XML File <b>WITH</b> <code>slice:decl</code>: 

```xml
<decl_stmt><decl slice:decl="x"><type><name>int</name></type> <name>x</name> <init>= <expr><literal type="number">10</literal></expr></init></decl>;</decl_stmt>
<decl_stmt><decl slice:decl="y"><type><name>int</name></type> <name>y</name> <init>= <expr><literal type="number">20</literal></expr></init></decl>;</decl_stmt>
<expr_stmt><expr><name>y</name> <operator>=</operator> <name>x</name> <operator>+</operator> <literal type="number">40</literal></expr>;</expr_stmt>
```

<table>
    <thead>
        <tr>
            <th><code>slice:decl</code> Occurance</th>
            <th>Line Number</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td><code>slice:decl="x-hash"</code></td>
            <td>1</td>
            <td>Initialization of <code>x</code></td>
        </tr>
        <tr>
            <td><code>slice:decl="y-hash"</code></td>
            <td>2</td>
            <td>Initialization of <code>y</code></td>
        </tr>
    </tbody>
</table>

Note that <code>x</code> and <code>y</code> are being declared on lines 1 and 2, represented by a <code>"decl"</code> element. <code>slice:decl</code> isn't shown again in line 3 for <code>y</code> because <code>y</code> is not being declared, but rather redefined (later discussed with <code>slice:def</code>).

## slice:def
### <code>slice:def</code> represents the other times the variable is redefined.

Example: x's and y's

```cpp
int x = 10;
int y = 20;
y = x + 40;
```

Corresponding XML File <b>WITH</b> <code>slice:def</code>: 

```xml
<decl_stmt><decl><type><name>int</name></type> <name>x</name> <init>= <expr><literal type="number">10</literal></expr></init></decl>;</decl_stmt>
<decl_stmt><decl><type><name>int</name></type> <name>y</name> <init>= <expr><literal type="number">20</literal></expr></init></decl>;</decl_stmt>
<expr_stmt><expr slice:def="y"><name>y</name> <operator>=</operator> <name>x</name> <operator>+</operator> <literal type="number">40</literal></expr>;</expr_stmt>
```

## slice:use
### <code>slice:use</code>

```xml

```

# Elements Containing Slice Attributes

- <code>decl</code>
    - <code>decl</code>
    - <code>use</code>
- <code>expr</code>
    - <code>def</code>
    - <code>use</code>
- <code>while</code>
    - <code>use</code>
    - <code>def</code>
- <code>if</code>
    - <code>use</code>
    - <code>def</code>
