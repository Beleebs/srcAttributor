Quick Clarification about function parameters with attributor:

Should any parameters be declared in the <parameter_list> element, or all of the <decl> elements in the inside?

for example:
slice:

```xml
<function><type><name>int</name></type> <name>pbv</name><parameter_list>(<parameter><decl><type><name>int</name></type> <name>v</name></decl></parameter>)</parameter_list> <block>{<block_content>
    <decl_stmt><decl><type><name>int</name></type> <name>a</name> <init>= <expr><name>v</name> <operator>*</operator> <literal type="number">2</literal></expr></init></decl>;</decl_stmt>
    <return>return <expr><name>a</name></expr>;</return>
</block_content>}</block></function>
```

Slice profile: v, type int, scope: pbv()

Should it be:

(in the parameter_list)
```xml
<function><type><name>int</name></type> <name>pbv</name><parameter_list slice:decl="HASH_HERE!!!!">(<parameter><decl><type><name>int</name></type> <name>v</name></decl></parameter>)</parameter_list>...
```

or 

(in the decl elements inside of the parameter list)
```xml
<function><type><name>int</name></type> <name>pbv</name><parameter_list>(<parameter><decl slice:decl="HASH_HERE!!!!"><type><name>int</name></type> <name>v</name></decl></parameter>)</parameter_list>...
```

!!!!!!!!!DECL ELEMENTS DING DING DING!!!!!!!!!!

ONLY 3 MORE PROBLEMS
- def cannot be placed if there is already a decl attribute
- make sure if there is a def containing the same hash as a use, use cannot be placed on same line.
- use cannot be placed if there is already a decl attribute (in the decl) (found from for loops)