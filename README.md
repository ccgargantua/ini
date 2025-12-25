# ini

A simple and small ini file parsing library. Licensed under MIT.

- [Including in Your Project](#including-in-your-project)
  - [Source Inclusion (Copy+Paste)](#source-inclusion-copypaste)
  - [CMake](#cmake)
  - [Makefile](#makefile)
- [Documentation in Brief](#documentation-in-brief)
  - [Parsing Files](#parsing-files)
  - [Making Queries](#making-queries)
  - [Using the Stack](#using-the-stack)
  - [Syntax Support](#syntax-support)
- [Contributing](#contributing)
- [Accreditations](#accreditations)

## Including in Your Project

I tried to make this easily usable for anyone, no matter how
inexperienced or stubborn. If you are new to C, I recommend
you use the [source inclusion](#source-inclusion-copypaste) method. I plan on adding
a header-only "mirror" of this library in the future, so check
back if that's your kinda thing.

### Source Inclusion (Copy+Paste)

Just paste `ini.c` and `ini.h` into your project and compile
`ini.c` along with your own project files.

### CMake

Just drop this repo into your project and add it as a
subdirectory to your target. Something like:
```CMake
add_subdirectory(ini)
target_link_libraries(project_name PRIVATE ini)
```

### Makefile

If you the modularity of CMake but are allergic to modern build
systems for some reason, you can slap this repo into your root
and call the Makefile from your own. You can grab the object file
from `ini/build/` afterwards and do what you please with it.

```
$(MAKE) -C ini/ release   # release build
$(MAKE) -C ini/ debug     # debug build
INI_OBJ = ini/build/ini.o # grab the object file. you're on
                          # your own from here
```

## Documentation in Brief

For documentation on each function, macro, struct, and more, you
can and should consult [the header](ini.h). For all examples that
you see in this section, you can find compiler-ready code in the
`example/` directory. Some things, like parsing raw text rather
than files, are not covered in this document.

### Parsing Files

Consider a basic `.ini` file.

```ini
[Text]

greeting     = Hello
subject      = World
enthusiastic = true

[Execution]

iterations = 5

[Pi]

string = A proper number of digits for pi are:
pi     = 3.1415
```

The easiest way to parse this file is to create our database object
and pass the filepath to the parsing function.

```c++
INIData_t *ini = ini_create_data();
ini_read_file("example/example.ini", ini, NULL, 0);
// do what you need
ini_free_data(ini);
```

On success, the `ini_read_file` function returns the very same
`ini` parser that you passed. **On failure**, it returns `NULL`.
This library doesn't necessarily force you to check for this, but
you would probably still like to know if that happens.

```c++
if (!ini_read_file("example/example.ini", ini, NULL, 0)) {
    // Handle accordingly.
}
```

You'll notice that we have two additional arguments. Above, we
pass `NULL` where we *could* pass a pointer to an `INIError_t`
object. If we passed a valid pointer instead, the object would
populate the appropriate struct members with details of any
error encountered while parsing.

```c++
INIData_t *ini = ini_create_data();
INIError_t error;
ini_read_file("example/example.ini", ini, &error, 0);
```

After parsing our file, we could print out the details of any
error that might occur.

```c++
ini_read_file("example/example.ini", &ini, &error, 0);
if (error.encountered)
{
  printf("%s\n%s\n%*s^\n", error.msg, error.line, error.offset, "");
  // do anything else you need to do
}
```

That fourth parameter is a 64-bit integer that accepts flags.
As of right now, there are only two flags implemented:

* `INI_CONTINUE_PAST_ERROR` will allow a call to `ini_read_file()` to continue parsing after encountering an error.
* `INI_ALLOW_DUPLICATE_SECTIONS` allows duplicate sections to be parsed, and will place pairs under the duplicate into the original section.

So, we could have done:

```c++
INIData_t *ini = ini_create_data();
INIError_t error;
ini_read_file("example/example.ini", ini, &error, INI_CONTINUE_PAST_ERROR);
if (error.encountered)
    print_error(&error);    
```

Now, the parser will only fail if it can't continue, and you can see what
exactly what went wrong if that happens.

### Making Queries

After we construct our `INIData_t` object, we can query for the
data. The most basic way to do this is to just get the `value` string
by providing the `key` and `section` strings.

```c++
const char *greeting = ini_get_value(&ini, "Text", "greeting");
```

On success, `greeting` will be a **null-terminated** string with the
value "Hello" from our `.ini` file. If this failed, then `greeting`
will be a `NULL` pointer. This works fine and the most basic way
to make a query.

There are numerous other functions available to make queries for
specific types *and* provide a default value. Note, queries for
numeric types use the largest of the appropriate primitives, so
to avoid aggressive compiler warnings you can simply cast the return
value.

```c++
// Query for strings
const char *subject = ini_get_string(&ini, "Text", "subject", "nobody...");

// Query for bools
bool enthusiastic = ini_get_bool(&ini, "Text", "enthusiastic", false);
const char enthusiasm = enthusiastic ? '!' : '.';

// Query for signed integers
int iterations = (int)ini_get_signed(&ini, "Execution", "iterations", 1);

for (int i = 0; i < iterations; i++)
    printf("%s, %s%c\n", greeting, subject, enthusiasm);

const char *pi_string = ini_get_string(&ini, "Pi", "string", "pi =");
// Query for floats
float pi = (float)ini_get_float(&ini, "Pi", "pi", 3);
printf("%s %f\n", pi_string, pi);
```

### Using the Stack

This library gives you the option to use the stack rather than the
heap. All this changes about our code is how we initialize the
`INIData_t` object.

```c++
const int max_sections = 32;
const int max_pairs = 32;

// Create our data on the stack
INISection_t sections[max_sections];
INIPair_t pairs[max_sections][max_pairs];
INIPair_t *section_pairs[max_sections];
for (int i = 0; i < max_sections; i++)
    section_pairs[i] = pairs[i];

INIData_t ini;
ini_init_data(&ini, sections, section_pairs, max_sections, max_pairs);
```

This block of code shows us create stack-allocated arrays of objects
that we will need, then uses the `ini_init_data()` helper function
to initialize our object. Everything else stays the same, except if
we parse a file with more sections/pairs than we have allocated, the
parsing will fail. However, if we pass the `INI_CONTINUE_PAST_ERROR`
flag, then the excess sections and pairs will simply be ignored.

### Syntax Support

```ini
; this style of comment
# ... and this one too

[Sections]

[Duplicate Sections]
# [Duplicate Sections]
; ... can only be duplicated with 
; `INI_CONTINUE_PAST_ERROR` or
; `INI_ALLOW_DUPLICATE_SECTIONS` passed to the parsing functions

[Sections with spaces]
key=value
key=value with spaces
this: style of pair is legal too

[_Section_with_underscores_]
key1: this is allowed
_key_: this is also allowed

; special characters are allowed in values (aside from comments and [])
path=C:\path\on\windows\gross\
unix=/much/better.txt
```

## Contributing

The library is open to contributions. I don't have any strict guidelines
besides **please don't make any stylistic contributions**. I'm open to
anything else, please don't make me change my mind on that :)

Make sure all test cases pass under both debug and release. Ensure no
memory is leaked. I will be running these tests if you open a PR. If
they fail or a memory profiler screams about your code, I'm going
to immediately close your pull request. I've made this easy for you
with a `build.sh` build script. Here is your short usage guide:

```bash
bash build.sh -rt # build tests under release
bash build.sh -r  # build library under release
bash build.sh -t  # build tests under debug
bash build.sh     # build library under debug
```

***If you make a low-effort contribution (issue, PR, anything)
it will be ignored and you will be blocked. This includes obvious
use of AI code generation.***

## Accreditations

- [rktest](https://github.com/Warwolt/rktest) by Rasmus Källqvist
- [Yousha's .gitignore for C/C++ developers](https://gist.github.com/Yousha/3830712334ac30a90eb6041b932b68d7)
