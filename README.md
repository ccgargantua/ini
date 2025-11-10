# ini

A simple and small ini file parsing library. Licensed under MIT.

## Including in your project

There are two very easy ways to include this INI library in your project.
You can simply add the `ini.c` and `ini.h` to your project, or you can
include the project via CMake submodule and add the static library to your
target. If you don't understand the first, do the second :)

## Documentation

For now, the functions are documented in `ini.h` via comments.

## Contributing

The library is open to contributions. I don't have any strict guidelines
besides **please don't make any stylistic contributions**. I'm open to
anything else, please don't make me change my mind on that :)

Make sure all test cases pass under both debug and release. Ensure no
memory is leaked. I will be running these tests if you open a PR. If
they fail, I'm going to immediately close your pull request.

## Accreditations

- [rktest](https://github.com/Warwolt/rktest) by Rasmus Källqvist
- [Yousha's .gitignore for C/C++ developers](https://gist.github.com/Yousha/3830712334ac30a90eb6041b932b68d7)
