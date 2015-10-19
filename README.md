LolToml [![Build Status](https://travis-ci.org/andrusha97/loltoml.svg?branch=master)](https://travis-ci.org/andrusha97/loltoml)
=======
Loltoml is a library implementing SAX-style parser of the [TOML](https://github.com/toml-lang/toml) language.

Current status
==============
This library aims to support the latest version of TOML until TOML 1.0 is released.
So any breaking changes in the language spec will be reflected in the library. Beware.
Currently supported version is [v0.4.0](https://github.com/toml-lang/toml/tree/v0.4.0).

It passes all tests from the [toml test suite](https://github.com/BurntSushi/toml-test)
using the [decoder](https://github.com/andrusha97/loltoml/blob/master/examples/toml-test-decoder.cpp) from examples.

Build and dependencies
=====================
It's header-only and depends only on the C++ standard library.

Documentation
=============
Since the API of the library consists of a single function `parse`, it should be enough
to read the [comments](https://github.com/andrusha97/loltoml/blob/master/include/loltoml/parse.hpp)
and take a look at [examples](https://github.com/andrusha97/loltoml/tree/master/examples).
