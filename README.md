# ini

A simple and small ini file parsing library. Licensed under MIT.

## Including in your project

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

## Documentation

You can find the documentation [here](ini.h).

## Contributing

The library is open to contributions. I don't have any strict guidelines
besides **please don't make any stylistic contributions**. I'm open to
anything else, please don't make me change my mind on that :)

Make sure all test cases pass under both debug and release. Ensure no
memory is leaked. I will be running these tests if you open a PR. If
they fail or a memory profiler screams about your code, I'm going
to immediately close your pull request.

## Accreditations

- [rktest](https://github.com/Warwolt/rktest) by Rasmus Källqvist
- [Yousha's .gitignore for C/C++ developers](https://gist.github.com/Yousha/3830712334ac30a90eb6041b932b68d7)
