hello_result = "Hello World\nHello World"
vars_result = """
a: 4 b: 5 c: Hello d: true
a + b: 9
a - b: -1
a * b: 20
a / b: 0.8

a + c: 4Hello
a + d: 5"""

for_result = """
i: 0
i: 1
i: 2
i: 3
i: 4

j: 5
j: 6
j: 7
j: 8
j: 9"""

while_result = """
0
1
2
3
4
5

0
1
2
3
4
5
"""

conditions_result = """
X is bigger than Y
B is bigger than A
C contains the same value as D
Negation works fine
"""

functions_result = """
10
1
20
4
Hello
TestTestTestTestTestTest
"""

nested_functions_result = """
3
"""

classes_result = """
Name: John
Age: 24
Job: Artist

Name: Eric
Age: 42
Job: Doctor

Name: Peter
Age: 31
Job: Police Officer
"""

classes_no_init_result = """
Name: John
Age: 28
Job: Miner
"""

classes_return_result = """
Name: John
Age: 24
Job: Artist

Name: Eric
Age: 42
Job: Doctor

Name: Peter
Age: 31
Job: Police Officer
"""

classes_inherit_result = """
Name: John
Age: 28
Job: Miner
The name is John
"""

nested_classes = """
Name: John
Age: 28
"""

nested_classes_functions_result = """
RAW DATA:
The Engine: <Engine instance>
Color: <Color instance>


CLEAN DATA:
The Engine: 501
Color: 255 144 65

Invalid type
"""